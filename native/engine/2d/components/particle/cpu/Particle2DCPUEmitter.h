#pragma once

#include "../Particle2DEmitterDefine.h"
#include "../Particle2DEmitter.h"
#include "./shape/Particle2DEmitterShape.h"
#include "./Particle2DCPUTransemitter.h"
#include "../Particle2DEmitterDefine.h"
#include "./Particle2DCPUVelocity.h"
#include "../../../../scene/graphics/Mesh.h"

class Particles2DComponent;

/**
 *
 * 有些平台不支持RenderTexture为32F浮点的，所以不能再GPU端算
 * 所以这里提供了一个在CPU算的
 *
 * 1、将GPU端的Update阶段挪到CPU算
 * 2、渲染阶段由于不需要RenderTexture，所以渲染阶段和GPU渲染阶段是一样的
 */

class Particle2DCPUEmitter : public Particle2DEmitter
{
private:
    Particle2DCPUTransemitter emitter;
    ParticleEmitterShape shapeType;
    Particle2DEmitterShape *emitterShape;
    Particle2DEmitterDefine *define;
    Particle2DCPUVelocity velocityUpdate;

    Material *renderMaterial;
    Mesh *renderMesh;
    Texture2d *propertyTexture;
    VertexBuffer *particleVertexBuffer;
    int renderCount = 0;

    int loopStart = 0;

    void updateRenderMaterialParam();
    void updateRenderParticleSize();

public:
    Particle2DCPUEmitter(Particles2DComponent *c);
    virtual ~Particle2DCPUEmitter();
    virtual void initialize(Particle2DEmitterDefine *) override;
    virtual void setSeed(float) override;
    virtual void setRandomness(unsigned int) override;
    virtual void setAmount(int amount) override;
    virtual void setAmountRatio(float) override;

    // 获取当前已经激活的粒子的个数
    virtual int getActiveCount() override;
    virtual void setOneShot(bool b) override;
    virtual void setEmit(bool b) override;
    virtual void setEmitSpreadAngle(float angle) override;
    virtual void setEmitSpreadDirection(Vec2) override;
    virtual void setEmitInitMinMaxVelocityMuti(float, float) override;
    virtual void setEmitShape(ParticleEmitterShape) override;
    virtual void setEmitShapeRadius(float) override;
    virtual void setEmitRingShapeHeight(float) override;
    virtual void setEmitRingShapeConeAngle(float) override;
    virtual void setEmitRingShapeInnerRadius(float) override;
    virtual void setEmitRingShapeAxis(const Vec2 &) override;
    virtual void setEmitBoxExtents(const Vec2 &) override;
    virtual void setLifeTime(int lifeTime) override;
    virtual void setInheritEmitterVelocityRatio(float) override;
    virtual void setMinMaxLinearAccel(float, float) override;
    virtual void setMinMaxRadialAccel(float, float) override;
    virtual void setMinMaxTangentAccel(float, float) override;
    virtual void setMinMaxDamping(float, float) override;
    virtual void setMinMaxAngularVelocity(float, float) override;
    virtual void setMinMaxDirectionalVelocity(float, float) override;
    virtual void setMinMaxRadialVelocity(float, float) override;
    virtual void setMinMaxOrbitVelocity(float, float) override;
    virtual void setMinMaxTurbulenceInfluence(float, float) override;
    virtual void setMinMaxInitialAngle(float, float) override;
    virtual void setMinMaxScale(float, float) override;
    virtual void setInitColor(const Color &) override;
    virtual void setParticleTexture(Texture2d *texture) override;
    virtual void setGravity(const Vec2 &) override;
    virtual void setVelocityPivot(const Vec2 &) override;
    virtual void setRenderMaterial(Material *) override;
    virtual void setRenderSceneMarkMaterial(Material *) override;
    virtual void setPropertyTexture(Texture2d *propertyTexture) override;
    virtual int getRenderNum() override;
    virtual void update(float dt) override;
    virtual void setAttractorShape(AttractorShapeType type) override;
    virtual void setAttractorShapeTransform(Mat3 &mat) override;
    virtual void setAttractorAttenuation(float attenuation) override;
    virtual void setAttractorStrength(float strength) override;
};
