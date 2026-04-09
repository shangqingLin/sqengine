#pragma once

/**
 * https://www.pixiparticles.com/pixijs-particle-emitter
 *
 * https://www.semanticscholar.org/paper/Building-a-Million-Particle-System-Latta/140c40f5f75f191a78e808588987600179ed0625?p2df
 */

#include "../../../framework/component/RenderComponent.h"
#include "Particle2DEmitterDefine.h"
#include "./Particle2DEmitter.h"

void bindingProcessParticles2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

class Particles2DComponent : public RenderComponent
{
private:
    Particle2DEmitterDefine define;
    Particle2DEmitter *particle2DEmitter = nullptr;
    bool useCPU = false;
    virtual bool canRender();
    virtual void onUpdate();
    virtual void onStart();
    void initEmitter();
public:
    friend class Particle2DGPUEmitter;

    Particles2DComponent();
    ~Particles2DComponent();

    void setUpdateGPUProcessMaterial(Material *);
    MeshInstance *getGPUUpdateMeshInstance();
    MeshInstance *getTestRenderMeshInstance();
    void setGPURenderMaterial(Material *);
    void setCPURenderMaterial(Material *);
    void setRenderSceneMarkMaterial(Material *m);
    inline Material *getRenderSceneMarkMaterial() { return define.renderSceneMarkMaterial; };
    Mesh *getMeshRenderSceneMark();

    /**
     * @return
     * 0：还没有指定渲染模式
     * 1：表示使用CPU
     * 2：表使用GPU
     */
    int getRenderModel();

    /**
     * 设置粒子系统总粒子的个数。
     * 这是一个非常重度的操作，只有初始化的时候设置一下，运行过程中不要通过这个控制粒子的数量
     * 应该通过AmountRatio来控制粒子发射的数量
     */
    void setAmount(int amount);

    /**
     * 查看Particle2DEmitterDefine的说明
     */
    void setAmountRatio(float);
    inline int getAmount() { return define.amount; };
    inline int getRenderNum() { return particle2DEmitter->getRenderNum(); };
    /**
     * 每个粒子的生命时长
     */
    void setLifeTime(float);

    //=== 粒子发射阶段的相关设置======

    // 是否发射一次之后就不会再发射
    void setOneShot(bool b);

    // 是否自动发射
    void setEmit(bool b);

    // 全局随机种子。通过设置这个使得每个粒子系统看起来不一样
    void setSeed(float seed);

    /**
     * 给粒子发射时的速度方向一个随机角度范围。单位为角度
     * SpreadAngle和SpreadDirection可以结合起来决定一个扇形范围的速度方向
     *
     * SpreadDirection 决定其实角度，SpreadAngle决定范围
     *
     *
     *  \                  ∧这条线为SpreadDirection
     *   \ SpreadAngle范围 /
     *    \              /
     *     \            /
     *      \          /
     *       \       /
     *        \     /
     *         \   /
     */
    void setEmitSpreadAngle(float angle);
    void setEmitSpreadDirection(Vec2 &);

    /**
     * setEmitSpreadAngle 和 setEmitSpreadDirection计算出速度方向，设为direction
     * 这里设置初始速度的大小，在setEmitInitMinMaxVelocityMuti范围内伪随机
     * v = direction * setEmitInitMinMaxVelocityMuti
     */
    void setEmitInitMinMaxVelocityMuti(float, float);

    void setEmitShape(ParticleEmitterShape);
    void setEmitShapeOffset(const Vec2 &);
    void setEmitShapeRadius(float);
    void setEmitTexturePoints(Texture2d *);
    void setEmitTexturePointCount(float);
    void setEmitTexturePointEnableNormal(bool);
    void setEmitPointsBuffer(char *buffer, int size);

    // 设置圆台形状的相关参数
    void setEmitRingShapeHeight(float);
    void setEmitRingShapeConeAngle(float);
    void setEmitRingShapeInnerRadius(float);
    void setEmitRingShapeAxis(const Vec2 &);
    void setEmitBoxExtents(const Vec2 &);

    void setInheritEmitterVelocityRatio(float);

    void setMinMaxAngularVelocity(float, float);
    void setMinMaxDirectionalVelocity(float, float);

    /**
     * 实现径向效果.
     * 即每次速度方向的更新是：上一个位置与发射初始的位置构建一个向量，这个向量就是速度更新的方向
     */
    void setMinMaxRadialVelocity(float, float);
    void setVelocityPivot(const Vec2 &);

    /**
     * 环绕效果。使得速度绕圆形旋转，类似于龙卷风的效果
     * 这里设置的是角度的倍数。 OrbitVelocity * 360 * dt;
     */
    void setMinMaxOrbitVelocity(float, float);

    void setMinMaxTurbulenceInfluence(float, float);

    //==============显示设置=================

    void setParticleTexture(Texture2d *texture);
    void setPartilceRenderSize(float width, float height);

    // 粒子旋转角度吗，在这个范围内随机
    void setMinMaxInitialAngle(float, float);

    // 粒子缩放
    void setMinMaxScale(float, float);

    void setPropertyTexture(Texture2d *propertyTexture);

    // 设置粒子的初始颜色。如果设置颜色插值，那么粒子是以这个颜色作为初始值开始插值
    virtual void setColor(Color &);

    //===============物理效果相关设置==================
    void setGravity(const Vec2 &);
    void setMinMaxDamping(float, float);
    void setMinMaxRadialAccel(float, float);
    void setMinMaxTangentAccel(float, float);
    void setMinMaxLinearAccel(float, float);

    //===================吸引器=========================
    void setAttractorShape(AttractorShapeType type);
    void setAttractorShapeTransform(Mat3 &mat);
    void setAttractorAttenuation(float attenuation);
    void setAttractorStrength(float strength);
};
