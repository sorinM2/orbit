#include "orbit/core/application.h"
#include "orbit/core/platform/platform_common.h"
#include <thread>
#include <chrono>
#include "editor.h"
#include <glfw/glfw3.h>

#include "orbit/core/application.h"

namespace
{
    constexpr int FPS = 120;
    constexpr double FRAME_DURATION = 1.0 / FPS;

    double total = 0.;
    int count = 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    if (!orbit::application::initialize()) return false;
    editor::initialize();

    while ( !orbit::application::should_close() )
    {
        double frame_start = glfwGetTime();
        orbit::application::update();
        editor::update();
        orbit::application::render();
        double frame_end = glfwGetTime();
        double dt = frame_end - frame_start;
        total += std::max(dt, FRAME_DURATION); ++count;

        if ( total > 1.)
        {
            OutputDebugString(("FPS: " + std::to_string(count) + "\n").c_str());
            count = 0;
            total = 0.;
        }

        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>((FRAME_DURATION - dt) * 1000.));
    }

    editor::shutdown();
    orbit::application::shutdown();

    return 0;
}