#include "d3d11device.h"

#include <ranges>

#include "d3d11sampler.h"
#include "d3d11shader_resource.h"
#include "orbit/utility/vector.h"

namespace orbit::graphics
{
    using idxgi_factory = IDXGIFactory7;
    using idxgi_adapter = IDXGIAdapter4;

    namespace
    {
        const D3D_FEATURE_LEVEL supported_feature_levels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };

        idxgi_adapter* get_best_adapter(idxgi_factory* dxgi_factory)
        {
            int adapter_index = 0, best_index = 0, best_index_feature_level = -1;
            idxgi_adapter* adapter = nullptr;

            while (dxgi_factory->EnumAdapters1(adapter_index, (IDXGIAdapter1**)&adapter) != DXGI_ERROR_NOT_FOUND)
            {
                DXGI_ADAPTER_DESC3 adapter_desc;
                DXCALL(adapter->GetDesc3(&adapter_desc));

                D3D_FEATURE_LEVEL maximum_feature_level;

                DXCALL(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, supported_feature_levels, _countof(supported_feature_levels), D3D11_SDK_VERSION, NULL, &maximum_feature_level, NULL));

                for ( unsigned int i = 0; i < _countof(supported_feature_levels); ++i )
                    if (supported_feature_levels[i] == maximum_feature_level and i > best_index_feature_level)
                    {
                        best_index_feature_level = i;
                        best_index = adapter_index;
                    }

                adapter->Release();
                adapter = nullptr;

                ++adapter_index;
            }

            if (adapter_index == 0)
                return nullptr;

            DXCALL(dxgi_factory->EnumAdapters1(best_index, (IDXGIAdapter1**)&adapter));

            return adapter;
        }
    }

    d3d11_factory::d3d11_factory()
    {
        DXCALL(CreateDXGIFactory1(__uuidof(idxgi_factory), (void**)&_internal_factory));
        _internal_adapter = get_best_adapter(_internal_factory);
    }

    d3d11_rendering_device::d3d11_rendering_device(d3d11_factory* factory, rendering_device_context** context)
    {
        unsigned int creation_flags = 0;

#ifdef _DEBUG
        creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        ID3D11DeviceContext* internal_device_context;
        DXCALL(D3D11CreateDevice(factory->get_internal_adapter(), D3D_DRIVER_TYPE_UNKNOWN, NULL, creation_flags, supported_feature_levels,
            _countof(supported_feature_levels), D3D11_SDK_VERSION, &_internal_device, nullptr, &internal_device_context));

        *context = new d3d11_rendering_device_context(internal_device_context);
        _context = *context;
        _context->add_ref();

#ifdef _DEBUG
        ID3D11InfoQueue* info_queue = nullptr;
        DXCALL(_internal_device->QueryInterface(IID_PPV_ARGS(&info_queue)));
        info_queue->GetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR);

        util::safe_release(info_queue);
#endif
    }

    void d3d11_factory::create_device_and_context(rendering_device** device, rendering_device_context** context)
    {
        *device = new d3d11_rendering_device(this, context);
    }

    void d3d11_factory::create_swap_chain(const swap_chain_desc& desc, rendering_device* device, rendering_device_context* context, swap_chain** swap_chain)
    {
        *swap_chain = new d3d11_swap_chain(device, context, desc, _internal_factory);
    }

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

    void d3d11_rendering_device_context::ps_set_constant_buffers(buffer** buffers, unsigned int count, unsigned int slot)
    {
        if ( !count )
        {
            _internal_device_context->PSSetConstantBuffers(0, 0, nullptr);
            return;
        }

        utl::vector<ID3D11Buffer*> d3d11_buffers;
        for ( unsigned int i = 0; i < count; ++i )
            d3d11_buffers.emplace_back(static_cast<const d3d11_buffer* const>(buffers[i])->_buffer);
        _internal_device_context->PSSetConstantBuffers(slot, count, d3d11_buffers.data());
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

    void d3d11_rendering_device::create_texture2D(const texture2D_desc& desc, texture2D** texture, ID3D11Texture2D* d3d11_texture)
    {
        *texture = new d3d11_texture2D(this, _context, desc, d3d11_texture);
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

    void d3d11_rendering_device::create_shader(const shader_desc& desc, shader** shader)
    {
        *shader = new d3d11_shader(desc, this, _context);
    }

    void d3d11_rendering_device::create_program(const program_desc& desc, program** program)
    {
        *program = new class program(desc, this, _context);
    }

    void d3d11_rendering_device_context::set_program(program* program)
    {
        program_desc desc = program->get_desc();

        d3d11_shader* vertex = dynamic_cast<d3d11_shader*>(desc.vertex_shader);
        d3d11_shader* pixel = dynamic_cast<d3d11_shader*>(desc.pixel_shader);

        if ( vertex )
        {
            _internal_device_context->IASetInputLayout(vertex->_internal_layout);
            _internal_device_context->VSSetShader(vertex->_internal_vertex_shader, nullptr, 0);
        }
        if ( pixel )
            _internal_device_context->PSSetShader(pixel->_internal_pixel_shader, nullptr, 0);
    }

    void d3d11_rendering_device_context::set_vertex_buffers( unsigned int num_buffers, unsigned int* strides, buffer** buffers)
    {
        utl::vector<ID3D11Buffer*> v_buffers;
        for ( int i = 0; i < num_buffers; ++i )
            v_buffers.emplace_back(dynamic_cast<d3d11_buffer*>(buffers[i])->_buffer);

        unsigned int offset = 0;
        _internal_device_context->IASetVertexBuffers(0, v_buffers.size(), v_buffers.data(), strides, &offset);
    }

    void d3d11_rendering_device_context::set_index_buffer(buffer* buffer)
    {
        d3d11_buffer* ib = dynamic_cast<d3d11_buffer*>(buffer);
        _internal_device_context->IASetIndexBuffer(ib->_buffer, DXGI_FORMAT_R32_UINT, 0);
        _internal_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void d3d11_rendering_device_context::draw_indexed(unsigned int no_indices)
    {
        _internal_device_context->DrawIndexed(no_indices, 0, 0);
    }
}
