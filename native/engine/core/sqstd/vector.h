
#ifndef SQ_STD_VECTOR_H_
#define SQ_STD_VECTOR_H_

#include <climits>
#include <initializer_list>
#include "cowdata.h"

/**
 * 模拟动态数组，类似std::vector
 */

namespace sqstd
{
	template <typename T>
	class VectorWriteProxy
	{
	public:
		T &operator[](typename CowData<T>::Size p_index)
		{
			CRASH_BAD_INDEX(p_index, ((Vector<T> *)(this))->_cowdata.size());

			return ((Vector<T> *)(this))->_cowdata.ptrw()[p_index];
		}
	};

	template <typename T>
	class Vector
	{
		friend class VectorWriteProxy<T>;

	public:
		VectorWriteProxy<T> write;
		typedef typename CowData<T>::Size Size;

	private:
		CowData<T> _cowdata;

	public:
		/**
		 * 往后面添加元素。
		 */
		bool push_back(T p_elem);

		/**
		 * 将整个数组中的元素填充为p_elem值
		 */
		void fill(T p_elem);

		/**
		 * 删除指定位置上的元素
		 */
		void remove_at(Size p_index) { _cowdata.remove_at(p_index); }

		/**
		 * 根据元素值来删除指定的元素
		 */
		bool erase(const T &p_val)
		{
			Size idx = find(p_val);
			if (idx >= 0)
			{
				remove_at(idx);
				return true;
			}
			return false;
		}

		void reverse();

		T *ptrw() { return _cowdata.ptrw(); }
		const T *ptr() const { return _cowdata.ptr(); }
		void clear() { resize(0); }
		bool empty() const { return _cowdata.is_empty(); }
		const T &get(Size p_index) const { return _cowdata.get(p_index); }
		void set(Size p_index, const T &p_elem) { _cowdata.set(p_index, p_elem); }
		Size size() const { return _cowdata.size(); }
		int resize(Size p_size) { return _cowdata.resize(p_size); }
		int resize_zeroed(Size p_size) { return _cowdata.template resize<true>(p_size); }
		const T &operator[](Size p_index) const { return _cowdata.get(p_index); }
		int insert(Size p_pos, T p_val) { return _cowdata.insert(p_pos, p_val); }
		Size find(const T &p_val, Size p_from = 0) const { return _cowdata.find(p_val, p_from); }
		Size rfind(const T &p_val, Size p_from = -1) const { return _cowdata.rfind(p_val, p_from); }
		Size count(const T &p_val) const { return _cowdata.count(p_val); }

		void append_array(const Vector<T> &p_other);

		/**
		 * 判断是否有指定的元素
		 */
		bool has(const T &p_val) const { return find(p_val) != -1; }

		void sort()
		{
			// sort_custom<_DefaultComparator<T>>();
		}

		// template <typename Comparator, bool Validate = SORT_ARRAY_VALIDATE_ENABLED, typename... Args>
		// void sort_custom(Args &&...args)
		// {
		// 	Size len = _cowdata.size();
		// 	if (len == 0)
		// 	{
		// 		return;
		// 	}

		// 	T *data = ptrw();
		// 	SortArray<T, Comparator, Validate> sorter{args...};
		// 	sorter.sort(data, len);
		// }

		// Size bsearch(const T &p_value, bool p_before)
		// {
		// 	return bsearch_custom<_DefaultComparator<T>>(p_value, p_before);
		// }

		// template <typename Comparator, typename Value, typename... Args>
		// Size bsearch_custom(const Value &p_value, bool p_before, Args &&...args)
		// {
		// 	SearchArray<T, Comparator> search{args...};
		// 	return search.bisect(ptrw(), size(), p_value, p_before);
		// }

		Vector<T> duplicate()
		{
			return *this;
		}

		void ordered_insert(const T &p_val)
		{
			Size i;
			for (i = 0; i < _cowdata.size(); i++)
			{
				if (p_val < operator[](i))
				{
					break;
				}
			}
			insert(i, p_val);
		}

		inline void operator=(const Vector &p_from)
		{
			_cowdata._ref(p_from._cowdata);
		}

		Vector<uint8_t> to_byte_array() const
		{
			Vector<uint8_t> ret;
			if (empty())
			{
				return ret;
			}
			ret.resize(size() * sizeof(T));
			memcpy(ret.ptrw(), ptr(), sizeof(T) * size());
			return ret;
		}

