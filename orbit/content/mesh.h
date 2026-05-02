#pragma once 
#include "orbit/utility/vector.h"
#include <glm/glm.hpp>
#include "orbit/utility/freelist.h"
#include <functional>

#include "orbit/graphics/common/buffer.h"

namespace orbit::content::mesh
{
	struct mesh_data
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

	class mesh
	{
	public:
		explicit mesh(const mesh_data& data);
		~mesh(){ util::safe_release(_vertex_buffer); util::safe_release(_index_buffer); }
		void render();
	private:
		unsigned int _vertices_count;
		unsigned int _indices_count;

		graphics::buffer* _vertex_buffer = nullptr;
		graphics::buffer* _index_buffer = nullptr;
	};

	DEFINE_LIST_TYPE(mesh)

	handle_type add_mesh(const mesh_data& mesh_data);
	mesh& get_mesh(const handle_type& handle);
	void remove_mesh(const handle_type& handle);
	void render(const handle_type& handle);
}