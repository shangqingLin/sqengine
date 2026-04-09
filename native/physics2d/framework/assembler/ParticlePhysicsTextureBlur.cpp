#include "ParticlePhysicsTextureBlur.h"
#include "../../../engine/2d/render/vertex-format.h"
#include "../component/ParticlePhysics2DComponent.h"
#include "../../../engine/core/sqstd/Array.h"
#include "../../../engine/core/sqstd/StackTempArenaAllocator.h"
using namespace physics2d;

ParticlePhysicsTextureBlur::ParticlePhysicsTextureBlur(ParticlePhysics2DComponent *component) : ParticlePhysicsRender(component),
                                                                                                mesh(nullptr),
                                                                                                posBuffer(nullptr),
                                                                                                weightBuffer(nullptr)
{
    // std::vector<Attribute> aWeight;
    // std::vector<Attribute> aColorV;
    std::vector<Attribute> aParticlePosV;

    // aWeight.assign(1, Attribute());
    // aWeight[0].name = "aWeight";
    // aWeight[0].format = Format::R32F;
    // aWeight[0].custom = AttributeCustomSetting();
    // aWeight[0].custom->offset = 0;
    // aWeight[0].custom->count = 1;
    // aWeight[0].custom->stride = 4;
    // aWeight[0].custom->type = Format::R32F;
    // aWeight[0].custom->instanceStride = 1;

    aParticlePosV.assign(1, Attribute());
    aParticlePosV[0].name = "aParticlePos";
    aParticlePosV[0].format = Format::RG32F;
    aParticlePosV[0].custom = AttributeCustomSetting();
    aParticlePosV[0].custom->count = 2;
    aParticlePosV[0].custom->offset = 0;
    aParticlePosV[0].custom->stride = 8;
    aParticlePosV[0].custom->type = Format::RG32F;
    aParticlePosV[0].custom->instanceStride = 1;

    IndexBuffer *indicesBuffer = new IndexBuffer(1);
    indicesBuffer->append<unsigned short>(0);
    posBuffer = new VertexBuffer();
    mesh = new Mesh();
    mesh->setIndexBuffer(indicesBuffer);
    mesh->addVertextBuffer(posBuffer, aParticlePosV);
    mesh->autoDeleteBuffer = true;
    component->setMesh(mesh);
}

ParticlePhysicsTextureBlur::~ParticlePhysicsTextureBlur()
{
    delete mesh;
}

void ParticlePhysicsTextureBlur::update()
{
    const int count = component->system->GetParticleCount();

    // printf("particle num %d \n", component->system->GetParticleCount());

    if (count == 0)
        return;

    if (particleCount != count)
    {
        particleCount = count;
        char *posData = (char *)component->system->GetPositionBuffer();
        // char *colorData = (char *)component->system->GetColorBuffer();
        // char *weigthData = (char *)component->system->GetWeightBuffer();

        posBuffer->setExternalBuffer(posData, 8 * count);
        // colorBuffer->setExternalBuffer(colorData, count << 2); // 乘以4
        // weightBuffer->setExternalBuffer(weigthData, count << 2);
    }

    posBuffer->setDirty(true);
    // weightBuffer->setDirty(true);
}

void ParticlePhysicsTextureBlur::allocate(int particleNum)
{
    posBuffer->resizeBuffer(particleNum << 1 << 2, false);
    // weightBuffer->resizeBuffer(particleNum << 2, false);
}