#include "popup.h"
#include "imgui/imgui.h"

namespace editor
{
    void model_popup::initialize()
    {
        ImGui::OpenPopup( _name.c_str(), ImGuiPopupFlags_AnyPopupLevel);
    }

    void model_popup::update()
    {
        if ( ImGui::BeginPopupModal(_name.c_str()) )
        {
            ImGui::BeginChild("filesystem");
            std::filesystem::path result = _file_panel.update();
            ImGui::EndChild();

            if ( !result.empty() )
            {
                ImGui::CloseCurrentPopup();
                orbit::content::model::add_model(result);
            }
            ImGui::EndPopup();
        }
    }
}