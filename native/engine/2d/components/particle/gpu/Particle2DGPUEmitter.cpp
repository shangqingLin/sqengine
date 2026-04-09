#include "Particle2DGPUEmitter.h"
#include "../../../../core/utils/texture-hepler.h"
#include "../../../../core/common/ArrayBuffer.h"
#include "../../Transform2DComponent.h"
#include "../../../../gfx/gfx.h"
#include "../../../../core/math/math.h"
#include "../../../../scene/Camera.h"
#include "../../../../assets/BuildinResManager.h"
#include "../Particles2DComponent.h"
#include "../../../../rendering/BuildInMeshDataManager.h"
#include "../../../../core/common/Date.h"
#include "../common/math.h"

Particle2DGPUEmitter::Particle2DGPUEmitter(Particles2DComponent *c)
    : Particle2DEmitter(c),
      textureA(nullptr),
      textureB(nullptr),
      swapRenderTexture(nullptr),
      swapUpdateTexture(nullptr),
      particleIndexvertexBuffer(nullptr),
      particleEmitterTexture(nullptr),
      updateProcessMaterial(nullptr),
      renderMaterial(nullptr),
      renderScenMarkMaterial(nullptr),
      renderSceneMarkMesh(nullptr),
      renderMesh(nullptr),
      propertyTexture(nullptr),
      updateRenderMeshInstance(nullptr)
{
}

Particle2DGPUEmitter::~Particle2DGPUEmitter()
{

    delete textureA;
    delete textureB;
    delete particleEmitterTexture;
    delete renderMesh;
    if (updateRenderMeshInstance)
    {
        delete updateRenderMeshInstance;
    }

    if (renderSceneMarkMesh)
    {
        delete renderSceneMarkMesh;
    }
    delete particleIndexvertexBuffer;
}

void Particle2DGPUEmitter::initialize(Particle2DEmitterDefine *define)
{

    // for (int i = 0; i < define->amount; ++i)
    // {
    //     unsigned int randSeed = (unsigned int)hash(i + 1u + 10u);
    //     printf(" %d randIndex %f \n", i, floor(rand_from_seed(randSeed) * 100.f));
    // }

    ERR_FAIL_NULL_V(define->gpuUpdateProcessMaterial && define->gpuRenderMaterial);

    this->define = define;
    updateProcessMaterial = define->gpuUpdateProcessMaterial;
    renderMaterial = define->gpuRenderMaterial;
    renderScenMarkMaterial = define->renderSceneMarkMaterial;

    int particleSize = define->amount;

    // 粒子发射器
    Particle2DTransmitterDefine emitterDefine;
    emitterDefine.amount = particleSize;
    emitterDefine.amoutRatio = define->amountRatio;
    emitterDefine.lifeTime = define->lifeTime > 0 ? define->lifeTime : 5000;
    emitterDefine.randomness = define->randomness;
    emitterDefine.randomSeed = define->randomSeed;
    emitter.initialize(emitterDefine);

    createUpdateMeshInstance();
    createRenderModel(particleSize);
    // createTestRenderModel(400, particleSize);

    setAmountRatio(define->amountRatio);
    if (particleSize > 0)
        setAmount(particleSize);
    else
    {
        updateProcessMaterialParam();
        updateRenderMaterialParam();
    }

    if (define->renderSceneMarkMaterial)
        setRenderSceneMarkMaterial(define->renderSceneMarkMaterial);
}

