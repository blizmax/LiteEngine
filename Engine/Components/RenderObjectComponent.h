#ifndef RENDEROBJECTCOMPONENT_H
#define RENDEROBJECTCOMPONENT_H

#include "Systems/Components/Component.h"

#include "Renderer/RenderObject.h"

class ENGINE_API RenderObjectComponent : public Component
{
	DECLARE_COMPONENT(RenderObjectComponent)

protected:
	ATTRIBUTE(EditAnywhere, Meta)
	int _renderStage;
	ATTRIBUTE(EditAnywhere, Meta)
	int _layer;
	ATTRIBUTE(EditAnywhere, Meta=(Model))
	Resource<Model> _model;

	RenderObject* _renderObject;

public:
	RenderObjectComponent ();
	~RenderObjectComponent ();

	void Awake ();

	void SetActive (bool isActive);

	void OnAttachedToScene ();
	void OnDetachedFromScene ();

	void SetModel (const Resource<Model>& model);
	void SetRenderObject (RenderObject* renderObject);

	const Resource<Model>& GetModel () const;
	RenderObject* GetRenderObject ();
};

#endif