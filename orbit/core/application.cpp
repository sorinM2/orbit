#include "application.h"

#include "platform/platform_common.h"
#include "orbit/content/mesh.h"
#include "orbit/content/model.h"
#include "orbit/ecs/ecs.h"
#include "orbit/ecs/components.h"
#include "orbit/graphics/renderer.h"

namespace orbit::application
{

	namespace
	{
		//content::mesh::handle_type triangle_handle;
		content::model::handle_type model_handle;
		//content::texture::handle_type texture_handle;
	}

	bool initialize()
	{
		graphics::renderer::initialize();



		model_handle = content::model::add_model(std::filesystem::path("C:\\dev\\orbit\\orbit\\assets\\frank.glb"));

		return true;
	}

	void restart_graphics()
	{
		// graphics::platform::shutdown();
		// graphics::platform::initialize(false);
	}

	void update()
	{
		graphics::renderer::begin_frame();
		//content::mesh::render(triangle_handle);
		//content::model::render_model(model_handle);

		entt::registry& reg = ecs::get_instance()->registry;
		auto drawables = reg.view<components::transform, components::geometry>();

		drawables.each([](const auto entity, auto& transform, auto& geometry)
		{
			if ( geometry._handle.is_valid() )
				content::model::render_model(geometry._handle, transform);
		});

		graphics::renderer::begin_editor();
	}

	void render()
	{
		graphics::renderer::end_frame();
	}

	void shutdown()
	{
		content::model::remove_model(model_handle);
		graphics::renderer::shutdown();
	}
}