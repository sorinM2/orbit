#include "mesh.h"
#include "..\d3d11core.h"
#include "..\d3d11common.h"
#include <unordered_map>


namespace orbit::graphics::d3d11::content::mesh
{
	namespace
	{
		using vertex = hl::mesh_type::vertex;
	}

	namespace
	{
		list_type meshes;
		std::unordered_map<hl::handle_type, handle_type, hl::hash_type> hl_handles;
	}

	namespace
	{
		ID3D11Buffer* _world_cbuffer = nullptr;
	}

	void add_mesh(const hl::handle_type& hl_handle)
	{
		assert(!hl_handles.contains(hl_handle));
		if (hl_handles.contains(hl_handle))
			throw std::logic_error("low level mesh already exists");

		handle_type mesh_handle = meshes.emplace();
		mesh& mesh = meshes.get(mesh_handle);
		hl::mesh_type& hl_mesh = ::orbit::content::mesh::get_mesh(hl_handle);
		mesh.initialize(hl_mesh);

		hl_handles[hl_handle] = mesh_handle;
	}

	void render_mesh(const hl::handle_type& hl_handle)
	{
		assert(hl_handles.contains(hl_handle));
		if (!hl_handles.contains(hl_handle))
			throw std::logic_error("low level mesh doesn't exists");
		mesh& mesh = meshes.get(hl_handles[hl_handle]);
		mesh.bind_buffers();
		mesh.render();
	}

	void release_mesh(const hl::handle_type& hl_handle)
	{
		assert(hl_handles.contains(hl_handle));
		if (!hl_handles.contains(hl_handle))
			throw std::logic_error("low level mesh doesn't exists");
		mesh& mesh = meshes.get(hl_handles[hl_handle]);
		mesh.Release();

		meshes.erase(hl_handles[hl_handle]);
		hl_handles.erase(hl_handle);
	}

	void mesh::bind_buffers()
	{
		unsigned int stride = sizeof(vertex);
		unsigned int offset = 0;
		ID3D11DeviceContext* device_context = core::get_device_context();
		device_context->IASetVertexBuffers(0, 1, &_vertex_buffer, &stride, &offset);
		device_context->IASetIndexBuffer(_index_buffer, DXGI_FORMAT_R32_UINT, 0);
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void mesh::initialize(const hl::mesh_type& hl_mesh)
	{
		auto& _vertices = hl_mesh._vertices;
		auto& _indices = hl_mesh._indices;

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

		_indices_count = _indices.size();
		_vertices_count = _vertices.size();
	}

	void mesh::render()
	{
		ID3D11DeviceContext* device_context = core::get_device_context();
		device_context->DrawIndexed(_indices_count, 0, 0);
	}

	void mesh::Release()
	{
		util::safe_release(_vertex_buffer);
		util::safe_release(_index_buffer);
	}

	void initialize()
	{

		D3D11_BUFFER_DESC world_buffer_desc;
		world_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		world_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		world_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		world_buffer_desc.MiscFlags = 0;
		world_buffer_desc.StructureByteStride = 0;
		world_buffer_desc.ByteWidth = sizeof(glm::mat4);

		glm::mat4 initial_data = glm::mat4(1.f);

		D3D11_SUBRESOURCE_DATA world_buffer_data;
		world_buffer_data.pSysMem = &initial_data;
		world_buffer_data.SysMemPitch = 0;
		world_buffer_data.SysMemSlicePitch = 0;

		auto* _device = core::get_device();
		auto* _device_context = core::get_device_context();

		DXCALL(_device->CreateBuffer(&world_buffer_desc, &world_buffer_data, &_world_cbuffer));

		_device_context->VSSetConstantBuffers(0, 1, &_world_cbuffer);
	}

	void bind_world(const glm::mat4& world_matrix)
	{
		auto* _device_context = core::get_device_context();

		D3D11_MAPPED_SUBRESOURCE msr;
		_device_context->Map(_world_cbuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
		memcpy(msr.pData, &world_matrix, sizeof(world_matrix));
		_device_context->Unmap(_world_cbuffer, 0u);
	}

	void shutdown()
	{
		util::safe_release(_world_cbuffer);
	}

}