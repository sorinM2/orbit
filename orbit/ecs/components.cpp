#pragma once
#include "components.h"
#include "ecs.h"

namespace orbit::components
{
    void geometry_system::on_model_change(model_change_event& e) const
    {
        instanced_geometry* _instanced_geometry = _registry->try_get<instanced_geometry>(e.entity);

        if ( !_instanced_geometry )
            return;

        if ( _instanced_geometry->model_handle.is_valid() )
            content::model::get_model(_instanced_geometry->model_handle).remove_instance(e.entity);

        if ( e.removed )
            _instanced_geometry->model_handle.invalidate();
        else
        {
            content::model::get_model(e.model_handle).add_instance(e.entity);
            _instanced_geometry->model_handle = e.model_handle;
        }
    }

    void geometry_system::on_transform_change(transform_change_event& e) const
    {
        instanced_geometry* _instanced_geometry = _registry->try_get<instanced_geometry>(e.entity);
        if ( !_instanced_geometry )
            return;
        auto model_handle = _instanced_geometry->model_handle;
        if ( model_handle.is_valid() )
        {
            content::model::get_model(model_handle).remove_instance(e.entity);
            content::model::get_model(model_handle).add_instance(e.entity);
        }
    }

    void geometry_system::initialize(entt::registry* registry)
    {
        _registry = registry;
    }
}