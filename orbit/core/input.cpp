#include "input.h"
#include <unordered_map>

namespace orbit::input
{
    namespace
    {
        std::unordered_map<int, bool> key_map;
        cursor_position _cursor_position;
        cursor_position initial_cursor_position;
    }

    void initialize(GLFWwindow* window)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        initial_cursor_position._xpos = xpos;
        initial_cursor_position._ypos = ypos;
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if ( action == GLFW_PRESS )
            key_map[key] = true;
        else if ( action == GLFW_RELEASE )
            key_map[key] = false;
    }

    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        _cursor_position._xpos = xpos - initial_cursor_position._xpos;
        _cursor_position._ypos = ypos - initial_cursor_position._ypos;
    }

    bool get_key(int key)
    {
        return key_map[key];
    }

    cursor_position get_cursor_position()
    {
        return _cursor_position;
    }
}