void Particle2DGPUEmitter::createUpdateMeshInstance()
{
    // 创建更新Data的Mesh
    AttributeCustomSetting aPos = {
        0,
        2,
        8,
        Format::RG32F};

    Attribute vfmtPos = {
        "aPos", Format::RG32F, aPos};

    VertexBuffer *vertexBuffer = new VertexBuffer(BufferUsage::STATIC_DRAW);
    vertexBuffer->resizeBuffer(4 * getAttributeStride(vfmtPos), true);

    IndexBuffer *indexBuffer = new IndexBuffer(BufferUsage::STATIC_DRAW, 6);
    indexBuffer->append<unsigned short>(0);
    indexBuffer->append<unsigned short>(1);
    indexBuffer->append<unsigned short>(2);
    indexBuffer->append<unsigned short>(0);
    indexBuffer->append<unsigned short>(2);
    indexBuffer->append<unsigned short>(3);

    updateMesh = new Mesh();
    updateMesh->addVertextBuffer(vertexBuffer, vfmtPos);
    updateMesh->setIndexBuffer(indexBuffer);

    updateRenderMeshInstance = new MeshInstance();
    updateRenderMeshInstance->primitive.count = 6;
    updateRenderMeshInstance->primitive.offset = 0;
    updateRenderMeshInstance->setMesh(updateMesh, true);
    updateRenderMeshInstance->setMaterial(updateProcessMaterial);

    Camera *camera = new Camera();
    camera->setProjection(CameraProjection::ORTHO);
    updateRenderMeshInstance->setCamera(camera, true);
}

void Particle2DGPUEmitter::createTestRenderModel(int size, int particleCount)
{

    /*IndexBuffer *indexBuffer = updateRenderMeshInstance->getMesh()->getIndexBuffer();
    testMeshInstance = new MeshInstance();
    Mesh *mesh = new Mesh();
    mesh->setIndexBuffer(indexBuffer);
    testMeshInstance->setMesh(mesh);
    // testMeshInstance->setMaterial(BuildinResManager::getParticleRenderMaterial());
    testMeshInstance->primitive.count = 6;
    testMeshInstance->primitive.offset = 0;

    AttributeCustomSetting aPos = {
        0,
        2,
        16,
        Format::RG32F};

    AttributeCustomSetting aUv = {
        8,
        2,
        16,
        Format::RG32F};

    std::vector<Attribute> vfmtPos = {
        {"aPos", Format::RG32F, aPos},
        {"aUv", Format::RG32F, aUv},
    };

    VertexBuffer *particlePosVertexBuffer = new VertexBuffer(BufferUsage::STATIC_DRAW);
    particlePosVertexBuffer->resizeBuffer(getAttributeStride(vfmtPos) * 4, true);

    particlePosVertexBuffer->append<float>(-size);
    particlePosVertexBuffer->append<float>(size);
    particlePosVertexBuffer->append<float>(0);
    particlePosVertexBuffer->append<float>(1);

    particlePosVertexBuffer->append<float>(size);
    particlePosVertexBuffer->append<float>(size);
    particlePosVertexBuffer->append<float>(1);
    particlePosVertexBuffer->append<float>(1);

    particlePosVertexBuffer->append<float>(size);
    particlePosVertexBuffer->append<float>(-size);
    particlePosVertexBuffer->append<float>(1);
    particlePosVertexBuffer->append<float>(0);

    particlePosVertexBuffer->append<float>(-size);
    particlePosVertexBuffer->append<float>(-size);
    particlePosVertexBuffer->append<float>(0);
    particlePosVertexBuffer->append<float>(0);

    mesh->addVertextBuffer(particlePosVertexBuffer, vfmtPos);*/
}

