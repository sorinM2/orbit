#include "d3d11render_target.h"
#include "d3d11device.h"
#include "util.h"
#include "orbit/core/system.h"

namespace orbit::graphics
{
    d3d11_render_target::d3d11_render_target(rendering_device* device, rendering_device_context* context,
        const render_target_desc& desc, device_resource *resource)
        : device_resource(device, context), render_target(desc, resource)
    {
        if (  desc.type == render_target_type::texture2D )
        {
            d3d11_texture2D* d3d11_texture = dynamic_cast<d3d11_texture2D*>(resource);
            DXCALL(static_cast<d3d11_rendering_device*>(device)->_internal_device->CreateRenderTargetView(d3d11_texture->get_internal_texture(), nullptr, &_internal_rtv));
        } else __debugbreak();
    }

    d3d11_depth_stencil_state::d3d11_depth_stencil_state(rendering_device* device, rendering_device_context* context, const depth_stencil_state_desc& desc)
    : device_resource(device, context), depth_stencil_state(desc)
    {
        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;

        depth_stencil_desc.StencilEnable = desc.stencil_enabled;
        if ( desc.stencil_enabled )
        {
            depth_stencil_desc.BackFace.StencilFunc = d3d11::convert_comparison_func_flag(desc.back_face.stencil_func);
            depth_stencil_desc.FrontFace.StencilFunc = d3d11::convert_comparison_func_flag(desc.front_face.stencil_func);

            depth_stencil_desc.BackFace.StencilDepthFailOp = d3d11::convert_stencil_op(desc.back_face.stencil_depth_fail_op);
            depth_stencil_desc.BackFace.StencilFailOp = d3d11::convert_stencil_op(desc.back_face.stencil_fail_op);
            depth_stencil_desc.BackFace.StencilPassOp = d3d11::convert_stencil_op(desc.back_face.stencil_pass_op);

            depth_stencil_desc.FrontFace.StencilDepthFailOp = d3d11::convert_stencil_op(desc.front_face.stencil_depth_fail_op);
            depth_stencil_desc.FrontFace.StencilFailOp = d3d11::convert_stencil_op(desc.front_face.stencil_fail_op);
            depth_stencil_desc.FrontFace.StencilPassOp = d3d11::convert_stencil_op(desc.front_face.stencil_pass_op);

            depth_stencil_desc.StencilReadMask = 255;
            depth_stencil_desc.StencilWriteMask = 255;
        }

        depth_stencil_desc.DepthEnable = desc.depth_enabled;
        if ( desc.depth_enabled )
        {
            depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depth_stencil_desc.DepthFunc = d3d11::convert_comparison_func_flag(desc.depth_func);
        }
        DXCALL(static_cast<d3d11_rendering_device*>(device)->_internal_device->CreateDepthStencilState(&depth_stencil_desc, &_internal_depth_stencil_state));
    }

    d3d11_depth_stencil::d3d11_depth_stencil(rendering_device* device, rendering_device_context* context, const depth_stencil_desc& desc, device_resource *resource)
    : device_resource(device, context), depth_stencil(desc, resource)
    {
        if ( desc.type == depth_stencil_type::texture2D )
        {
            d3d11_texture2D* d3d11_texture = dynamic_cast<d3d11_texture2D*>(resource);
            texture2D_desc tex_desc = d3d11_texture->get_desc();

            D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
            ZeroMemory(&dsv_desc, sizeof(dsv_desc));
            dsv_desc.Format = d3d11::convert_format(tex_desc.format);
            dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

            DXCALL(static_cast<d3d11_rendering_device*>(device)->_internal_device->
                CreateDepthStencilView(d3d11_texture->get_internal_texture(), &dsv_desc, &_internal_depth_stencil_view));
        }
        else __debugbreak();
    }

    d3d11_swap_chain::d3d11_swap_chain(rendering_device* device, rendering_device_context* context, const swap_chain_desc& desc, IDXGIFactory7* factory)
        : device_resource(device, context), swap_chain(desc)
    {
        DXGI_SWAP_CHAIN_DESC sc_desc;
        ZeroMemory(&sc_desc, sizeof(sc_desc));
        sc_desc.BufferDesc.Format = d3d11::convert_format(desc.buffer_desc.format);
        sc_desc.BufferDesc.Width = desc.buffer_desc.width;
        sc_desc.BufferDesc.Height = desc.buffer_desc.height;
        sc_desc.BufferDesc.RefreshRate.Numerator = desc.vsync_enabled ? 0 : desc.buffer_desc.refresh_rate.numerator;
        sc_desc.BufferDesc.RefreshRate.Denominator = desc.vsync_enabled ? 1 : desc.buffer_desc.refresh_rate.denominator;
        sc_desc.BufferDesc.Scaling = d3d11::convert_scaling_mode(desc.buffer_desc.scaling);
        sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

        sc_desc.BufferCount = desc.buffer_count;
        sc_desc.BufferUsage = d3d11::convert_swap_chain_flags(desc.buffer_usage);
        sc_desc.OutputWindow = system::get_main_hwnd(); //TODO make a window class
        sc_desc.Windowed = desc.windowed;
        sc_desc.SampleDesc.Count = desc.sample_desc.count;
        sc_desc.SampleDesc.Quality = desc.sample_desc.quality;
        sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sc_desc.Flags = 0;

        ID3D11Device* i_device = static_cast<d3d11_rendering_device*>(device)->_internal_device;
        DXCALL(factory->CreateSwapChain(i_device, &sc_desc, &_internal_swap_chain));

        d3d11::name_com_object(_internal_swap_chain, "aaaaaaaa");
        ID3D11Texture2D* back_buffer = nullptr;
        DXCALL(_internal_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &back_buffer));
        texture2D_desc empty_desc;
        ZeroMemory(&empty_desc, sizeof(empty_desc));

        _buffer = new d3d11_texture2D(device, context, empty_desc, back_buffer);
        util::safe_release(back_buffer);

        render_target_desc rt_desc;
        ZeroMemory(&rt_desc, sizeof(rt_desc));
        rt_desc.type = render_target_type::texture2D;
        device->create_render_target(rt_desc, _buffer, &_render_target);

        framebuffer_desc fb_desc;
        ZeroMemory(&fb_desc, sizeof(fb_desc));
        fb_desc.count = 1;

        device->create_framebuffer(fb_desc, &_render_target, &_frame_buffer);

        _frame_buffer->set_depth_stencil(nullptr);
    }
}
