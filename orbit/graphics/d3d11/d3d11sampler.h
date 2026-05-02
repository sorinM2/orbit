#pragma once
#include "d3d11common.h"
#include "orbit/graphics/common/sampler.h"

namespace orbit::graphics
{
    class d3d11_sampler final : public sampler
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;

    private:
        d3d11_sampler(rendering_device* device, rendering_device_context* context, const sampler_desc& desc);
        ~d3d11_sampler() override { util::safe_release(_internal_sampler_state); }

        ID3D11SamplerState* _internal_sampler_state = nullptr;
    };
}