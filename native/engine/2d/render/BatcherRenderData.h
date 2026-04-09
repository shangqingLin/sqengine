#ifndef _2D_BATCHER_RENDER_DATA_H_
#define _2D_BATCHER_RENDER_DATA_H_
#include <map>
#include "../../scene/graphics/Mesh.h"

class BatcherRenderData
{
private:
    std::map<int, Mesh *> _bufferAccessors;
    Mesh *allocateMesh(const std::vector<Attribute> &vertexFormat, int numVertices, int numIndices);
    Mesh *getMesh(const std::vector<Attribute> &vertexFormat, int numVertices);

public:
    static BatcherRenderData *getInstance();
    void allocateMeshChunk(const std::vector<Attribute> &vertexFormat, int numVertices, Mesh *&mesh, sqstd::ByteBlockChunk *&chunk);
    void reset();
};

#endif