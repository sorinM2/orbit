#pragma once

namespace utl::iterator
{

	template<typename C, typename T, bool (C::* comp)(T*) = nullptr>
	class iterator
	{
	public:
		explicit iterator(T* ptr) : _ptr{ ptr } {}
		explicit iterator(T* ptr, C* container) : _ptr{ ptr }, _container{ container }
		{
			if constexpr (comp)
				while ((_container->*comp)(_ptr))
					++_ptr;
		}

		iterator operator++(int)
		{
			iterator copy = *this;
			++_ptr;

			if constexpr (comp)
				while ((_container->*comp)(_ptr))
					++_ptr;
			return copy;
		}

		iterator& operator++()
		{
			++_ptr;
			if constexpr (comp)
				while ((_container->*comp)(_ptr))
					++_ptr;
			return *this;
		}

		auto operator<=>(const iterator& other)
		{
			return _ptr <=> other._ptr;
		}

		T* operator->() const { return _ptr; }
		T& operator*() const { return *_ptr; }

		T* operator+(unsigned int increment) { return _ptr + increment; }

		operator T* () const { return _ptr; }
	private:
		T* _ptr;
		C* _container;
	};

}