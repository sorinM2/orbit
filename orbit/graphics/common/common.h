#pragma once
#include "util.h"

namespace orbit::graphics
{
    class rendering_device;
    class rendering_device_context;

    class device_resource : public virtual ref_counted
    {
    protected:
        ~device_resource() override;
        device_resource(rendering_device* owner, rendering_device_context* context) : _owner(owner), _context(context) {}

        rendering_device* get_device() const { return _owner; }
        rendering_device_context* get_context() const { return _context; }
    private:
        rendering_device* _owner = nullptr;
        rendering_device_context* _context = nullptr;
    };

    enum class comparison_func
    {
        comparison_func_never,
        comparison_func_less,
        comparison_func_equal,
        comparison_func_less_equal,
        comparison_func_greater,
        comparison_func_not_equal,
        comparison_func_greater_equal,
        comparison_func_always,
    };

    enum class format
    {
        FORMAT_R32G32B32A32_FLOAT,
        FORMAT_R32G32B32A32_UINT,
        FORMAT_R32G32B32A32_SINT,

        FORMAT_R32G32B32_FLOAT,
        FORMAT_R32G32B32_UINT,
        FORMAT_R32G32B32_SINT,

        FORMAT_R32G32_FLOAT,
        FORMAT_R32G32_UINT,
        FORMAT_R32G32_SINT,

        FORMAT_R32_FLOAT,
        FORMAT_R32_UINT,
        FORMAT_R32_SINT,

        FORMAT_R8G8B8A8_UNORM,
        FORMAT_R8G8B8A8_UINT,
        FORMAT_R8G8B8A8_SINT,

        FORMAT_R8G8_UNORM,
        FORMAT_R8G8_UINT,
        FORMAT_R8G8_SINT,

        FORMAT_R8_UNORM,
        FORMAT_R8_UINT,
        FORMAT_R8_SINT,

        FORMAT_D24_UNORM_S8_UINT
    };

    enum class map_type
    {
        map_type_read,
        map_type_write,
        map_type_read_write,
    };

    enum class resource_usage
    {
        resource_default_usage,
        resource_constant_usage,
        resource_dynamic_usage
    };

    enum swap_chain_resource_usage : unsigned int
    {
        back_buffer = 1 << 0,
        discard_on_present = 1 << 1,
        read_only = 1 << 2,
        render_target_output = 1 << 3,
        shader_input = 1 << 4,
        shared = 1 << 5,
        unordered_access = 1 << 6
    };

    enum cpu_access_flags : unsigned int
    {
        cpu_access_flags_read = 1 << 0,
        cpu_access_flags_write = 1 << 1
    };

    enum bind_flags : unsigned int
    {
        bind_flag_vertex_buffer = 1 << 0,
        bind_flag_index_buffer = 1 << 1,
        bind_flag_constant_buffer = 1 << 2,
        bind_flag_shader_resource = 1 << 3,
        bind_flag_render_target = 1 << 4,
        bind_flag_depth_stencil = 1 << 5
    };

    enum class texture_address_mode
    {
        texture_address_repeat,
        texture_address_mirror,
        texture_address_clamp,
        texture_address_border,
    };

    enum class fill_mode
    {
        solid,
        wireframe
    };

    enum class cull_mode
    {
        none,
        front,
        back
    };

    enum class stencil_op
    {
        keep,
        zero,
        replace,
        incr,
        incr_wrap,
        decr,
        decr_wrap,
        invert
    };

    enum class scaling_mode
    {
        unspecified,
        centered,
        stretched
    };

    struct sample_desc
    {
        unsigned int count;
        unsigned int quality;
    };

    struct rect
    {
        int left;
        int right;
        int bottom;
        int top;
    };

    struct rational
    {
        unsigned int numerator;
        unsigned int denominator;
    };

    struct mode_desc
    {
        unsigned int width;
        unsigned int height;
        rational refresh_rate;
        format format;
        scaling_mode scaling;
    };
}