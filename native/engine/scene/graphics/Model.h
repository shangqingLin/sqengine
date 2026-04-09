#ifndef _MODEL_H_
#define _MODEL_H_
#include <vector>
#include "MeshInstance.h"
#include "../Camera.h"

/**
 * 代表一个场景中显示的模型
 */
class Model
{
public:
    std::vector<MeshInstance*> meshInstances;
    ~Model();

    /**
     * 强制这个Model使用指定的Camera渲染
     * 忽略Camera Layer的控制
     */
    void setCamera(Camera* camera);
    void addMeshInstance(MeshInstance* meshInstance);
    void removeMeshInstance(MeshInstance* meshInstance);
};
#endif