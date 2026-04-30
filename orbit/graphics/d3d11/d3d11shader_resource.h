#pragma once
#include "d3d11common.h"
#include "orbit/graphics/common/shader_resource.h"

namespace orbit::graphics
{
    class d3d11_shader_resource final : public shader_resource
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;
    public:
        d3d11_shader_resource(rendering_device* device, rendering_device_context* context, const shader_resource_desc& desc, device_resource* resource);
        ~d3d11_shader_resource() override { util::safe_release(_internal_shader_resource_view); }

        ID3D11ShaderResourceView* get_internal() const { return _internal_shader_resource_view;}
    private:
        ID3D11ShaderResourceView* _internal_shader_resource_view = nullptr;
    };
}