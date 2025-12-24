#pragma once 
#include "utility/vector.h"
#include <glm/glm.hpp>

namespace orbit::content
{
	struct mesh
	{
		struct vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			//glm::vec2 uv;
		};

		utl::vector<vertex> _vertices;
		utl::vector<unsigned int> _indices;
	};
}