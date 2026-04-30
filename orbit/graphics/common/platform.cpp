#include "platform.h"
#include "../d3d11/content/mesh.h"
#include "../d3d11/d3d11core.h"
#include "../common/camera.h"
#include "../d3d11/content/texture.h"

namespace orbit::graphics::platform
{
	namespace
	{
		platform_descriptor _platform;

		void set_d3d11()
		{
			_platform.initialize = d3d11::core::initialize;
			_platform.begin_frame = d3d11::core::begin_frame;
			_platform.begin_editor = d3d11::core::begin_editor;
			_platform.end_frame = d3d11::core::end_frame;
			_platform.shutdown = d3d11::core::shutdown_device;
			_platform.resize = d3d11::core::resize_swap_chain;
			_platform.get_render_target_texture = d3d11::core::get_render_target_texture;

			_platform.mesh.add = d3d11::content::mesh::add_mesh;
			_platform.mesh.render = d3d11::content::mesh::render_mesh;
			_platform.mesh.release = d3d11::content::mesh::release_mesh;
			_platform.mesh.bind_world = d3d11::content::mesh::bind_world;
			content::mesh::set_platform(_platform.mesh);

			_platform.texture.add = d3d11::content::texture::add;
			_platform.texture.bind = d3d11::content::texture::bind;
			_platform.texture.release = d3d11::content::texture::release;
			content::texture::set_platform(_platform.texture);
		}
	}

	platform_descriptor get_platform() { return _platform; }


	void set_platform(platform platform)
	{
		if (platform == platform::d3d11)
			set_d3d11();
		else throw std::logic_error("invalid graphics api");
	}

	bool initialize(bool runtime)
	{
		camera::initialize();
		return _platform.initialize(runtime);
	}

	void begin_frame()
	{
		camera::update();
		_platform.begin_frame();
	}

	void begin_editor()
	{
		_platform.begin_editor();
	}

	void end_frame()
	{
		_platform.end_frame();
	}

	void shutdown()
	{
		_platform.shutdown();
	}
}
