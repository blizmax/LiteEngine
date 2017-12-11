#ifndef GBUFFER_H
#define GBUFFER_H

#include "RenderPasses/MultipleRenderTargetsVolume.h"

class GBuffer : public MultipleRenderTargetsVolume
{
public:
    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_SPECULAR,
        GBUFFER_NUM_TEXTURES
    };

public:
    GBuffer();

    void BindForGeomPass();
    void BindForStencilPass();
    void BindForLightPass();
}; 

#endif