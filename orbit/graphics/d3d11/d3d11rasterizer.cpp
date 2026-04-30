#include "d3d11rasterizer.h"

#include "d3d11device.h"
#include "util.h"

namespace orbit::graphics
{
    // D3D11_RASTERIZER_DESC rasterizer_desc;
    // rasterizer_desc.CullMode = D3D11_CULL_NONE;
    // rasterizer_desc.FillMode = D3D11_FILL_SOLID;
    // rasterizer_desc.FrontCounterClockwise = true;
    // rasterizer_desc.AntialiasedLineEnable = false;
    // rasterizer_desc.DepthBias = 0;
    // rasterizer_desc.DepthBiasClamp = 0.f;
    // rasterizer_desc.SlopeScaledDepthBias = 0.f;
    // rasterizer_desc.DepthClipEnable = true;
    // rasterizer_desc.ScissorEnable = false;
    // rasterizer_desc.MultisampleEnable = false;

    d3d11_rasterizer::d3d11_rasterizer(rendering_device* device, rendering_device_context* context, const rasterizer_desc& desc) : device_resource(device, context), rasterizer(desc)
    {
        D3D11_RASTERIZER_DESC raster_desc;
        ZeroMemory(&raster_desc, sizeof(raster_desc));
        raster_desc.CullMode = d3d11::convert_cull_mode_flag(desc.cull_mode);
        raster_desc.FillMode = d3d11::convert_fill_mode_flag(desc.fill_mode);
        raster_desc.FrontCounterClockwise = desc.front_counter_clockwise;
        raster_desc.DepthBias = 0;
        raster_desc.DepthBiasClamp = 0.0f;
        raster_desc.SlopeScaledDepthBias = 0.0f;
        raster_desc.DepthClipEnable = desc.depth_clip_enabled;
        raster_desc.ScissorEnable = desc.scissor_enabled;
        raster_desc.MultisampleEnable = desc.multisample_enabled;

        d3d11_rendering_device* d3d11_device = static_cast<d3d11_rendering_device*>(device);
        DXCALL(d3d11_device->_internal_device->CreateRasterizerState(&raster_desc, &_internal_rasterizer_state));
    }

}