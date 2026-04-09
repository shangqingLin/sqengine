#pragma once
#include "Byte.h"
#include <vector>

namespace sqstd
{

   /**
    * 记录ArrayBufferView中哪些区域可以使用的
    */
   struct ByteBlockEntry
   {
      int offset = 0;
      int length = 0;
   };

   class ByteBlock;

   /**
    * 提供给外部可以使用的内存块
    */
   class ByteBlockChunk
   {
   private:
      Byte *srcBuffer;
      void initialize();

   public:
      friend class ByteBlock;

      // 映射的这一段内存可以通过这里读取
      Byte buffer;

      /**
       * 使用Byte内存的起点，字节为单位
       */
      int offset;

      /**
       * 使用内存的长度，单位为字节
       */
      int length;

      /**
       * 关联的ByteBlock
       */
      ByteBlock *bufferView;

      void recycle();

      void *getData();
      ~ByteBlockChunk();
   };

   class ByteBlock
   {
   private:
      Byte *buffer;
      bool needDeleteBuffer;

      // 当前外部正在使用的Chunks
      std::vector<ByteBlockChunk *> activeChunks;
      std::vector<ByteBlockChunk *> chunkPool;

      // 记录可用的块
      std::vector<ByteBlockEntry *> freeLists;
      std::vector<ByteBlockEntry *> entryPool;
      
      void initialize();
      void updateUseMemory();
      void clear();

   public:
      friend class StackTempArenaAllocator;
      ByteBlock();
      ByteBlock(Byte *buffer);
      ByteBlock(Byte *buffer, bool needDeleteBuffer);
      ~ByteBlock();

      void setByte(Byte *, bool needDeleteBuffer);
      ByteBlockChunk *allocateChunk(int size);
      void recycleChunk(ByteBlockChunk *);
      inline Byte *getArrayBuffer() { return buffer; };
      /**
       * 判断是否还有足够的空间
       * 可以在调用allocateChunk之前用这个做一下判断
       */
      bool hasEnoughSpace(int needSize);
   };

}