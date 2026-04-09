#include "Particle2DCPUEmitter.h"
#include "../Particles2DComponent.h"
#include "./shape/Paritcle2DSphereEmitterShape.h"
#include "./shape/Particle2DBoxEmitterShape.h"
#include "./shape/Particle2DPointEmitterShape.h"
#include "./shape/Particle2DRingEmitterShape.h"
#include "./shape/Particle2DPathEmitter.h"
#include "../common/math.h"
#include "../../../../rendering/BuildInMeshDataManager.h"
#include "../../../../core/sqstd/StackTempArenaAllocator.h"
#include "../../../../framework/Application.h"

Particle2DCPUEmitter::Particle2DCPUEmitter(Particles2DComponent *c) : Particle2DEmitter(c),
                                                                      emitterShape(nullptr),
                                                                      shapeType(ParticleEmitterShape::NONE),
                                                                      particleVertexBuffer{nullptr},
                                                                      renderMesh{nullptr},
                                                                      propertyTexture{nullptr},
                                                                      renderMaterial{nullptr}
{
}

Particle2DCPUEmitter::~Particle2DCPUEmitter()
{
    delete emitterShape;
    emitterShape = nullptr;

    delete particleVertexBuffer;
    particleVertexBuffer = nullptr;

    delete renderMesh;
    renderMesh = nullptr;
}

void Particle2DCPUEmitter::initialize(Particle2DEmitterDefine *define)
{
    this->define = define;
    Particle2DTransmitterDefine emitterDefine;
    emitterDefine.amount = define->amount;
    emitterDefine.amoutRatio = define->amountRatio;
    emitterDefine.lifeTime = define->lifeTime > 0 ? define->lifeTime : 5000;
    emitterDefine.randomness = define->randomness;
    emitterDefine.randomSeed = define->randomSeed;
    emitter.initialize(emitterDefine);

    setEmitShape(define->shape);
    setEmit(define->emit);
    setOneShot(define->oneShot);
    renderMesh = new Mesh();
    component->setMesh(renderMesh);

    AttributeCustomSetting particlePosition = {
        0,
        2,
        12,
        Format::RG32F,
        1};

    AttributeCustomSetting lifeTimePercent = {
        8,
        1,
        12,
        Format::R32F,
        1};

    std::vector<Attribute> vfmtParticle = {
        {"particlePosition", Format::RG32F, particlePosition},
        {"lifeTimePercent", Format::R32F, lifeTimePercent},
    };

    particleVertexBuffer = new VertexBuffer(BufferUsage::DYNAMIC_DRAW);
    renderMesh->addVertextBuffer(particleVertexBuffer, vfmtParticle);

    // 绘制一个矩形作为一个粒子。
    renderMesh->addVertextBuffer(pipeline::BuildInMeshDataManager::getIntance()->getPosUvVertexBuffer(), pipeline::BuildInMeshDataManager::posUvVertexAttributes);
    renderMesh->setIndexBuffer(pipeline::BuildInMeshDataManager::getIntance()->getPosUvIndexBuffer());
    renderMesh->autoDeleteBuffer = false;

    if (define->cpuRenderMaterial)
    {
        setRenderMaterial(define->cpuRenderMaterial);
    }

    if (define->renderSceneMarkMaterial)
        setRenderSceneMarkMaterial(define->renderSceneMarkMaterial);
}

void Particle2DCPUEmitter::setSeed(float seed)
{
    emitter.setRandomSeed(seed);
}
void Particle2DCPUEmitter::setRandomness(unsigned int seed)
{
    emitter.setRandomness(seed);
}
void Particle2DCPUEmitter::setAmount(int amount)
{
    loopStart = 0;
    emitter.setAmount(amount);
}
void Particle2DCPUEmitter::setAmountRatio(float ratio)
{
    emitter.setAmountRatio(ratio);
}
int Particle2DCPUEmitter::getActiveCount()
{
    return emitter.getActiveCount();
}

int Particle2DCPUEmitter::getRenderNum()
{
    return renderCount;
}

