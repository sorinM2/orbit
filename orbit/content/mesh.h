#pragma once 
#include "orbit/utility/vector.h"
#include <glm/glm.hpp>
#include "orbit/utility/freelist.h"
#include <functional>

namespace orbit::content::mesh
{
	struct mesh
	{
		struct vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 uv;
		};

		utl::vector<vertex> _vertices;
		utl::vector<unsigned int> _indices;
	};

	DEFINE_LIST_TYPE(mesh)

	handle_type add_mesh(const mesh& mesh);
	mesh& get_mesh(const handle_type& handle);
	void remove_mesh(const handle_type& handle);

	void render(const handle_type& handle);

	struct platform_descriptor
	{
		std::function<void(const handle_type&)> add;
		std::function<void(const handle_type&)> render;
		std::function<void(const handle_type&)> release;

		std::function<void(const glm::mat4&)> bind_world;
	};

	void set_platform(const platform_descriptor& platform_descriptor);
}