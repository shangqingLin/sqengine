
#pragma once

#include "./os/memory.h"
#include <string.h>
#include <type_traits>
#include "./error/error_macros.h"
#include "../base/config.h"

namespace sqstd
{
	template <typename T>
	class Vector;

	/**
	 * 表示一段连续的字节数组
	 * 1、记录内存引用的次数
	 * 2、记录内存大小
	 * 3、记录元素的个数
	 */
	template <typename T>
	class CowData
	{
		template <typename TV>
		friend class Vector;

	public:
		typedef int64_t Size;
		typedef uint64_t USize;
		static constexpr USize MAX_INT = INT64_MAX;

	private:
		/**
		 *  _ptr = REF_COUNT_OFFSET + SIZE_OFFSET 的地址，所以这里是data_ptr的起始地址
		 * 因为我们会非常频繁地进行数据的访问操作，所以这里干脆直接存储data_ptr，不用每次访问数据的时候都需要加上“REF_COUNT_OFFSET + SIZE_OFFSET”
		 */
		mutable T *_ptr = nullptr;

		/*
		 * 计算大于或等于给定值的最小的2的幂次值
		 * 这里主要为保证分配的内存的大小必须是2的幂次方，从而保证内存对齐
		 */
		static USize next_po2(USize x)
		{
			if (x == 0)
			{
				return 0;
			}

			--x;
			x |= x >> 1;
			x |= x >> 2;
			x |= x >> 4;
			x |= x >> 8;
			x |= x >> 16;
			if (sizeof(USize) == 8)
			{
				x |= x >> 32;
			}

			return ++x;
		}

		//          存储当前的内存被引用的次数 记录内存的大小（可以存储元素的个数，不是字节个数） 真实的数据
		// Alignment:  ↓ max_align_t           ↓ USize         ↓ max_align_t
		//             ┌────────────────────┬──┬─────────────┬──┬───────────...
		//             │ SafeNumeric<USize> │░░│ USize       │░░│ T[]
		//             │ ref. count         │░░│ data size   │░░│ data
		//             └────────────────────┴──┴─────────────┴──┴───────────...
		// Offset:     ↑ REF_COUNT_OFFSET      ↑ SIZE_OFFSET    ↑ DATA_OFFSET

		static constexpr size_t REF_COUNT_OFFSET = 0;
		static constexpr size_t SIZE_OFFSET = ((REF_COUNT_OFFSET + sizeof(USize)) % alignof(USize) == 0) ? (REF_COUNT_OFFSET + sizeof(USize)) : ((REF_COUNT_OFFSET + sizeof(USize)) + alignof(USize) - ((REF_COUNT_OFFSET + sizeof(USize)) % alignof(USize)));
		static constexpr size_t DATA_OFFSET = ((SIZE_OFFSET + sizeof(USize)) % alignof(max_align_t) == 0) ? (SIZE_OFFSET + sizeof(USize)) : ((SIZE_OFFSET + sizeof(USize)) + alignof(max_align_t) - ((SIZE_OFFSET + sizeof(USize)) % alignof(max_align_t)));

		/**
		 * 获取当前这块内存被引用的次数
		 */
		static USize *_get_refcount_ptr(uint8_t *p_ptr)
		{
			return (USize *)(p_ptr + REF_COUNT_OFFSET);
		}

		static USize *_get_size_ptr(uint8_t *p_ptr)
		{
			return (USize *)(p_ptr + SIZE_OFFSET);
		}

		static T *_get_data_ptr(uint8_t *p_ptr)
		{
			return (T *)(p_ptr + DATA_OFFSET);
		}

		/**
		 * 获取当前内存被引用的次数
		 * 1、通过_ref函数可以可以增加内存的引用次数
		 * 2、通过_unref减少引用的次数
		 */
		USize *_get_refcount() const
		{
			if (!_ptr)
			{
				return nullptr;
			}

			return (USize *)((uint8_t *)_ptr - DATA_OFFSET + REF_COUNT_OFFSET);
		}

		/**
		 * 获取当前内存存储了多少个元素（不是字节个数哦），由外部维护
		 */
		USize *_get_size() const
		{
			if (!_ptr)
			{
				return nullptr;
			}

			return (USize *)((uint8_t *)_ptr - DATA_OFFSET + SIZE_OFFSET);
		}

		USize _get_alloc_size(USize p_elements) const
		{
			return next_po2(p_elements * sizeof(T));
		}

		/**
		 * 减少当前内存引用的次数。并且当前引用次数为0时释放内存
		 */
		void _unref();

		/**
		 * 使用另外一个CowData替代当前的ptr，这里不是复制，
		 * 即当前的CowData和p_from 指向了同一块内存了
		 * 并记录p_from被多少个CowData引用了
		 */
		void _ref(const CowData *p_from);
		void _ref(const CowData &p_from);

		USize _copy_on_write();

	public:
		
