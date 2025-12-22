#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace orbit::system
{
	bool initialize(HINSTANCE instance_handle, int show);
	int run();

	LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	HWND get_main_hwnd();
	int get_monitor_width();
	int get_monitor_height();

}