		Vector<T> slice(Size p_begin, Size p_end = CowData<T>::MAX_INT) const
		{
			Vector<T> result;

			const Size s = size();

			Size begin = CLAMP(p_begin, -s, s);
			if (begin < 0)
			{
				begin += s;
			}
			Size end = CLAMP(p_end, -s, s);
			if (end < 0)
			{
				end += s;
			}

			ERR_FAIL_COND_V(begin > end, result);

			Size result_size = end - begin;
			result.resize(result_size);

			const T *const r = ptr();
			T *const w = result.ptrw();
			for (Size i = 0; i < result_size; ++i)
			{
				w[i] = r[begin + i];
			}

			return result;
		}

		bool operator==(const Vector<T> &p_arr) const
		{
			Size s = size();
			if (s != p_arr.size())
			{
				return false;
			}
			for (Size i = 0; i < s; i++)
			{
				if (operator[](i) != p_arr[i])
				{
					return false;
				}
			}
			return true;
		}

		bool operator!=(const Vector<T> &p_arr) const
		{
			Size s = size();
			if (s != p_arr.size())
			{
				return true;
			}
			for (Size i = 0; i < s; i++)
			{
				if (operator[](i) != p_arr[i])
				{
					return true;
				}
			}
			return false;
		}

		struct Iterator
		{
			T &operator*() const
			{
				return *elem_ptr;
			}
			T *operator->() const { return elem_ptr; }
			Iterator &operator++()
			{
				elem_ptr++;
				return *this;
			}
			Iterator &operator--()
			{
				elem_ptr--;
				return *this;
			}

			bool operator==(const Iterator &b) const { return elem_ptr == b.elem_ptr; }
			bool operator!=(const Iterator &b) const { return elem_ptr != b.elem_ptr; }

			Iterator(T *p_ptr) { elem_ptr = p_ptr; }
			Iterator() {}
			Iterator(const Iterator &p_it) { elem_ptr = p_it.elem_ptr; }

		private:
			T *elem_ptr = nullptr;
		};

		struct ConstIterator
		{
			const T &operator*() const
			{
				return *elem_ptr;
			}
			const T *operator->() const { return elem_ptr; }
			ConstIterator &operator++()
			{
				elem_ptr++;
				return *this;
			}
			ConstIterator &operator--()
			{
				elem_ptr--;
				return *this;
			}

			bool operator==(const ConstIterator &b) const { return elem_ptr == b.elem_ptr; }
			bool operator!=(const ConstIterator &b) const { return elem_ptr != b.elem_ptr; }

			ConstIterator(const T *p_ptr) { elem_ptr = p_ptr; }
			ConstIterator() {}
			ConstIterator(const ConstIterator &p_it) { elem_ptr = p_it.elem_ptr; }

		private:
			const T *elem_ptr = nullptr;
		};

		Iterator begin()
		{
			return Iterator(ptrw());
		}
		Iterator end()
		{
			return Iterator(ptrw() + size());
		}

		ConstIterator begin() const
		{
			return ConstIterator(ptr());
		}
		ConstIterator end() const
		{
			return ConstIterator(ptr() + size());
		}

		Vector() {}
		Vector(std::initializer_list<T> p_init)
		{
			int err = _cowdata.resize(p_init.size());
			// ERR_FAIL_COND(err);

			Size i = 0;
			for (const T &element : p_init)
			{
				_cowdata.set(i++, element);
			}
		}
		Vector(const Vector &p_from) { _cowdata._ref(p_from._cowdata); }
		Vector(const CowData<T> &p_from) { _cowdata._ref(p_from._cowdata); }
		~Vector() {}
	};

	template <typename T>
	void Vector<T>::reverse()
	{
		for (Size i = 0; i < size() / 2; i++)
		{
			T *p = ptrw();
			SWAP(p[i], p[size() - i - 1]);
		}
	}

	template <typename T>
	void Vector<T>::append_array(const Vector<T> &p_other)
	{
		const Size ds = p_other.size();
		if (ds == 0)
		{
			return;
		}
		const Size bs = size();
		resize(bs + ds);
		for (Size i = 0; i < ds; ++i)
		{
			ptrw()[bs + i] = p_other[i];
		}
	}

	template <typename T>
	bool Vector<T>::push_back(T p_elem)
	{
		int err = resize(size() + 1);
		// ERR_FAIL_COND_V(err, true);
		set(size() - 1, p_elem);
		return false;
	}

	template <typename T>
	void Vector<T>::fill(T p_elem)
	{
		T *p = ptrw();
		for (Size i = 0; i < size(); i++)
		{
			p[i] = p_elem;
		}
	}
}

#endif // VECTOR_H
