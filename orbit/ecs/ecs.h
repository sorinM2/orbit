#pragma once
#include "entt/entt.hpp"
#include "components.h"

namespace orbit
{
    class ecs
    {
    public:
        entt::registry registry;
        entt::dispatcher dispatcher;

        ecs(ecs& other) = delete;
        ecs& operator= (ecs& other) = delete;

        static ecs* get_instance();
    private:
        components::geometry_system _geometry_system;

        ecs()
        {
            _geometry_system.initialize(&registry);
            dispatcher.sink<components::model_change_event>().connect<&components::geometry_system::on_model_change>(_geometry_system);
            dispatcher.sink<components::transform_change_event>().connect<&components::geometry_system::on_transform_change>(_geometry_system);
        }

        static ecs* _instance;
    };
}