void Particle2DGPUEmitter::createRenderModel(int particleCount)
{

    // printf("++++++++++++++++CreateRender Count %d \n",particleCount);
    SQ_ASSERT(particleCount);

    // IndexBuffer *indexBuffer = updateMesh->getIndexBuffer();

    // printf("++++++++++++++++++++++++++ createRenderModel %p \n", meshInstance);
    renderMesh = new Mesh();

    // 与上面使用同一个index就可以，都是绘制矩形，6个顶点
    // renderMesh->setIndexBuffer(indexBuffer);
    component->setMesh(renderMesh);
    component->setMaterial(renderMaterial);

    /**
     *
     * 顶点着色器需要知道当前计算是哪个粒子，因为是多实例化绘制，在WebGL2.0可以使用gl_InstanceID，但WebGL1.0没有
     * 本来想在顶点着色器中使用gl_InstanceId来标记粒子索引的
     * 但WebGL要求必须至少有一个属性设置为vertexAttribDivisor属性，gl_InstanceId才有效（有点操蛋）。
     *
     * 所以需要一个Buffer存储索引
     */
    AttributeCustomSetting particleIndex = {
        0,
        1,
        4,
        Format::R32F,
        1 // 标记是多实例的
    };
    std::vector<Attribute> vfmtParticle = {
        {"particleIndex", Format::R32F, particleIndex},
    };
    particleIndexvertexBuffer = new VertexBuffer(BufferUsage::DYNAMIC_DRAW);
    particleIndexvertexBuffer->resizeBuffer(getAttributeStride(vfmtParticle) * particleCount, true);
    renderMesh->addVertextBuffer(particleIndexvertexBuffer, vfmtParticle);

    // 绘制一个矩形作为一个粒子。
    renderMesh->addVertextBuffer(pipeline::BuildInMeshDataManager::getIntance()->getPosUvVertexBuffer(), pipeline::BuildInMeshDataManager::posUvVertexAttributes);
    renderMesh->setIndexBuffer(pipeline::BuildInMeshDataManager::getIntance()->getPosUvIndexBuffer());
    renderMesh->autoDeleteBuffer = false;
}

void Particle2DGPUEmitter::update(float dt)
{

    // printf("use gpu \n");

    // long time = getDateNow();
    if (emitter.update(dt))
    {
        if (emitter.getActiveCount() == 0)
        {
            return;
        }

        BufferTextureCopyRegion region;
        region.buffCopyToNative = false;
        region.texWidth = particleEmitterTexture->getWidth();
        region.texHeight = particleEmitterTexture->getHeight();
        particleEmitterTexture->updateData((unsigned char *)emitter.getGPUParticles(), region);
        // printf("update emiiter \n");
    }

    // printf("start: %ld \n", getDateNow() - time);

    if (emitter.getActiveCount() == 0)
    {
        return;
    }

    // printf("update GPU Emitter \n");

    if (swapUpdateTexture == textureA)
    {
        swapUpdateTexture = textureB;
        swapRenderTexture = textureA;
    }
    else
    {
        swapUpdateTexture = textureA;
        swapRenderTexture = textureB;
    }

    updateRenderMeshInstance->getCamera()->changeTargetWindow(swapUpdateTexture->getRenderWindow());
    updateProcessMaterial->setTexture("aTexture", swapRenderTexture);

    Material *renderMaterial = component->getMaterial();
    renderMaterial->setTexture("aTexture", swapUpdateTexture);

    if (renderScenMarkMaterial)
    {
        renderScenMarkMaterial->setTexture("aTexture", swapUpdateTexture);
    }

    if (testMeshInstance)
        testMeshInstance->getMaterial()->setTexture("aTexture", swapUpdateTexture);

    // printf("??????????????? %p \n",swapUpdateTexture->getGFXTexture());
}

