#include "Particles2DComponent.h"
#include "../../../framework/Application.h"
#include "./gpu/Particle2DGPUEmitter.h"
#include "./cpu/Particle2DCPUEmitter.h"
#include "../../../assets/AssetManager.h"

Particles2DComponent::Particles2DComponent()
{
}

void Particles2DComponent::onStart()
{
    initEmitter();
}


Particles2DComponent::~Particles2DComponent()
{
    delete particle2DEmitter;
    particle2DEmitter = nullptr;
}

bool Particles2DComponent::canRender()
{

    // printf("canRender %p  %d %d \n", particle2DEmitter, define.amount, particle2DEmitter ? particle2DEmitter->getActiveCount() : 0);
    return particle2DEmitter != nullptr && define.amount > 0 && particle2DEmitter->getActiveCount() > 0;
}

void Particles2DComponent::onUpdate()
{

    if (particle2DEmitter != nullptr && define.amount > 0)
    {
        particle2DEmitter->update(Application::getInstance()->dt);
        // printf("Particle render %d \n",particle2DEmitter->getActiveCount() );
    }
    // printf("=========update\n");
}

void Particles2DComponent::setAmount(int amount)
{
    if (define.amount != amount)
    {
        define.amount = amount;
        if (particle2DEmitter)
        {
            particle2DEmitter->setAmount(amount);
        }
        else
        {
            initEmitter();
        }
    }
}

void Particles2DComponent::setAmountRatio(float ratio)
{
    if (define.amountRatio != ratio)
    {
        define.amountRatio = ratio;
        if (particle2DEmitter)
        {
            particle2DEmitter->setAmountRatio(ratio);
        }
    }
}

void Particles2DComponent::setOneShot(bool b)
{
    if (define.oneShot != b)
    {
        define.oneShot = b;
        if (particle2DEmitter)
        {
            particle2DEmitter->setOneShot(b);
        }
    }
}

void Particles2DComponent::setEmit(bool b)
{
    if (define.emit != b)
    {
        define.emit = b;
        if (particle2DEmitter)
        {
            particle2DEmitter->setEmit(b);
        }
    }
}

void Particles2DComponent::setSeed(float seed)
{
    if (define.randomSeed != seed)
    {
        define.randomSeed = seed;
        if (particle2DEmitter)
        {
            particle2DEmitter->setSeed(seed);
        }
    }
}

void Particles2DComponent::setEmitSpreadAngle(float angle)
{
    if (define.spreadAngle != angle)
    {
        define.spreadAngle = angle;
        if (particle2DEmitter)
        {
            particle2DEmitter->setEmitSpreadAngle(angle);
        }
    }
}

void Particles2DComponent::setEmitSpreadDirection(Vec2 &d)
{
    if (define.spreadDirection.x != d.x || define.spreadDirection.y != d.y)
    {
        define.spreadDirection.x = d.x;
        define.spreadDirection.y = d.y;
        if (particle2DEmitter)
        {
            particle2DEmitter->setEmitSpreadDirection(d);
        }
    }
}

void Particles2DComponent::setEmitInitMinMaxVelocityMuti(float min, float max)
{
    if (define.initEmitVelocityMinMuti != min || define.initEmitVelocityMaxMuti != max)
    {
        define.initEmitVelocityMinMuti = min;
        define.initEmitVelocityMaxMuti = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setEmitInitMinMaxVelocityMuti(min, max);
        }
    }
}

void Particles2DComponent::setMinMaxLinearAccel(float min, float max)
{
    if (define.linear_accel_min != min || define.linear_accel_max != max)
    {
        define.linear_accel_min = min;
        define.linear_accel_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxLinearAccel(min, max);
        }
    }
}

void Particles2DComponent::setMinMaxRadialAccel(float min, float max)
{
    if (define.radial_accel_min != min || define.radial_accel_max != max)
    {
        define.radial_accel_min = min;
        define.radial_accel_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxRadialAccel(min, max);
        }
    }
}

