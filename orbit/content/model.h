#pragma once
#include "mesh.h"
#include "texture.h"

#include "orbit/utility/vector.h"
#include "orbit/utility/freelist.h"
#include <filesystem>

#include <unordered_set>

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

		void render(const components::transform& transform) const;
		void Release();

		[[nodiscard]] std::string get_name() const { return _path.filename().string(); }

	private:
		texture::handle_type _texture;
		std::filesystem::path _path;

		void process_scene(const aiScene* scene);
		utl::vector<mesh::handle_type> _meshes;
	};

	DEFINE_LIST_TYPE(model)

	model& get_model(handle_type handle);
	handle_type add_model(const std::filesystem::path& model_path);
	void remove_model(const handle_type& model_handle);
	void render_model(const handle_type& model_handle, const components::transform& transform);

	const list_type& get_models_view();
	const std::unordered_set<handle_type, hash_type>& get_handles();

}