void Particle2DGPUEmitter::setAmount(int count)
{

    // 修改粒子数量是一个比较耗时的操作，需要重新创建RenderTexture

    // printf("+++++++++++++++++ count %d \n", count);

    if (textureA)
    {
        delete textureA;
        delete textureB;
        swapRenderTexture = nullptr;
        swapUpdateTexture = nullptr;
    }

    {
        RenderTextureDefine textureDefine;
        int textureWidth, textureHeight;

        // 计算需要多大的分辨率
        utils::calculateTextureSize(count, textureWidth, textureHeight, false);

        // printf("Particle renderTextSize count %d  textureWidth %d textureHeight %d\n",count, textureWidth, textureHeight);

        float halfWidth = textureWidth * 0.5;
        float halfHeight = textureHeight * 0.5;

        textureDefine.width = textureWidth;
        textureDefine.height = textureHeight;
        // 必须是浮点数纹理，因为位置等信息都是浮点数，一个分量是32位的浮点数
        textureDefine.format = Format::RGBA32F;

        // 每帧都是全屏覆盖的，没必要clear,节省一点图形API调用
        textureDefine.op = gfx::AttachmentOp::LOAD;

        // 设置纹理过滤，告诉不要进行插值，直接使用最近的像素(相当于在着色器中使用数组index方式访问)
        SamplerInfo sampler;
        sampler.minFilter = TextureFilter::NEAREST;
        sampler.magFilter = TextureFilter::NEAREST;
        textureDefine.sampler = sampler;

        textureA = new RenderTexture();
        textureB = new RenderTexture();

        textureA->initiliaze(textureDefine);
        textureB->initiliaze(textureDefine);

        // WebGL2Texture *t = (WebGL2Texture *)textureA->getGFXTexture();
        // WebGL2Texture *t1 = (WebGL2Texture *)textureB->getGFXTexture();
        // printf(">>>>>>>>>>> %d %d \n", t->getGPUTexture().glTexture, t1->getGPUTexture().glTexture);

        // testTexture = new Texture2d();
        // ITexture2DCreateInfo info;
        // info.format = toNumber(Format::RGBA32F);
        // SamplerObject sampler2;
        // sampler.setFilter(TextureFilter::NEAREST_MIPMAP_NEAREST, TextureFilter::NEAREST);
        // info.sampler = sampler2;
        // info.width = textureWidth;
        // info.height = textureHeight;
        // testTexture->create(&info);

        swapUpdateTexture = textureA;

        // createTextureInitData(textureA, textureB);

        // 构建一个分辨率和纹理一致的正交投影
        Camera *camera = updateRenderMeshInstance->getCamera();
        camera->setOrthoSize((float)textureDefine.width, (float)textureDefine.height);

        // MeshInstance *meshInstance = updateModel->meshInstances[0];

        VertexBuffer *vertexBuffer = updateMesh->getVertexBufer(0);
        vertexBuffer->clearData();

        // printf("draw renderTextSize %f %f\n", halfWidth, halfHeight);

        // 全屏绘制
        vertexBuffer->append(-halfWidth);
        vertexBuffer->append(-halfHeight);

        vertexBuffer->append(halfWidth);
        vertexBuffer->append(-halfHeight);

        vertexBuffer->append(halfWidth);
        vertexBuffer->append(halfHeight);

        vertexBuffer->append(-halfWidth);
        vertexBuffer->append(halfHeight);

        /*
        Mat4& mat4 = camera->getViewProjMat();
        mat4.toString();
        Vec3 point;
        point.set(-halfWidth,halfHeight, 0.);
        mat4.transformPoint(point, point);
        printf("point1 %f %f\n", point.x, point.y);

        point.set(halfWidth,halfHeight, 0.);
        mat4.transformPoint(point, point);
        printf("point2 %f %f\n", point.x, point.y);

        point.set(halfWidth,-halfHeight, 0.);
        mat4.transformPoint(point, point);
        printf("point3 %f %f\n", point.x, point.y);

        point.set(-halfWidth,-halfHeight, 0.);
        mat4.transformPoint(point, point);
        printf("point4 %f %f\n", point.x, point.y);*/

        // MeshInstance *renderMeshInstance = renderModel->meshInstances[0];
        // renderMeshInstance->primitive.value().instance = count;

        if (particleIndexvertexBuffer)
        {
            particleIndexvertexBuffer->resizeBuffer(count * 4, true);
            for (int i = 0; i < count; ++i)
            {
                particleIndexvertexBuffer->append<float>((float)i);
            }
        }
    }

    {

        if (particleEmitterTexture)
        {
            delete particleEmitterTexture;
        }

        particleEmitterTexture = new Texture2d();
        ITexture2DCreateInfo info;
        info.format = toNumber(Format::RGBA32F);
        SamplerInfo sampler = {TextureFilter::NEAREST, TextureFilter::NEAREST};
        info.sampler = sampler;

        // 不够一个像素也填充满一个像素，多余的内存不存储数据，这是为了方便在着色器中读取
        int numProperties = sizeof(Particle2DGPUTexture) >> 2;
        int preNumPixles = (numProperties >> 2) + (numProperties % 4 > 0 ? 1 : 0);
        int numPixles = preNumPixles * count;
        utils::calculateTextureSize(numPixles, info.width, info.height, preNumPixles);

        // printf("particleEmitterTexture %d %d %d %d %d\n",numPixles,numProperties,preNumPixles,info.width,info.height);

        particleEmitterTexture->create(&info);
    }

    emitter.setAmount(count);
    updateProcessMaterialParam();
    updateRenderSceneMarkMaterialParam();
    updateRenderMaterialParam();
    // createTextureInitData(textureA, textureB);
}

