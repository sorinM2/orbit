#include "editor.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_dx11.h"

#include "orbit/core/system.h"
#include "orbit/graphics/d3d11/d3d11core.h"

#include "panel.h"

namespace editor
{
    namespace
    {
        entity_panel entities;
        selected_entity_panel selected;
        model_panel models;
        view_panel view;
    }
    void initialize()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // IF using Docking Branch

        ImGui_ImplGlfw_InitForOther(orbit::system::get_window(), true);
        ImGui_ImplDX11_Init(orbit::graphics::d3d11::core::get_device(), orbit::graphics::d3d11::core::get_device_context());

        entities.initialize();
        selected.initialize();
        models.initialize();
        view.initialize();
    }

    void update()
    {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

        entities.update();
        selected.update();
        models.update();
        view.update();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

    }

    void shutdown()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }
}