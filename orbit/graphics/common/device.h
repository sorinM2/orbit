#pragma once
#include <filesystem>
#include <unordered_set>
#include "util.h"
#include "buffer.h"
#include "sampler.h"
#include "rasterizer.h"
#include "texture.h"
#include "shader_resource.h"
#include "render_target.h"
#include "shader.h"

#include "orbit/utility/vector.h"

namespace orbit::graphics
{
    class rendering_device;
    class rendering_device_context;

    class rendering_device : public virtual ref_counted
    {
        friend class device_resource;
    public:

        virtual void create_buffer(const buffer_desc& desc, buffer** buffer) = 0;
        virtual void create_framebuffer(const framebuffer_desc& desc, render_target** targets, framebuffer** framebuffer) = 0;
        virtual void create_sampler(const sampler_desc& desc, sampler** sampler) = 0;
        virtual void create_rasterizer(const rasterizer_desc& desc, rasterizer** rasterizer) = 0;
        virtual void create_texture2D(const texture2D_desc& desc, texture2D** texture) = 0;
        virtual void create_shader_resource(const shader_resource_desc& desc, device_resource* resource, shader_resource** shader_resource) = 0;

        virtual void create_render_target(const render_target_desc& desc, device_resource* resource, render_target** render_target) = 0;
        virtual void create_depth_stencil(const depth_stencil_desc& desc, device_resource* resource, depth_stencil** depth_stencil) = 0;
        virtual void create_depth_stencil_state(const depth_stencil_state_desc& desc, depth_stencil_state** depth_stencil_state) = 0;
        virtual void create_swap_chain(const swap_chain_desc& desc, swap_chain** swap_chain) = 0;

        virtual void create_shader(const shader_desc& desc, shader** shader) = 0;
        virtual void create_program(const program_desc& desc, program** program) = 0;

        ~rendering_device() override;

    protected:
        void remove_resource(device_resource* resource) { if ( resource ) _resources.erase(resource); };
        void add_resource(device_resource* resource) { _resources.insert(resource); };
        std::unordered_set<device_resource*> _resources;

        rendering_device_context* _context = nullptr;
    };

    class rendering_device_context : public virtual ref_counted
    {
    public:

        virtual void clear_render_target(render_target* target) = 0;
        virtual void clear_depth_stencil(depth_stencil* depth_stencil) = 0;

        virtual void set_vertex_buffers( unsigned int num_buffers, unsigned int* strides, buffer** buffers) = 0;
        virtual void set_index_buffer(buffer* buffer) = 0;
        virtual void draw_indexed(unsigned int no_indices) = 0;

        virtual void set_rasterizer(rasterizer* rasterizer) = 0;
        virtual void set_viewports(viewport* viewports, unsigned int count) = 0;
        virtual void set_scissors(rect* rects, unsigned int count) = 0;
        virtual void set_samplers(sampler** samplers, unsigned int count, unsigned int start_slot = 0) = 0;

        virtual void vs_set_constant_buffers(buffer** buffer, unsigned int count, unsigned int slot) = 0;
        virtual void ps_set_constant_buffers(buffer** buffer, unsigned int count, unsigned int slot) = 0;
        virtual void ps_set_shader_resources(shader_resource** srs, unsigned int count, unsigned int slot) = 0;

        virtual void set_frame_buffer(framebuffer* framebuffer) = 0;
        virtual void set_depth_stencil_state(depth_stencil_state* dss) = 0;

        virtual void set_program(program* program) = 0;
        ~rendering_device_context() override {}

        void get_viewports(int* count, viewport* viewports) const;
        void get_scissors(int* count, rect* rects) const;
    protected:
        utl::vector<viewport> _viewports;
        utl::vector<rect> _scissors;
    };

    class factory : public virtual ref_counted
    {
    public:
        ~factory() override = default;
        virtual void create_device_and_context(rendering_device** device, rendering_device_context** context) = 0;
        virtual void create_swap_chain(const swap_chain_desc& desc, rendering_device* device, rendering_device_context* context, swap_chain** sawp_chain) = 0;
    };
}