#include "system.h"
#include "platform/platform_common.h"
#include "application.h"

#include "GLFWcommon.h"
#include "input.h"
#include "orbit/logger/logger.h"


namespace orbit::system
{
	namespace
	{
		GLFWwindow* _window;
		int _width = 960, _height = 540;
	}

	GLFWwindow* get_window()
	{
		return _window;
	}


#ifdef WIN32
	HWND get_main_hwnd()
	{
		return glfwGetWin32Window(_window);
	}
#endif

	int get_monitor_width()
	{
		return GetSystemMetrics(SM_CXSCREEN);
	}

	int get_monitor_height()
	{
		return GetSystemMetrics(SM_CYSCREEN);
	}

	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		_width = width;
		_height = height;

		// auto platform = graphics::platform::get_platform();
		// platform.resize();
	}

	int get_window_width()
	{
		return _width;
	}

	int get_window_height()
	{
		return _height;
	}

	bool initialize(HINSTANCE instance_handle, int show)
	{
		logger::initialize();

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		_window = glfwCreateWindow(_width, _height, "fereastra glfw", nullptr, nullptr);

		//glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		input::initialize(_window);

		glfwSetCursorPosCallback(_window, input::cursor_position_callback);
		glfwSetKeyCallback(_window, input::key_callback);
		glfwSetWindowSizeCallback(_window, window_size_callback);

		application::initialize();
		return true;
	}

	void run()
	{
		glfwPollEvents();
		application::update();
	}

	bool should_close()
	{
		return glfwWindowShouldClose(_window);
	}

	void shutdown()
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
		application::shutdown();
	}

}