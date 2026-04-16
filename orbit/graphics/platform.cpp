#include "platform.h"
#include "d3d11/content/mesh.h"
#include "d3d11/d3d11core.h"
#include "camera.h"
#include "d3d11/content/texture.h"

namespace orbit::graphics::platform
{
	namespace
	{
		struct platform_descriptor
		{
			std::function<bool()> initialize;
			std::function<void()> begin_frame;
			std::function<void()> end_frame;
			std::function<void()> shutdown;

			orbit::content::mesh::platform_descriptor mesh;
			orbit::content::texture::platform_descriptor texture;

		} _platform;

		void set_d3d11()
		{
			_platform.initialize = d3d11::core::initialize;
			_platform.begin_frame = d3d11::core::begin_frame;
			_platform.end_frame = d3d11::core::end_frame;
			_platform.shutdown = d3d11::core::shutdown_device;

			_platform.mesh.add = d3d11::content::mesh::add_mesh;
			_platform.mesh.render = d3d11::content::mesh::render_mesh;
			_platform.mesh.release = d3d11::content::mesh::release_mesh;
			content::mesh::set_platform(_platform.mesh);

			_platform.texture.add = d3d11::content::texture::add;
			_platform.texture.bind = d3d11::content::texture::bind;
			_platform.texture.release = d3d11::content::texture::release;
			content::texture::set_platform(_platform.texture);
		}
	}

	void set_platform(platform platform)
	{
		if (platform == platform::d3d11)
			set_d3d11();
		else throw std::logic_error("invalid graphics api");
	}

	bool initialize()
	{
		camera::initialize();
		return _platform.initialize();
	}

	void begin_frame()
	{
		camera::update();
		_platform.begin_frame();
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
