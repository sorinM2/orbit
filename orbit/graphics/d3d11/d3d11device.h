#pragma once
#include "orbit/graphics/common/device.h"
#include <d3d11.h>
#include "d3d11buffer.h"
#include "d3d11sampler.h"
#include "d3d11rasterizer.h"
#include "d3d11texture.h"
#include "d3d11render_target.h"
#include "d3d11shader.h"

#include "orbit/utility/vector.h"

namespace orbit::graphics
{
    class d3d11_rendering_device;
    class d3d11_rendering_device_context;
    class d3d11_factory;

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
        friend class d3d11_shader;

    public:
        explicit d3d11_rendering_device(d3d11_factory* factory, rendering_device_context** context);

        void create_buffer(const buffer_desc& desc, buffer** buffer) override;
        void create_framebuffer(const framebuffer_desc& desc, render_target** targets, framebuffer** framebuffer) override;

        void create_sampler(const sampler_desc& desc, sampler** sampler) override;
        void create_rasterizer(const rasterizer_desc& desc, rasterizer** rasterizer) override;
        void create_texture2D(const texture2D_desc& desc, texture2D** texture) override;
        void create_texture2D(const texture2D_desc& desc, texture2D** texture, ID3D11Texture2D* d3d11_texture);
        void create_shader_resource(const shader_resource_desc& desc, device_resource* resource, shader_resource** shader_resource) override;
        void create_render_target(const render_target_desc& desc, device_resource* resource, render_target** render_target) override;
        void create_depth_stencil(const depth_stencil_desc& desc, device_resource* resource, depth_stencil** depth_stencil) override;
        void create_depth_stencil_state(const depth_stencil_state_desc& desc, depth_stencil_state** depth_stencil_state) override;
        void create_swap_chain(const swap_chain_desc& desc, swap_chain** swap_chain) override;
        void create_swap_chain(const swap_chain_desc& desc, swap_chain** swap_chain, IDXGIFactory7* factory);
        void create_shader(const shader_desc& desc, shader** shader) override;
        void create_program(const program_desc& desc, program** program) override;

        ~d3d11_rendering_device() override
        {
            util::safe_release(_context);
#ifdef _DEBUG
            ID3D11Debug* debug_controller;
            _internal_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug_controller);
            debug_controller->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
            util::safe_release(debug_controller);
#endif // _DEBUG
            util::safe_release(_internal_device);
        }

        ID3D11Device* get_internal() const { return _internal_device; }
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
        friend class d3d11_shader;

    public:
        explicit d3d11_rendering_device_context(ID3D11DeviceContext* context) : _internal_device_context(context) {}

        void clear_render_target(render_target* target) override;
        void clear_depth_stencil(depth_stencil* depth_stencil) override;

        void set_vertex_buffers( unsigned int num_buffers, unsigned int* strides, buffer** buffers) override;
        void set_index_buffer(buffer* buffer) override;
        void draw_indexed(unsigned int no_indices) override;

        void set_rasterizer(rasterizer* rasterizer) override;
        void set_viewports(viewport* viewports, unsigned int count) override;
        void set_scissors(rect* rects, unsigned int count) override;
        void set_frame_buffer(framebuffer* framebuffer) override;

        void set_samplers(sampler** samplers, unsigned int count, unsigned int start_slot = 0) override;
        void vs_set_constant_buffers(buffer** buffer, unsigned int count, unsigned int slot) override;
        void ps_set_constant_buffers(buffer** buffer, unsigned int count, unsigned int slot) override;
        void ps_set_shader_resources(shader_resource** srs, unsigned int count, unsigned int slot) override;
        void set_depth_stencil_state(depth_stencil_state* dss) override;

        void set_program(program* program) override;

        ~d3d11_rendering_device_context() override { util::safe_release(_internal_device_context);}

        //TODO own the actual internal context and release it in the destructor
        ID3D11DeviceContext* get_internal() const { return _internal_device_context; }

    private:
        ID3D11DeviceContext* _internal_device_context = nullptr;
    };

    class d3d11_factory final : public factory
    {
    public:

        d3d11_factory();
        ~d3d11_factory() override { util::safe_release(_internal_factory); util::safe_release(_internal_adapter); }

        void create_device_and_context(rendering_device** device, rendering_device_context** context) override;

        void create_swap_chain(const swap_chain_desc& desc, rendering_device* device, rendering_device_context* context, swap_chain** swap_chain) override;

        IDXGIFactory7* get_internal_factory() const { return _internal_factory; }
        IDXGIAdapter4* get_internal_adapter() const { return _internal_adapter; }
    private:
        IDXGIFactory7* _internal_factory = nullptr;
        IDXGIAdapter4* _internal_adapter = nullptr;
    };
}
