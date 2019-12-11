#include "RSMIndirectLightRenderPass.h"

#include "RSMIndirectLightMapVolume.h"

#include "Debug/Statistics/StatisticsManager.h"
#include "Debug/Statistics/RSMStatisticsObject.h"

RSMIndirectLightRenderPass::RSMIndirectLightRenderPass () :
	_lastPostProcessMapVolume (nullptr)
{

}

RSMIndirectLightRenderPass::~RSMIndirectLightRenderPass ()
{
	delete _lastPostProcessMapVolume;
}

void RSMIndirectLightRenderPass::Init (const RenderSettings& settings)
{
	/*
	 * Initialize post process render pass
	*/

	PostProcessRenderPass::Init (settings);

	/*
	 * Initialize last post process map volume
	*/

	_lastPostProcessMapVolume = CreatePostProcessVolume ();

	InitRSMIndirectMapVolume (settings);

	/*
	 * Initialize ping-pong buffers
	*/

	auto rsmIndirectMapVolume = (RSMIndirectLightMapVolume*) _postProcessMapVolume;
	auto rsmLastIndirectMapVolume = (RSMIndirectLightMapVolume*) _lastPostProcessMapVolume;

	rsmIndirectMapVolume->SetCurrent(true);
	rsmLastIndirectMapVolume->SetCurrent(false);
}

RenderVolumeCollection* RSMIndirectLightRenderPass::Execute (const RenderScene* renderScene, const Camera* camera,
	const RenderSettings& settings, RenderVolumeCollection* rvc)
{
	/*
	 * Update temporal anti-aliasing map volume
	*/

	UpdateRSMIndirectMapVolume (settings);

	/*
	 * Execute post process render pass
	*/

	rvc = PostProcessRenderPass::Execute (renderScene, camera, settings, rvc);

	/*
	 * Keep current camera view projection matrix
	*/

	return rvc->Insert ("LastIndirectMap", _lastPostProcessMapVolume);
}

void RSMIndirectLightRenderPass::Clear ()
{
	/*
	 * Clear last post processing volume
	*/

	_lastPostProcessMapVolume->Clear ();

	/*
	 * Clear post process render pass
	*/

	PostProcessRenderPass::Clear ();
}

bool RSMIndirectLightRenderPass::IsAvailable (const RenderScene* renderScene, const Camera* camera,
	const RenderSettings& settings, const RenderVolumeCollection* rvc) const
{
	/*
	 * Always execure reflective shadow mapping indirect light render pass
	*/

	return true;
}

std::string RSMIndirectLightRenderPass::GetPostProcessFragmentShaderPath () const
{
	return "Assets/Shaders/ReflectiveShadowMapping/reflectiveShadowMapIndirectFragment.glsl";
}

std::string RSMIndirectLightRenderPass::GetPostProcessVolumeName () const
{
	return "IndirectMap";
}

glm::ivec2 RSMIndirectLightRenderPass::GetPostProcessVolumeResolution (const RenderSettings& settings) const
{
	if (_postProcessMapVolume != nullptr) {
		StatisticsObject* stat = StatisticsManager::Instance ()->GetStatisticsObject ("RSMStatisticsObject");
		RSMStatisticsObject* rsmStatisticsObject = nullptr;

		if (stat == nullptr) {
			stat = new RSMStatisticsObject ();
			StatisticsManager::Instance ()->SetStatisticsObject ("RSMStatisticsObject", stat);
		}

		rsmStatisticsObject = dynamic_cast<RSMStatisticsObject*> (stat);

		rsmStatisticsObject->rsmLastIndirectMapVolume = _lastPostProcessMapVolume;
		rsmStatisticsObject->rsmIndirectMapVolume = _postProcessMapVolume;
	}

	return glm::ivec2 (glm::vec2 (settings.framebuffer.width, settings.framebuffer.height) * settings.rsm_scale);
}

PostProcessMapVolume* RSMIndirectLightRenderPass::CreatePostProcessVolume () const
{
	RSMIndirectLightMapVolume* rsmIndirectLightMapVolume = new RSMIndirectLightMapVolume ();

	return rsmIndirectLightMapVolume;
}

std::vector<PipelineAttribute> RSMIndirectLightRenderPass::GetCustomAttributes (const Camera* camera,
	const RenderSettings& settings, RenderVolumeCollection* rvc)
{
	/*
	 * Attach post process volume attributes to pipeline
	*/

	std::vector<PipelineAttribute> attributes = PostProcessRenderPass::GetCustomAttributes (camera, settings, rvc);

	/*
	 * Attach screen space ambient occlusion attributes to pipeline
	*/

	PipelineAttribute rsmResolution;
	PipelineAttribute rsmRadius;
	PipelineAttribute rsmIntensity;
	PipelineAttribute rsmNoiseEnabled;

	rsmResolution.type = PipelineAttribute::AttrType::ATTR_2F;
	rsmRadius.type = PipelineAttribute::AttrType::ATTR_1F;
	rsmIntensity.type = PipelineAttribute::AttrType::ATTR_1F;
	rsmNoiseEnabled.type = PipelineAttribute::AttrType::ATTR_1I;

	rsmResolution.name = "rsmResolution";
	rsmRadius.name = "rsmRadius";
	rsmIntensity.name = "rsmIntensity";
	rsmNoiseEnabled.name = "rsmNoiseEnabled";

	glm::ivec2 resolution = glm::ivec2 (glm::vec2 (settings.framebuffer.width, settings.framebuffer.height) * settings.rsm_scale);

	rsmResolution.value = glm::vec3 (resolution, 0.0f);
	rsmRadius.value.x = settings.rsm_radius;
	rsmIntensity.value.x = settings.rsm_intensity;
	rsmNoiseEnabled.value.x = settings.rsm_noise_enabled;

	attributes.push_back (rsmResolution);
	attributes.push_back (rsmRadius);
	attributes.push_back (rsmIntensity);
	attributes.push_back (rsmNoiseEnabled);

	return attributes;
}

void RSMIndirectLightRenderPass::UpdateRSMIndirectMapVolume (const RenderSettings& settings)
{
	glm::ivec2 framebufferSize = GetPostProcessVolumeResolution (settings);

	glm::ivec2 rsmIndirectMapSize = _lastPostProcessMapVolume->GetSize ();

	if (rsmIndirectMapSize != framebufferSize) {

		/*
		 * Clear temporal anti-aliasing map volume
		*/

		_lastPostProcessMapVolume->Clear ();

		/*
		 * Initialize temporal anti-aliasing map volume
		*/

		InitRSMIndirectMapVolume (settings);
	}
}

void RSMIndirectLightRenderPass::InitRSMIndirectMapVolume (const RenderSettings& settings)
{
	/*
	 * Initialize temporal anti-aliasing map volume
	*/

	glm::ivec2 volumeResolution = GetPostProcessVolumeResolution (settings);

	if (!_lastPostProcessMapVolume->Init (volumeResolution)) {
		Console::LogError (std::string () + "Post-process volume cannot be initialized!" +
			"It is not possible to continue the process. End now!");
		exit (POST_PROCESS_MAP_VOLUME_NOT_INIT);
	}
}
