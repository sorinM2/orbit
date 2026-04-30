#pragma once
#include "d3d11common.h"
#include "orbit/graphics/common/texture.h"

namespace orbit::graphics
{
    class d3d11_texture2D final : public texture2D
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;

    public:
        d3d11_texture2D(rendering_device* device, rendering_device_context* context, const texture2D_desc& desc);
        d3d11_texture2D(rendering_device* device, rendering_device_context* context, const texture2D_desc& desc, ID3D11Texture2D* texture);

        ~d3d11_texture2D() override { util::safe_release(_internal_texture); }

        ID3D11Texture2D* get_internal_texture() const { return _internal_texture; }
    private:
        ID3D11Texture2D* _internal_texture = nullptr;
    };
}