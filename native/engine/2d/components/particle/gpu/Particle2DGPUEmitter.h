#pragma once

/**
 * 放到GPU端计算的粒子发射器
 *
 * 实现原理：
 *
 * 基本是需要以下阶段
 * 第一阶段：
 * 1、在CPU端生成粒子，记录粒子的生命周期，进度
 *
 * 第二个阶段：
 * 1、离屏渲染一块区域
 * 2、在片元着色器上更新粒子的位置和速度，这些信息存储在一个纹理中。
 *  这张纹理需要在顶点着色器读取，所以着色器必须支持浮点纹理。每个分量使用32位的浮点数存储
 *  所以，当前纹理采样之后 ：xy 表示 位置，zw表示速度
 *
 * 第三个阶段：
 * 1、在顶点着色器中计算粒子的缩放、旋转、颜色、Alpha属性，其实为了统一这些计算最后放到第二阶段中，但由于
 * 片元着色器每次运行只能（OpenGL可以支持多个，但WebGL1.0可能有兼容性问题）存储一个像素即4个数字，所以放弃这种想法
 * 转而将这些属性计算放到渲染阶段的顶点着色器中计算
 *
 * 2、基于第一个阶段得出的纹理得到粒子位置，然后进行渲染粒子
 *
 *
 * 构建离屏的RenderTexture
 *    需要两张RenderTexture，他们之间交替使用，存储粒子上一帧的状态和当前帧的状态，并且用当前帧的状态来更新上一帧的状态
 *    当前帧的状态放到渲染阶段中使用。这种两个Buffer相互交替的技术叫做乒乓缓冲区（ping-pong buffer）
 *
 *
 *  渲染问题：
 *   渲染粒子的时候，我们很多时候都要求粒子之间可以进行融合，所以在渲染粒子的时候开启了Blend，需要和粒子之间的颜色进行融合，
 *   但这样就会和背景也进行融合了，唯一的办法就是粒子系统进行离屏渲染了。
 */

#include "../../../../assets/RenderTexture.h"
#include "../Particle2DEmitter.h"
#include "./Particle2DGPUTransmitter.h"
#include "../../../../scene/graphics/Mesh.h"
#include "../../../../scene/graphics/MeshInstance.h"

class Particle2DGPUEmitter : public Particle2DEmitter
{
private:
    // 第一阶段
    //  存储粒子在CPU端的粒子发射信息，传递到update shader\render Shader中
    Texture2d *particleEmitterTexture;

    // 粒子CPU端的发射器
    Particle2DGPUTransmitter emitter;

    // 第二阶段的两个RenderTExture
    Mesh *updateMesh;
    RenderTexture *textureA;
    RenderTexture *textureB;
    RenderTexture *swapRenderTexture;
    RenderTexture *swapUpdateTexture;
    Material *updateProcessMaterial;
    MeshInstance *updateRenderMeshInstance;

    // 第三阶段
    /**
     *
     * 1、 粒子的数据太多了，使用纹理来存储粒子的属性
     *    这种纹理将记录粒子的旋转、缩放、颜色数据和对应的插值的信息。
     * 2、这张纹理是1xn大小，使用一维纹理足够了
     *
     * 3、 在第二阶段的顶点着色器中使用。
     */
    Texture2d *propertyTexture;

    Material *renderMaterial;
    Mesh *renderMesh;

    // 有Blend的时候离屏渲染的处理
    Material *renderScenMarkMaterial;
    Mesh *renderSceneMarkMesh;

    Texture2d *testTexture;
    Particle2DEmitterDefine *define{nullptr};
    VertexBuffer *particleIndexvertexBuffer = nullptr;
    // VertexBuffer *particlePosVertexBuffer = nullptr;

    MeshInstance *testMeshInstance{nullptr};

    void createUpdateMeshInstance();
    void createTextureInitData(RenderTexture *textureA, RenderTexture *textureB);
    void createTestRenderModel(int size, int particleCount);
    void createRenderModel(int particleCount);
    void updateRenderParticleSize();

    void updateRenderSceneMarkMaterialParam();
    void updateRenderMaterialParam();
    void updateProcessMaterialParam();

public:
    Particle2DGPUEmitter(Particles2DComponent *c);
    ~Particle2DGPUEmitter();
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
    virtual void update(float dt) override;
    virtual void setRenderMaterial(Material *) override;
    virtual void setPropertyTexture(Texture2d *propertyTexture) override;
    virtual int getRenderNum() override;
    virtual void setRenderSceneMarkMaterial(Material *) override;
    virtual void setAttractorShape(AttractorShapeType type) override;
    virtual void setAttractorShapeTransform(Mat3 &mat) override;
    virtual void setAttractorAttenuation(float attenuation) override;
    virtual void setAttractorStrength(float strength) override;
    void setUpdateProcessMaterial(Material *);
    void setEmitTexturePoints(Texture2d *emitTexturePoints);
    void setPartilceRenderSize(float width, float height);
    void setEmitTexturePointCount(float num);
    void setEmitTexturePointEnableNormal(bool b);
    inline MeshInstance *getGPUUpdateMeshInstance() { return updateRenderMeshInstance; };
    inline MeshInstance *getTestRenderMeshInstance() { return testMeshInstance; };
    inline Mesh *getMeshRenderSceneMark() { return renderSceneMarkMesh; };
};
