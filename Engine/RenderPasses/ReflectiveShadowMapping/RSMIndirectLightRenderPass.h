#ifndef RSMINDIRECTLIGHTRENDERPASS_H
#define RSMINDIRECTLIGHTRENDERPASS_H

#include "RenderPasses/PostProcess/PostProcessRenderPass.h"

class ENGINE_API RSMIndirectLightRenderPass : public PostProcessRenderPass
{
	DECLARE_RENDER_PASS(RSMIndirectLightRenderPass)

protected:
	PostProcessMapVolume* _lastPostProcessMapVolume;

public:
	RSMIndirectLightRenderPass ();
	~RSMIndirectLightRenderPass ();

	void Init (const RenderSettings& settings);
	RenderVolumeCollection* Execute (const RenderScene* renderScene, const Camera* camera,
		const RenderSettings& settings, RenderVolumeCollection* rvc);

	void Clear ();

	bool IsAvailable (const RenderScene* renderScene, const Camera* camera,
		const RenderSettings& settings, const RenderVolumeCollection* rvc) const;
protected:
	std::string GetPostProcessFragmentShaderPath () const;
	std::string GetPostProcessVolumeName () const;
	glm::ivec2 GetPostProcessVolumeResolution (const RenderSettings& settings) const;
	PostProcessMapVolume* CreatePostProcessVolume () const;

	std::vector<PipelineAttribute> GetCustomAttributes (const Camera* camera,
		const RenderSettings& settings, RenderVolumeCollection* rvc);

	void UpdateRSMIndirectMapVolume (const RenderSettings& settings);
	void InitRSMIndirectMapVolume (const RenderSettings& settings);
};

#endif