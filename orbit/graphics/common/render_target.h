#pragma once
#include "common.h"
#include "texture.h"

namespace orbit::graphics
{
    struct depth_stencil_op_desc
    {
        stencil_op stencil_fail_op;
        stencil_op stencil_depth_fail_op;
        stencil_op stencil_pass_op;
        comparison_func stencil_func;
    };

    struct depth_stencil_state_desc
    {
        bool depth_enabled;
        comparison_func depth_func;
        bool stencil_enabled;
        depth_stencil_op_desc front_face;
        depth_stencil_op_desc back_face;
    };

    class depth_stencil_state : public virtual device_resource
    {
    public:
        explicit depth_stencil_state(const depth_stencil_state_desc& desc) : device_resource(nullptr, nullptr), _desc(desc) {}
        ~depth_stencil_state() override {}

        depth_stencil_state_desc get_desc() const { return _desc; }
    protected:
        depth_stencil_state_desc _desc;
    };

    enum class depth_stencil_type
    {
        texture2D,
    };

    struct depth_stencil_desc
    {
        depth_stencil_type type;
    };

    class depth_stencil : public virtual device_resource
    {
    public:
        explicit depth_stencil(const depth_stencil_desc& desc) : device_resource(nullptr, nullptr), _desc(desc) {}
        ~depth_stencil() override {}

        depth_stencil_desc get_desc() const { return _desc; }
    protected:
        depth_stencil_desc _desc;
    };

    enum class render_target_type
    {
        texture2D,
    };

    struct render_target_desc
    {
        render_target_type type;
    };

    class render_target : public virtual device_resource
    {
    public:
        explicit render_target(const render_target_desc& desc, rendering_device* device = nullptr, rendering_device_context* context = nullptr )
        : device_resource(nullptr, nullptr), _desc(desc) {}
        ~render_target() override {}

        render_target_desc get_desc() const { return _desc; }
    protected:
        render_target_desc _desc;
    };

    struct framebuffer_desc
    {
        unsigned int count;
    };

    class framebuffer : public virtual device_resource
    {
    public:
        explicit framebuffer(render_target** targets, const framebuffer_desc& desc, rendering_device* device = nullptr, rendering_device_context* context = nullptr)
            : device_resource(device, context), _desc(desc)
        {
            _targets = new render_target*[desc.count];
            for ( int i = 0; i < desc.count; i++ )
                _targets[i] = targets[i];
        }

        render_target** get_targets() const { return _targets; }
        framebuffer_desc get_desc() const { return _desc; }
        void set_depth_stencil(depth_stencil* depth_stencil) { _depth_stencil = depth_stencil; }
        depth_stencil* get_depth_stencil() const { return _depth_stencil; }

    private:
        ~framebuffer() override { if (_targets) delete[] _targets; }

        render_target** _targets = nullptr;
        framebuffer_desc _desc{};
        depth_stencil* _depth_stencil = nullptr;
    };

    struct swap_chain_desc
    {
        mode_desc buffer_desc;
        sample_desc sample_desc;
        unsigned int buffer_usage;
        unsigned int buffer_count;
        bool windowed;
        bool vsync_enabled;
    };

    class swap_chain : public virtual device_resource
    {
    public:
        swap_chain(const swap_chain_desc& desc, rendering_device* device = nullptr, rendering_device_context* context = nullptr)
            : device_resource(nullptr, nullptr), _desc(desc) {}

        ~swap_chain() override { util::safe_release(_frame_buffer); util::safe_release(_render_target);};
        framebuffer* get_framebuffer() const { return _frame_buffer; }
        swap_chain_desc get_desc() const { return _desc; }

    protected:
        swap_chain_desc _desc{};
        framebuffer* _frame_buffer = nullptr;
        render_target* _render_target = nullptr;

    };
}