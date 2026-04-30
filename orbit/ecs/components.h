#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "orbit/content/model.h"

namespace orbit::components
{
    struct transform
    {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    };

    struct geometry
    {
        content::model::handle_type _handle;
    };
}