#include "editor.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_dx11.h"

#include "orbit/graphics/renderer.h"

#include "panel.h"
#include "orbit/graphics/d3d11/d3d11device.h"

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

        ImGui_ImplGlfw_InitForOther(orbit::graphics::renderer::get_window(), true);

        orbit::graphics::d3d11_rendering_device* device = static_cast<orbit::graphics::d3d11_rendering_device*>(orbit::graphics::renderer::get_device());
        orbit::graphics::d3d11_rendering_device_context* context = static_cast<orbit::graphics::d3d11_rendering_device_context*>(orbit::graphics::renderer::get_context());
        ImGui_ImplDX11_Init(device->get_internal(), context->get_internal());

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
