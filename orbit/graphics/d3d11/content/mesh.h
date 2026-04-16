#pragma once 
#include "orbit/content/mesh.h"
#include <d3d11.h>
#include "orbit/utility/freelist.h"

namespace orbit::graphics::d3d11::content::mesh
{


	namespace hl
	{
		using mesh_type = ::orbit::content::mesh::mesh;
		DEFINE_LIST_TYPE(::orbit::content::mesh::mesh)
	}

	class mesh
	{
	public:
		void bind_buffers();
		void initialize(const hl::mesh_type& hl_mesh);
		void render();
		void release();
	private:

		unsigned int _vertices_count;
		unsigned int _indices_count;

		ID3D11Buffer* _vertex_buffer = nullptr;
		ID3D11Buffer* _index_buffer = nullptr;
	};

	DEFINE_LIST_TYPE(mesh)

	void add_mesh(const hl::handle_type& hl_handle);
	void render_mesh(const hl::handle_type& hl_handle);
	void release_mesh(const hl::handle_type& hl_handle);

}