		//浅拷贝,单纯的添加引用计数而已
		void operator=(const CowData<T> &p_from) { _ref(p_from); }

		T *ptrw()
		{
			_copy_on_write();
			return _ptr;
		}

		const T *ptr() const
		{
			return _ptr;
		}

		/**
		 * 分配内存
		 * 如果返回0表示此次没有执行分配内存
		 */
		template <bool p_ensure_zero = false>
		int resize(Size p_size);

		/**
		 * 获取当前元素T的个数。是T实例的个数，不是字节大小
		 */
		Size size() const
		{
			USize *size = (USize *)_get_size();
			if (size)
			{
				return *size;
			}
			else
			{
				return 0;
			}
		}

		void clear() { resize(0); }
		bool is_empty() const { return _ptr == nullptr; }

		void set(Size p_index, const T &p_elem)
		{
			// ERR_FAIL_INDEX(p_index, size());
			_copy_on_write();
			_ptr[p_index] = p_elem;
		}

		T &get_m(Size p_index)
		{
			// CRASH_BAD_INDEX(p_index, size());
			_copy_on_write();
			return _ptr[p_index];
		}

		const T &get(Size p_index) const
		{
			SQ_ASSERT(p_index < size());
			return _ptr[p_index];
		}

		/**
		 * 删除一个元素，同时会减少内存的，所以这里会涉及内存重分配操作
		 */
		void remove_at(Size p_index)
		{
			// ERR_FAIL_INDEX(p_index, size());
			T *p = ptrw();
			Size len = size();
			for (Size i = p_index; i < len - 1; i++)
			{
				p[i] = p[i + 1];
			}

			resize(len - 1);
		}

		/**
		 * 在指定的位置上插入一个元素
		 */
		int insert(Size p_pos, const T &p_val)
		{
			Size new_size = size() + 1;
			int err = resize(new_size);
			T *p = ptrw();

			// 在p_pos后面的数据往后面挪
			for (Size i = new_size - 1; i > p_pos; i--)
			{
				p[i] = p[i - 1];
			}
			p[p_pos] = p_val;

			return 0;
		}

		/**
		 * 查找一个与p_val相等的元素，所以T类型需要支持"==" 运算符
		 * p_from : 设置从哪个下表开始查找
		 */
		Size find(const T &p_val, Size p_from = 0) const;

		/**
		 * 与find差不多，只不过这里是从后面开始遍历查找
		 */
		Size rfind(const T &p_val, Size p_from = -1) const;

		/**
		 * 计算与p_val相等的元素有多少个
		 */
		Size count(const T &p_val) const;

		CowData() {}
		~CowData();
		CowData(CowData<T> &p_from) { _ref(p_from); }
	};

	template <typename T>
	void CowData<T>::_unref()
	{
		if (!_ptr)
		{
			return;
		}

		USize &refc = *_get_refcount();
		--refc;

		if (refc > 0)
		{
			return; // still in use
		}

		// 引用次数为0，则释放内存

		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			USize current_size = *_get_size();

			for (USize i = 0; i < current_size; ++i)
			{
				// 调用析构函数
				T *t = &_ptr[i];
				t->~T();
			}
		}

