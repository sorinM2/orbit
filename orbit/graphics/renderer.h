#pragma once
#include "common/device.h"
#include "glm/glm.hpp"
#include "common/camera.h"

namespace orbit::graphics::renderer
{
    struct mesh_data
    {
        buffer* _vertex_buffer;
        buffer* _index_buffer;

        buffer* _transforms;

        unsigned int instances;
    };

    class render_pass_data
    {
        mesh_data** meshes = nullptr;
    };

    struct render_target_texture
    {
        unsigned int width;
        unsigned int height;
        void* resource;
    };

    render_target_texture get_render_target_texture();


    void initialize();
    rendering_device* get_device();
    rendering_device_context* get_context();
    void bind_world(const glm::mat4& world_matrix);

    void bind_world(const glm::mat4& world_matrix);

    void begin_frame();
    void begin_editor();
    void end_frame();

    bool should_close();
    void shutdown();

    GLFWwindow* get_window();
}