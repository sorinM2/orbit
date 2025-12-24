#pragma once
#include <cassert>
#include <new>
#include <utility>
#include <algorithm>
#include <cstring>
#include <memory>
#include "iterator.h"
#include <iostream>
namespace utl
{

	template<typename T, bool disable_tombstoning = true, unsigned int _min_tombstones = 0>
	class vector
	{
	public:
		static_assert(_min_tombstones < 1e16 + 1, "Number of needed tombstones is too big!");
		static_assert(disable_tombstoning or sizeof(T) >= sizeof(int));


		vector()
		{
			reserve(1);
		}

		vector(vector& other)
		{
			*this = other;
		}

		vector(const vector& other)
		{
			*this = other;
		}

		constexpr vector& operator=(vector& other)
		{
			if (this == &other)
				return *this;

			clear();
			reserve(other.size());

			for (unsigned int i = 0; i < other.size(); ++i)
			{
				T& c = *(other.data() + i);
				if constexpr (disable_tombstoning)
					emplace_back(c);
				else if (!other.is_tombstone(&c))
					emplace_back(c);
				else {
					memset(end(), 'd', sizeof(T));
					_size++;
				}
			}

			return *this;
		}
		constexpr vector& operator=(const vector& other)
		{
			if (this == &other)
				return *this;

			clear();
			reserve(other.size());

			for (unsigned int i = 0; i < other.size(); ++i)
			{
				T& c = *(other.data() + i);
				if constexpr (disable_tombstoning)
					emplace_back(c);
				else if (!other.is_tombstone(&c))
					emplace_back(c);
				else {
					memset(end(), 'd', sizeof(T));
					_size++;
				}
			}
			return *this;
		}
		vector(vector&& other) noexcept
		{
			*this = std::move(other);
		}

		constexpr vector& operator=(vector&& other) noexcept
		{
			clear();
			::operator delete(_data);

			_data = other._data;
			_size = other._size;
			_capacity = other._capacity;

			other._data = nullptr;
			other._size = other._capacity = 0;

			return *this;
		}

		vector(unsigned int size)
		{
			reserve(std::max(size, 1u));
			for (unsigned int i = 0; i < size; ++i)
				emplace_back();
		}

		vector(unsigned int size, const T& value)
		{
			assert(size);
			resize(size, value);
		}

		constexpr void push_back(const T& value)
		{
			emplace_back(value);
		}

		constexpr void push_back(T&& value)
		{
			emplace_back(std::move(value));
		}

		template<typename... Targs>
		constexpr void emplace_back(Targs&&... args)
		{
			controlled_reserve(_size + 1);
			new (_data + _size) T(std::forward<Targs>(args)...);
			++_size;
		}

		constexpr decltype(auto) begin()
		{
			if constexpr (!disable_tombstoning)
				return iterator::iterator<vector, T, &vector::is_tombstone>(_data, this);
			else return iterator::iterator<vector, T>(_data);
		}

		constexpr decltype(auto) end()
		{
			if constexpr (!disable_tombstoning)
				return iterator::iterator<vector, T, &vector::is_tombstone>(_data + _size, this);
			else return iterator::iterator<vector, T>(_data + _size);
		}

		T* internal_begin() const
		{
			return _data;
		}

		T* internal_end() const
		{
			return _data + _size;
		}

		constexpr void insert(T* position, const T& value)
		{
			static_assert(disable_tombstoning);
			emplace(position, value);
		}

		constexpr void insert(T* position, T* range_start, T* range_end)
		{
			static_assert(disable_tombstoning);
			assert(range_end > range_start);
			assert(position >= internal_begin() && position <= internal_end());

			unsigned int spaces = range_end - range_start;

			unsigned int offset = position - internal_begin();

			controlled_reserve(_size + spaces);

			position = internal_begin() + offset;

			for (T* it = end() + spaces - 1; it >= position + spaces; --it)
			{
				new (it) T(std::move(*(it - spaces)));
				(it - spaces)->~T();
			}

			std::copy(range_start, range_end, position);

			_size += spaces;
		}


		template<typename... Targs>
		constexpr void emplace(T* position, Targs&&... args)
		{
			static_assert(disable_tombstoning);
			assert(position >= internal_begin() && position <= internal_end());

			unsigned int offset = position - internal_begin();

			controlled_reserve(_size + 1);

			position = internal_begin() + offset;

			for (T* it = end(); it > position; --it)
			{
				new (it) T(std::move(*(it - 1)));
				(it - 1)->~T();
			}

			new (position) T(std::forward<Targs>(args)...);

			++_size;
		}

		template<typename... Targs>
		unsigned int emplace_tombstone(Targs&&... args)
		{
			static_assert(!disable_tombstoning);
			if (_first_tombstone < 0 or _first_tombstone >= _size or _tombstones < _min_tombstones)
			{
				emplace_back(std::forward<Targs>(args)...);
				return _size - 1;
			}

			T* position = _data + _first_tombstone;
			assert(is_tombstone(position));

			*(_is_tombstone + _first_tombstone) = false;

			_first_tombstone = *reinterpret_cast<int*>(position);

			new (position) T(std::forward<Targs>(args)...);

			--_tombstones;

			return position - _data;
		}

