#pragma once
#include "Byte.h"
#include "ByteBlock.h"

/**
 * 用于函数作用域内的临时的、但又需要动态生成的内存，这样避免频繁去申请临时的动态内存，优化内存管理
 * 在函数内必须用完立刻释放
 * 整个应用只有一个StackTempMemory实例
 */
namespace sqstd
{

    template <typename T>
    class Array;

    struct StackTempChunk
    {
        ByteBlockChunk *chunk{nullptr};
        void *data{nullptr};
        int size;
    };

    /**
     * 分配一个大的连续内存空间。然后外部可以从中提取一块来使用
     * 并且是作为临时内存空间使用。因为在局部作用域上我们有时候需要使用动态堆上的内存以便在多个作用域中传递，等用完了就释放，这个类就是用于此场景
     *
     *
     * 他的使用的方式类似于栈：allocateChunk1->freeChunk1,allocateChunk2->freeChunk2,
     */
    class StackTempArenaAllocator
    {
    private:
        Byte data;
        ByteBlock dataBlock;

        Array<StackTempChunk>* chunks;

        // 当前正在使用的内存大小
        int allocation = 0;

        StackTempArenaAllocator(int, int);

    public:
        static StackTempArenaAllocator *getInstance();
        static void preAllocation(int size);
        ~StackTempArenaAllocator();
        void *allocateChunk(int size);
        void freeChunk(void *mem);
        inline int getAllocation() const { return allocation; }
    };
}