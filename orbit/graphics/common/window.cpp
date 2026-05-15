#include "window.h"

#include "orbit/core/input.h"


namespace orbit::graphics
{
    window::window(const window_desc& desc) : _desc{desc}
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        _internal_window = glfwCreateWindow(desc.width, desc.height, "fereastra glfw", nullptr, nullptr);

        input::initialize(_internal_window);
    }

    window::~window()
    {
        glfwDestroyWindow(_internal_window);
    }

#ifdef WIN32
    HWND window::get_main_hwnd() const
    {
        return glfwGetWin32Window(_internal_window);
    }
#endif

    int window::get_window_width() const
    {
        return _desc.width;
    }

    int window::get_window_height() const
    {
        return _desc.height;
    }


    void initialize_api()
    {
        glfwInit();
    }

}
