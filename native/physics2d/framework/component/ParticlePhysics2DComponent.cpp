#include "ParticlePhysics2DComponent.h"
#include "../PhysicsSystem.h"
#include "../PhysicsWorld.h"

#include "../assembler/ParticlePhysicsSdfRender.h"
#include "../assembler/ParticlePhysicsTextureBlur.h"

using namespace physics2d;

ParticlePhysics2DComponent::ParticlePhysics2DComponent() : RenderComponent(), particleRender(nullptr)
{
}

void ParticlePhysics2DComponent::onAwake()
{
    createParticleSystem();
}

void ParticlePhysics2DComponent::createParticleSystem()
{
    PhysicsWorld *gameBWorld = PhysicsSystem::getInstance()->getWorld();
    phxy::b2ParticleSystemDef def;
    def.userData = this;
    system = gameBWorld->CreateParticleSystem(def);
    system->SetPaused(true);
}

void ParticlePhysics2DComponent::onEnable()
{
    system->SetPaused(false);
}

void ParticlePhysics2DComponent::onDisable()
{
    if (system)
    {
        destroyAllParticle();
        system->SetPaused(true);
    }
}

void ParticlePhysics2DComponent::setUseParticleRenderType(int type)
{
    if (renderType == type)
        return;

    renderType = type;
    if (particleRender)
    {
        delete particleRender;
        particleRender = nullptr;
        setMaterial(nullptr);
    }

    /**
     * 1、使用RenderTexture，再在RenderTexture取内容
     * 2、使用SDF
     * 3、直接渲染
     * 
     */
    if (type == 1 || type == 3)
    {
        particleRender = new ParticlePhysicsTextureBlur(this);
    }
    else if (type == 2)
    {
        particleRender = new ParticlePhysicsSdfRender(this);
    }

    // printf("???????????????????? setUseParticleRenderType %d %p \n", renderType, particleRender);

    if (!particleRender)
        return;

    if (system->GetMaxParticleCount() > 0)
    {
        particleRender->allocate(system->GetMaxParticleCount());
    }

    if (getMaterial())
    {
        particleRender->onUpdateMaterial();
    }
}

void ParticlePhysics2DComponent::addParticleGroup(ParticleGroupDef &def)
{
    system->CreateParticleGroup(def);
}

void ParticlePhysics2DComponent::onUpdate()
{
    if (system && particleRender)
    {
        particleRender->update();
    }
}

bool ParticlePhysics2DComponent::canRender()
{
    return particleRender && system && getParticleCount() > 0;
}

void ParticlePhysics2DComponent::_onMaterialModified()
{
    if (particleRender)
        particleRender->onUpdateMaterial();
}

ParticlePhysics2DComponent::~ParticlePhysics2DComponent()
{
    system->getWorld()->removeContantListener(system);
    PhysicsWorld *gameBWorld = PhysicsSystem::getInstance()->getWorld();
    gameBWorld->destroyParticleSystem(system);
    system = nullptr;
}

void ParticlePhysics2DComponent::applyLinearImpulseInRang(int firstParticleIndex, int lastParticleIndexr, const Vec2 &force)
{
    phxy::SqVec2 bVec;
    bVec.x = force.x;
    bVec.y = force.y;
    // system->ApplyLinearImpulse(firstParticleIndex, lastParticleIndexr, bVec);
}

void ParticlePhysics2DComponent::applyForceInRang(int firstParticleIndex, int lastParticleIndexr, const Vec2 &force)
{
    phxy::SqVec2 bVec;
    bVec.x = force.x;
    bVec.y = force.y;
    // system->ApplyForce(firstParticleIndex, lastParticleIndexr, bVec);
}

void ParticlePhysics2DComponent::setViscousStrength(float v)
{
    system->SetViscousStrength(v);
}

void ParticlePhysics2DComponent::setSurfaceTensionPressureStrength(float v)
{
    system->SetSurfaceTensionPressureStrength(v);
}

void ParticlePhysics2DComponent::setSurfaceTensionNormalStrength(float v)
{
    system->SetSurfaceTensionNormalStrength(v);
}

