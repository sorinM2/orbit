#include "d3d11device.h"

#include "d3d11sampler.h"
#include "d3d11shader_resource.h"
#include "orbit/utility/vector.h"

namespace orbit::graphics
{
    void d3d11_rendering_device::create_buffer(const buffer_desc& desc, buffer** buffer)
    {
        *buffer =  new d3d11_buffer(this, _context, desc);
    }

    void d3d11_rendering_device_context::vs_set_constant_buffers(buffer** buffers, unsigned int count, unsigned int slot)
    {
        if ( !count )
        {
            _internal_device_context->VSSetConstantBuffers(0, 0, nullptr);
            return;
        }

        utl::vector<ID3D11Buffer*> d3d11_buffers;
        for ( unsigned int i = 0; i < count; ++i )
            d3d11_buffers.emplace_back(static_cast<const d3d11_buffer* const>(buffers[i])->_buffer);

        _internal_device_context->VSSetConstantBuffers(slot, count, d3d11_buffers.data());

    }

    void d3d11_rendering_device_context::ps_set_shader_resources(shader_resource** srs, unsigned int count, unsigned int slot)
    {
        if ( !count )
        {
            _internal_device_context->PSSetShaderResources(0, 0, nullptr);
            return;
        }

        utl::vector<ID3D11ShaderResourceView*> d3d11_srvs;
        for ( unsigned int i = 0; i < count; ++i )
            d3d11_srvs.emplace_back(static_cast<const d3d11_shader_resource* const>(srs[i])->_internal_shader_resource_view);

        _internal_device_context->PSSetShaderResources(slot, count, d3d11_srvs.data());
    }


    void d3d11_rendering_device::create_sampler(const sampler_desc& desc, sampler** sampler)
    {
        *sampler = new d3d11_sampler(this, _context, desc);
    }

    void d3d11_rendering_device_context::set_samplers(sampler** samplers, unsigned int count, unsigned int start_slot)
    {
        if ( !count )
        {
            _internal_device_context->PSSetSamplers(0, 0, nullptr);
            return;
        }

        utl::vector<ID3D11SamplerState*> d3d11_samplers;
        for ( unsigned int i = 0; i < count; ++i )
            d3d11_samplers.emplace_back(static_cast<d3d11_sampler*>(samplers[i])->_internal_sampler_state);

        _internal_device_context->PSSetSamplers(0, count, d3d11_samplers.data());
    }

    void d3d11_rendering_device::create_rasterizer(const rasterizer_desc& desc, rasterizer** rasterizer)
    {
        *rasterizer = new d3d11_rasterizer(this, _context, desc);
    }

    void d3d11_rendering_device_context::set_rasterizer(rasterizer* rasterizer)
    {
        if ( !rasterizer )
        {
            _internal_device_context->RSSetState(nullptr);
            return;
        }

        d3d11_rasterizer* _d3d11_rasterizer = static_cast<d3d11_rasterizer*>(rasterizer);
        _internal_device_context->RSSetState(_d3d11_rasterizer->_internal_rasterizer_state);
    }

    void d3d11_rendering_device::create_texture2D(const texture2D_desc& desc, texture2D** texture)
    {
        *texture = new d3d11_texture2D(this, _context, desc);
    }

    void d3d11_rendering_device::create_shader_resource(const shader_resource_desc& desc, device_resource* resource, shader_resource** shader_resource)
    {
        *shader_resource = new d3d11_shader_resource(this, _context, desc, resource);
    }

    void d3d11_rendering_device_context::set_viewports(viewport* viewports, unsigned int count)
    {
        utl::vector<D3D11_VIEWPORT> d3d11_viewports;
        _viewports.clear();

        if ( !count )
        {
            _internal_device_context->RSSetViewports(0, nullptr);
            return;
        }

        for ( int i = 0; i < count; ++i )
        {
            _viewports.emplace_back(viewports[i]);

            D3D11_VIEWPORT d3d11_viewport;

            d3d11_viewport.MinDepth = 0.f;
            d3d11_viewport.MaxDepth = 1.f;
            d3d11_viewport.TopLeftX = viewports[i].x;
            d3d11_viewport.TopLeftY = viewports[i].y;
            d3d11_viewport.Width = viewports[i].width;
            d3d11_viewport.Height = viewports[i].height;

            d3d11_viewports.emplace_back(d3d11_viewport);
        }

        _internal_device_context->RSSetViewports(count, d3d11_viewports.data());
    }