void Particle2DGPUEmitter::setAmountRatio(float ratio)
{
    emitter.setAmountRatio(ratio);
}

int Particle2DGPUEmitter::getActiveCount()
{
    return emitter.getActiveCount();
}

int Particle2DGPUEmitter::getRenderNum()
{
    return emitter.getActiveCount();
}

void Particle2DGPUEmitter::createTextureInitData(RenderTexture *textureA, RenderTexture *textureB)
{
    ArrayBuffer buffer;
    int texSize = formatTexSize(Format::RGBA32F, textureA->getWidth(), textureA->getHeight(), 1);
    buffer.resize(texSize);

    int partilceCount = textureA->getWidth() * textureA->getHeight();

    // printf("createTextureInitData size %d %d %d \n", texSize, textureA->getWidth(), textureA->getHeight());

    Vec2 vec2;
    for (int i = 0; i < partilceCount; ++i)
    {
        float x = Math::randomFloat(-300.f, 300.f);
        float y = Math::randomFloat(-300.f, 300.f);
        buffer.append<float>(x);
        buffer.append<float>(y);

        // vec2.set(Math::randomFloat(-1.f, 1.f), Math::randomFloat(-1.f, 1.f));
        // vec2.normalize();

        buffer.append<float>(0.f);
        buffer.append<float>(0.f);

        // printf("----%d %f %f \n", i, x, y);
    }

    BufferTextureCopyRegion region;
    region.buffCopyToNative = true;
    region.texWidth = textureA->getWidth();
    region.texHeight = textureA->getHeight();
    textureA->updateData((unsigned char *)buffer.getBuffer(), region);
    textureB->updateData((unsigned char *)buffer.getBuffer(), region);
    // testTexture->updateData((unsigned char *)buffer.getBuffer(), region);
}

void Particle2DGPUEmitter::setSeed(float seed)
{
    // printf("+++ setSeed %f \n",seed);
    emitter.setRandomSeed(seed);
    updateProcessMaterial->setProperty("SEED", seed);
}

void Particle2DGPUEmitter::setRandomness(unsigned int seed)
{
    emitter.setRandomness(seed);
}

void Particle2DGPUEmitter::setEmitSpreadAngle(float angle)
{
    updateProcessMaterial->setProperty("spread_angle", angle);
}

void Particle2DGPUEmitter::setEmitSpreadDirection(Vec2 direction)
{
    float d[2];
    d[0] = direction.x;
    d[1] = direction.y;
    updateProcessMaterial->setProperty("spread_direction", d);
}

