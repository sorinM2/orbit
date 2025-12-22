#pragma once 
#include "utility/vector.h"
#include "core/primitive_types.h"

namespace orbit::content::model
{
	class model
	{
	public:
		struct vertex
		{
			vec3 position;
			vec2 normal;
		};
	private:
		utl::vector<vertex> vertices;
	};
}