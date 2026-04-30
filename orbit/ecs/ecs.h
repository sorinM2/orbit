#pragma once
#include "entt/entt.hpp"
#include "components.h"

namespace orbit
{
    class ecs
    {
    public:
        entt::registry registry;

    public:
        ecs(ecs& other) = delete;
        ecs& operator= (ecs& other) = delete;

        static ecs* get_instance();
    private:
        ecs(){}
        static ecs* _instance;
    };
}