void Particle2DCPUEmitter::setOneShot(bool b)
{
    emitter.setOneShot(b);
}
void Particle2DCPUEmitter::setEmit(bool b)
{
    emitter.setEmit(b);
}
void Particle2DCPUEmitter::setEmitSpreadAngle(float angle) {}
void Particle2DCPUEmitter::setEmitSpreadDirection(Vec2) {}
void Particle2DCPUEmitter::setEmitInitMinMaxVelocityMuti(float, float) {}
void Particle2DCPUEmitter::setEmitShape(ParticleEmitterShape shape)
{
    if (shapeType == shape)
        return;

    if (emitterShape)
    {
        delete emitterShape;
        emitterShape = nullptr;
    }

    switch (shape)
    {
    case ParticleEmitterShape::POINT:
        emitterShape = new Particle2DPointEmitterShape();
        break;
    case ParticleEmitterShape::BOX:
        emitterShape = new Particle2DBoxEmitterShape();
        break;
    case ParticleEmitterShape::PATH:
        emitterShape = new Particle2DPathEmitter();
        break;
    case ParticleEmitterShape::SPHERE:
        emitterShape = new Paritcle2DSphereEmitterShape();
        break;
    case ParticleEmitterShape::RING:
        emitterShape = new Particle2DRingEmitterShape();
        break;
    default:
        SQ_ASSERT(false);
    }
}
void Particle2DCPUEmitter::setEmitShapeRadius(float) {}
void Particle2DCPUEmitter::setEmitRingShapeHeight(float) {}
void Particle2DCPUEmitter::setEmitRingShapeConeAngle(float) {}
void Particle2DCPUEmitter::setEmitRingShapeInnerRadius(float) {}
void Particle2DCPUEmitter::setEmitRingShapeAxis(const Vec2 &) {}
void Particle2DCPUEmitter::setEmitBoxExtents(const Vec2 &) {}
void Particle2DCPUEmitter::setLifeTime(int lifeTime)
{
    SQ_ASSERT(lifeTime);
    emitter.setLifeTime(lifeTime);
}

void Particle2DCPUEmitter::setInheritEmitterVelocityRatio(float) {}
void Particle2DCPUEmitter::setMinMaxLinearAccel(float, float) {}
void Particle2DCPUEmitter::setMinMaxRadialAccel(float, float) {}
void Particle2DCPUEmitter::setMinMaxTangentAccel(float, float) {}
void Particle2DCPUEmitter::setMinMaxDamping(float, float) {}
void Particle2DCPUEmitter::setMinMaxAngularVelocity(float, float) {}
void Particle2DCPUEmitter::setMinMaxDirectionalVelocity(float, float) {}
void Particle2DCPUEmitter::setMinMaxRadialVelocity(float, float) {}
void Particle2DCPUEmitter::setMinMaxOrbitVelocity(float, float) {}
void Particle2DCPUEmitter::setMinMaxTurbulenceInfluence(float, float) {}
void Particle2DCPUEmitter::setMinMaxInitialAngle(float min, float max)
{
    float value[2] = {min, max};
    renderMaterial->setProperty("initialAngleMinMax", value);
}
void Particle2DCPUEmitter::setMinMaxScale(float min, float max)
{
    float value[2] = {min, max};
    renderMaterial->setProperty("initialScaleMinMax", value);
}
void Particle2DCPUEmitter::setInitColor(const Color &color)
{
    float size[4] = {color.r, color.g, color.b, color.a};
    renderMaterial->setProperty("initColor", size);
}
void Particle2DCPUEmitter::updateRenderParticleSize()
{

    if (!renderMaterial)
        return;

    float width = 0.f, height = 0.f;
    if (define->particleTexture && (define->width <= 0.f || define->height <= 0.f)) // 如果外部不设置，则使用texture的大小渲染
    {
        width = define->particleTexture->getWidth();
        height = define->particleTexture->getHeight();
    }
    else
    {
        width = define->width > 0.f ? define->width : 4.f;
        height = define->height > 0.f ? define->height : 4.f;
    }
    float size[2] = {width, height};
    renderMaterial->setProperty("renderSize", size);
}

