#pragma once
#include "d3d11common.h"
#include "orbit/graphics/common/render_target.h"

namespace orbit::graphics
{
    class d3d11_depth_stencil_state final : public depth_stencil_state
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;
    public:
        d3d11_depth_stencil_state(rendering_device* device, rendering_device_context* context, const depth_stencil_state_desc& desc);
        ~d3d11_depth_stencil_state() override { util::safe_release(_internal_depth_stencil_state); };
    private:
        ID3D11DepthStencilState* _internal_depth_stencil_state = nullptr;
    };

    class d3d11_depth_stencil final : public depth_stencil
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;
    public:
        d3d11_depth_stencil(rendering_device* device, rendering_device_context* context, const depth_stencil_desc& desc, device_resource *resource);
        ~d3d11_depth_stencil() override { util::safe_release(_internal_depth_stencil_view); };

    private:
        ID3D11DepthStencilView* _internal_depth_stencil_view = nullptr;
    };

    class d3d11_render_target final : public render_target
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;
    public:
        d3d11_render_target(rendering_device* device, rendering_device_context* context, const render_target_desc& desc, device_resource *resource);
        ~d3d11_render_target() override { util::safe_release(_internal_rtv); };

    private:
        ID3D11RenderTargetView* _internal_rtv = nullptr;
    };

    class d3d11_swap_chain final : public swap_chain
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;
    public:
        d3d11_swap_chain(rendering_device* device, rendering_device_context* context, const swap_chain_desc& desc, IDXGIFactory7* factory);
        ~d3d11_swap_chain() override { util::safe_release(_internal_swap_chain); };

        IDXGISwapChain* get_internal() const { return _internal_swap_chain; };
    private:
        IDXGISwapChain* _internal_swap_chain = nullptr;
    };
}