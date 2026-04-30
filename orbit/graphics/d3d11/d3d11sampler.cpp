#include "d3d11sampler.h"

#include "d3d11device.h"
#include "util.h"

namespace orbit::graphics
{
    d3d11_sampler::d3d11_sampler(rendering_device* device, rendering_device_context* context, const sampler_desc& desc) : device_resource(device, context), sampler(desc)
    {
        D3D11_SAMPLER_DESC sampler_desc;
        ZeroMemory(&sampler_desc, sizeof(sampler_desc));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler_desc.AddressU = d3d11::convert_texture_address_mode_flag(desc.address_u);
        sampler_desc.AddressV = d3d11::convert_texture_address_mode_flag(desc.address_v);
        sampler_desc.AddressW = d3d11::convert_texture_address_mode_flag(desc.address_w);
        memcpy(&sampler_desc.BorderColor, &desc.border_color, sizeof(float) * 4);
        sampler_desc.ComparisonFunc = d3d11::convert_comparison_func_flag(desc.comparison_func);
        sampler_desc.MinLOD = 0;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
        sampler_desc.MaxAnisotropy = 1;
        sampler_desc.MipLODBias = 0.0f;

        d3d11_rendering_device* d3d11_device = static_cast<d3d11_rendering_device*>(device);
        DXCALL(d3d11_device->_internal_device->CreateSamplerState(&sampler_desc, &_internal_sampler_state));
    }

}