void Particle2DCPUEmitter::setParticleTexture(Texture2d *texture)
{
    renderMaterial->setTexture("renderTexture", texture);
    updateRenderParticleSize();
}
void Particle2DCPUEmitter::setGravity(const Vec2 &) {}
void Particle2DCPUEmitter::setVelocityPivot(const Vec2 &) {}
void Particle2DCPUEmitter::setRenderMaterial(Material *renderMaterial)
{
    if (renderMaterial == nullptr)
    {
        this->renderMaterial = nullptr;
        return;
    }
    if (this->renderMaterial != renderMaterial)
    {
        this->renderMaterial = renderMaterial;
        updateRenderMaterialParam();
    }
}
void Particle2DCPUEmitter::setRenderSceneMarkMaterial(Material *) {}
void Particle2DCPUEmitter::setPropertyTexture(Texture2d *propertyTexture)
{

    if (this->propertyTexture != propertyTexture)
    {
        this->propertyTexture = propertyTexture;
        if (renderMaterial)
        {
            renderMaterial->setTexture("propertyTexture", propertyTexture);
            float size[2];
            size[0] = propertyTexture->getWidth();
            size[1] = propertyTexture->getHeight();
            renderMaterial->setProperty("propertyTextureSize", size);
        }
    }
}

void Particle2DCPUEmitter::updateRenderMaterialParam()
{
    if (!renderMaterial)
        return;

    setMinMaxScale(define->initial_scale_min, define->initial_scale_max);
    setMinMaxInitialAngle(define->initial_angle_min, define->initial_angle_max);
    setInitColor(define->initColor);

    if (define->propertyTexture)
    {
        this->propertyTexture = nullptr;
        setPropertyTexture(define->propertyTexture);
    }

    if (define->particleTexture)
        setParticleTexture(define->particleTexture);

    updateRenderParticleSize();
}

void Particle2DCPUEmitter::update(float dt)
{
    emitter.update(dt);
    if (emitter.getActiveCount() == 0)
    {
        return;
    }
    const Particle2DCPUUpdate *particles = emitter.getParticleCPU();

    // printf("int loopStart %d count %d \n", loopStart, define->amount);

    {
        int i = loopStart;
        for (; i < define->amount; ++i)
        {
            Particle2DCPUUpdate &particle = (Particle2DCPUUpdate &)particles[i];
            particle.randomSeed = particle.initSeed;
            if (particle.particle->flag & toNumber(ParticleFlag::PARTICLE_FLAG_STARTED))
            {
                emitterShape->emit(&particle, define);
            }

            if (particle.particle->flag & toNumber(ParticleFlag::PARTICLE_FLAG_ACTIVE))
            {
                velocityUpdate.updateVelocity(dt, &particle, define);
            }

            // 分帧
            if (Application::getInstance()->getFromFrameStartTime() >= 16)
            {

                loopStart = i + 1;
                break;
            }
        }

        if (i == define->amount)
        {
            loopStart = 0;
        }
    }

    renderCount = 0;

    Paritlce2DCPURenderData *datas = emitter.getRenderData();
    for (int i = 0; i < define->amount; ++i)
    {
        Particle2DCPUUpdate &particle = (Particle2DCPUUpdate &)particles[i];
        if (particle.particle->flag & toNumber(ParticleFlag::PARTICLE_FLAG_ACTIVE))
        {
            int index = renderCount;
            ++renderCount;
            if (i != index)
            {

                datas[index].particlePosition = *particle.position;
                datas[index].lifeTimePercent = *particle.lifeTimePercent;
                particle.position = &datas[index].particlePosition;
                particle.lifeTimePercent = &datas[index].lifeTimePercent;

                Particle2DCPUUpdate &switchParticle = (Particle2DCPUUpdate &)particles[index];
                switchParticle.position = &datas[i].particlePosition;
                switchParticle.lifeTimePercent = &datas[i].lifeTimePercent;
            }
        }
    }

    if (renderCount > 0)
    {
        particleVertexBuffer->setExternalBuffer((char *)datas, renderCount * sizeof(Paritlce2DCPURenderData));
        particleVertexBuffer->setDirty(true);
    }
}

void Particle2DCPUEmitter::setAttractorShape(AttractorShapeType type) {}
void Particle2DCPUEmitter::setAttractorShapeTransform(Mat3 &mat) {}
void Particle2DCPUEmitter::setAttractorAttenuation(float attenuation) {}
void Particle2DCPUEmitter::setAttractorStrength(float strength) {}