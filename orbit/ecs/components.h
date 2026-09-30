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
        content::model::handle_type model_handle;
    };

    struct instanced_geometry
    {
        content::model::handle_type model_handle;
    };

    struct model_change_event
    {
        bool removed = false;
        entt::entity entity;
        content::model::handle_type model_handle;
    };

    struct transform_change_event
    {
        entt::entity entity;
    };

    class geometry_system
    {
        public:
            void initialize(entt::registry* registry);
            void on_model_change(model_change_event& e) const;
            void on_transform_change(transform_change_event& e) const;
        private:
            entt::registry* _registry = nullptr;
    };

}