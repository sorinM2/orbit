#pragma once
#include "d3d11common.h"
#include "orbit/graphics/common/rasterizer.h"

namespace orbit::graphics
{
    class d3d11_rasterizer final : public rasterizer
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;

    private:
        d3d11_rasterizer(rendering_device* device, rendering_device_context* context, const rasterizer_desc& desc);
        ~d3d11_rasterizer() override { util::safe_release(_internal_rasterizer_state); }
        ID3D11RasterizerState* _internal_rasterizer_state;
    };
}