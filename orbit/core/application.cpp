#include "application.h"

#include "platform/platform_common.h"
#include "orbit/graphics/platform.h"
#include "orbit/content/mesh.h"
#include "orbit/content/model.h"
#include "orbit/content/texture.h"

namespace orbit::application
{

	// float vertices[] = {
	// 	// positions // normals // texture coords
	// 	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	// 	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
	// 	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
	// 	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	// 	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	// 	0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	// 	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	// 	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	// 	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	// 	-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	// 	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	// 	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	// 	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	// 	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	// 	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	// 	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	// 	0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
	// 	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	// 	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	// 	-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	// 	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	// 	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	// 	0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	// 	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	// 	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	// 	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	// 	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	// };

	namespace
	{
		//content::mesh::handle_type triangle_handle;
		content::model::handle_type model_handle;
		content::texture::handle_type texture_handle;
	}

	bool initialize()
	{
		bool result;

		graphics::platform::set_platform(graphics::platform::platform::d3d11);
		result = graphics::platform::initialize();
	
		if (!result)
		{
			MessageBox(0, "Failed to initialize d3d11!", "ERROR", MB_OK);
			return false;
		}

		// content::mesh::mesh triangle;
		//
		// for ( unsigned int i = 0; i < sizeof(vertices) / sizeof(vertices[0]); i+=8 )
		// {
		// 	triangle._indices.emplace_back(i / 8);
		// 	content::mesh::mesh::vertex v{};
		//
		// 	v = {
		// 		glm::vec3(vertices[i], vertices[i+1], vertices[i+2]), glm::vec3(vertices[i+3], vertices[i+4], vertices[i+5]),
		// 			glm::vec2(vertices[i+6], vertices[i+7])
		// 	};
		// 	triangle._vertices.emplace_back(v);
		//
		// }
		//
		// triangle_handle = content::mesh::add_mesh(triangle);

		model_handle = content::model::add_model(std::filesystem::path("C:\\dev\\orbit\\orbit\\assets\\frank.glb"));
		//texture_handle = content::texture::add_texture(std::filesystem::path("C:\\dev\\orbit\\orbit\\assets\\poza.jpg"));
		//content::texture::bind_texture(texture_handle);
		return true;
	}

	void restart_graphics()
	{
		graphics::platform::shutdown();
		graphics::platform::initialize();
	}

	void update()
	{
		graphics::platform::begin_frame();
		//content::mesh::render(triangle_handle);
		content::model::render_model(model_handle);
		graphics::platform::end_frame();
	}

	void shutdown()
	{
		graphics::platform::shutdown();
	}
}