void Particles2DComponent::setMinMaxTangentAccel(float min, float max)
{
    if (define.tangent_accel_min != min || define.tangent_accel_max != max)
    {
        define.tangent_accel_min = min;
        define.tangent_accel_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxTangentAccel(min, max);
        }
    }
}

void Particles2DComponent::setMinMaxDamping(float min, float max)
{
    if (define.damping_min != min || define.damping_max != max)
    {
        define.damping_min = min;
        define.damping_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxDamping(min, max);
        }
    }
}

void Particles2DComponent::setInheritEmitterVelocityRatio(float v)
{
    if (define.inheritEmitterVelocityRatio != v)
    {
        define.inheritEmitterVelocityRatio = v;
        if (particle2DEmitter)
        {
            particle2DEmitter->setInheritEmitterVelocityRatio(v);
        }
    }
}

void Particles2DComponent::setMinMaxAngularVelocity(float min, float max)
{
    if (define.angular_velocity_min != min || define.angular_velocity_max != max)
    {
        define.angular_velocity_min = min;
        define.angular_velocity_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxAngularVelocity(min, max);
        }
    }
}
void Particles2DComponent::setMinMaxDirectionalVelocity(float min, float max)
{
    if (define.directional_velocity_min != min || define.directional_velocity_max != max)
    {
        define.directional_velocity_min = min;
        define.directional_velocity_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxDirectionalVelocity(min, max);
        }
    }
}
void Particles2DComponent::setMinMaxRadialVelocity(float min, float max)
{
    if (define.radial_velocity_min != min || define.radial_velocity_max != max)
    {
        define.radial_velocity_min = min;
        define.radial_velocity_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxRadialVelocity(min, max);
        }
    }
}
void Particles2DComponent::setMinMaxOrbitVelocity(float min, float max)
{
    if (define.orbit_velocity_min != min || define.orbit_velocity_max != max)
    {
        define.orbit_velocity_min = min;
        define.orbit_velocity_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxOrbitVelocity(min, max);
        }
    }
}
void Particles2DComponent::setMinMaxTurbulenceInfluence(float min, float max)
{
    if (define.turbulence_influence_min != min || define.turbulence_influence_max != max)
    {
        define.turbulence_influence_min = min;
        define.turbulence_influence_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxTurbulenceInfluence(min, max);
        }
    }
}
void Particles2DComponent::setMinMaxInitialAngle(float min, float max)
{
    if (define.initial_angle_min != min || define.initial_angle_max != max)
    {
        define.initial_angle_min = min;
        define.initial_angle_max = max;
        if (particle2DEmitter)
        {
            min = Math::angleToRadian(min);
            max = Math::angleToRadian(max);
            particle2DEmitter->setMinMaxInitialAngle(min, max);
        }
    }
}

void Particles2DComponent::setMinMaxScale(float min, float max)
{
    if (define.initial_scale_min != min || define.initial_scale_max != max)
    {
        define.initial_scale_min = min;
        define.initial_scale_max = max;
        if (particle2DEmitter)
        {
            particle2DEmitter->setMinMaxScale(min, max);
        }
    }
}

void Particles2DComponent::setParticleTexture(Texture2d *texture)
{
    if (define.particleTexture == texture)
        return;
    define.particleTexture = texture;
    if (particle2DEmitter)
    {
        particle2DEmitter->setParticleTexture(texture);
    }
}

void Particles2DComponent::setPropertyTexture(Texture2d *propertyTexture)
{
    if (define.propertyTexture == propertyTexture)
        return;
    define.propertyTexture = propertyTexture;
    if (particle2DEmitter)
    {
        particle2DEmitter->setPropertyTexture(propertyTexture);
    }
}

void Particles2DComponent::setGravity(const Vec2 &gravity)
{
    if (define.gravity.x != gravity.x || define.gravity.y != gravity.y)
    {
        define.gravity = gravity;
        if (particle2DEmitter)
        {
            particle2DEmitter->setGravity(gravity);
        }
    }
}

void Particles2DComponent::setVelocityPivot(const Vec2 &v)
{
    define.velocityPivot = v;
    if (particle2DEmitter)
    {
        particle2DEmitter->setVelocityPivot(v);
    }
}

