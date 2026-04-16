#pragma once 
#include "orbit/content/mesh.h"
#include "orbit/content/texture.h"
#include <functional>

namespace orbit::graphics::platform
{
	enum class platform
	{
		d3d11 = 0,

		invalid
	};

	bool initialize();
	void begin_frame();
	void end_frame();
	void shutdown();

	void set_platform(platform platform);
}