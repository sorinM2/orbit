#include "panel.h"

#include "imgui/imgui.h"
#include "orbit/ecs/ecs.h"
#include "orbit/ecs/components.h"
#include "orbit/content/model.h"
#include "../orbit/graphics/common/platform.h"
#include "orbit/graphics/d3d11/d3d11core.h"

namespace editor
{
    struct entity_name
    {
        std::string name;
    };
    entt::entity entity_panel::selected_entity;

    void entity_panel::update()
    {
        orbit::ecs* ecs = orbit::ecs::get_instance();
        entt::registry& registry = ecs->registry;
        auto entities = registry.view<entity_name>();

        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Entities");

        ImVec2 available = ImGui::GetContentRegionAvail();
        ImVec2 size_list = ImVec2(available.x, available.y * 0.8);

        ImGui::BeginChild("ListBox", size_list);
        entities.each([](const auto entity, auto& name)
        {
            if ( ImGui::Selectable(name.name.c_str(), entity == selected_entity) )
            {
                selected_entity = entity;

            }
        });

        ImGui::EndChild();

        if ( ImGui::Button("Add Entity") )
        {
            const auto entity = registry.create();
            registry.emplace<entity_name>(entity, "new_entity_" + std::to_string(entities.size()));
        }

        ImGui::End();
    }

    void selected_entity_panel::update()
    {
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Selected Entity");

        entt::entity& selected = entity_panel::selected_entity;
        orbit::ecs* ecs = orbit::ecs::get_instance();
        entt::registry& registry = ecs->registry;


        if ( registry.valid(selected) )
        {
            auto [name] = registry.get<entity_name>(selected);
            ImGui::Text("%s", name.c_str());
        }

        orbit::components::geometry* geometry = registry.try_get<orbit::components::geometry>(selected);
        orbit::components::transform* transform = registry.try_get<orbit::components::transform>(selected);

        if ( geometry )
        {
            if ( ImGui::Button("Set Model") && model_panel::selected_model.is_valid() )
                geometry->_handle = model_panel::selected_model;
        }

        if ( !geometry )
            if ( ImGui::Button("Add Mesh Component") )
                registry.emplace<orbit::components::geometry>(selected);

        if ( !transform )
            if ( ImGui::Button("Add Transform") )
                registry.emplace<orbit::components::transform>(selected);

        if ( transform )
        {
            ImGui::DragFloat3("Position", glm::value_ptr(transform->position), 0.3f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(transform->rotation));
            ImGui::DragFloat3("Scale", glm::value_ptr(transform->scale), 0.3f);
        }

        ImGui::End();
    }

    ::orbit::content::model::handle_type model_panel::selected_model;

    void model_panel::update()
    {
        const auto& handles = orbit::content::model::get_handles();

        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Models");

        for ( auto handle : handles )
        {
            orbit::content::model::model& _model = orbit::content::model::get_model(handle);
            if ( ImGui::Selectable(_model.get_name().c_str(), selected_model == handle) )
                selected_model = handle;
        }
        ImGui::End();
    }

    void view_panel::update()
    {
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("World View");

        orbit::graphics::platform::render_target_texture to_draw;
        to_draw = orbit::graphics::d3d11::core::get_render_target_texture();

        ImVec2 available = ImGui::GetContentRegionAvail();
        float image_aspect = float(to_draw.width) / float(to_draw.height);
        float available_aspect = available.x / available.y;

        ImVec2 display_size;

        if ( available_aspect > image_aspect )
        {
            display_size.x = available.y * image_aspect;
            display_size.y = available.y;
        }
        else
        {
            display_size.x = available.x;
            display_size.y = available.x / image_aspect;
        }


        float offsetX = (available.x - display_size.x) * 0.5f;
        float offsetY = (available.y - display_size.y) * 0.5f;

        ImVec2 pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(pos.x + offsetX, pos.y + offsetY));

        ImGui::Image((ImTextureID)to_draw.texture, display_size);
        ImGui::End();
    }



}
