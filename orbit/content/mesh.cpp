#include "mesh.h"
#include <unordered_set>

namespace orbit::content::mesh
{
	namespace
	{
		list_type _meshes;
		std::unordered_set<handle_type, hash_type> _handles;
	}

	namespace
	{
		platform_descriptor _platform;
	}

	handle_type add_mesh(const mesh& mesh)
	{
		handle_type handle = _meshes.emplace(mesh);
		_handles.insert(handle);
		_platform.add(handle);
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

		_platform.release(handle);
		_meshes.erase(handle);
		_handles.erase(handle);
	}

	void render(const handle_type& handle)
	{
		_platform.render(handle);
	}

	void set_platform(const platform_descriptor& platform_descriptor)
	{
		_platform = platform_descriptor;
	}


}