		constexpr void erase(T* position)
		{
			erase(position, position + 1);
		}
#include <iostream>
		constexpr void erase(T* range_start, T* range_end)
		{
			assert(_data);
			assert(range_start <= range_end);
			assert(internal_begin() <= range_start && range_end <= internal_end());

			unsigned int spaces = range_end - range_start;
			unsigned int spaces_to_move = internal_end() - range_end;

			if constexpr (disable_tombstoning)
			{
				std::move(range_end, static_cast<T*>(end()), range_start);

				for (T* it = internal_end() - spaces; it != internal_end(); ++it)
					it->~T();

				_size -= spaces;
			}
			else for (T* it = range_start; it < range_end; ++it)
				if (!is_tombstone(it))
				{
					it->~T();
					mark_as_tombstone(it);
				}
		}

		constexpr void controlled_reserve(unsigned int needed_capacity)
		{
			if (needed_capacity > _capacity)
				reserve(std::max(needed_capacity, _capacity * 3 / 2 + 1));
		}

		constexpr void reserve(unsigned int new_capacity)
		{
			if (new_capacity <= _capacity)
				return;

			T* new_buffer = reinterpret_cast<T*>(::operator new(new_capacity * sizeof(T)));

			if constexpr (disable_tombstoning)
			{
				std::uninitialized_move(_data, _data + _size, new_buffer);

				if constexpr (!std::is_trivially_destructible_v<T>)
					for (unsigned int inc = 0; inc < _size; ++inc)
						(_data + inc)->~T();
			}
			else if constexpr (std::is_trivially_copyable_v<T> == true)
				memcpy(new_buffer, _data, _size * sizeof(T));
			else for (unsigned int inc = 0; inc < _size; ++inc)
				if (!is_tombstone(_data + inc))
				{
					new (new_buffer + inc) T(std::move(*(_data + inc)));
					(_data + inc)->~T();
				}
			if constexpr (!disable_tombstoning)
			{
				bool* new_graveyard = reinterpret_cast<bool*>(::operator new(new_capacity * sizeof(bool)));
				memset(new_graveyard, 0, new_capacity * sizeof(bool));
				if (_is_tombstone) {
					memcpy(new_graveyard, _is_tombstone, _size * sizeof(bool));
					::operator delete(_is_tombstone);
				}
				_is_tombstone = new_graveyard;
			}

			::operator delete(_data);

			_data = new_buffer;
			_capacity = new_capacity;

		}

		constexpr void resize(unsigned int size)
		{
			controlled_reserve(std::max(size, 1u));
			while (_size < size)
				emplace_back();
		}

		constexpr void resize(unsigned int size, const T& value)
		{
			controlled_reserve(size);
			while (_size < size)
				emplace_back(value);
		}

		[[nodiscard]]
		constexpr unsigned int size() const
		{
			return _size;
		}

		[[nodiscard]]
		constexpr unsigned int allocated() const {
			if constexpr (disable_tombstoning)
				return _size;
			else return _size - _tombstones;
		}

		[[nodiscard]]
		constexpr bool empty() const
		{
			return _size == 0;
		}

		[[nodiscard]]
		constexpr unsigned int capacity() const
		{
			return _capacity;
		}

		constexpr T* data() const
		{
			return _data;
		}

		constexpr void clear()
		{
			if (!_capacity)
				return;

			for (auto& c : *this)
				if constexpr (disable_tombstoning)
					c.~T();
				else if (!is_tombstone(&c))
					c.~T();

			::operator delete(_data);
			::operator delete(_is_tombstone);
			_data = nullptr;
			_size = _capacity = 0;
			if constexpr (!disable_tombstoning)
				_first_tombstone = -1, _tombstones = 0, _is_tombstone = nullptr;
			reserve(1);
		}


		constexpr T& operator[](unsigned int pos)
		{
			if (pos >= _size)
				std::cout << "error " << pos << " " << _size << std::endl;
			assert(pos >= 0 && pos < _size);
			if constexpr (!disable_tombstoning)
				assert(!is_tombstone(_data + pos));
			return *(_data + pos);
		}

		constexpr const T& operator[](unsigned int pos) const
		{
			if (pos >= _size)
				std::cout << "error " << pos << " " << _size << std::endl;

			assert(pos >= 0 && pos < _size);
			if constexpr (!disable_tombstoning)
				assert(!is_tombstone(_data + pos));
			return *(_data + pos);
		}

		~vector()
		{
			for (T* value = internal_begin(); value < internal_end(); ++value)
				if constexpr (disable_tombstoning)
					value->~T();
				else
				{
					if (!is_tombstone(value))
						value->~T();
				}

			::operator delete(_is_tombstone);
			::operator delete(_data);
		}

		constexpr bool is_tombstone(T* position)
		{
			static_assert(!disable_tombstoning);
			if (position < _data or position >= _data + _size)
				return false;

			unsigned int offset = position - _data;
			return *(_is_tombstone + offset);
		}

		constexpr void mark_as_tombstone(T* position)
		{
			static_assert(!disable_tombstoning);
			assert(position >= _data && position < _data + _size);

			*reinterpret_cast<int*>(position) = _first_tombstone;
			_first_tombstone = position - _data;
			*(_is_tombstone + _first_tombstone) = true;
			++_tombstones;
		}

	private:
		T* _data = nullptr;
		unsigned int _size = 0;
		unsigned int _capacity = 0;

		bool* _is_tombstone = nullptr;
		//TODO use a queue
		int _first_tombstone = -1;
		unsigned int _tombstones = 0;
	};

}
