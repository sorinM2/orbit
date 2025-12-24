#pragma once
#include "../d3d11common.h"
#include <filesystem>
#include <string>

namespace orbit::graphics::d3d11::shaders
{
	class shader
	{
	public:
		bool initialize();
		void bind();
	public:
		std::string _vertex_shader_entry_point;
		std::string _pixel_shader_entry_point;

		std::filesystem::path _vertex_shader_path;
		std::filesystem::path _pixel_shader_path;
	private:
		ID3D11VertexShader* _vertex_shader;
		ID3D11PixelShader* _pixel_shader;

		ID3D11InputLayout* _input_layout;
	};
}