void Particles2DComponent::setEmitShape(ParticleEmitterShape shape)
{
    if (define.shape == shape)
        return;
    define.shape = shape;
    if (particle2DEmitter)
    {
        particle2DEmitter->setEmitShape(shape);
    }
}

void Particles2DComponent::setEmitShapeOffset(const Vec2 &v)
{
    define.shapeOffet = v;
    if (particle2DEmitter)
    {
    }
}
void Particles2DComponent::setEmitShapeRadius(float radius)
{
    if (define.emssion_shape_radius == radius)
        return;
    define.emssion_shape_radius = radius;
    if (particle2DEmitter)
    {
        particle2DEmitter->setEmitShapeRadius(radius);
    }
}

void Particles2DComponent::setEmitRingShapeHeight(float height)
{
    if (define.emission_ring_height == height)
    {
        return;
    }
    define.emission_ring_height = height;
    if (particle2DEmitter)
    {
        particle2DEmitter->setEmitRingShapeHeight(height);
    }
}

void Particles2DComponent::setEmitRingShapeConeAngle(float angle)
{
    if (define.emission_ring_cone_angle == angle)
        return;
    define.emission_ring_cone_angle = angle;
    if (particle2DEmitter)
    {
        particle2DEmitter->setEmitRingShapeConeAngle(angle);
    }
}

void Particles2DComponent::setEmitRingShapeInnerRadius(float radius)
{
    if (define.emission_ring_inner_radius == radius)
        return;
    define.emission_ring_inner_radius = radius;
    if (particle2DEmitter)
    {
        particle2DEmitter->setEmitRingShapeInnerRadius(radius);
    }
}

void Particles2DComponent::setEmitRingShapeAxis(const Vec2 &axis)
{
    if (define.emission_ring_axis.x == axis.x && define.emission_ring_axis.y == axis.y)
    {
        return;
    }
    define.emission_ring_axis = axis;
    if (particle2DEmitter)
    {
        particle2DEmitter->setEmitRingShapeAxis(axis);
    }
}

void Particles2DComponent::setEmitBoxExtents(const Vec2 &extents)
{
    if (define.emission_box_extents.x == extents.x && define.emission_box_extents.y == extents.y)
    {
        return;
    }
    define.emission_box_extents = extents;
    if (particle2DEmitter)
    {
        particle2DEmitter->setEmitBoxExtents(extents);
    }
}

void Particles2DComponent::setLifeTime(float lifeTime)
{
    if (define.lifeTime == lifeTime)
        return;
    define.lifeTime = lifeTime;
    if (particle2DEmitter)
    {
        particle2DEmitter->setLifeTime(lifeTime);
    }
}

void Particles2DComponent::Particles2DComponent::setColor(Color &initColor)
{
    if (define.initColor == initColor)
        return;

    define.initColor = initColor;
    if (particle2DEmitter)
    {
        particle2DEmitter->setInitColor(initColor);
    }
}

void Particles2DComponent::initEmitter()
{

    if (particle2DEmitter || define.amount <= 0 || !node->activeInHierarchy())
        return;

    if (define.gpuRenderMaterial && define.gpuUpdateProcessMaterial)
    {
        useCPU = false;
        particle2DEmitter = new Particle2DGPUEmitter(this);
        particle2DEmitter->initialize(&define);
    }
    else if (define.cpuRenderMaterial)
    {
        useCPU = true;
        particle2DEmitter = new Particle2DCPUEmitter(this);
        particle2DEmitter->initialize(&define);
    }
}

void Particles2DComponent::setUpdateGPUProcessMaterial(Material *m)
{
    if (define.gpuUpdateProcessMaterial == m)
        return;
    define.gpuUpdateProcessMaterial = m;
    if (particle2DEmitter && !useCPU)
    {
        static_cast<Particle2DGPUEmitter *>(particle2DEmitter)->setUpdateProcessMaterial(define.gpuUpdateProcessMaterial);
    }
    else
    {
        initEmitter();
    }
}

/**
 * 切换到GPU渲染模式
 */
