#pragma once
#include "orbit/graphics/common/device.h"
#include <d3d11.h>
#include "d3d11buffer.h"
#include "d3d11sampler.h"
#include "d3d11rasterizer.h"
#include "d3d11texture.h"
#include "d3d11render_target.h"

#include "orbit/utility/vector.h"

namespace orbit::graphics
{
    class d3d11_rendering_device_context;

    class d3d11_rendering_device final : public rendering_device
    {
        friend class d3d11_buffer;
        friend class d3d11_sampler;
        friend class d3d11_rasterizer;
        friend class d3d11_texture2D;
        friend class d3d11_shader_resource;
        friend class d3d11_render_target;
        friend class d3d11_depth_stencil;
        friend class d3d11_depth_stencil_state;
        friend class d3d11_swap_chain;

    public:
        explicit d3d11_rendering_device(ID3D11Device* device) : _internal_device(device){}

        void create_buffer(const buffer_desc& desc, buffer** buffer) override;
        void create_framebuffer(const framebuffer_desc& desc, render_target** targets, framebuffer** framebuffer) override;

        void create_sampler(const sampler_desc& desc, sampler** sampler) override;
        void create_rasterizer(const rasterizer_desc& desc, rasterizer** rasterizer) override;
        void create_texture2D(const texture2D_desc& desc, texture2D** texture) override;
        void create_shader_resource(const shader_resource_desc& desc, device_resource* resource, shader_resource** shader_resource) override;
        void create_render_target(const render_target_desc& desc, device_resource* resource, render_target** render_target) override;
        void create_depth_stencil(const depth_stencil_desc& desc, device_resource* resource, depth_stencil** depth_stencil) override;
        void create_depth_stencil_state(const depth_stencil_state_desc& desc, depth_stencil_state** depth_stencil_state) override;
        void create_swap_chain(const swap_chain_desc& desc, swap_chain** swap_chain) override;
        void create_swap_chain(const swap_chain_desc& desc, swap_chain** swap_chain, IDXGIFactory7* factory);

        void create_vertex_shader() override{}
        void create_pixel_shader() override{}
        void create_swap_chain() override{}

        ~d3d11_rendering_device() override{}

    private:
        ID3D11Device* _internal_device = nullptr;
    };

    class d3d11_rendering_device_context final : public rendering_device_context
    {
        friend class d3d11_buffer;
        friend class d3d11_sampler;
        friend class d3d11_rasterizer;
        friend class d3d11_texture2D;
        friend class d3d11_shader_resource;
        friend class d3d11_render_target;
        friend class d3d11_depth_stencil;
        friend class d3d11_depth_stencil_state;
        friend class d3d11_swap_chain;

    public:
        explicit d3d11_rendering_device_context(ID3D11DeviceContext* internal) : _internal_device_context(internal){}


        void clear_render_target(render_target* target) override;
        void clear_depth_stencil(depth_stencil* depth_stencil) override;
        void set_vertex_buffers( unsigned int num_buffers, buffer** buffers) override{}
        void set_index_buffer(buffer* buffer) override{}
        void set_input_layout(input_layout* layout) override{}
        void set_vertex_shader(vertex_shader* shader) override{}
        void set_pixel_shader(pixel_shader* shader) override{}
        void set_rasterizer(rasterizer* rasterizer) override;
        void set_viewports(viewport* viewports, unsigned int count) override;
        void set_scissors(rect* rects, unsigned int count) override;
        void set_frame_buffer(framebuffer* framebuffer) override;

        void set_samplers(sampler** samplers, unsigned int count, unsigned int start_slot = 0) override;
        void vs_set_constant_buffers(buffer** buffer, unsigned int count, unsigned int slot) override;
        void ps_set_shader_resources(shader_resource** srs, unsigned int count, unsigned int slot) override;
        void set_depth_stencil_state(depth_stencil_state* dss) override;

        ~d3d11_rendering_device_context() override {}

        //TODO own the actual internal context and release it in the destructor

    private:
        ID3D11DeviceContext* _internal_device_context = nullptr;
    };
}
