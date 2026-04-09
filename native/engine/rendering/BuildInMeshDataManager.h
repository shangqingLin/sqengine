#pragma once

#include "../scene/graphics/Mesh.h"

namespace pipeline
{

    enum BuildInMeshType
    {
        posRectMesh,
        posUvRectMesh,
        screenMesh
    };

    class BuildInMeshDataManager
    {
    private:
        Mesh *posRectMesh;
        Mesh *posUvRectMesh;

        // 绘制一个全屏区域
        Mesh *screenMesh;
        IndexBuffer *drawQuatIndex;

        IndexBuffer *getDrawQuatIndex();

    public:
        BuildInMeshDataManager();
        static BuildInMeshDataManager *getIntance();
        static std::vector<Attribute> posUvVertexAttributes;
        static Attribute posVertexAttributes;
        Mesh *getPosRectMesh();
        Mesh *getPosUvRectMesh();
        VertexBuffer *getPosVertexBuffer();
        IndexBuffer *getPosIndexBuffer();
        VertexBuffer *getPosUvVertexBuffer();
        IndexBuffer *getPosUvIndexBuffer();

        Mesh *getScreenMesh();
    };

}
