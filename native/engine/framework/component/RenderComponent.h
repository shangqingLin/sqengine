#pragma once
#include "./Component.h"
#include "../../assets/Material.h"
#include <string>
#include "../../scene/graphics/MeshInstance.h"
#include "../../rendering/BuildInMeshDataManager.h"
#include <optional>

void bindingProcessRenderComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
void bindingProcessRenderComponent(RenderComponent *, bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
class RenderComponent : public Component
{
protected:
    Material *material;
    Mesh *mesh;
    std::optional<DrawPrimitiveMesh> drawInfo;

    /**
     * 如果一个Node有多个RenderComponent，则这里控制渲染顺序
     * 数值越小越先渲染
     */
    int renderSortOrder = 0;

    virtual void _onMaterialModified();
    void updateNodeRenderOrder();

public:
    friend class NodeTreeManager;
    friend class Node;
    virtual bool canRender();
    RenderComponent();
    void setMaterial(Material *mat);
    Material *getMaterial();
    inline virtual void setMesh(Mesh *mesh) { this->mesh = mesh; };
    inline Mesh *getMesh() { return mesh; };
    inline void setDrawInfo(const DrawPrimitiveMesh &drawInfo) { this->drawInfo = drawInfo; };
    inline DrawPrimitiveMesh *getDrawInfo() { return drawInfo.has_value() ? &drawInfo.value() : nullptr; }
    void setRenderSortOrder(int order);
    void setShaderMaterialFromRes(std::string url);
    void useBuildInMesh(pipeline::BuildInMeshType);
};
