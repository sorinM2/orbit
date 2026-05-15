#pragma once
#include "common.h"
#include "util.h"
#include "GLFW/glfw3.h"
#ifdef WIN32
#include <windows.h>
#endif

namespace orbit::graphics
{
    struct window_desc
    {
        unsigned int width;
        unsigned int height;
    };

    class window : public virtual ref_counted
    {
    public:
        GLFWwindow* get_internal() const { return _internal_window; }
        window(const window_desc& desc);

#ifdef WIN32
        HWND get_main_hwnd() const;
#endif

        int get_window_width() const;
        int get_window_height() const;
    private:
        ~window() override;
        window_desc _desc{};

        GLFWwindow* _internal_window = nullptr;
    };

    void initialize_api();
}
