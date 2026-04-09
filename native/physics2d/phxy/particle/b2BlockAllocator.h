
#pragma once

namespace phxy
{
	const int b2_blockSizeCount = 14;

	struct b2Block;
	struct b2Chunk
	{
		// 当前的chunk中每个block的大小
		int blockSize;
		b2Block *blocks;
	};

	struct b2Block
	{
		b2Block *next;
	};

	/**
	 * 将不连续的内存组成一个大的内存空间，并可以重复利用。
	 * 每一个Chunk是一段连续的内存（内存段），Chunk与chunk之间是可以不连续的，BlockAllocator中就组合了所有的Chunk。
	 * 然后每个Chunk块再划分为多个b2Block块。
	 * See: http://www.codeproject.com/useritems/Small_Block_Allocator.asp
	 */
	class b2BlockAllocator
	{
	public:
		b2BlockAllocator();
		~b2BlockAllocator();

		/// Allocate memory. This will use b2Alloc if the size is larger than b2_maxBlockSize.
		void *Allocate(int size);

		/// Free memory. This will use b2Free if the size is larger than b2_maxBlockSize.
		void Free(void *p, int size);

		void Clear();

	private:
		/**
		 * 记录当前分配了多少个Chunk
		 * m_chunks数组的大小
		 */
		int m_chunkSpace;
		b2Chunk *m_chunks;

		/**
		 * 当使用了多少个b2Chunk
		 */
		int m_chunkCount;

		b2Block *m_freeLists[b2_blockSizeCount];
	};

}
