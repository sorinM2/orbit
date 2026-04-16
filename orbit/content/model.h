#pragma once
#include "mesh.h"
#include "texture.h"

#include "orbit/utility/vector.h"
#include "orbit/utility/freelist.h"
#include <filesystem>

struct aiScene;

namespace orbit::content::model
{
	class model
	{
	public:
		struct world_view_projection_buffer
		{
			glm::mat4 world;
		} _wvp_buffer;

		struct transform
		{
			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 scale;
		} _transform;

		explicit model(const std::filesystem::path& model_path);
		~model();

		void set_position(glm::vec3 position);
		void set_rotation(glm::vec3 rotation);
		void set_scale(glm::vec3 scale);

		void render();
	private:
		void compute_world_matrix();

	private:
		texture::handle_type _texture;
		std::filesystem::path _path;

		void process_scene(const aiScene* scene);
		utl::vector<mesh::handle_type> _meshes;
	};

	DEFINE_LIST_TYPE(model)

	handle_type add_model(const std::filesystem::path& model_path);
	void remove_model(const handle_type& model_handle);
	void render_model(const handle_type& model_handle);
}