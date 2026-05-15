#pragma once
#include <d3d11.h>
#include <cstdint>

#include  "orbit/graphics/common/common.h"

namespace orbit::graphics::d3d11
{

    inline unsigned int convert_bind_flags(unsigned int bind_flags)
    {
        unsigned int result = 0;

        if ( bind_flags & bind_flag_vertex_buffer )
            result |= D3D11_BIND_VERTEX_BUFFER;
        if ( bind_flags & bind_flag_index_buffer )
            result |= D3D11_BIND_INDEX_BUFFER;
        if ( bind_flags & bind_flag_constant_buffer )
            result |= D3D11_BIND_CONSTANT_BUFFER;
        if ( bind_flags & bind_flag_shader_resource )
            result |= D3D11_BIND_SHADER_RESOURCE;
        if ( bind_flags & bind_flag_render_target )
            result |= D3D11_BIND_RENDER_TARGET;
        if ( bind_flags & bind_flag_depth_stencil )
            result |= D3D11_BIND_DEPTH_STENCIL;

        return result;
    }

    inline unsigned int convert_cpu_access_flags(unsigned int flags)
    {
        unsigned int result = 0;
        if ( flags & cpu_access_flags_read )
            result |= D3D11_CPU_ACCESS_READ;
        if ( flags & cpu_access_flags_write )
            result |= D3D11_CPU_ACCESS_WRITE;
        return result;
    }

    inline unsigned int convert_swap_chain_flags(unsigned int flags)
    {
        unsigned int result = 0;
        if ( flags & back_buffer )
            result |= DXGI_USAGE_BACK_BUFFER;
        if ( flags & discard_on_present )
            result |= DXGI_USAGE_DISCARD_ON_PRESENT;
        if ( flags & read_only )
            result |= DXGI_USAGE_READ_ONLY;
        if ( flags & render_target_output )
            result |= DXGI_USAGE_RENDER_TARGET_OUTPUT;
        if ( flags & shader_input )
            result |= DXGI_USAGE_SHADER_INPUT;
        if ( flags & shared )
            result |= DXGI_USAGE_SHARED;
        if ( flags & discard_on_present )
            result |= DXGI_USAGE_UNORDERED_ACCESS;

        return result;
    }
    inline D3D11_USAGE convert_resource_usage_flag(resource_usage flag)
    {
        switch (flag)
        {
        case resource_usage::resource_constant_usage:
            return D3D11_USAGE_IMMUTABLE;
        case resource_usage::resource_default_usage:
            return D3D11_USAGE_DEFAULT;
        case resource_usage::resource_dynamic_usage:
            return D3D11_USAGE_DYNAMIC;
        default:
            __debugbreak();
            return D3D11_USAGE_DEFAULT;
        }
    }

    inline D3D11_MAP convert_map_type_flag(map_type flag)
    {
        if ( flag == map_type::map_type_read )
            return D3D11_MAP_READ;
        if ( flag == map_type::map_type_write )
            return D3D11_MAP_WRITE_DISCARD;
        if ( flag == map_type::map_type_read_write )
            return D3D11_MAP_READ_WRITE;

        return D3D11_MAP_READ_WRITE;
    }

    inline D3D11_TEXTURE_ADDRESS_MODE convert_texture_address_mode_flag(texture_address_mode flag)
    {
        if ( flag == texture_address_mode::texture_address_repeat )
            return D3D11_TEXTURE_ADDRESS_WRAP;
        if ( flag == texture_address_mode::texture_address_mirror )
            return D3D11_TEXTURE_ADDRESS_MIRROR;
        if ( flag == texture_address_mode::texture_address_clamp )
            return D3D11_TEXTURE_ADDRESS_CLAMP;
        if ( flag == texture_address_mode::texture_address_border )
            return D3D11_TEXTURE_ADDRESS_BORDER;

        return D3D11_TEXTURE_ADDRESS_WRAP;
    }

    inline D3D11_COMPARISON_FUNC convert_comparison_func_flag(comparison_func flag)
    {
        if ( flag == comparison_func::comparison_func_never )
            return D3D11_COMPARISON_NEVER;
        if ( flag == comparison_func::comparison_func_less )
            return D3D11_COMPARISON_LESS;
        if ( flag == comparison_func::comparison_func_greater )
            return D3D11_COMPARISON_GREATER;
        if ( flag == comparison_func::comparison_func_less_equal )
            return D3D11_COMPARISON_LESS_EQUAL;
        if ( flag == comparison_func::comparison_func_greater_equal )
            return D3D11_COMPARISON_GREATER_EQUAL;
        if ( flag == comparison_func::comparison_func_equal )
            return D3D11_COMPARISON_EQUAL;
        if ( flag == comparison_func::comparison_func_not_equal )
            return D3D11_COMPARISON_NOT_EQUAL;
        if ( flag == comparison_func::comparison_func_always)
            return D3D11_COMPARISON_ALWAYS;

        return D3D11_COMPARISON_LESS;
    }

