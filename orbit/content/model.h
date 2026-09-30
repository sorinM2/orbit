#pragma once
#include "mesh.h"
#include "material.h"

#include "orbit/utility/vector.h"
#include "orbit/utility/freelist.h"
#include <filesystem>

#include <unordered_set>
#include <unordered_map>

#include "orbit/graphics/common/shader_resource.h"
#include "orbit/graphics/common/buffer.h"

#include "entt/entt.hpp"

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
			_instance_buffers_entities = other._instance_buffers_entities;
			_instance_buffers_data = other._instance_buffers_data;
			_instance_buffer_chunks = other._instance_buffer_chunks;
			_chunk_buffer_desc = other._chunk_buffer_desc;
		}

		model(model&& other) noexcept
		{
			_meshes = other._meshes;
			other._meshes.clear();
			_materials = other._materials;
			other._materials.clear();
			_path = other._path;
			other._path.clear();
			_instance_buffers_entities = other._instance_buffers_entities;
			other._instance_buffers_entities.clear();
			_instance_buffers_data = other._instance_buffers_data;
			other._instance_buffers_data.clear();
			_instance_buffer_chunks = other._instance_buffer_chunks;
			other._instance_buffer_chunks.clear();
			_chunk_buffer_desc = other._chunk_buffer_desc;
		}

		[[nodiscard]] std::string get_name() const { return _path.filename().string(); }

	private:
		utl::vector<material::handle_type> _materials;
		std::filesystem::path _path;

		void process_scene(const aiScene* scene);
		static void create_constant_texture_from_path(const std::filesystem::path& path, graphics::shader_resource** sr);
		utl::vector<mesh::handle_type> _meshes;

	//instancing
	public:
		void add_instance(entt::entity e);
		void remove_instance(entt::entity e);
		void render_instanced();
	private:

		void update_chunks(int needed_chunks);

		struct chunk
		{
			graphics::buffer* _buffer = nullptr;
			graphics::mapped_resource _map;
		};

		std::unordered_map<entt::entity, unsigned int> _instance_buffers_entities;


		utl::vector<glm::mat4> _instance_buffers_data;

		utl::vector<chunk> _instance_buffer_chunks;
		graphics::buffer_desc _chunk_buffer_desc;
		unsigned int _chunk_width = 1000;
	};

	DEFINE_LIST_TYPE(model)

	model& get_model(handle_type handle);
	handle_type add_model(const std::filesystem::path& model_path);

	void remove_model(const handle_type& model_handle);

	void render_model(const handle_type& model_handle, const components::transform& transforme);
	void render_model_instanced(const handle_type& model_handle);

	void shutdown();
	const list_type& get_models_view();
	const std::unordered_set<handle_type, hash_type>& get_handles();

}