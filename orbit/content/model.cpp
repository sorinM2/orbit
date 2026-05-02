#include "model.h"
#include "glm/gtc/matrix_transform.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <fstream>
#include <queue>

#include "orbit/ecs/components.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/sdbi.h"

#include "orbit/graphics/renderer.h"

namespace orbit::content::model
{
	namespace
	{
		list_type _models;
		std::unordered_set<handle_type, hash_type> _handles;
	}

	const list_type& get_models_view()
	{
		return _models;
	}

	const std::unordered_set<handle_type, hash_type>& get_handles()
	{
		return _handles;
	}

	model::model(const std::filesystem::path& model_path) : _path(model_path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(model_path.string().c_str(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		process_scene(scene);
	}

	void model::Release()
	{
		for ( auto mesh_handle : _meshes )
			mesh::remove_mesh(mesh_handle);
		util::safe_release(_texture_shader_resource);
	}

	void model::create_constant_texture_from_path(const std::filesystem::path& path)
	{
		graphics::texture2D* new_tex = nullptr;
		graphics::rendering_device* device = graphics::renderer::get_device();

		int width, height, channels;
		unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 4);

		graphics::texture2D_desc desc;
		desc.width = width;
		desc.height = height;
		desc.bind_flags = graphics::bind_flags::bind_flag_shader_resource;// | graphics::bind_flags::bind_flag_render_target;
		desc.cpu_access_flags = 0;
		desc.format = graphics::format::FORMAT_R8G8B8A8_UNORM;
		desc.initial_data = data;
		desc.mips = 1;
		desc.sample_desc.count = 1;
		desc.sample_desc.quality = 0;
		desc.usage = graphics::resource_usage::resource_default_usage;

		device->create_texture2D(desc, &new_tex);

		graphics::shader_resource_desc sr_desc;
		sr_desc.format =  graphics::format::FORMAT_R8G8B8A8_UNORM;
		sr_desc.type = graphics::shader_resource_type::texture2D;

		device->create_shader_resource(sr_desc, new_tex, &_texture_shader_resource);

		util::safe_release(new_tex);
		stbi_image_free(data);
	}

	void model::process_scene(const aiScene* scene)
	{
		unsigned int no_meshes = scene->mNumMeshes;
		unsigned int no_materials = scene->mNumMaterials;

		utl::vector<mesh::handle_type> unique_meshes;
		unique_meshes.reserve(no_meshes);

		for ( int current_mesh = 0; current_mesh < no_meshes; ++current_mesh )
		{
			mesh::mesh_data mesh;

			aiMesh* ai_mesh = scene->mMeshes[current_mesh];

			unsigned int no_vertices = ai_mesh->mNumVertices;
			unsigned int no_faces = ai_mesh->mNumFaces;

			mesh._vertices.reserve(no_vertices);

			for ( int i = 0; i < no_vertices; ++i )
			{
				mesh::mesh_data::vertex vertex{};
				vertex.position = glm::vec3(ai_mesh->mVertices[i].x * 0.1, ai_mesh->mVertices[i].y * 0.1, ai_mesh->mVertices[i].z * 0.1);
				vertex.normal = glm::vec3(ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z);

				vertex.uv = glm::vec2(ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y);

				mesh._vertices.emplace_back(vertex);
			}

			for ( int i = 0; i < no_faces; ++i )
			{
				aiFace face = ai_mesh->mFaces[i];
				assert(face.mNumIndices == 3);
				for ( int j = 0; j < face.mNumIndices; ++j )
					mesh._indices.emplace_back(face.mIndices[j]);
			}

			mesh::handle_type mesh_handle = mesh::add_mesh(mesh);
			unique_meshes.emplace_back(mesh_handle);
		}

		std::queue<aiNode*> q;
		q.emplace(scene->mRootNode);

		if ( no_materials )
		{
			aiMaterial* ai_material = scene->mMaterials[0];
			aiString ai_path;
			ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_path );
			std::string path_s(ai_path.C_Str());
			std::filesystem::path path(path_s);
			unsigned int texture_index = std::stoi(path_s.substr(1));


			unsigned int no_textures = ai_material->GetTextureCount(aiTextureType_DIFFUSE);

			if ( no_textures )
			{
				if ( path_s[0] == '*' )
				{
					aiTexture* ai_texture = scene->mTextures[texture_index];

					std::filesystem::path dest_path = _path.parent_path() / "_tex";
					dest_path.replace_extension(ai_texture->achFormatHint);
					std::ofstream file(dest_path, std::ios::binary);

					assert(ai_texture->mHeight == 0 );

					file.write(reinterpret_cast<const char*>(ai_texture->pcData), ai_texture->mWidth);
					file.close();
					create_constant_texture_from_path(dest_path);
				}
				else create_constant_texture_from_path(path);
			}
		}

		while (!q.empty())
		{
			aiNode* node = q.front();
			q.pop();

			unsigned int no_node_meshes = node->mNumMeshes;
			for ( int i = 0; i < no_node_meshes; ++i )
				_meshes.emplace_back(unique_meshes[node->mMeshes[i]]);

			for ( int i = 0; i < node->mNumChildren; ++i )
				q.emplace(node->mChildren[i]);
		}
	}

	handle_type add_model(const std::filesystem::path& model_path)
	{
		handle_type handle = _models.emplace(model_path);
		_handles.emplace(handle);
		return handle;
	}

	void remove_model(const handle_type& model_handle)
	{
		if( !_models.is_alive(model_handle) )
			return;

		model& model = _models.get(model_handle);
		model.Release();

		_models.erase(model_handle);
		_handles.erase(model_handle);
	}

	void render_model(const handle_type& model_handle,const components::transform& transform)
	{
		if( !_models.is_alive(model_handle) )
			return;

		_models.get(model_handle).render(transform);
	}

	model& get_model(handle_type handle)
	{
		return _models.get(handle);
	}

	void model::render(const components::transform& transform)
	{
		graphics::rendering_device_context* context = graphics::renderer::get_context();
		context->ps_set_shader_resources(&_texture_shader_resource, 1, 0);

		glm::mat4 world_matrix = glm::mat4(1.f);

		world_matrix = glm::translate(world_matrix, transform.position);

		world_matrix = glm::rotate(world_matrix, glm::radians(transform.rotation.z), glm::vec3(0.f, 0.f, 1.f));
		world_matrix = glm::rotate(world_matrix, glm::radians(transform.rotation.y), glm::vec3(0.f, 1.f, 0.f));
		world_matrix = glm::rotate(world_matrix, glm::radians(transform.rotation.x), glm::vec3(1.f, 0.f, 0.f));

		world_matrix = glm::scale(world_matrix, transform.scale);

		world_matrix = glm::transpose(world_matrix);

		graphics::renderer::bind_world(world_matrix);

		for ( auto& mesh_handle : _meshes )
			mesh::render(mesh_handle);
	}
}