void Particles2DComponent::setGPURenderMaterial(Material *m)
{
    if (define.gpuRenderMaterial == m)
        return;
    define.gpuRenderMaterial = m;
    if (particle2DEmitter)
    {

        if (useCPU)
        {
            delete particle2DEmitter;
            define.cpuRenderMaterial = nullptr;
            particle2DEmitter = new Particle2DGPUEmitter(this);
        }
        useCPU = false;
        particle2DEmitter->setRenderMaterial(define.gpuRenderMaterial);
    }
    else
    {
        initEmitter();
    }
    setMaterial(define.gpuRenderMaterial);
}

/**
 * 调用此方法设置CPU Material表示切换到CPU计算模式
 */
void Particles2DComponent::setCPURenderMaterial(Material *cpuMaterial)
{
    if (define.cpuRenderMaterial == cpuMaterial)
        return;
    define.cpuRenderMaterial = cpuMaterial;
    if (particle2DEmitter)
    {
        if (!useCPU)
        {
            delete particle2DEmitter;
            define.gpuRenderMaterial = nullptr;
            define.gpuRenderMaterial = nullptr;
            particle2DEmitter = new Particle2DCPUEmitter(this);
        }
        useCPU = true;
        particle2DEmitter->setRenderMaterial(define.cpuRenderMaterial);
    }
    else
    {
        initEmitter();
    }
    setMaterial(define.cpuRenderMaterial);
}

int Particles2DComponent::getRenderModel()
{
    return define.cpuRenderMaterial != nullptr ? 1 : define.gpuRenderMaterial != nullptr ? 2
                                                                                         : 0;
}

void Particles2DComponent::setRenderSceneMarkMaterial(Material *m)
{
    if (define.renderSceneMarkMaterial == m)
        return;
    define.renderSceneMarkMaterial = m;
    if (particle2DEmitter)
    {
        particle2DEmitter->setRenderSceneMarkMaterial(define.renderSceneMarkMaterial);
    }
}

MeshInstance *Particles2DComponent::getGPUUpdateMeshInstance()
{
    if (particle2DEmitter && !useCPU)
    {
        return ((Particle2DGPUEmitter *)particle2DEmitter)->getGPUUpdateMeshInstance();
    }
    return nullptr;
}

MeshInstance *Particles2DComponent::getTestRenderMeshInstance()
{
    if (particle2DEmitter && !useCPU)
    {
        return ((Particle2DGPUEmitter *)particle2DEmitter)->getTestRenderMeshInstance();
    }
    return nullptr;
}

Mesh *Particles2DComponent::getMeshRenderSceneMark()
{
    if (particle2DEmitter && dynamic_cast<Particle2DGPUEmitter *>(particle2DEmitter))
    {
        return ((Particle2DGPUEmitter *)particle2DEmitter)->getMeshRenderSceneMark();
    }
    return nullptr;
}

void Particles2DComponent::setEmitTexturePoints(Texture2d *emitTexturePoints)
{
    if (define.emitTexturePoints != emitTexturePoints)
    {
        define.emitTexturePoints = emitTexturePoints;
        if (particle2DEmitter)
        {
            static_cast<Particle2DGPUEmitter *>(particle2DEmitter)->setEmitTexturePoints(emitTexturePoints);
        }
    }
}

void Particles2DComponent::setPartilceRenderSize(float width, float height)
{
    if (define.width != width || define.height != height)
    {
        define.width = width;
        define.height = height;
        if (particle2DEmitter)
        {
            static_cast<Particle2DGPUEmitter *>(particle2DEmitter)->setPartilceRenderSize(width, height);
        }
    }
}

void Particles2DComponent::setEmitTexturePointCount(float num)
{
    if (define.emissionTexturePointCount != num)
    {
        define.emissionTexturePointCount = num;
        if (particle2DEmitter)
        {
            static_cast<Particle2DGPUEmitter *>(particle2DEmitter)->setEmitTexturePointCount(num);
        }
    }
}

void Particles2DComponent::setEmitTexturePointEnableNormal(bool b)
{
    if (define.emitEnableTexturePointNomral != b)
    {
        define.emitEnableTexturePointNomral = b;
        if (particle2DEmitter)
        {
            static_cast<Particle2DGPUEmitter *>(particle2DEmitter)->setEmitTexturePointEnableNormal(b);
        }
    }
}