int ParticlePhysics2DComponent::createParticle(const ParticlePhysicsCreateDefine *const def)
{

    phxy::SqParticleDef b2Def;
    b2Def.flags = def->flags;
    b2Def.lifetime = def->lifetime;
    b2Def.position.x = def->position.x;
    b2Def.position.y = def->position.y;
    b2Def.velocity.x = def->velocity.x;
    b2Def.velocity.y = def->velocity.y;
    b2Def.color.r = def->r;
    b2Def.color.g = def->g;
    b2Def.color.b = def->b;
    b2Def.color.a = def->a;
    return system->CreateParticle(b2Def);
}

void ParticlePhysics2DComponent::setParticleRadius(float radius)
{

    system->SetRadius(radius);
}

void ParticlePhysics2DComponent::allocate(int count)
{
    if (count > system->GetMaxParticleCount())
    {
        system->SetMaxParticleCount(count);
    }
    system->allocateParticle(count);
    if (particleRender)
        particleRender->allocate(count);
}

void ParticlePhysics2DComponent::setPressureStrength(float v)
{
    system->setPressureStrength(v);
}

void ParticlePhysics2DComponent::setDestructionByAge(bool b)
{
    system->SetDestructionByAge(b);
}

void ParticlePhysics2DComponent::setParticleLifetime(int particleIndex, float time)
{
    system->SetParticleLifetime(particleIndex, time);
}

void ParticlePhysics2DComponent::destroyParticle(int index)
{
    system->DestroyParticle(index, true);
}

void ParticlePhysics2DComponent::setPaused(bool b)
{
    system->SetPaused(b);
}

void ParticlePhysics2DComponent::destroyOldestParticle(int num)
{

    // 位于最前面的粒子是最早创建的，那么就认为这些粒子是最old的
    int count = system->GetParticleCount();
    num = Math::min(count, num);

    for (int i = 0; i < num; ++i)
    {
        int particleId = system->findParticleIdByIndex(i);
        if (particleId != -1)
        {
            system->DestroyParticle(particleId, true);
        }
    }
}

void ParticlePhysics2DComponent::destroyAllParticle()
{
    int count = system->GetParticleCount();
    for (int i = 0; i < count; ++i)
    {
        int particleId = system->findParticleIdByIndex(i);
        system->DestroyParticle(particleId, true);
    }
}

void ParticlePhysics2DComponent::setBodyContactFilter(uint32_t categoryBits, uint32_t maskBits)
{
    phxy::SqShapeFilter filter;
    filter.categoryBits = categoryBits;
    filter.maskBits = maskBits;
    system->setBodyContactFilter(filter);
}

void ParticlePhysics2DComponent::checkAnClearEnableListener()
{
    if (listenerState == 0 && !beginBodyContactCallHandler.has_value() && !endBodyContactCallHandler.has_value() && !beginParticleContactCallHandler.has_value() && endParticleContactCallHandler.has_value())
    {
        system->getWorld()->removeContantListener(system);
    }
}

void ParticlePhysics2DComponent::enableListener(bool b, char stage)
{

    if (b)
    {
        system->getWorld()->registerContactListener(system, this);
    }

    if (stage == 0)
    {
        if (!b)
            checkAnClearEnableListener();
        return;
    }

    /**
     * 记录JS端是否监听了ContactListener
     */
    unsigned int op = 0;
    switch (stage)
    {
    case 1:              // BodyContact
        op = b ? 2 : ~2; // 1 << 1
        break;
    case 2:              // ParticleContact
        op = b ? 4 : ~4; // 1 << 2
        break;
    default:
        SQ_ASSERT(false);
    }

    if (b)
    {
        listenerState |= op;
    }
    else
    {
        listenerState &= op;
    }

    if (!b)
        checkAnClearEnableListener();
}

void ParticlePhysics2DComponent::setBeginBodyContactCallback(std::function<BodyContactCallback> beginContactCallback)
{
    beginBodyContactCallHandler = beginContactCallback;
    enableListener(true, 0);
}

void ParticlePhysics2DComponent::offBeginBodyContactCallback()
{
    beginBodyContactCallHandler.reset();
    enableListener(false, 0);
}

void ParticlePhysics2DComponent::setEndBodyContactCallback(std::function<BodyContactCallback> endContactCallback)
{
    endBodyContactCallHandler = endContactCallback;
    enableListener(true, 0);
}

