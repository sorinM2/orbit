#pragma once
#include <filesystem>

namespace orbit::graphics::d3d11::shaders
{
	std::filesystem::path get_shaders_path();
	std::filesystem::path get_shader_path(const std::filesystem::path& shader_relative_path);
	std::filesystem::path get_compile_output_path();
	std::filesystem::path get_shader_output_path(const std::filesystem::path& shader_relative_path);
	void make_relative(std::filesystem::path& absolute_path);
}