void Particles2DComponent::setEmitPointsBuffer(char *buffer, int size)
{
    if (particle2DEmitter && dynamic_cast<Particle2DCPUEmitter *>(particle2DEmitter))
    {
        // dynamic_cast<Particle2DCPUEmitter*>(particle2DEmitter)
    }
}

void Particles2DComponent::setAttractorShape(AttractorShapeType type)
{
    if (define.attractorShapeType != type)
    {
        define.attractorShapeType = type;
        if (particle2DEmitter)
        {
            particle2DEmitter->setAttractorShape(type);
        }
    }
}

void Particles2DComponent::setAttractorShapeTransform(Mat3 &mat)
{
    if (define.attractorShapeTransform != mat)
    {
        define.attractorShapeTransform = mat;
        if (particle2DEmitter)
        {
            particle2DEmitter->setAttractorShapeTransform(mat);
        }
    }
}

void Particles2DComponent::setAttractorAttenuation(float attenuation)
{
    if (define.attractor_attenuation != attenuation)
    {
        define.attractor_attenuation = attenuation;
        if (particle2DEmitter)
        {
            particle2DEmitter->setAttractorAttenuation(attenuation);
        }
    }
}

void Particles2DComponent::setAttractorStrength(float strength)
{
    if (define.attractor_strength != strength)
    {
        define.attractor_strength = strength;
        if (particle2DEmitter)
        {
            particle2DEmitter->setAttractorStrength(strength);
        }
    }
}

