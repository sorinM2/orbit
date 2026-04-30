#include "ecs.h"

namespace orbit
{
    ecs* ecs::_instance = nullptr;

    ecs* ecs::get_instance()
    {
        if ( !_instance )
            _instance = new ecs();
        return _instance;
    }
}