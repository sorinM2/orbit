#pragma once
#include "d3d11common.h"
#include "orbit/graphics/common/shader.h"

namespace orbit::graphics
{
    class d3d11_shader final : public shader
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;

        d3d11_shader(const shader_desc& desc, rendering_device* device, rendering_device_context* context);
        ~d3d11_shader() override;

        ID3D11VertexShader* _internal_vertex_shader = nullptr;
        ID3D11PixelShader* _internal_pixel_shader = nullptr;
        ID3D11InputLayout* _internal_layout = nullptr;
    };
}