#pragma once
#include <cassert>
#include "primitive_types.h"
#include <type_traits>
#include <iostream>
#include <type_traits>

namespace id
{
	using id_type = u32;

	constexpr u32 generation_bits = 12;
	constexpr u32 index_bits = sizeof(id_type) * 8 - generation_bits;

	constexpr id_type index_mask = (id_type(1) << index_bits) - 1;
	constexpr id_type generation_mask = ~index_mask;

	constexpr id_type invalid_id = index_mask;

	using generation_type = std::conditional <
		generation_bits <= 16,
		std::conditional<generation_bits <= 8, u8, u16>::type,
		std::conditional<generation_bits <= 32, u32, u64>::type
	>::type;

	constexpr bool is_valid(id_type id)
	{
		return (id & index_mask) != index_mask;
	}

	constexpr generation_type generation(id_type id)
	{
		assert(is_valid(id));
		generation_type result = (id & generation_mask) >> index_bits;
		return result;
	}

	constexpr id_type index(id_type id)
	{
		assert(is_valid(id));
		return id & index_mask;
	}

	constexpr id_type set_generation(id_type id, unsigned int gen)
	{
		assert(is_valid(id));
		id_type ind = index(id);
		id = gen;
		id = (id << index_bits) | ind;
		return id;
	}

	constexpr id_type new_generation(id_type id)
	{
		assert(is_valid(id));
		id_type ind = index(id);
		id >>= index_bits; ++id;
		id <<= index_bits;
		id &= generation_mask;
		id |= ind;
		return id;
	}

#define DEFINE_TYPED_ID(name) using name##_id = id::id_type;

}