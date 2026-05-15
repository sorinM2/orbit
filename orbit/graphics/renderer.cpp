#include "renderer.h"
#include "common/buffer.h"
#include "common/camera.h"
#include "common/window.h"
#include "d3d11/d3d11device.h"
#include "d3d11/d3d11shader_resource.h"

#include "glm/gtc/type_ptr.hpp"
#include "orbit/core/input.h"

namespace orbit::graphics::renderer
{
    namespace
    {
        unsigned int _render_target_width = 1920;
        unsigned int _render_target_height = 1080;

        rendering_device* device = nullptr;
        rendering_device_context* context = nullptr;
        swap_chain* swap_chain = nullptr;

        render_target* view_render_target = nullptr;
        framebuffer* view_framebuffer = nullptr;
        shader_resource* view_shader_resource = nullptr;
        depth_stencil* view_depth_stencil = nullptr;

        depth_stencil_state* depth_stencil_state = nullptr;
        rasterizer* rasterizer = nullptr;
        sampler* sampler = nullptr;

        buffer* camera_buffer = nullptr;
        buffer* world_buffer = nullptr;

        program* main_program = nullptr;

        window* main_window = nullptr;
        viewport viewport{};
    }

    namespace
    {
        void initialize_view_rt()
        {
            texture2D* new_tex;

            texture2D_desc rtv_texture_desc;
            ZeroMemory(&rtv_texture_desc, sizeof(rtv_texture_desc));
            rtv_texture_desc.format = format::FORMAT_R8G8B8A8_UNORM;
            rtv_texture_desc.width = _render_target_width;
            rtv_texture_desc.height = _render_target_height;
            rtv_texture_desc.mips = 1;
            rtv_texture_desc.usage = resource_usage::resource_default_usage;
            rtv_texture_desc.sample_desc.count = 1;
            rtv_texture_desc.sample_desc.quality = 0;
            rtv_texture_desc.bind_flags = bind_flag_render_target | bind_flag_shader_resource;
            rtv_texture_desc.cpu_access_flags = 0;

            device->create_texture2D(rtv_texture_desc, &new_tex);

            shader_resource_desc shader_resource_desc;
            ZeroMemory(&shader_resource_desc, sizeof(shader_resource_desc));
            shader_resource_desc.type = shader_resource_type::texture2D;
            shader_resource_desc.format = format::FORMAT_R8G8B8A8_UNORM;

            util::safe_release(view_shader_resource);
            device->create_shader_resource(shader_resource_desc, new_tex, &view_shader_resource);

            render_target_desc desc;
            desc.type = render_target_type::texture2D;
            util::safe_release(view_render_target);
            device->create_render_target(desc, new_tex, &view_render_target);

            framebuffer_desc fb_desc;
            fb_desc.count = 1;
            util::safe_release(view_framebuffer);
            device->create_framebuffer(fb_desc, &view_render_target, &view_framebuffer);

            util::safe_release(new_tex);
        }

        void initialize_view_depth_stencil()
        {
            texture2D* new_tex;

            texture2D_desc depth_buffer_desc;
            ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));
            depth_buffer_desc.format = format::FORMAT_D24_UNORM_S8_UINT;
            depth_buffer_desc.width = _render_target_width;
            depth_buffer_desc.height = _render_target_height;
            depth_buffer_desc.sample_desc.count = 1;
            depth_buffer_desc.sample_desc.quality = 0;
            depth_buffer_desc.mips = 1;
            depth_buffer_desc.usage = resource_usage::resource_default_usage;
            depth_buffer_desc.bind_flags = bind_flag_depth_stencil;
            depth_buffer_desc.cpu_access_flags = 0;

            device->create_texture2D(depth_buffer_desc, &new_tex);

            depth_stencil_desc depth_stencil_desc;
            ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
            depth_stencil_desc.type = depth_stencil_type::texture2D;

            util::safe_release(view_depth_stencil);
            device->create_depth_stencil(depth_stencil_desc, new_tex, &view_depth_stencil);

