#include "application.h"
#include "graphics/d3d11/d3d11core.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace orbit::application
{
	bool initialize()
	{
		bool result;

		result = graphics::d3d11::core::initialize();
	
		if (!result)
		{
			MessageBox(0, "Failed to initialize d3d11!", "ERROR", MB_OK);
			return false;
		}

		return true;
	}

	void restart_graphics()
	{
		graphics::d3d11::core::shutdown();
		graphics::d3d11::core::initialize();
	}

	void update()
	{
		graphics::d3d11::core::update();
	}

	void shutdown()
	{
		graphics::d3d11::core::shutdown_device();
	}
}