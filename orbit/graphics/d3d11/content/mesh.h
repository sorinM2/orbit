#pragma once 
#include "orbit/content/mesh.h"
#include <d3d11.h>

namespace orbit::graphics::d3d11::content
{
	class mesh : public ::orbit::content::mesh
	{
	public:
		void bind_buffers();
		void initialize();
		void release();
	private:
		ID3D11Buffer* _vertex_buffer = nullptr;
		ID3D11Buffer* _index_buffer = nullptr;


	};

	
}