#ifndef _MESH_INSTANCE_H_
#define _MESH_INSTANCE_H_
#include "../../assets/Material.h"
#include "../../gfx/gfx.h"
#include "../Layers.h"
#include "Mesh.h"
#include <memory.h>
#include <optional>
#include "../../core/sqstd/Array.h"

class Camera;

/**
 *  MeshInstance 销毁的时候，不会对内部持有的
 * material、mesh、camera等进行销毁，需要外部自行维护
 */
class MeshInstance
{
protected:
    /**
     * 渲染使用的材质资源。必须设置
     */
    Material *material;

    sqstd::Array<Pass> passes;

    /**
     * 网格数据。必须设置
     *
     */
    Mesh *mesh;

    /**
     * 强制使用指定的Camera渲染
     * 忽略Camera Layer的机制控制
     */
    Camera *camera;

    int autoDeleteState = 0;

public:
    friend class Mesh;
    friend class Model;

    Layers layer;

    DescriptorSet *localDescriptorSet;

    /**
     * 如果你想要将MeshInstance扔到Pipleline中渲染
     * 则必须由你手动外部设置如何绘制这个Instance
     */
    DrawPrimitiveMesh primitive;

    MeshInstance();
    virtual ~MeshInstance();

    void fillPass(Pass *, PassStates *overrideState);
    void setMaterial(Material *, PassStates *overrideState, bool autoDelete = false);
    void setMaterial(Material *, bool autoDelete = false);
    Material *getMaterial();
    inline const sqstd::Array<Pass> &getPasses() { return passes; };
    void setMesh(Mesh *, bool autoDelete = false);
    Mesh *getMesh();
    virtual void clear();
    void setCamera(Camera *camera, bool autoDelete = false);
    inline Camera *getCamera() { return camera; };
};

#endif