    inline D3D11_FILL_MODE convert_fill_mode_flag(fill_mode flag)
    {
        if ( flag == fill_mode::solid )
            return D3D11_FILL_SOLID;
        if ( flag == fill_mode::wireframe )
            return D3D11_FILL_WIREFRAME;

        return D3D11_FILL_SOLID;
    }

    inline D3D11_CULL_MODE convert_cull_mode_flag(cull_mode flag)
    {
        if ( flag == cull_mode::none)
            return D3D11_CULL_NONE;
        if ( flag == cull_mode::back )
            return D3D11_CULL_BACK;
        if ( flag == cull_mode::front )
            return D3D11_CULL_FRONT;

        return D3D11_CULL_NONE;
    }

    inline D3D11_STENCIL_OP convert_stencil_op(stencil_op op)
    {
        if ( op == stencil_op::decr )
            return D3D11_STENCIL_OP_DECR;
        if ( op == stencil_op::decr_wrap )
            return D3D11_STENCIL_OP_DECR_SAT;
        if ( op == stencil_op::incr )
            return D3D11_STENCIL_OP_INCR;
        if ( op == stencil_op::incr_wrap )
            return D3D11_STENCIL_OP_INCR_SAT;
        if ( op == stencil_op::invert )
            return D3D11_STENCIL_OP_INVERT;
        if ( op == stencil_op::keep )
            return D3D11_STENCIL_OP_KEEP;
        if ( op == stencil_op::replace )
            return D3D11_STENCIL_OP_REPLACE;
        if ( op == stencil_op::zero )
            return D3D11_STENCIL_OP_ZERO;

        __debugbreak();
        return D3D11_STENCIL_OP_KEEP;
    }

    inline DXGI_MODE_SCALING convert_scaling_mode(scaling_mode mode)
    {
        if ( mode == scaling_mode::unspecified )
            return DXGI_MODE_SCALING_UNSPECIFIED;
        if ( mode == scaling_mode::centered )
            return DXGI_MODE_SCALING_CENTERED;
        if ( mode == scaling_mode::stretched )
            return  DXGI_MODE_SCALING_STRETCHED;

        __debugbreak();
        return DXGI_MODE_SCALING_UNSPECIFIED;
    }

    inline D3D11_INPUT_CLASSIFICATION convert_input_classification(input_classification type)
    {
        if ( type == input_classification::per_vertex_data )
            return D3D11_INPUT_PER_VERTEX_DATA;
        if ( type == input_classification::per_instance_data )
            return D3D11_INPUT_PER_INSTANCE_DATA;
        __debugbreak();
        return D3D11_INPUT_PER_VERTEX_DATA;
    }

    DXGI_FORMAT convert_format(format format);

    inline unsigned int get_format_size(format format)
    {
        switch (format)
        {
        case format::FORMAT_UNKNOWN:
            return 0;

        case format::FORMAT_R32_FLOAT:
        case format::FORMAT_R32_UINT:
        case format::FORMAT_R32_SINT:
        case format::FORMAT_D24_UNORM_S8_UINT:
            return sizeof(int32_t);

        case format::FORMAT_R32G32_FLOAT:
        case format::FORMAT_R32G32_UINT:
        case format::FORMAT_R32G32_SINT:
            return sizeof(int32_t) * 2;

        case format::FORMAT_R32G32B32_FLOAT:
        case format::FORMAT_R32G32B32_UINT:
        case format::FORMAT_R32G32B32_SINT:
            return sizeof(int32_t) * 3;

        case format::FORMAT_R32G32B32A32_FLOAT:
        case format::FORMAT_R32G32B32A32_UINT:
        case format::FORMAT_R32G32B32A32_SINT:
            return sizeof(int32_t) * 4;

        case format::FORMAT_R8_SINT:
        case format::FORMAT_R8_UINT:
        case format::FORMAT_R8_UNORM:
            return sizeof(uint8_t);

        case format::FORMAT_R8G8_SINT:
        case format::FORMAT_R8G8_UINT:
        case format::FORMAT_R8G8_UNORM:
            return sizeof(uint8_t) * 2;

        case format::FORMAT_R8G8B8A8_SINT:
        case format::FORMAT_R8G8B8A8_UINT:
        case format::FORMAT_R8G8B8A8_UNORM:
            return sizeof(uint8_t) * 4;
        }

        __debugbreak();
        return 0;
    }
}