#pragma once

#include "Particle2DEmitterDefine.h"
#include "../../../core/math/Color.h"
class Particles2DComponent;

class Particle2DEmitter
{

protected:
    Particles2DComponent *component;
public:
    Particle2DEmitter(Particles2DComponent *c) : component(c) {};
    
    virtual ~Particle2DEmitter() = default;
    virtual void initialize(Particle2DEmitterDefine *) = 0;

    /**
     * 设置粒子系统总共最大可以发射多少粒子
     * 这是一个非常重度的操作
     */
    virtual void setAmount(int amount) = 0;

    virtual void setAmountRatio(float) = 0;

    virtual void setLifeTime(int lifeTime) = 0;

    virtual void setOneShot(bool b) = 0;

    virtual void setEmit(bool b) = 0;

    //获取当前已经激活的粒子的个数
    virtual int getActiveCount() = 0;

    /**
     *
     * 全局所有的随机行为都受到此种子的限制
     * 使得粒子系统在同一个种子之下表现的行为一样
     */
    virtual void setSeed(float) = 0;

    virtual void setRandomness(unsigned int) = 0;

    /**
     * 发射的时候速度的角度，所有的粒子在这个范围内随机
     * 用此控制发射的速度的方向
     */
    virtual void setEmitSpreadAngle(float angle) = 0;

    /**
     * 与spreadAngle一起控制粒子发射速度的方向
     * 设置此方向使得粒子偏向集中在这个方向上发射
     */
    virtual void setEmitSpreadDirection(Vec2) = 0;

    /**
     * 设置发射的时候粒子的发射速度的大小。会在最小速度和最大速度这个范围内随机
     * 随机也是基于上面设置的seed进行随机
     *
     * 这里速度的大小+上面速度的方向共同确定了发射的初始速度。
     */
    virtual void setEmitInitMinMaxVelocityMuti(float, float) = 0;

    /**
     * 在发射的时候，上一帧的速度乘以这里设置的比例，再加到初始速度上，得到最终的发射速度
     */
    virtual void setInheritEmitterVelocityRatio(float) = 0;

    virtual void setEmitShape(ParticleEmitterShape) = 0;
    virtual void setEmitShapeRadius(float) = 0;
    virtual void setEmitRingShapeHeight(float) = 0;
    virtual void setEmitRingShapeConeAngle(float) = 0;
    virtual void setEmitRingShapeInnerRadius(float) = 0;
    virtual void setEmitRingShapeAxis(const Vec2 &) = 0;
    virtual void setEmitBoxExtents(const Vec2 &) = 0;

    /**
     * 设置线加速度的随机范围值
     */
    virtual void setMinMaxLinearAccel(float, float) = 0;

    /**
     * 设置环绕加速度的随机范围
     */
    virtual void setMinMaxRadialAccel(float, float) = 0;

    /**
     * 设置切线加速度的随机范围
     */
    virtual void setMinMaxTangentAccel(float, float) = 0;

    /**
     * 设置阻尼的随机范围
     */
    virtual void setMinMaxDamping(float, float) = 0;

    virtual void setMinMaxAngularVelocity(float, float) = 0;
    virtual void setMinMaxDirectionalVelocity(float, float) = 0;

    /**
     * 计算环绕速度
     */
    virtual void setMinMaxRadialVelocity(float, float) = 0;

    /**
     * 计算径向速度
     */
    virtual void setMinMaxOrbitVelocity(float, float) = 0;
    virtual void setMinMaxTurbulenceInfluence(float, float) = 0;

    virtual void setMinMaxInitialAngle(float, float) = 0;
    virtual void setMinMaxScale(float, float) = 0;
    virtual void setParticleTexture(Texture2d *texture) = 0;
    virtual void setInitColor(const Color&) = 0;
    virtual void setGravity(const Vec2 &) = 0;

    /**
     * 当计算Oribt和Radia速度时，都需要使用到这个速度锚点
     */
    virtual void setVelocityPivot(const Vec2 &) = 0;


    virtual void update(float dt) = 0;

    virtual void setRenderMaterial(Material *)  = 0;
    virtual void setPropertyTexture(Texture2d *propertyTexture) = 0;
    virtual void setRenderSceneMarkMaterial(Material *) = 0;
    virtual int getRenderNum() = 0;

    virtual void setAttractorShape(AttractorShapeType type) = 0;
    virtual void setAttractorShapeTransform(Mat3 &mat) = 0;
    virtual void setAttractorAttenuation(float attenuation) = 0;
    virtual void setAttractorStrength(float strength) = 0;
};
