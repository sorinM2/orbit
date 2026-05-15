#pragma once 
#include "orbit/utility/vector.h"
#include <glm/glm.hpp>
#include "orbit/utility/freelist.h"
#include <functional>

#include "orbit/graphics/common/buffer.h"
#include "material.h"

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

		material::handle_type _material;
	};

	class mesh
	{
	public:
		explicit mesh(const mesh_data& data);
		~mesh(){ util::safe_release(_vertex_buffer); util::safe_release(_index_buffer); }
		mesh(const mesh& other)
		{
			_vertex_buffer = other._vertex_buffer;
			_index_buffer = other._index_buffer;
			_vertices_count = other._vertices_count;
			_indices_count = other._indices_count;

			if ( _vertex_buffer )
				_vertex_buffer->add_ref();
			if ( _index_buffer )
				_index_buffer->add_ref();
		}

		mesh(mesh&& other) noexcept
		{
			_vertex_buffer = other._vertex_buffer;
			_index_buffer = other._index_buffer;
			_vertices_count = other._vertices_count;
			_indices_count = other._indices_count;

			if ( _vertex_buffer )
				_vertex_buffer->add_ref();
			if ( _index_buffer )
				_index_buffer->add_ref();
		}

		void render();
	private:
		material::handle_type _material;

		unsigned int _vertices_count;
		unsigned int _indices_count;

		graphics::buffer* _vertex_buffer = nullptr;
		graphics::buffer* _index_buffer = nullptr;
	};

	DEFINE_LIST_TYPE(mesh)

	void shutdown();

	handle_type add_mesh(const mesh_data& mesh_data);
	mesh& get_mesh(const handle_type& handle);
	void remove_mesh(const handle_type& handle);
	void render(const handle_type& handle);
}