    void d3d11_rendering_device_context::set_scissors(rect* rects, unsigned int count)
    {
        utl::vector<D3D11_RECT> d3d11_scissors;
        _scissors.clear();

        if ( !count )
        {
            _internal_device_context->RSSetScissorRects(0, nullptr);
            return;
        }

        for ( int i = 0; i < count; ++i )
        {
            _scissors.emplace_back(rects[i]);

            D3D11_RECT d3d11_scissor;

            d3d11_scissor.left = rects[i].left;
            d3d11_scissor.right = rects[i].right;
            d3d11_scissor.bottom = rects[i].bottom;
            d3d11_scissor.top = rects[i].top;

            d3d11_scissors.emplace_back(d3d11_scissor);
        }

        _internal_device_context->RSSetScissorRects(count, d3d11_scissors.data());
    }

    void rendering_device::create_render_target(const render_target_desc& desc, device_resource* resource, render_target** render_target)
    {
        *render_target = new d3d11_render_target(this, _context, desc, resource);
    }

    void d3d11_rendering_device::create_render_target(const render_target_desc& desc, device_resource* resource, render_target** render_target)
    {
        *render_target = new d3d11_render_target(this, _context, desc, resource);
    }

    void d3d11_rendering_device::create_depth_stencil(const depth_stencil_desc& desc, device_resource* resource, depth_stencil** depth_stencil)
    {
        *depth_stencil = new d3d11_depth_stencil(this, _context, desc, resource);
    }

    void d3d11_rendering_device::create_depth_stencil_state(const depth_stencil_state_desc& desc, depth_stencil_state** depth_stencil_state)
    {
        *depth_stencil_state = new d3d11_depth_stencil_state(this, _context, desc);
    }

    void d3d11_rendering_device_context::set_depth_stencil_state(depth_stencil_state* dss)
    {
        d3d11_depth_stencil_state* state = static_cast<d3d11_depth_stencil_state*>(dss);
        _internal_device_context->OMSetDepthStencilState(state->_internal_depth_stencil_state, 1);
    }

    void d3d11_rendering_device::create_swap_chain(const swap_chain_desc& desc, swap_chain** swap_chain){}

    void d3d11_rendering_device::create_swap_chain(const swap_chain_desc& desc, swap_chain** swap_chain, IDXGIFactory7* factory)
    {
        *swap_chain = new d3d11_swap_chain(this, _context, desc, factory);
    }

    void d3d11_rendering_device::create_framebuffer(const framebuffer_desc& desc, render_target** targets, framebuffer** frame_buffer)
    {
        *frame_buffer = new framebuffer(targets, desc, this, _context);
    }

    void d3d11_rendering_device_context::set_frame_buffer(framebuffer* framebuffer)
    {
        framebuffer_desc desc = framebuffer->get_desc();
        if ( !desc.count )
        {
            _internal_device_context->OMSetRenderTargets(0, nullptr, nullptr);
            return;
        }

        render_target** targets = framebuffer->get_targets();
        utl::vector<ID3D11RenderTargetView*> rtvs;
        for ( int i = 0; i < desc.count; ++i )
            rtvs.emplace_back(dynamic_cast<d3d11_render_target*>(targets[i])->_internal_rtv);

        depth_stencil* depth_stencil = framebuffer->get_depth_stencil();

        d3d11_depth_stencil* d3d11_ds = dynamic_cast<d3d11_depth_stencil*>(depth_stencil);
        ID3D11DepthStencilView* dsv = d3d11_ds ? d3d11_ds->_internal_depth_stencil_view : nullptr;

        _internal_device_context->OMSetRenderTargets(desc.count, rtvs.data(), dsv);
    }

    void d3d11_rendering_device_context::clear_render_target(render_target* target)
    {
        ID3D11RenderTargetView* view = dynamic_cast<d3d11_render_target*>(target)->_internal_rtv;

        float color[] = {0.f, 0.5f, 0.8f, 1.f};
        _internal_device_context->ClearRenderTargetView(view, color);
    }

    void d3d11_rendering_device_context::clear_depth_stencil(depth_stencil* depth_stencil)
    {
        ID3D11DepthStencilView* view = dynamic_cast<d3d11_depth_stencil*>(depth_stencil)->_internal_depth_stencil_view;

        _internal_device_context->ClearDepthStencilView(view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    }


}
