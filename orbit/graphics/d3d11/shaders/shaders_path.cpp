#include "shaders_path.h"
#include <cassert>

namespace orbit::graphics::d3d11::shaders
{
	std::filesystem::path get_shaders_path()
	{
		 std::filesystem::path shaders_path(__FILE__);
		 shaders_path.remove_filename();
		 return shaders_path;
	}

	std::filesystem::path get_shader_path(const std::filesystem::path& shader_relative_path)
	{
		if (shader_relative_path.is_absolute())
			return shader_relative_path;
		return get_shaders_path() / shader_relative_path;
	}

	std::filesystem::path get_compile_output_path()
	{
		std::filesystem::path result = get_shaders_path() / "output";

		std::filesystem::create_directories(result);

		assert(std::filesystem::is_directory(result));

		return result;
	}

	std::filesystem::path get_shader_output_path(const std::filesystem::path& shader_relative_path)
	{
		std::filesystem::path shader_output_path = get_compile_output_path() / shader_relative_path;
		shader_output_path.remove_filename();
		std::filesystem::create_directories(shader_output_path);
		shader_output_path = get_compile_output_path() / shader_relative_path;
		shader_output_path.replace_extension("txt");
		return shader_output_path;
 	}

	void make_relative(std::filesystem::path& absolute_path)
	{
		absolute_path = std::filesystem::relative(absolute_path, get_shaders_path());
	}


}