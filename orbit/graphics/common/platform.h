#pragma once 
#include "orbit/content/mesh.h"
#include "orbit/content/texture.h"
#include <functional>

namespace orbit::graphics::platform
{
	struct render_target_texture
	{
		int width, height;
		void* texture = nullptr;
	};

	struct platform_descriptor
	{
		std::function<bool(bool)> initialize;
		std::function<void()> begin_frame;
		std::function<void()> begin_editor; //TODO remove
		std::function<void()> end_frame;
		std::function<void()> shutdown;
		std::function<void()> resize;
		std::function<render_target_texture()> get_render_target_texture;

		::orbit::content::mesh::platform_descriptor mesh;
		::orbit::content::texture::platform_descriptor texture;
	};

	enum class platform
	{
		d3d11 = 0,

		invalid
	};

	bool initialize(bool runtime = true);
	void begin_frame();
	void begin_editor();//TODO remove
	void end_frame();
	void shutdown();

	void set_platform(platform platform);

	platform_descriptor get_platform();
}