void bindingProcessParticles2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<Particles2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<Particles2DComponent>();
        return;
    }
    Particles2DComponent *component = node->getComponent<Particles2DComponent>();
    if (op < 50)
    {
        bindingProcessRenderComponent(component, type, nodeOp, op, buffer, node);
        return;
    }

    switch (op)
    {
    case 51:
        component->setAmount(*buffer.popp<int>());
        break;
    case 52:
        component->setEmitSpreadAngle(*buffer.popp<float>());
        break;
    case 53:
        component->setEmitInitMinMaxVelocityMuti(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 54:
        component->setMinMaxLinearAccel(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 55:
    {
        Vec2 direction;
        direction.x = *buffer.popp<float>();
        direction.y = *buffer.popp<float>();
        component->setEmitSpreadDirection(direction);
        break;
    }
    case 56:
        component->setMinMaxRadialAccel(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 57:
        component->setMinMaxTangentAccel(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 58:
        component->setMinMaxDamping(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 59:
        component->setMinMaxAngularVelocity(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 60:
        component->setMinMaxDirectionalVelocity(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 61:
        component->setMinMaxOrbitVelocity(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 62:
        component->setMinMaxRadialVelocity(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 63:
        component->setMinMaxTurbulenceInfluence(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 64:
        component->setMinMaxInitialAngle(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    case 65:
    {
        Vec2 gravity(*buffer.popp<float>(), *buffer.popp<float>());
        component->setGravity(gravity);
        break;
    }
    case 66:
    {
        Vec2 velocityPrivot(*buffer.popp<float>(), *buffer.popp<float>());
        component->setVelocityPivot(velocityPrivot);
        break;
    }
    case 67:
    {
        ParticleEmitterShape shape = ParticleEmitterShape(*buffer.popp<char>());
        component->setEmitShape(shape);
        break;
    }
    case 68:
    {
        Vec2 offset(*buffer.popp<float>(), *buffer.popp<float>());
        component->setEmitShapeOffset(offset);
        break;
    }
    case 69:
    {
        component->setEmitShapeRadius(*buffer.popp<float>());
        break;
    }
    case 70:
    {
        component->setEmitRingShapeHeight(*buffer.popp<float>());
        break;
    }
    case 71:
    {
        component->setEmitRingShapeConeAngle(*buffer.popp<float>());
        break;
    }
    case 72:
    {
        component->setEmitRingShapeInnerRadius(*buffer.popp<float>());
        break;
    }
    case 73:
    {
        Vec2 axis(*buffer.popp<float>(), *buffer.popp<float>());
        component->setEmitRingShapeAxis(axis);
        break;
    }
    case 74:
    {
        component->setLifeTime(*buffer.popp<float>());
        break;
    }
    case 75:
    {
        Texture2d *texture = static_cast<Texture2d *>(AssetManager::getInstance()->findById(*buffer.popp<int>()));
        SQ_ASSERT(texture);
        component->setParticleTexture(texture);
        break;
    }
    case 76:
    {
        component->setEmit(*buffer.popp<char>());
        break;
    }
    case 77:
    {
        component->setOneShot(*buffer.popp<char>());
        break;
    }
    case 78:
    {
        Texture2d *texture = static_cast<Texture2d *>(AssetManager::getInstance()->findById(*buffer.popp<int>()));
        SQ_ASSERT(texture);
        component->setPropertyTexture(texture);
        break;
    }
    case 79:
    {
        component->setMinMaxScale(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    }

    case 80:
    {
        int assetId = *buffer.popp<int>();
        Material *m = static_cast<Material *>(AssetManager::getInstance()->findById(assetId));
        SQ_ASSERT(m);
        component->setUpdateGPUProcessMaterial(m);
        break;
    }
    case 81:
    {
        char type = *buffer.popp<char>();
        Material *m = static_cast<Material *>(AssetManager::getInstance()->findById(*buffer.popp<int>()));
        SQ_ASSERT(m);
        if (type == 1)
        {
            component->setCPURenderMaterial(m);
        }
        else
        {
            component->setGPURenderMaterial(m);
        }
        break;
    }
    case 82:
    {
        Texture2d *m = static_cast<Texture2d *>(AssetManager::getInstance()->findById(*buffer.popp<int>()));
        SQ_ASSERT(m);
        component->setEmitTexturePoints(m);
        break;
    }
    case 83:
    {
        component->setPartilceRenderSize(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    }
    case 84:
    {
        component->setEmitTexturePointCount(*buffer.popp<float>());
        break;
    }
    case 85:
    {
        component->setEmitTexturePointEnableNormal(*buffer.popp<char>());
        break;
    }
    case 86:
    {
        component->setAmountRatio(*buffer.popp<float>());
        break;
    }
    case 87:
    {
        Color color;
        color.r = *buffer.popp<float>();
        color.g = *buffer.popp<float>();
        color.b = *buffer.popp<float>();
        color.a = *buffer.popp<float>();
        component->setColor(color);
        break;
    }
    case 88:
    {
        Material *m = static_cast<Material *>(AssetManager::getInstance()->findById(*buffer.popp<int>()));
        SQ_ASSERT(m);
        component->setRenderSceneMarkMaterial(m);
        break;
    }
    case 89:
    {
        component->setInheritEmitterVelocityRatio(*buffer.popp<float>());
        break;
    }
    case 90:
    {
        component->setSeed(*buffer.popp<float>());
        break;
    }
    case 91:
    {
        Vec2 extents(*buffer.popp<float>(), *buffer.popp<float>());
        component->setEmitBoxExtents(extents);
        break;
    }
    case 92:
    {
        component->setEmitPointsBuffer((char *)buffer.popp<unsigned int>(), *buffer.popp<int>());
        break;
    }
    case 93:
    {
        component->setAttractorShape(AttractorShapeType(*buffer.popp<char>()));
        break;
    }
    case 94:
    {
        Mat3 mat;
        mat.data[0] = *buffer.popp<float>();
        mat.data[1] = *buffer.popp<float>();
        mat.data[2] = *buffer.popp<float>();
        mat.data[3] = *buffer.popp<float>();
        mat.data[4] = *buffer.popp<float>();
        mat.data[5] = *buffer.popp<float>();
        component->setAttractorShapeTransform(mat);
        break;
    }
    case 95:
    {
        component->setAttractorAttenuation(*buffer.popp<float>());
        break;
    }
    case 96:
    {
        component->setAttractorStrength(*buffer.popp<float>());
        break;
    }
    default:
        SQ_ASSERT(false);
    }
}