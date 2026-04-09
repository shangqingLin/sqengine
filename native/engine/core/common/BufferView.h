#ifndef _BUFFER_VIEW_H
#define _BUFFER_VIEW_H
#include "ArrayBuffer.h"
#include <vector>

/**
 * 记录ArrayBufferView中哪些区域可以使用的
*/
struct ArrayBufferEntry{
  int offset = 0;
  int length = 0;
};


class ArrayBufferView;

/**
 * 提供给外部可以使用的内存块
 */
class ArrayBufferChunk
{
private:
   ArrayBuffer *srcBuffer;
   void initialize();   
public:
   friend class ArrayBufferView;

   ArrayBuffer buffer;
   
   /**
    * 使用ArrayBuffer内存的起点，字节为单位
    */
   int offset;

   /**
    * 使用内存的长度，单位为字节
    */
   int length;

   /**
    * 关联的ArrayBufferView
    */
   ArrayBufferView *bufferView;

   void recycle();
};

class ArrayBufferView
{
private:
   ArrayBuffer *buffer;
   bool needDeleteBuffer;

   //记录可用的块
   std::vector<ArrayBufferEntry*> freeLists;

   void initialize();
   void updateUseMemory();
public:
   ArrayBufferView(ArrayBuffer *buffer);
   ArrayBufferView(ArrayBuffer *buffer,bool needDeleteBuffer);
   ~ArrayBufferView();
   ArrayBufferChunk *allocateChunk(int size);
   void recycleChunk(ArrayBufferChunk*);
   inline ArrayBuffer* getArrayBuffer(){return buffer;};

   /**
    * 判断是否还有足够的空间
    * 可以在调用allocateChunk之前用这个做一下判断
    */
   bool hasEnoughSpace(int needSize);
};

#endif