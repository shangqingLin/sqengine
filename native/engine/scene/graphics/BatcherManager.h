#ifndef GRAPHICS_BATCHER_MANAGER_H_
#define GRAPHICS_BATCHER_MANAGER_H_

#include "MeshInstance.h"

class BatcherManager
{
private:
   Material* currentMaterial;
   IndexBuffer* currentIndex;
public:
    void commit(MeshInstance*);
};



#endif