#include "model.h"
#include "model.h"
#include "glm/gtc/matrix_transform.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <fstream>
#include <queue>

namespace orbit::content::model
{
	namespace
	{
		list_type _models;
	}

	model::model(const std::filesystem::path& model_path) : _path(model_path)
	{
		_transform.scale = glm::vec3(1.f, 1.f, 1.f);
		_transform.rotation = glm::vec3(0.f, 0.f, 0.f);
		_transform.position = glm::vec3(0.f, 0.f, 0.f);

		compute_world_matrix();

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(model_path.string().c_str(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		process_scene(scene);
	}

	model::~model()
	{
		// for ( auto& mesh_handle : _meshes )
		// 	mesh::remove_mesh(mesh_handle);
	}


	void model::process_scene(const aiScene* scene)
	{
		unsigned int no_meshes = scene->mNumMeshes;
		unsigned int no_materials = scene->mNumMaterials;

		utl::vector<mesh::handle_type> unique_meshes;
		unique_meshes.reserve(no_meshes);

		for ( int current_mesh = 0; current_mesh < no_meshes; ++current_mesh )
		{
			mesh::mesh mesh;

			aiMesh* ai_mesh = scene->mMeshes[current_mesh];

			unsigned int no_vertices = ai_mesh->mNumVertices;
			unsigned int no_faces = ai_mesh->mNumFaces;

			mesh._vertices.reserve(no_vertices);

			for ( int i = 0; i < no_vertices; ++i )
			{
				mesh::mesh::vertex vertex{};
				vertex.position = glm::vec3(ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z);
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
					_texture = texture::add_texture(dest_path);
				}
				else _texture = texture::add_texture(path);
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

	void model::set_position(glm::vec3 position)
	{
		if (position == _transform.position)
			return;
	}

	void model::set_rotation(glm::vec3 rotation)
	{
		if (rotation == _transform.rotation)
			return;
	}

	void model::set_scale(glm::vec3 scale)
	{
		if (scale == _transform.scale)
			return;
	}

	void model::compute_world_matrix()
	{
		glm::mat4& world_matrix = _wvp_buffer.world;

		world_matrix = glm::mat4(1.f);
		
		world_matrix = glm::translate(world_matrix, _transform.position);
		world_matrix = glm::rotate(world_matrix, _transform.rotation.x, glm::vec3(1.f, 0.f, 0.f));
		world_matrix = glm::rotate(world_matrix, _transform.rotation.y, glm::vec3(0.f, 1.f, 0.f));
		world_matrix = glm::rotate(world_matrix, _transform.rotation.z, glm::vec3(0.f, 0.f, 1.f));

		world_matrix = glm::scale(world_matrix, _transform.scale);
	}

	handle_type add_model(const std::filesystem::path& model_path)
	{
		handle_type handle = _models.emplace(model_path);
		return handle;
	}

	void remove_model(const handle_type& model_handle)
	{
		if( !_models.is_alive(model_handle) )
			return;

		_models.erase(model_handle);
	}

	void render_model(const handle_type& model_handle)
	{
		if( !_models.is_alive(model_handle) )
			return;

		_models.get(model_handle).render();
	}

	void model::render()
	{
		if (_texture.is_valid())
			texture::bind_texture(_texture);
		for ( auto& mesh_handle : _meshes )
			mesh::render(mesh_handle);
	}
}