            util::safe_release(new_tex);
        }

        void initialize_main_shader()
        {

            shader* vertex_shader = nullptr;
            shader* pixel_shader = nullptr;

            shader_desc vertex_desc;
            vertex_desc.path = std::filesystem::path("src/color/color_vs.hlsl");
            vertex_desc.type = shader_type::vertex;
            vertex_desc.entry_point = "vs_main";

            utl::vector<input_element> elements(3);
            vertex_desc.vertex_shader.element_count = 3;

            elements[0].classification = input_classification::per_vertex_data;
            elements[0].semantic_name = "POSITION";
            elements[0].semantic_index = 0;
            elements[0].format = format::FORMAT_R32G32B32_FLOAT;
            elements[0].input_slot = 0;

            elements[1].classification = input_classification::per_vertex_data;
            elements[1].semantic_name = "NORMAL";
            elements[1].semantic_index = 0;
            elements[1].format = format::FORMAT_R32G32B32_FLOAT;
            elements[1].input_slot = 0;

            elements[2].classification = input_classification::per_vertex_data;
            elements[2].semantic_name = "TEXCOORD";
            elements[2].semantic_index = 0;
            elements[2].format = format::FORMAT_R32G32_FLOAT;
            elements[2].input_slot = 0;

            vertex_desc.vertex_shader.elements = elements.data();

            device->create_shader(vertex_desc, &vertex_shader);

            shader_desc pixel_desc;
            vertex_desc.path = std::filesystem::path("src/color/color_ps.hlsl");
            vertex_desc.type = shader_type::pixel;
            vertex_desc.entry_point = "ps_main";
            device->create_shader(vertex_desc, &pixel_shader);

            program_desc program_desc;
            program_desc.vertex_shader = vertex_shader;
            program_desc.pixel_shader = pixel_shader;

            device->create_program(program_desc, &main_program);

            util::safe_release(vertex_shader);
            util::safe_release(pixel_shader);
        }

        void initialize_viewport()
        {
            viewport.x = 0;
            viewport.y = 0;
            viewport.width = _render_target_width;
            viewport.height = _render_target_height;

            context->set_viewports(&viewport, 1);
        }

    }

    rendering_device* get_device()
    {
        return device;
    }

    rendering_device_context* get_context()
    {
        return context;
    }

    void window_size_callback(GLFWwindow* window, int width, int height)
    {
        swap_chain->resize_swap_chain();
    }

    bool should_close()
    {
        return glfwWindowShouldClose(main_window->get_internal());
    }

    GLFWwindow* get_window()
    {
        return main_window->get_internal();
    }

    void initialize()
    {
        initialize_api();
        window_desc window_desc;
        window_desc.width = 960;
        window_desc.height = 540;
        main_window = new window(window_desc);

        glfwSetCursorPosCallback(main_window->get_internal(), input::cursor_position_callback);
        glfwSetKeyCallback(main_window->get_internal(), input::key_callback);
        glfwSetWindowSizeCallback(main_window->get_internal(), window_size_callback);

        //main hwnd information
        D3D11_RECT window_rect;
        GetClientRect(main_window->get_main_hwnd(), &window_rect);
        unsigned int window_width = window_rect.right - window_rect.left;
        unsigned int window_height = window_rect.bottom - window_rect.top;

        HWND main_hwnd = main_window->get_main_hwnd();
        HMONITOR hwnd_monitor = MonitorFromWindow(main_hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitor_info;
        monitor_info.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hwnd_monitor, &monitor_info);

        DEVMODE dev_mode;
        dev_mode.dmSize = sizeof(dev_mode);
        EnumDisplaySettings(monitor_info.szDevice, ENUM_CURRENT_SETTINGS, &dev_mode);

        unsigned int vsync_numerator = dev_mode.dmDisplayFrequency;
        unsigned int vsync_denominator = 1;

        factory* factory = new d3d11_factory();
        factory->create_device_and_context(&device, &context);

        swap_chain_desc swap_chain_desc;
        ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
        swap_chain_desc.buffer_desc.width = window_width;
        swap_chain_desc.buffer_desc.height = window_height;
        swap_chain_desc.buffer_desc.refresh_rate.numerator = vsync_numerator;
        swap_chain_desc.buffer_desc.refresh_rate.denominator = vsync_denominator;
        swap_chain_desc.buffer_desc.format = format::FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc.buffer_desc.scaling = scaling_mode::unspecified;
        swap_chain_desc.vsync_enabled = true;
        swap_chain_desc.window = main_window;

        swap_chain_desc.sample_desc.count = 1;
        swap_chain_desc.sample_desc.quality = 0;

        swap_chain_desc.buffer_count = 1;

        swap_chain_desc.buffer_usage = render_target_output | shader_input;
        swap_chain_desc.windowed = true;

        factory->create_swap_chain(swap_chain_desc, device, context, &swap_chain);

        util::safe_release(factory);

        initialize_view_rt();
        initialize_view_depth_stencil();
        initialize_viewport();

        depth_stencil_state_desc ds_desc;
        ZeroMemory(&ds_desc, sizeof(ds_desc));
        ds_desc.depth_enabled = true;
        ds_desc.depth_func = comparison_func::comparison_func_less;
        ds_desc.stencil_enabled = false;

        device->create_depth_stencil_state(ds_desc, &depth_stencil_state);
        context->set_depth_stencil_state(depth_stencil_state);

        rasterizer_desc raster_desc;
        ZeroMemory(&raster_desc, sizeof(raster_desc));
        raster_desc.cull_mode = cull_mode::none;
        raster_desc.fill_mode = fill_mode::solid;
        raster_desc.front_counter_clockwise = true;
        raster_desc.multisample_enabled = false;
        raster_desc.scissor_enabled = false;
        raster_desc.depth_clip_enabled = true;

        device->create_rasterizer(raster_desc, &rasterizer);
        context->set_rasterizer(rasterizer);

        sampler_desc sampler_desc;
        ZeroMemory(&sampler_desc, sizeof(sampler_desc));
        sampler_desc.address_u = texture_address_mode::texture_address_clamp;
        sampler_desc.address_v = texture_address_mode::texture_address_clamp;
        sampler_desc.address_w = texture_address_mode::texture_address_clamp;
        sampler_desc.comparison_func = comparison_func::comparison_func_always;

        device->create_sampler(sampler_desc, &sampler);
        context->set_samplers(&sampler, 1, 0);

        initialize_main_shader();

        camera::initialize();
        buffer_desc camera_buffer_desc;
        camera_buffer_desc.bind_flags = bind_flag_constant_buffer;
        camera_buffer_desc.cpu_access_flags = cpu_access_flags_write;
        camera_buffer_desc.initial_data = &camera::get_camera_buffer();
        camera_buffer_desc.usage = resource_usage::resource_dynamic_usage;
        camera_buffer_desc.width = sizeof(camera::camera_buffer);

        device->create_buffer(camera_buffer_desc, &camera_buffer);
        context->vs_set_constant_buffers(&camera_buffer, 1, 1);

        buffer_desc world_buffer_desc;
        world_buffer_desc.bind_flags = bind_flag_constant_buffer;
        world_buffer_desc.cpu_access_flags = cpu_access_flags_write;
        world_buffer_desc.initial_data = nullptr;
        world_buffer_desc.usage = resource_usage::resource_dynamic_usage;
        world_buffer_desc.width = sizeof(glm::mat4);

        device->create_buffer(camera_buffer_desc, &world_buffer);
        context->vs_set_constant_buffers(&world_buffer, 1, 0);
    }

    void bind_world(const glm::mat4& world_matrix)
    {
        mapped_resource mapped_resource = world_buffer->map(map_type::map_type_write);
        memcpy(mapped_resource._data, glm::value_ptr(world_matrix), sizeof(glm::mat4));
        world_buffer->unmap();
    }

    void begin_frame()
    {
        context->clear_render_target(view_render_target);
        context->clear_depth_stencil(view_depth_stencil);
        context->set_program(main_program);

        mapped_resource mapped_resource = camera_buffer->map(map_type::map_type_write);
        memcpy(mapped_resource._data, &camera::get_camera_buffer(), sizeof(camera::camera_buffer));
        camera_buffer->unmap();

        view_framebuffer->set_depth_stencil(view_depth_stencil);
        context->set_frame_buffer(view_framebuffer);

        camera::update();
    }

    void begin_editor()
    {
        context->set_frame_buffer(swap_chain->get_framebuffer());
        context->clear_render_target(swap_chain->get_framebuffer()->get_targets()[0]);
    }

    void end_frame()
    {
        swap_chain->present();
    }

    /*
        rendering_device* device = nullptr;
        rendering_device_context* context = nullptr;
        swap_chain* swap_chain = nullptr;

        render_target* view_render_target = nullptr;
        framebuffer* view_framebuffer = nullptr;
        shader_resource* view_shader_resource = nullptr;
        depth_stencil* view_depth_stencil = nullptr;

        depth_stencil_state* depth_stencil_state = nullptr;
        rasterizer* rasterizer = nullptr;
        sampler* sampler = nullptr;

        buffer* camera_buffer = nullptr;
        buffer* world_buffer = nullptr;

        program* main_program = nullptr;
     */
    void shutdown()
    {
        util::safe_release(camera_buffer);
        util::safe_release(world_buffer);

        util::safe_release(sampler);
        util::safe_release(rasterizer);
        util::safe_release(depth_stencil_state);

        util::safe_release(view_render_target);
        util::safe_release(view_framebuffer);
        util::safe_release(view_shader_resource);
        util::safe_release(view_depth_stencil);

        util::safe_release(swap_chain);
        util::safe_release(context);

        util::safe_release(main_program);

        util::safe_release(device);
    }

    render_target_texture get_render_target_texture()
    {
        render_target_texture result;
        result.width = _render_target_width;
        result.height = _render_target_height;
        result.resource = (void*)dynamic_cast<d3d11_shader_resource*>(view_shader_resource)->get_internal();
        return result;
    }
}
