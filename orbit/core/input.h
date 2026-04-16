#pragma once
#include "GLFWcommon.h"

namespace orbit::input
{
    struct cursor_position
    {
        double _xpos, _ypos;
    };

    void initialize(GLFWwindow* window);

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    bool get_key(int key);
    cursor_position get_cursor_position();
}