void ParticlePhysics2DComponent::offEndBodyContactCallback()
{
    endBodyContactCallHandler.reset();
    enableListener(false, 0);
}

void ParticlePhysics2DComponent::setBeginParticleContactCallback(std::function<BeginParticleContactCallback> callback)
{
    beginParticleContactCallHandler = callback;
    enableListener(true, 0);
}

void ParticlePhysics2DComponent::offBeginParticleContactCallback()
{
    beginParticleContactCallHandler.reset();
    enableListener(false, 0);
}

void ParticlePhysics2DComponent::setEndParticleContactCallback(std::function<EndParticleContactCallback> callback)
{
    endParticleContactCallHandler = callback;
    enableListener(true, 0);
}

void ParticlePhysics2DComponent::offEndParticleContactCallback()
{
    endParticleContactCallHandler.reset();
    enableListener(false, 0);
}

void ParticlePhysics2DComponent::onBeginContactBody(phxy::SqParticleBodyContact *contact)
{

    if (beginBodyContactCallHandler.has_value())
    {
        beginBodyContactCallHandler.value()(contact);
    }

    // js端的监听
    if (listenerState & 2)
    {
        bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
        nativeToJs.beginOp(PhysicsWorld::bridgeOp_onParticleBeginContantBody);
        unsigned int dataAdress = reinterpret_cast<unsigned int>(contact);
        nativeToJs.writeOpArg(dataAdress);
        nativeToJs.endOp();
    }
}

void ParticlePhysics2DComponent::onEndContactBody(phxy::SqParticleBodyContact *contact)
{
    if (endBodyContactCallHandler.has_value())
    {
        endBodyContactCallHandler.value()(contact);
    }

    // js端的监听
    if (listenerState & 2)
    {
        bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
        nativeToJs.beginOp(PhysicsWorld::bridgeOp_onParticleEndContantBody);
        unsigned int dataAdress = reinterpret_cast<unsigned int>(contact);
        nativeToJs.writeOpArg(dataAdress);
        nativeToJs.endOp();
    }
}

void ParticlePhysics2DComponent::onBeginContactParticle(phxy::SqParticleContact *particleContact)
{
    if (beginParticleContactCallHandler.has_value())
    {
        beginParticleContactCallHandler.value()(particleContact);
    }

    // js端的监听
    if (listenerState & 4)
    {
        SQ_ASSERT(node->nativeId != -1);
        bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
        nativeToJs.beginOp(PhysicsWorld::bridgeOp_onParticleBeginContantParticle);
        nativeToJs.writeOpArg(node->nativeId);
        unsigned int dataAdress = reinterpret_cast<unsigned int>(particleContact);
        nativeToJs.writeOpArg(dataAdress);
        nativeToJs.endOp();
    }
}

void ParticlePhysics2DComponent::onEndContactParticle(int particleIdA, int particleIdB)
{
    if (endParticleContactCallHandler.has_value())
    {
        endParticleContactCallHandler.value()(particleIdA, particleIdB);
    }

    // js端的监听
    if (listenerState & 4)
    {
        SQ_ASSERT(node->nativeId != -1);
        bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
        nativeToJs.beginOp(PhysicsWorld::bridgeOp_onParticleEndContantParticle);
        nativeToJs.writeOpArg(node->nativeId);
        nativeToJs.writeOpArg(particleIdA);
        nativeToJs.writeOpArg(particleIdB);
        nativeToJs.endOp();
    }
}

void ParticlePhysics2DComponent::setViewportSize(float w, float h)
{
    // printf("???????????????????? renderType %d \n", renderType);
    if (renderType == 2)
        static_cast<ParticlePhysicsSdfRender *>(particleRender)->setViewportSize(w, h);
}

void ParticlePhysics2DComponent::setViewportPos(float x, float y)
{
    if (renderType == 2)
        static_cast<ParticlePhysicsSdfRender *>(particleRender)->setViewportPos(x, y);
}

Vec2 ParticlePhysics2DComponent::getParticlePosition(int particleId)
{
    phxy::SqParticle *particle = system->findById(particleId);
    phxy::SqVec2 &v = particle->getPosition();
    return Vec2(v.x, v.y);
}
