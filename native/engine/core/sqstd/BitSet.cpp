#include "BitSet.h"
#include <string>
#include "../base/config.h"

using namespace sqstd;

BitSet::BitSet()
{
}

BitSet::BitSet(int bitCapacity)
{
	blockCapacity = (bitCapacity + sizeof(uint64_t) * 8 - 1) / (sizeof(uint64_t) * 8);
	blockCount = 0;
	bits = allocData(blockCapacity);
	memset(bits, 0, blockCapacity * sizeof(uint64_t));
}

uint64_t *BitSet::allocData(int blockCapacity)
{
	// if (this->alloctor)
	// {
	// 	return (uint64_t *)this->alloctor->allocateChunk(blockCapacity * sizeof(uint64_t));
	// }
	// else
	// {
	// 	return (uint64_t *)SqAlloc(blockCapacity * sizeof(uint64_t));
	// }
	return (uint64_t *)malloc(blockCapacity * sizeof(uint64_t));
}

void BitSet::freeData(void *data, int blockCapacity)
{
	// if (this->alloctor)
	// {
	// 	this->alloctor->freeChunk(data);
	// }
	// else
	// {
	// 	SqFree(data, blockCapacity * sizeof(uint64_t));
	// }
	free(data);
}

// void BitSet::setAlloc(SqTempArenaAllocator *alloctor)
// {
// 	this->alloctor = alloctor;
// }

BitSet::~BitSet()
{
	clearData();
	// this->alloctor = nullptr;
}

void BitSet::clearData()
{
	if (bits)
	{
		freeData(bits, blockCapacity);
		bits = nullptr;
	}
	blockCapacity = 0;
	blockCount = 0;
}

void BitSet::clearValue()
{
	for (int i = 0; i < blockCount; ++i)
	{
		bits[i] = 0;
	}
}

void BitSet::removeValue(uint32_t bitIndex)
{
	uint32_t blockIndex = bitIndex / 64;
	if (blockIndex >= blockCount)
	{
		return;
	}
	bits[blockIndex] &= ~((uint64_t)1 << bitIndex % 64);
}

void BitSet::addValue(uint32_t bitIndex)
{
	/**
	 * 计算属于哪个block
	 * bitIndex < 64 属于第0个block
	 * 64 < bitIndex < 128 属于第1个block
	 * 128 <= bitIndex < 192 属于第2个block
	 * 依此类推……
	 *
	 */
	uint32_t blockIndex = bitIndex / 64;

	if (blockIndex >= blockCount)
	{
		growBitSet(blockIndex + 1);
	}

	/**
	 * bitIndex % 64 = 这个数字应该占第几位
	 * 比如：
	 * 65%64=1
	 * 66%64=2
	 * 67%64=3
	 * ……
	 **/
	bits[blockIndex] |= ((uint64_t)1 << bitIndex % 64);
}

bool BitSet::hasValue(uint32_t bitIndex)
{
	uint32_t blockIndex = bitIndex / 64;
	if (blockIndex >= blockCount)
	{
		return false;
	}
	return (bits[blockIndex] & ((uint64_t)1 << bitIndex % 64)) != 0;
}

bool BitSet::isEmpty()
{
	if (blockCount == 0)
		return true;
	for (int i = 0; i < blockCount; ++i)
	{
		uint64_t value = bits[i];
		if (value != 0)
		{
			return false;
		}
	}
	return true;
}

int BitSet::getBitSetBytes()
{
	return blockCapacity * sizeof(uint64_t);
}

void BitSet::growBitSet(uint32_t newBlockCount)
{
	if (newBlockCount > blockCount)
	{
		uint32_t oldCapacity = blockCapacity;
		blockCapacity = newBlockCount + newBlockCount / 2;
		uint64_t *newBits = allocData(blockCapacity);
		memset(newBits, 0, blockCapacity * sizeof(uint64_t));
		if (bits)
		{
			memcpy(newBits, bits, oldCapacity * sizeof(uint64_t));
			freeData(bits, oldCapacity);
		}
		bits = newBits;
	}
	blockCount = newBlockCount;
}

void BitSet::setBitCountAndClear(uint32_t bitCount)
{
	// uint32_t blockCount = ( bitCount + sizeof( uint64_t ) * 8 - 1 ) / ( sizeof( uint64_t ) * 8 );
	// if ( blockCapacity < blockCount )
	// {
	// 	b2DestroyBitSet( bitSet );
	// 	uint32_t newBitCapacity = bitCount + ( bitCount >> 1 );
	// 	*bitSet = b2CreateBitSet( newBitCapacity );
	// }

	// blockCount = blockCount;
	// memset( bits, 0, blockCount * sizeof( uint64_t ) );
}

void BitSet::inPlaceUnion(const BitSet *setA, const BitSet *setB)
{
	SQ_ASSERT(setA->blockCount == setB->blockCount);
	uint32_t blockCount = setA->blockCount;
	for (uint32_t i = 0; i < blockCount; ++i)
	{
		setA->bits[i] |= setB->bits[i];
	}
}
