#pragma once
#include <windows.h>

#include "GLFW/glfw3.h"

namespace orbit::system
{
	bool initialize(HINSTANCE instance_handle, int show);
	void run();

	LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	GLFWwindow* get_window();

#ifdef WIN32
	HWND get_main_hwnd();
#endif

	int get_monitor_width();
	int get_monitor_height();
	int get_window_width();
	int get_window_height();

	bool should_close();
	void shutdown();


}
