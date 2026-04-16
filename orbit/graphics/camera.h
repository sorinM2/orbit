#pragma once
#include "glm/glm.hpp"

namespace orbit::graphics::camera
{
    struct camera_buffer
    {
        glm::mat4 view;
        glm::mat4 projection;
    };

    void initialize();
    void update();

    const camera_buffer& get_camera_buffer();
}