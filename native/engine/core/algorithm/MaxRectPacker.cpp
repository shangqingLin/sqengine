#include "MaxRectPacker.h"
#include "../base/config.h"

using namespace algorithm;

MaxRectPacker::MaxRectPacker(int width, int height)
    : width(width), height(height), bin(width, height, &option)
{
}

MaxRectPacker::MaxRectPacker(int width, int height, MaxRectPackerOptions &p)
    : width(width), height(height), option(p), bin(width, height, &option)
{
}

MaxRectPackerNode *MaxRectPacker::add(int width, int height)
{
    MaxRectPackerNode* node =  bin.add(width, height);
    if(node){
        node->packer = this;
    }
    return node;
}

void MaxRectPacker::pruneFreeList()
{
    bin.pruneFreeList();
}

void MaxRectPacker::remove(MaxRectPackerNode *node)
{
    SQ_ASSERT(node->packer == this);
    bin.remove(node);
}

void MaxRectPacker::repack()
{
    bin.repack();
}