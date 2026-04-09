#include "RenderComponent.h"
#include "../../assets/AssetManager.h"
#include <algorithm>

RenderComponent::RenderComponent() : Component(),
                                     material(nullptr)
{
}

void RenderComponent::setMaterial(Material *mat)
{
    if (material != mat)
    {
        material = mat;
        _onMaterialModified();
    }
}

Material *RenderComponent::getMaterial()
{
    return material;
}

void RenderComponent::_onMaterialModified()
{
}

void RenderComponent::setShaderMaterialFromRes(std::string url)
{
    auto completeCall = [this](int error, Asset *asset)
    {
        if (!error)
        {
            setMaterial(dynamic_cast<Material *>(asset));
        }
    };
    LOAD_COMPLETE_FUNCTION c = completeCall;
    AssetManager::getInstance()->loadOne(url, c);
}

bool RenderComponent::canRender()
{
    // printf("RenderComponent  canRender %p %d %p \n", material, drawInfo.has_value(), mesh);
    return drawInfo.has_value() && material != nullptr && mesh != nullptr;
}

void RenderComponent::setRenderSortOrder(int order)
{
    if (renderSortOrder != order)
    {
        renderSortOrder = order;
        updateNodeRenderOrder();
    }
}

void RenderComponent::updateNodeRenderOrder()
{
    std::vector<RenderComponent *> &renderComponents = (std::vector<RenderComponent *> &)node->getRenderComponents();

    if (renderComponents.size() > 1)
    {
        std::sort(renderComponents.begin(), renderComponents.end(), [](const RenderComponent *renderA, const RenderComponent *renderB)
                  { return renderA->renderSortOrder - renderB->renderSortOrder <= 0; });
    }
}

void RenderComponent::useBuildInMesh(pipeline::BuildInMeshType type)
{
    Mesh *mesh = nullptr;
    switch (type)
    {
    case pipeline::BuildInMeshType::posRectMesh:
        mesh = pipeline::BuildInMeshDataManager::getIntance()->getPosRectMesh();
        break;
    case pipeline::BuildInMeshType::posUvRectMesh:
        mesh = pipeline::BuildInMeshDataManager::getIntance()->getPosUvRectMesh();
        break;
    case pipeline::BuildInMeshType::screenMesh:
        mesh = pipeline::BuildInMeshDataManager::getIntance()->getScreenMesh();
        break;
    }
    SQ_ASSERT(mesh);
    setMesh(mesh);
}

void bindingProcessRenderComponent(RenderComponent *component, bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    switch (op)
    {
    case 1:
    {

        DrawPrimitiveMesh drawInfo;
        drawInfo.count = *buffer.popp<int>();
        drawInfo.offset = *buffer.popp<int>();
        drawInfo.instance = *buffer.popp<int>();
        component->setDrawInfo(drawInfo);
        break;
    }
    case 2:
    {
        int assetId = *buffer.popp<int>();
        if (assetId > 0)
        {
            Material *material = static_cast<Material *>(AssetManager::getInstance()->findById(assetId));
            SQ_ASSERT(material);
            component->setMaterial(material);
        }
        else
        {
            component->setMaterial(nullptr);
        }
        break;
    }
    case 3:
    {
        component->useBuildInMesh(pipeline::BuildInMeshType(*buffer.popp<char>()));
        break;
    }
    case 4:
    {
        component->setRenderSortOrder(*buffer.popp<int>());
        break;
    }
    default:
        SQ_ASSERT(false);
    }
}

void bindingProcessRenderComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<RenderComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<RenderComponent>();
        return;
    }

    RenderComponent *meshComponent = node->getComponent<RenderComponent>();
    SQ_ASSERT(meshComponent);
    bindingProcessRenderComponent(meshComponent, type, nodeOp, op, buffer, node);
}