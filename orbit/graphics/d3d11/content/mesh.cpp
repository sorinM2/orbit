#include "mesh.h"
#include "..\d3d11core.h"
#include "..\d3d11common.h"

namespace orbit::graphics::d3d11::content
{
	void mesh::bind_buffers()
	{
		unsigned int stride = sizeof(vertex);
		unsigned int offset = 0;
		ID3D11DeviceContext* device_context = core::get_device_context();
		device_context->IASetVertexBuffers(0, 1, &_vertex_buffer, &stride, &offset);
		device_context->IASetIndexBuffer(_index_buffer, DXGI_FORMAT_R32_UINT, 0);
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void mesh::initialize()
	{
		assert(!_vertex_buffer && !_index_buffer);
		assert(_vertices.size() && _indices.size());

		D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
		D3D11_SUBRESOURCE_DATA vertex_bufer_data, index_buffer_data;

		vertex_buffer_desc.ByteWidth = sizeof(vertex) * _vertices.size();
		vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertex_buffer_desc.CPUAccessFlags = 0;
		vertex_buffer_desc.MiscFlags = 0;
		vertex_buffer_desc.StructureByteStride = 0;

		index_buffer_desc.ByteWidth = sizeof(unsigned int) * _indices.size();
		index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		index_buffer_desc.CPUAccessFlags = 0;
		index_buffer_desc.MiscFlags = 0;
		index_buffer_desc.StructureByteStride = 0;

		vertex_bufer_data.pSysMem = _vertices.data();
		vertex_bufer_data.SysMemPitch = 0;
		vertex_bufer_data.SysMemSlicePitch = 0;

		index_buffer_data.pSysMem = _indices.data();
		index_buffer_data.SysMemPitch = 0;
		index_buffer_data.SysMemSlicePitch = 0;

		ID3D11Device* device = core::get_device();

		DXCALL(device->CreateBuffer(&vertex_buffer_desc, &vertex_bufer_data, &_vertex_buffer));
		DXCALL(device->CreateBuffer(&index_buffer_desc, &index_buffer_data, &_index_buffer));

		_indices.clear();
		_vertices.clear();
	}

	void mesh::release()
	{
		util::safe_release(_vertex_buffer);
		util::safe_release(_index_buffer);
	}
}