#include "system.h"
#include "platform/platform_common.h"
#include "application.h"

namespace orbit::system
{
	namespace
	{
		HWND hwnd = 0;
		unsigned int width = 960, height = 540;
	}

	HWND get_main_hwnd()
	{
		return hwnd;
	}

	int get_monitor_width()
	{
		return GetSystemMetrics(SM_CXSCREEN);
	}

	int get_monitor_height()
	{
		return GetSystemMetrics(SM_CYSCREEN);
	}

	bool initialize(HINSTANCE instance_handle, int show)
	{
		WNDCLASS wc;

		wc.style = CS_HREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = window_proc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = instance_handle;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "default class";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, "Register class failed!", 0, 0);
			return false;
		}

		hwnd = CreateWindowEx(0, "default class", "orbit", WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, instance_handle, 0);

		if (!hwnd)
		{
			MessageBox(0, "Failed to create window!", 0, 0);
			return false;
		}

		application::initialize();

		ShowWindow(hwnd, show);
		UpdateWindow(hwnd);


		return true;
	}

	int run()
	{
		MSG msg = { 0 };

		bool bRet = 1;

		while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0)
		{
			if (bRet == -1)
			{
				MessageBox(0, "GetMessage FAILED", "Error", MB_OK);
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			application::update();
			
		}
		application::shutdown();

		return int(msg.wParam);
	}

	LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		//case WM_MOVE:
		//{
		//	HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		//	MONITORINFOEX monitor_info;
		//	monitor_info.cbSize = sizeof(MONITORINFOEX);
		//	GetMonitorInfo(monitor, &monitor_info);
		//	OutputDebugString("Moved window to monitor:\n");
		//	OutputDebugString(monitor_info.szDevice);
		//	OutputDebugString("\n");
		//	return 0;
		//}
		case WM_SIZE:
			//application::restart_graphics();
			return 0;
		case WM_DISPLAYCHANGE:
			MessageBox(0, "Hello world!", "Hello", MB_OK);
			return 0;
		case WM_KEYDOWN:
			if (wparam = VK_ESCAPE)
				DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hwnd, msg, wparam, lparam);
		
	}
}