void Particle2DGPUEmitter::setEmitInitMinMaxVelocityMuti(float min, float max)
{
    float value[2] = {min, max};
    updateProcessMaterial->setProperty("initialLinearVelocityMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxLinearAccel(float min, float max)
{
    float value[2] = {min, max};
    updateProcessMaterial->setProperty("linearAccelMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxRadialAccel(float min, float max)
{
    float value[2] = {min, max};
    updateProcessMaterial->setProperty("radialAccelMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxTangentAccel(float min, float max)
{

    float value[2] = {min, max};
    updateProcessMaterial->setProperty("tangentAccelMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxDamping(float min, float max)
{
    float value[2] = {min, max};
    updateProcessMaterial->setProperty("dampingMinMax", value);
}

void Particle2DGPUEmitter::setLifeTime(int lifeTime)
{
    SQ_ASSERT(lifeTime);
    emitter.setLifeTime(lifeTime);
}

void Particle2DGPUEmitter::setInheritEmitterVelocityRatio(float v)
{
    updateProcessMaterial->setProperty("inherit_emitter_velocity_ratio", v);
}

void Particle2DGPUEmitter::setMinMaxAngularVelocity(float min, float max)
{
    float value[2] = {min, max};
    updateProcessMaterial->setProperty("angularVelocityMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxDirectionalVelocity(float min, float max)
{
    float value[2] = {min, max};
    updateProcessMaterial->setProperty("directionalVelocityMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxRadialVelocity(float min, float max)
{
    float value[2] = {min, max};
    updateProcessMaterial->setProperty("radialVelocityMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxOrbitVelocity(float min, float max)
{
    float value[2] = {min, max};
    updateProcessMaterial->setProperty("orbitVelocitMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxTurbulenceInfluence(float min, float max)
{
    float value[2] = {min, max};
    updateProcessMaterial->setProperty("turbulenceInfluenceMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxInitialAngle(float min, float max)
{
    float value[2] = {min, max};
    renderMaterial->setProperty("initialAngleMinMax", value);
}

void Particle2DGPUEmitter::setMinMaxScale(float min, float max)
{
    float value[2] = {min, max};
    renderMaterial->setProperty("initialScaleMinMax", value);
}

void Particle2DGPUEmitter::setPartilceRenderSize(float width, float height)
{
    define->width = width;
    define->height = height;
    updateRenderParticleSize();
}

void Particle2DGPUEmitter::updateRenderParticleSize()
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

void Particle2DGPUEmitter::setInitColor(const Color &color)
{
    float size[4] = {color.r, color.g, color.b, color.a};
    renderMaterial->setProperty("initColor", size);
}

void Particle2DGPUEmitter::setParticleTexture(Texture2d *texture)
{
    renderMaterial->setTexture("renderTexture", texture);
    updateRenderParticleSize();
}

void Particle2DGPUEmitter::setGravity(const Vec2 &gravity)
{
    float g[2];
    g[0] = gravity.x;
    g[1] = gravity.y;
    updateProcessMaterial->setProperty("gravity", g);
}

void Particle2DGPUEmitter::setVelocityPivot(const Vec2 &v)
{
    float g[2];
    g[0] = v.x;
    g[1] = v.y;
    updateProcessMaterial->setProperty("velocity_pivot", g);
}

void Particle2DGPUEmitter::setEmitShape(ParticleEmitterShape shape)
{
    // Particle2DEmitterShape *shapeObj = createShape(shape);
    // emitter.setEmitterShape(shapeObj);
    updateProcessMaterial->setProperty("emssion_shape", float(toNumber(shape)));
}

void Particle2DGPUEmitter::setEmitShapeRadius(float raduis)
{
    updateProcessMaterial->setProperty("emssion_shape_radius", raduis);
}

void Particle2DGPUEmitter::setEmitRingShapeHeight(float heihgt)
{
    updateProcessMaterial->setProperty("emission_ring_height", heihgt);
}

void Particle2DGPUEmitter::setEmitRingShapeConeAngle(float angle)
{
    updateProcessMaterial->setProperty("emission_ring_cone_angle", angle);
}

void Particle2DGPUEmitter::setEmitRingShapeInnerRadius(float raduis)
{
    updateProcessMaterial->setProperty("emission_ring_inner_radius", raduis);
}

void Particle2DGPUEmitter::setEmitBoxExtents(const Vec2 &extents)
{
    float value[2] = {extents.x, extents.y};
    updateProcessMaterial->setProperty("emission_box_extents", value);
}

void Particle2DGPUEmitter::setEmitRingShapeAxis(const Vec2 &axis)
{
    float g[2];
    g[0] = axis.x;
    g[1] = axis.y;
    updateProcessMaterial->setProperty("emission_ring_axis", g);
}

void Particle2DGPUEmitter::setOneShot(bool b)
{
    emitter.setOneShot(b);
}

void Particle2DGPUEmitter::setEmit(bool b)
{
    emitter.setEmit(b);
}

void Particle2DGPUEmitter::setPropertyTexture(Texture2d *propertyTexture)
{
    if (this->propertyTexture != propertyTexture)
    {
        this->propertyTexture = propertyTexture;
        renderMaterial->setTexture("propertyTexture", propertyTexture);
        float size[2];
        size[0] = propertyTexture->getWidth();
        size[1] = propertyTexture->getHeight();
        renderMaterial->setProperty("propertyTextureSize", size);
    }
}

void Particle2DGPUEmitter::setUpdateProcessMaterial(Material *updateMaterial)
{
    // if (updateMaterial == nullptr)
    // {
    //     this->updateProcessMaterial = nullptr;
    //     updateRenderMeshInstance->setMaterial(nullptr);
    //     return;
    // }

    if (this->updateProcessMaterial != updateMaterial)
    {
        updateRenderMeshInstance->setMaterial(updateMaterial);
        this->updateProcessMaterial = updateMaterial;
        updateProcessMaterialParam();
    }

    // initMaterialValue(true, oldMaterial, this->renderMaterial);
    // initMaterial2(true, oldMaterial, this->renderMaterial);
}

void Particle2DGPUEmitter::updateProcessMaterialParam()
{
    if (!this->updateProcessMaterial)
        return;
    float size[2];
    size[0] = particleEmitterTexture->getWidth();
    size[1] = particleEmitterTexture->getHeight();
    this->updateProcessMaterial->setTexture("particleTexture", particleEmitterTexture);
    this->updateProcessMaterial->setProperty("particleTextureSize", size);

    setGravity(define->gravity);
    setSeed(define->randomSeed);
    setEmitInitMinMaxVelocityMuti(define->initEmitVelocityMaxMuti, define->initEmitVelocityMinMuti);
    setEmitSpreadAngle(define->spreadAngle);
    setEmitSpreadDirection(define->spreadDirection);
    setInheritEmitterVelocityRatio(define->inheritEmitterVelocityRatio);
    setMinMaxLinearAccel(define->linear_accel_min, define->linear_accel_max);
    setMinMaxRadialAccel(define->radial_accel_min, define->radial_accel_max);
    setMinMaxTangentAccel(define->tangent_accel_min, define->tangent_accel_max);
    setMinMaxDamping(define->damping_min, define->damping_max);
    setMinMaxAngularVelocity(define->angular_velocity_min, define->angular_velocity_max);
    setMinMaxDirectionalVelocity(define->directional_velocity_min, define->directional_velocity_max);
    setMinMaxOrbitVelocity(define->orbit_velocity_min, define->orbit_velocity_max);
    setMinMaxRadialVelocity(define->radial_velocity_min, define->radial_velocity_max);
    setMinMaxTurbulenceInfluence(define->turbulence_influence_min, define->turbulence_influence_max);
    setVelocityPivot(define->velocityPivot);
    setEmitShape(define->shape);
    setEmitShapeRadius(define->emssion_shape_radius);
    setEmitRingShapeConeAngle(define->emission_ring_cone_angle);
    setEmitRingShapeHeight(define->emission_ring_height);
    setEmitRingShapeInnerRadius(define->emission_ring_inner_radius);
    setEmitRingShapeAxis(define->emission_ring_axis);
    setEmitBoxExtents(define->emission_box_extents);

    if (define->emitTexturePoints)
        setEmitTexturePoints(define->emitTexturePoints);
    setEmitTexturePointCount(define->emissionTexturePointCount);
    setEmitTexturePointEnableNormal(define->emitEnableTexturePointNomral);
}

void Particle2DGPUEmitter::setRenderMaterial(Material *renderMaterial)
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

    // initMaterialValue(true, this->updateProcessMaterial, oldMaterial);
    // initMaterial2(true, this->updateProcessMaterial, oldMaterial);
}

void Particle2DGPUEmitter::updateRenderMaterialParam()
{

    if (!this->renderMaterial)
        return;

    setMinMaxScale(define->initial_scale_min, define->initial_scale_max);
    setMinMaxInitialAngle(define->initial_angle_min, define->initial_angle_max);
    setInitColor(define->initColor);
    if (define->propertyTexture)
        setPropertyTexture(define->propertyTexture);
    if (define->particleTexture)
        setParticleTexture(define->particleTexture);

    updateRenderParticleSize();
    float size[2];
    size[0] = particleEmitterTexture->getWidth();
    size[1] = particleEmitterTexture->getHeight();
    renderMaterial->setTexture("particleTexture", particleEmitterTexture);
    renderMaterial->setProperty("particleTextureSize", size);

    if (textureA)
    {
        size[0] = textureA->getWidth();
        size[1] = textureA->getHeight();
        renderMaterial->setProperty("aTextureSize", size);
    }
}

void Particle2DGPUEmitter::setRenderSceneMarkMaterial(Material *m)
{
    renderScenMarkMaterial = m;
    updateRenderSceneMarkMaterialParam();
    if (renderScenMarkMaterial)
    {
        if (!renderSceneMarkMesh)
        {
            renderSceneMarkMesh = new Mesh();
            Mesh *mesh = component->getMesh();
            renderSceneMarkMesh->autoDeleteBuffer = false;
            renderSceneMarkMesh->setIndexBuffer(mesh->getIndexBuffer());
            renderSceneMarkMesh->addVertextBuffer(particleIndexvertexBuffer, mesh->getAttributes(0));
            // renderSceneMarkMesh->addVertextBuffer(particlePosVertexBuffer, mesh->getAttributes(1));
        }
    }
    else
    {
        delete renderSceneMarkMesh;
    }
}

void Particle2DGPUEmitter::updateRenderSceneMarkMaterialParam()
{
    if (renderScenMarkMaterial)
    {

        if (textureA)
        {
            float size[2];
            size[0] = textureA->getWidth();
            size[1] = textureA->getHeight();
            renderScenMarkMaterial->setProperty("aTextureSize", size);
        }
    }
}

void Particle2DGPUEmitter::setEmitTexturePoints(Texture2d *emitTexturePoints)
{
    this->updateProcessMaterial->setTexture("emission_texture_points", emitTexturePoints);
    float size[2];
    size[0] = emitTexturePoints->getWidth();
    size[1] = emitTexturePoints->getHeight();
    this->updateProcessMaterial->setProperty("emission_texture_size", size);
}

void Particle2DGPUEmitter::setEmitTexturePointCount(float num)
{
    this->updateProcessMaterial->setProperty("emission_texture_point_count", num);
}

void Particle2DGPUEmitter::setEmitTexturePointEnableNormal(bool b)
{
    this->updateProcessMaterial->setProperty("emission_texture_normal", b ? 1.0f : 0.f);
}

void Particle2DGPUEmitter::setAttractorShape(AttractorShapeType type)
{
    updateProcessMaterial->setProperty("attractorType",(float)type);
}

void Particle2DGPUEmitter::setAttractorShapeTransform(Mat3 &mat)
{

}

void Particle2DGPUEmitter::setAttractorAttenuation(float attenuation)
{
    float value[2] = { attenuation,define->attractor_strength };
    updateProcessMaterial->setProperty("attractorAttenuationAndStrength",value);
}

void Particle2DGPUEmitter::setAttractorStrength(float strength)
{
    float value[2] = { define->attractor_attenuation , strength};
    updateProcessMaterial->setProperty("attractorAttenuationAndStrength",value);
}