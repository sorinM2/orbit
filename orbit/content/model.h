#pragma once
#include "mesh.h"
#include "material.h"

#include "orbit/utility/vector.h"
#include "orbit/utility/freelist.h"
#include <filesystem>

#include <unordered_set>
#include <memory>

#include "orbit/graphics/common/shader_resource.h"

struct aiScene;

namespace orbit::components
{
	struct transform;
}

namespace orbit::content::model
{
	class model
	{
	public:
		explicit model(const std::filesystem::path& model_path);

		void render(const components::transform& transform);
		~model();

		model(const model& other)
		{
			_meshes = other._meshes;
			_materials = other._materials;
			_path = other._path;
		}

		model(model&& other) noexcept
		{
			_meshes = other._meshes;
			other._meshes.clear();
			_materials = other._materials;
			other._materials.clear();
			_path = other._path;
			other._path.clear();
		}

		[[nodiscard]] std::string get_name() const { return _path.filename().string(); }

	private:
		utl::vector<material::handle_type> _materials;
		std::filesystem::path _path;

		void process_scene(const aiScene* scene);
		static void create_constant_texture_from_path(const std::filesystem::path& path, graphics::shader_resource** sr);
		utl::vector<mesh::handle_type> _meshes;
	};

	DEFINE_LIST_TYPE(model)

	model& get_model(handle_type handle);
	handle_type add_model(const std::filesystem::path& model_path);
	void remove_model(const handle_type& model_handle);
	void render_model(const handle_type& model_handle, const components::transform& transform);

	void shutdown();
	const list_type& get_models_view();
	const std::unordered_set<handle_type, hash_type>& get_handles();

}