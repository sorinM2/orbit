#include "mesh.h"
#include <unordered_set>

#include "assimp/code/AssetLib/3MF/3MFXmlTags.h"
#include "orbit/graphics/renderer.h"
namespace orbit::content::mesh
{
	namespace
	{
		list_type _meshes;
		std::unordered_set<handle_type, hash_type> _handles;
	}

	mesh::mesh(const mesh_data& data)
	{
		_vertices_count = data._vertices.size();
		_indices_count = data._indices.size();

		graphics::rendering_device* device = graphics::renderer::get_device();

		graphics::buffer_desc vertex_desc;
		vertex_desc.width = sizeof(mesh_data::vertex) * _vertices_count;
		vertex_desc.usage = graphics::resource_usage::resource_default_usage;
		vertex_desc.bind_flags = graphics::bind_flags::bind_flag_vertex_buffer;
		vertex_desc.cpu_access_flags = 0;
		vertex_desc.initial_data = data._vertices.data();

		device->create_buffer(vertex_desc, &_vertex_buffer);

		graphics::buffer_desc index_desc;
		index_desc.width = sizeof(unsigned int) * _indices_count;
		index_desc.usage = graphics::resource_usage::resource_default_usage;
		index_desc.bind_flags = graphics::bind_flags::bind_flag_index_buffer;
		index_desc.cpu_access_flags = 0;
		index_desc.initial_data = data._indices.data();

		device->create_buffer(index_desc, &_index_buffer);
	}

	void mesh::render()
	{
		graphics::rendering_device_context* context = graphics::renderer::get_context();
		context->set_index_buffer(_index_buffer);
		unsigned int stride = sizeof(mesh_data::vertex);
		context->set_vertex_buffers(1, &stride, &_vertex_buffer);
		context->draw_indexed(_indices_count);
	}


	handle_type add_mesh(const mesh_data& mesh_data)
	{
		handle_type handle = _meshes.emplace(mesh_data);
		_handles.insert(handle);
		return handle;
	}

	mesh& get_mesh(const handle_type& handle)
	{
		return _meshes.get(handle);
	}

	void remove_mesh(const handle_type& handle)
	{
		if(!_meshes.is_alive(handle))
			return;

		_meshes.erase(handle);
		_handles.erase(handle);
	}

	void render(const handle_type& handle)
	{
		get_mesh(handle).render();
	}
}