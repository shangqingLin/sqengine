#include "MeshInstance.h"
#include "../Camera.h"

MeshInstance::MeshInstance() : material(NULL),
                               mesh(NULL),
                               layer(Layers::INHERIT),
                               localDescriptorSet(NULL),
                               camera(NULL)
{
}

MeshInstance::~MeshInstance()
{
    // 这里Pass都是引用其他Material的Pass，所以不能删除Pass中引用的数据
    for (int i = 0; i < passes.getCount(); ++i)
    {
        Pass *pass = &passes[i];
        pass->clear();
    }

    if (autoDeleteState & 2)
    {
        delete mesh;
    }

    if (autoDeleteState & 4)
    {
        delete camera;
    }

    if (autoDeleteState & 8)
    {
        delete material;
    }

    camera = nullptr;
    material = NULL;
    mesh = NULL;
}

void MeshInstance::fillPass(Pass *pass, PassStates *overrideState)
{
    Pass *drawPass = passes.Add();
    drawPass->copy(*pass);
    if (overrideState)
    {
        drawPass->fillPipelineInfo(*overrideState);
    }
}

void MeshInstance::setMaterial(Material *material, PassStates *overrideState, bool autoDelete)
{
    const std::vector<Pass *> matPasses = material->getPasses();
    SQ_ASSERT(matPasses.size() > 0);
    if (passes.getCapacity() < matPasses.size())
    {
        passes.resize(matPasses.size());
    }

    for (int i = 0; i < matPasses.size(); ++i)
    {
        fillPass(matPasses[i], overrideState);
    }

    this->material = material;
    if (autoDelete)
    {
        autoDelete |= 8;
    }
    else
    {
        autoDelete &= ~8;
    }
}

void MeshInstance::setMaterial(Material *material, bool autoDelete)
{

    const std::vector<Pass *> matPasses = material->getPasses();
    SQ_ASSERT(matPasses.size() > 0);
    if (passes.getCapacity() < matPasses.size())
    {
        passes.resize(matPasses.size());
    }

    for (int i = 0; i < matPasses.size(); ++i)
    {
        fillPass(matPasses[i], nullptr);
    }

    this->material = material;
    if (autoDelete)
    {
        autoDelete |= 8;
    }
    else
    {
        autoDelete &= ~8;
    }
}

Material *MeshInstance::getMaterial()
{
    return material;
}

void MeshInstance::setMesh(Mesh *mesh, bool autoDelete)
{
    this->mesh = mesh;

    if (autoDelete)
    {
        autoDelete |= 2;
    }
    else
    {
        autoDelete &= ~2;
    }
}

void MeshInstance::setCamera(Camera *camera, bool autoDelete)
{
    this->camera = camera;

    if (autoDelete)
    {
        autoDelete |= 4;
    }
    else
    {
        autoDelete &= ~4;
    }
};

Mesh *MeshInstance::getMesh()
{
    return mesh;
}

void MeshInstance::clear()
{
    primitive.offset = 0;
    primitive.count = 0;
    primitive.instance = 0;
    primitive.type = gfx::Type::UINT2;

    material = nullptr;
    layer = Layers::INHERIT;
    mesh = nullptr;
    camera = nullptr;
    localDescriptorSet = nullptr;
    autoDeleteState = 0;

    for (int i = 0; i < passes.getCount(); ++i)
    {
        Pass *pass = &passes[i];
        pass->clear();
    }
    passes.clear();
}