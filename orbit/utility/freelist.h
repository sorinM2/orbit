#pragma once

#include "vector.h"
#include "id.h"
#include <utility>
#include <type_traits>
#include <exception>
#include <functional>

#define DEFINE_LIST_TYPE(type) \
using list_type = utl::freelist<type, 0>; \
using handle_type = list_type::id_handle; \
using hash_type = list_type::handle_hash;

namespace utl
{
	template<typename U, unsigned int _repeat_index = 1024>
	class freelist;

	template <typename T, unsigned int _repeat_index>
	class freelist
	{
	public:

		class id_handle
		{
			friend class freelist;
			constexpr id_handle(id::id_type id) : _id{ id } {}

			id::id_type _id{ id::invalid_id };
		public:
			bool is_valid() const { return _id != id::invalid_id; }
			constexpr id_handle(){}
			constexpr id::id_type get_id() const  { return _id; }
			constexpr auto operator<=>(const id_handle& other) const = default;
			constexpr bool operator==(const id_handle& other) const {
				return _id == other._id;
			}
		};

		struct handle_hash
		{
			std::size_t operator()(const id_handle& key) const 
			{
				return std::hash<id::id_type>()(key.get_id());
			}
		};

		constexpr freelist() {}
		constexpr virtual ~freelist() {}

		constexpr freelist& operator=(freelist&& other)
		{
			_container = std::move(other._container);
			_generations = std::move(other._generations);
		}

		constexpr freelist(freelist&& other)
		{
			_container = std::move(other._container);
			_generations = std::move(other._generations);
		}

		constexpr freelist(const freelist& other) : freelist()
		{
			_generations = other._generations;
			_container = other._container;
		}

		constexpr freelist& operator=(const freelist& other)
		{
			
			_generations = other._generations;
			_container = other._container;
		}

		template<typename... Args>
		constexpr id_handle emplace(Args&&... args)
		{
			id::id_type index = _container.emplace_tombstone(std::forward<Args>(args)...);
			if (index >= _generations.size())
			{
				assert(index == _generations.size());
				_generations.emplace_back(0);
			}

			assert(_generations.size() == _container.size());

			auto gen_id = id::set_generation(index, _generations[index]);

			return id_handle(gen_id);
		}

		constexpr T& get(id_handle id)
		{
			assert(is_alive(id));

			if (!is_alive(id))
				throw std::runtime_error("id generation doesn't match");

			id::id_type internal_id = id.get_id();
			id::id_type index = id::index(internal_id);

			assert(index < _container.size());

			return _container[index];
		}

		constexpr const T& get(id_handle id) const
		{
			assert(is_alive(id));

			if (!is_alive(id))
				throw std::runtime_error("id generation doesn't match");

			id::id_type internal_id = id.get_id();
			id::id_type index = id::index(internal_id);

			assert(index < _container.size());

			return _container[index];
		}

		constexpr void erase(id_handle id)
		{
			assert(is_alive(id));
			if (!is_alive(id))
				throw std::runtime_error("tried to erase invalid handle from freelist");
			id::id_type index = id::index(id.get_id());

			assert(!_container.is_tombstone(_container.internal_begin() + index));

			_generations[index]++;
			_container.erase(_container.internal_begin() + index);
		}

		constexpr bool is_alive(id_handle id)
		{
			if ( !id.is_valid() ) return false;

			id::id_type internal_id = id.get_id();
			id::id_type index = id::index(internal_id);
			id::generation_type generation = id::generation(internal_id);

			assert(_generations.size() == _container.size());

			return _generations[index] == generation;
		}

		constexpr decltype(auto) begin() const
		{
			return _container.begin();
		}

		constexpr decltype(auto) end() const
		{
			return _container.end();
		}
	private:
		vector<id::generation_type> _generations;
		vector<T, false, _repeat_index> _container;
	};


}