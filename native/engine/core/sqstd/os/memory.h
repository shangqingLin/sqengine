#ifndef SQ_STD_MEMORY_H_
#define SQ_STD_MEMORY_H_

#include <stddef.h>
#include <new>
#include <type_traits>
#include <stdint.h>

namespace sqstd
{

	/**
	 *
	 * 统一整个应用的内存分配。
	 * 1、支持分配的内存的前头的字节记录一下这个内存分配的大小。如下面的注释说明
	 *
	 */
	class Memory
	{
	public:
		// Alignment:  ↓ max_align_t        ↓ uint64_t          ↓ max_align_t
		//             ┌─────────────────┬──┬────────────────┬──┬───────────...
		//             │ uint64_t        │░░│ uint64_t       │░░│ T[]
		//             │ alloc size      │░░│ element count  │░░│ data
		//             └─────────────────┴──┴────────────────┴──┴───────────...
		// Offset:     ↑ SIZE_OFFSET        ↑ ELEMENT_OFFSET    ↑ DATA_OFFSET

		static constexpr size_t SIZE_OFFSET = 0;
		static constexpr size_t ELEMENT_OFFSET = ((SIZE_OFFSET + sizeof(uint64_t)) % alignof(uint64_t) == 0) ? (SIZE_OFFSET + sizeof(uint64_t)) : ((SIZE_OFFSET + sizeof(uint64_t)) + alignof(uint64_t) - ((SIZE_OFFSET + sizeof(uint64_t)) % alignof(uint64_t)));
		static constexpr size_t DATA_OFFSET = ((ELEMENT_OFFSET + sizeof(uint64_t)) % alignof(max_align_t) == 0) ? (ELEMENT_OFFSET + sizeof(uint64_t)) : ((ELEMENT_OFFSET + sizeof(uint64_t)) + alignof(max_align_t) - ((ELEMENT_OFFSET + sizeof(uint64_t)) % alignof(max_align_t)));

		/**
		 * @param p_bytes 分配的内存大小，单位为字节
		 * @param p_pad_align 此参数就是来指定是否在前面添加一些字节记录当前分配的内存大小情况，如上面注释所述格式
		 */
		static void *alloc_static(size_t p_bytes, bool p_pad_align = false);
		static void *realloc_static(void *p_memory, size_t p_bytes, bool p_pad_align = false);
		static void free_static(void *p_ptr, bool p_pad_align = false);

		//	                            ↓ return value of alloc_aligned_static
		//	┌─────────────────┬─────────┬─────────┬──────────────────┐
		//	│ padding (up to  │ uint32_t│ void*   │ padding (up to   │
		//	│ p_alignment - 1)│ offset  │ p_bytes │ p_alignment - 1) │
		//	└─────────────────┴─────────┴─────────┴──────────────────┘
		//
		// alloc_aligned_static will allocate p_bytes + p_alignment - 1 + sizeof(uint32_t) and
		// then offset the pointer until alignment is satisfied.
		//
		// This offset is stored before the start of the returned ptr so we can retrieve the original/real
		// start of the ptr in order to free it.
		//
		// The rest is wasted as padding in the beginning and end of the ptr. The sum of padding at
		// both start and end of the block must add exactly to p_alignment - 1.
		//
		// p_alignment MUST be a power of 2.
		static void *alloc_aligned_static(size_t p_bytes, size_t p_alignment);
		static void *realloc_aligned_static(void *p_memory, size_t p_bytes, size_t p_prev_bytes, size_t p_alignment);
		// Pass the ptr returned by alloc_aligned_static to free it.
		// e.g.
		//	void *data = realloc_aligned_static( bytes, 16 );
		//  free_aligned_static( data );
		static void free_aligned_static(void *p_memory);

		static uint64_t get_mem_available();
		static uint64_t get_mem_usage();
		static uint64_t get_mem_max_usage();
	};

	void postinitialize_handler(void *);

	template <typename T>
	T *_post_initialize(T *p_obj)
	{
		postinitialize_handler(p_obj);
		return p_obj;
	}

	#define memnew(m_class) _post_initialize(new ("") m_class)

	#define memnew_allocator(m_class, m_allocator) _post_initialize(new (m_allocator::alloc) m_class)
	#define memnew_placement(m_placement, m_class) _post_initialize(new (m_placement) m_class)

	void *GrowAlloc(void *oldMem, int oldSize, int newSize);
}
#endif // MEMORY_H