		// free mem
		Memory::free_static(((uint8_t *)_ptr) - DATA_OFFSET, false);
	}

	/**
	 * 重新分配内存，将原来的数据复制到新的内存上
	 *
	 */
	template <typename T>
	typename CowData<T>::USize CowData<T>::_copy_on_write()
	{
		if (!_ptr)
		{
			return 0;
		}

		USize *rc = _get_refcount();
		if (*rc > 1)
		{
			/* in use by more than me */
			USize current_size = *_get_size();

			uint8_t *mem_new = (uint8_t *)Memory::alloc_static(_get_alloc_size(current_size) + DATA_OFFSET, false);

			USize *_refc_ptr = _get_refcount_ptr(mem_new);

			// 记录内存大小
			USize *_size_ptr = _get_size_ptr(mem_new);

			// 实际记录数据的地址
			T *_data_ptr = _get_data_ptr(mem_new);

			*(_size_ptr) = current_size; // size

			// initialize new elements
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				memcpy((uint8_t *)_data_ptr, _ptr, current_size * sizeof(T));
			}
			else
			{
				for (USize i = 0; i < current_size; i++)
				{
					memnew_placement(&_data_ptr[i], T(_ptr[i]));
				}
			}

			_unref();
			_ptr = _data_ptr;
			*rc = 1;
		}
		return *rc;
	}

	template <typename T>
	template <bool p_ensure_zero>
	int CowData<T>::resize(Size p_size)
	{

		Size current_size = size();

		if (p_size == current_size)
		{
			return 0;
		}

		// 传入0直接销毁内存
		if (p_size == 0)
		{
			// wants to clean up
			_unref();
			_ptr = nullptr;
			return 0;
		}

		USize rc = *_get_refcount();
		USize current_alloc_size = _get_alloc_size(current_size);
		USize alloc_size = _get_alloc_size(p_size);

		if (p_size > current_size)
		{
			if (alloc_size != current_alloc_size)
			{
				if (current_size == 0)
				{
					// alloc from scratch
					uint8_t *mem_new = (uint8_t *)Memory::alloc_static(alloc_size + DATA_OFFSET, false);

					USize *_refc_ptr = _get_refcount_ptr(mem_new);
					USize *_size_ptr = _get_size_ptr(mem_new);
					T *_data_ptr = _get_data_ptr(mem_new);

					*(_refc_ptr) = 1; // refcount
					*(_size_ptr) = 0; // size, currently none

					_ptr = _data_ptr;
				}
				else
				{
					uint8_t *mem_new = (uint8_t *)Memory::realloc_static(((uint8_t *)_ptr) - DATA_OFFSET, alloc_size + DATA_OFFSET, false);
					T *_data_ptr = _get_data_ptr(mem_new);
					_ptr = _data_ptr;
				}
			}

			// construct the newly created elements

			if constexpr (!std::is_trivially_constructible_v<T>) // 判断T不是为平凡类型
			{
				// 如果不是平凡类型，代表了T类型自定义了构造函数，则我们需要调用这个自定义的构造函数
				for (Size i = *_get_size(); i < p_size; i++)
				{
					memnew_placement(&_ptr[i], T);
				}
			}
			else if (p_ensure_zero)
			{
				/**
				 *
				 *
				 *  如果是平凡类型（没有自定义的构造函数），那么我们可以直接使用memset来初始化。
				 *
				 * p_ensure_zero参数就是用来控制是否需要初始化为0：
				 *	1、因为很多时候，我们其实不需要清0，所以就不用调用这个提升效率
				 *  2、如果你需要清0，则在调用的时候传递true即可，p_ensure_zero默认为false
				 *  	即这样调用：CowData<int> data;
				 *    			   data.resize<true>(100); 传递true表示对于平凡类型进行清0操作
				 */
				memset((void *)(_ptr + current_size), 0, (p_size - current_size) * sizeof(T));
			}

			*_get_size() = p_size;
		}
		else if (p_size < current_size)
		{
			// 表示缩小内存

			if constexpr (!std::is_trivially_destructible_v<T>)
			{
				// 如果T类型有自定义的析构函数，则这里调用析构函数
				for (USize i = p_size; i < *_get_size(); i++)
				{
					T *t = &_ptr[i];
					t->~T();
				}
			}

			if (alloc_size != current_alloc_size)
			{
				uint8_t *mem_new = (uint8_t *)Memory::realloc_static(((uint8_t *)_ptr) - DATA_OFFSET, alloc_size + DATA_OFFSET, false);

				USize *_refc_ptr = _get_refcount_ptr(mem_new);
				T *_data_ptr = _get_data_ptr(mem_new);

				*(_refc_ptr) = rc; // refcount

				_ptr = _data_ptr;
			}

			*_get_size() = p_size;
		}

		return 0;
	}

	template <typename T>
	typename CowData<T>::Size CowData<T>::find(const T &p_val, Size p_from) const
	{
		Size ret = -1;

		if (p_from < 0 || size() == 0)
		{
			return ret;
		}

		for (Size i = p_from; i < size(); i++)
		{
			if (get(i) == p_val)
			{
				ret = i;
				break;
			}
		}

		return ret;
	}

	template <typename T>
	typename CowData<T>::Size CowData<T>::rfind(const T &p_val, Size p_from) const
	{
		const Size s = size();

		if (p_from < 0)
		{
			p_from = s + p_from;
		}
		if (p_from < 0 || p_from >= s)
		{
			p_from = s - 1;
		}

		for (Size i = p_from; i >= 0; i--)
		{
			if (get(i) == p_val)
			{
				return i;
			}
		}
		return -1;
	}

	template <typename T>
	typename CowData<T>::Size CowData<T>::count(const T &p_val) const
	{
		Size amount = 0;
		for (Size i = 0; i < size(); i++)
		{
			if (get(i) == p_val)
			{
				amount++;
			}
		}
		return amount;
	}

	template <typename T>
	void CowData<T>::_ref(const CowData *p_from)
	{
		_ref(*p_from);
	}

	template <typename T>
	void CowData<T>::_ref(const CowData &p_from)
	{
		if (_ptr == p_from._ptr)
		{
			return; // self assign, do nothing.
		}

		_unref();
		_ptr = nullptr;

		if (!p_from._ptr)
		{
			return; // nothing to do
		}

		// 记录一下这段内存被多少个CowData引用了
		USize &ref = *p_from._get_refcount_ptr((uint8_t*)p_from._ptr);
		++ref;
		if (ref > 0)
		{ // could reference
			_ptr = p_from._ptr;
		}
	}

	template <typename T>
	CowData<T>::~CowData()
	{
		_unref();
	}

}
