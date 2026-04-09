#include "Batch.h"
#include "../2d/assembler/GraphicsAssembler.h"
#include "../2d/assembler/LabelAssembler.h"
#include "./buildin-block-define.h"
#include "../framework/Application.h"
#include "../assets/BuildinResManager.h"
#include "./BuildInMeshDataManager.h"

using namespace pipeline;

Batch::Batch() : batchStencil(this)
{
}
MeshInstance *Batch::createMeshInstance()
{
    MeshInstance *drawCall = nullptr;
    if (drawCallBatchPool.getCount() > 0)
    {
        drawCall = *drawCallBatchPool.pop();
    }
    if (!drawCall)
    {
        drawCall = new MeshInstance();
    }
    batches.push(drawCall);
    return drawCall;
}

void Batch::walkScene(sqstd::Array<MeshInstance *> &drawCalls)
{
    Scene *scene = Application::getInstance()->getRunScene();
    const std::vector<Node *> &children = scene->getChildren();
    for (int i = 0; i < children.size(); ++i)
    {
        walkNode(children[i], drawCalls);
    }
}

void Batch::walkNode(Node *node, sqstd::Array<MeshInstance *> &drawCalls)
{
    if (!node->getVisible() || !node->activeInHierarchy())
        return;

    const std::vector<RenderComponent *> &renders = node->getRenderComponents();
    
    bool hasStencilTest = false;
    for (int i = 0; i < renders.size(); ++i)
    {
        RenderComponent *render = renders[i];

        if (!(render->getState() & Component::ENABLE) || !render->canRender())
            continue;
        
        if (dynamic_cast<SpriteComponent *>(render))
        {
            //  printf("SpriteComponent \n");
            commitSprite(dynamic_cast<SpriteComponent *>(render), drawCalls);
        }
        else if (dynamic_cast<Graphics *>(render))
        {
            // printf("commitGraphics \n");
            commitGraphics(dynamic_cast<Graphics *>(render), drawCalls);
        }
        else if (dynamic_cast<LabelComponent *>(render))
        {
            commitLabel(dynamic_cast<LabelComponent *>(render), drawCalls);
        }
        else if (dynamic_cast<Particles2DComponent *>(render))
        {
            commitParticle((Particles2DComponent *)(render), drawCalls);
        }
        else if (dynamic_cast<physics2d::ParticlePhysics2DComponent *>(render))
        {
            // printf("fuck you ParticlePhysics2DComponent  \n");
            commitPhysicsParticle(dynamic_cast<physics2d::ParticlePhysics2DComponent *>(render), drawCalls);
        }
        else
        {
            commitRenderer(render, drawCalls);
        }

        if (!hasStencilTest)
        {
            UIContentComponent *uiRender = dynamic_cast<UIContentComponent *>(render);
            if (uiRender)
            {
                hasStencilTest = uiRender->getStencil() == StencilStage::ENTER_LEVEL || uiRender->getStencil() == StencilStage::ENTER_LEVEL_INVERTED;
            }
        }
    }

    const std::vector<Node *> &children = node->getChildren();
    for (int i = 0; i < children.size(); ++i)
    {
        walkNode(children[i], drawCalls);
    }

    if (hasStencilTest)
    {
        batchStencil.popMask();
        updatePrevDrawCall();
        // printf("dsiable stencilTest %d \n", batchStencil.enableStencilTest());
    }
}

void Batch::beginStencil(Layers layer, sqstd::Array<MeshInstance *> &drawCalls, StencilStage stencilStage)
{
    // 添加一个绘制全屏的操作，用来清除模板值
    updatePrevDrawCall();
    batchStencil.pushMask(stencilStage);

    Material *material = BuildinResManager::getMaterial("default-clear-stencil");
    PassStates overrideState;
    overrideState.stencilState = batchStencil.getStateFromStage(stencilStage == StencilStage::ENTER_LEVEL ? StencilStage::CLEAR : StencilStage::CLEAR_INVERTED);
    currentMeshInstance = createMeshInstance();
    currentMeshInstance->setMaterial(material, &overrideState);
    currentMeshInstance->layer = layer;
    currentMeshInstance->setMesh(BuildInMeshDataManager::getIntance()->getScreenMesh());
    currentMeshInstance->primitive.offset = 0;
    currentMeshInstance->primitive.count = 6;
    drawCalls.push(currentMeshInstance);

    // printf("beginStencil stage %d batch %p \n", stencilStage, currentMeshInstance);

    currentMeshInstance = nullptr;
}

void Batch::updatePrevDrawCall()
{
    if (currentMeshInstance)
    {
        Mesh *mesh = currentMeshInstance->getMesh();
        currentMeshInstance->primitive.count = mesh->getIndexBuffer()->getCount() - currentMeshInstance->primitive.offset;
        currentMeshInstance = nullptr;
    }
}

int Batch::combineTextureMaterial(Material *material, Texture2d *texture, Mesh *mesh, Layers layer, sqstd::Array<MeshInstance *> &drawCalls, StencilStage stencilStage)
{
    bool newDrawCall = false;

    if (!currentMeshInstance || material != currentMeshInstance->getMaterial() || currentMeshInstance->getMesh() != mesh || currentMeshInstance->layer != layer ||
        (stencilStage == StencilStage::ENTER_LEVEL || stencilStage == StencilStage::ENTER_LEVEL_INVERTED))
    {
        newDrawCall = true;
    }

    int textureBinding = toNumber(pipeline::LOCAL_SPRITE_TEXTURE::BINDING);
    int textureIndex = -1;
    if (!newDrawCall && currentMeshInstance)
    {
        if (currentMeshInstance->localDescriptorSet)
        {
            textureIndex = currentMeshInstance->localDescriptorSet->findBindTextureIndex(textureBinding, texture->getGFXTexture());
            // printf("????? %p %d %d \n", texture->getGFXTexture(), textureIndex,textureBinding);
            if (textureIndex == -1)
            {
                int numTexture = currentMeshInstance->localDescriptorSet->getSize(textureBinding);

                // printf("use textureIndex all %p %d \n", numTexture, DeviceManager::getInstance()->device->deviceCaps->maxTextureUnits);
                if (numTexture < DeviceManager::getInstance()->device->deviceCaps->maxTextureUnits)
                {
                    textureIndex = numTexture;
                    currentMeshInstance->localDescriptorSet->bindTexture(textureBinding, texture->getGFXTexture(), numTexture);
                    currentMeshInstance->localDescriptorSet->bindSampler(textureBinding, texture->getGFXSampler(), numTexture);
                }
                else
                {
                    newDrawCall = true;
                }
            }
        }
        else
        {
            newDrawCall = true;
        }
    }

    if (newDrawCall)
    {

        // 结束上一个DrawCall,需要对其进行一些设置
        updatePrevDrawCall();

        PassStates overrideState;
        if (stencilStage == StencilStage::ENTER_LEVEL || stencilStage == StencilStage::ENTER_LEVEL_INVERTED)
        {
            beginStencil(layer, drawCalls, stencilStage);

            overrideState.stencilState = batchStencil.getStateFromStage(stencilStage);
            // printf("enter stencil test for Sprite %d ref %u \n", stencilStage, overrideState.stencilState.value().stencilRefBack);
        }
        else
        {

            if (batchStencil.enableStencilTest())
            {
                // printf("enable stencil test for Sprite\n");
                overrideState.stencilState = batchStencil.getStateFromStage(StencilStage::ENABLED);
            }
        }

        //-------新的DrawCall-------
        MeshInstance *drawCall = createMeshInstance();

        drawCall->setMesh(mesh);
        drawCall->setMaterial(material, &overrideState);
        drawCall->layer = layer;

        // printf("draw call layer %u material %p \n",drawCall->layer,material);

        drawCall->primitive.offset = mesh->getIndexBuffer()->getCount();

        // printf("====new drawcall index offset %d \n", drawCall->primitive.value().offset);

        textureIndex = 0;
        drawCall->localDescriptorSet = localSets.getLocalDescriptorSet();
        drawCall->localDescriptorSet->bindTexture(textureBinding, texture->getGFXTexture());
        drawCall->localDescriptorSet->bindSampler(textureBinding, texture->getGFXSampler());
        currentMeshInstance = drawCall;
        drawCalls.push(drawCall);
    }

    // printf("commitTextureModel  mesh %p  texture %s index %d \n", currentMeshInstance, texture->getKeyUrl().c_str(), textureIndex);

    SQ_ASSERT(textureIndex != -1);
    return textureIndex;
}

void Batch::commitSprite(SpriteComponent *sprite, sqstd::Array<MeshInstance *> &drawCalls)
{
    Texture2d *texture = sprite->getTexture();
    Material *material = sprite->getMaterial();
    Mesh *mesh = sprite->getMesh();
    ERR_FAIL_NULL_V(mesh);
    ERR_FAIL_NULL_V(texture);
    ERR_FAIL_NULL_V(material);
    int textureIndex = combineTextureMaterial(material, texture, mesh, sprite->node->getLayer(), drawCalls, sprite->getStencil());
    sprite->getAssembley()->fillBuffers(sprite, textureIndex);

    // if (sprite->node->testNum != 0)
    // {
    //     printf(" %d draw sprite %p batch %p \n", sprite->node->testNum ,sprite, currentMeshInstance);
    // }
    // printf("draw sprite %p batch %p \n", sprite, currentMeshInstance);
}

void Batch::commitGraphics(Graphics *graphics, sqstd::Array<MeshInstance *> &drawCalls)
{
    updatePrevDrawCall();

    PassStates overrideState;
    if (graphics->getStencil() == StencilStage::ENTER_LEVEL || graphics->getStencil() == StencilStage::ENTER_LEVEL_INVERTED)
    {
        beginStencil(graphics->node->getLayer(), drawCalls, graphics->getStencil());

        overrideState.stencilState = batchStencil.getStateFromStage(graphics->getStencil());
        // printf("enter stencil test for graphics %d ref %u \n", graphics->getStencil(), overrideState.stencilState.value().stencilRefBack);
    }
    else if (batchStencil.enableStencilTest())
    {
        // printf("enable stencil test for graphics\n");
        overrideState.stencilState = batchStencil.getStateFromStage(StencilStage::ENABLED);
    }

    currentMeshInstance = createMeshInstance();
    drawCalls.push(currentMeshInstance);

    Mesh *mesh = graphics->getMesh();
    IndexBuffer *indexBuffer = mesh->getIndexBuffer();
    currentMeshInstance->primitive.offset = mesh->getIndexBuffer()->getCount();
    GraphicsAssembler::getInstance()->fillBuffers(graphics);
    currentMeshInstance->setMaterial(graphics->getMaterial(), &overrideState);
    currentMeshInstance->setMesh(graphics->getMesh());
    currentMeshInstance->localDescriptorSet = localSets.getLocalDescriptorSet();
    currentMeshInstance->layer = graphics->node->getLayer();
    localSets.setLocalTransform(currentMeshInstance->localDescriptorSet, graphics->node->getComponent<Transform2DComponent>());

    // printf("draw graphics %p batch %p \n", graphics, currentMeshInstance);
}

void Batch::commitLabel(LabelComponent *label, sqstd::Array<MeshInstance *> &drawCalls)
{
    Texture2d *texture = label->getTexture();
    Material *material = label->getMaterial();
    Mesh *mesh = label->getMesh();

    ERR_FAIL_NULL_V(mesh);
    ERR_FAIL_NULL_V(texture);
    ERR_FAIL_NULL_V(material);
    int textureIndex = combineTextureMaterial(material, texture, mesh, label->node->getLayer(), drawCalls, label->getStencil());
    LabelAssembler::getInstance()->fillBuffers(label, textureIndex);
    // printf("draw label %p batch %p \n", label, currentMeshInstance);
}

void Batch::commitPhysicsParticle(physics2d::ParticlePhysics2DComponent *component, sqstd::Array<MeshInstance *> &drawCalls)
{
    updatePrevDrawCall();

    PassStates overrideState;
    if (batchStencil.enableStencilTest())
    {
        overrideState.stencilState = batchStencil.getStateFromStage(StencilStage::ENABLED);
    }
    // printf("+++++render %d \n",component->getParticleCount());

    // data render
    currentMeshInstance = createMeshInstance();
    currentMeshInstance->setMaterial(component->getMaterial(), &overrideState);
    currentMeshInstance->setMesh(component->getMesh());
    currentMeshInstance->layer = component->getParticleRenderType() == 3 ?  component->node->getLayer() : Layers::FLUID;
    currentMeshInstance->primitive.count = component->getParticleRenderType() == 2 ? 6 : 1;
    currentMeshInstance->primitive.offset = 0;
    currentMeshInstance->primitive.instance = component->getParticleCount();
    drawCalls.push(currentMeshInstance);

    // show render
    // currentMeshInstance = createMeshInstance();
    // currentMeshInstance->setMaterial(component->getMaterial());
    // currentMeshInstance->setMesh(component->getMesh());
    // currentMeshInstance->layer = component->node->getLayer();
    // currentMeshInstance->primitive.count = 1;
    // currentMeshInstance->primitive.offset = 0;
    // currentMeshInstance->primitive.instance = component->getParticleCount();
    // drawCalls.push(currentMeshInstance);

    // Mesh *mesh = component->getMesh();
    // currentMeshInstance = createMeshInstance();
    // currentMeshInstance->setMaterial(component->getMaterial());
    // currentMeshInstance->setMesh(mesh);
    // currentMeshInstance->layer = component->node->getLayer();
    // currentMeshInstance->primitive.count = mesh->getIndexBuffer()->getCount();
    // currentMeshInstance->primitive.offset = 0;
    // drawCalls.push(currentMeshInstance);

    // printf("draw physicsParticle %p batch %p \n", component, currentMeshInstance);

    currentMeshInstance = nullptr;
}

void Batch::commitParticle(Particles2DComponent *component, sqstd::Array<MeshInstance *> &drawCalls)
{
    updatePrevDrawCall();

    if (component->getGPUUpdateMeshInstance())
    {
        // 粒子在GPU端Update阶段的drawCall
        MeshInstance *batch = component->getGPUUpdateMeshInstance();
        batch->layer = Layers::INHERIT;
        drawCalls.push(batch);
    }

    PassStates overrideState;
    if (batchStencil.enableStencilTest())
    {
        overrideState.stencilState = batchStencil.getStateFromStage(StencilStage::ENABLED);
    }

    Transform2DComponent *transform = component->node->getComponent<Transform2DComponent>();
    DescriptorSet *set = localSets.getLocalDescriptorSet();

    if (component->getTestRenderMeshInstance())
    {
        // 测试使用
        MeshInstance *testInstance = component->getTestRenderMeshInstance();
        testInstance->localDescriptorSet = set;
        testInstance->layer = component->node->getLayer();
        otherBatchers.push(testInstance);
        drawCalls.push(testInstance);
    }

    localSets.setLocalTransform(set, transform);

    if (component->getRenderSceneMarkMaterial())
    {

        currentMeshInstance = createMeshInstance();
        currentMeshInstance->setMaterial(component->getMaterial(), &overrideState);
        currentMeshInstance->setMesh(component->getMesh());
        currentMeshInstance->localDescriptorSet = set;
        currentMeshInstance->layer = Layers::FLMAE;
        currentMeshInstance->primitive.count = 6;
        currentMeshInstance->primitive.offset = 0;
        currentMeshInstance->primitive.instance = component->getRenderNum();
        drawCalls.push(currentMeshInstance);

        // currentMeshInstance = createMeshInstance();
        // currentMeshInstance->setMaterial(component->getRenderSceneMarkMaterial());
        // currentMeshInstance->setMesh(component->getMeshRenderSceneMark());
        // currentMeshInstance->localDescriptorSet = set;
        // currentMeshInstance->layer = component->node->getLayer();
        // currentMeshInstance->primitive.count = 6;
        // currentMeshInstance->primitive.offset = 0;
        // currentMeshInstance->primitive.instance = component->getAmount();
        // drawCalls.push(currentMeshInstance);
    }
    else
    {
        // 普通的直接渲染到场景层级上的方式
        currentMeshInstance = createMeshInstance();
        currentMeshInstance->setMaterial(component->getMaterial(), &overrideState);
        currentMeshInstance->setMesh(component->getMesh());
        currentMeshInstance->localDescriptorSet = set;
        currentMeshInstance->layer = component->node->getLayer();
        currentMeshInstance->primitive.count = 6;
        currentMeshInstance->primitive.offset = 0;
        currentMeshInstance->primitive.instance = component->getRenderNum();
        drawCalls.push(currentMeshInstance);
    }

    // printf("draw commitParticle %p batch %p \n", component, currentMeshInstance);

    currentMeshInstance = nullptr;
}

void Batch::commitRenderer(RenderComponent *component, sqstd::Array<MeshInstance *> &drawCalls)
{

    updatePrevDrawCall();

    PassStates overrideState;
    if (batchStencil.enableStencilTest())
    {
        overrideState.stencilState = batchStencil.getStateFromStage(StencilStage::ENABLED);
    }

    currentMeshInstance = createMeshInstance();
    currentMeshInstance->layer = component->node->getLayer();
    DrawPrimitiveMesh *drawInfo = component->getDrawInfo();
    SQ_ASSERT(drawInfo);
    currentMeshInstance->primitive.count = drawInfo->count;
    currentMeshInstance->primitive.offset = drawInfo->offset;
    currentMeshInstance->primitive.instance = drawInfo->instance;

    // printf("++++++commitRenderer %d %d \n",component->node->nativeId,component->node->getLayer());

    Transform2DComponent *transform = component->node->getComponent<Transform2DComponent>();
    DescriptorSet *set = localSets.getLocalDescriptorSet();
    localSets.setLocalTransform(set, transform);
    currentMeshInstance->localDescriptorSet = set;
    currentMeshInstance->setMaterial(component->getMaterial(), &overrideState);
    currentMeshInstance->setMesh(component->getMesh());
    drawCalls.push(currentMeshInstance);

    // printf("draw renderer %p batch %p \n", component,currentMeshInstance);

    currentMeshInstance = nullptr;
}

void Batch::commitOver()
{
    updatePrevDrawCall();
}

void Batch::reset()
{
    batchStencil.reset();
    currentMeshInstance = nullptr;
    for (int i = 0; i < batches.getCount(); ++i)
    {
        if (batches[i]->localDescriptorSet)
        {
            localSets.recvoery(batches[i]->localDescriptorSet);
        }
        batches[i]->clear();
        drawCallBatchPool.push(batches[i]);
    }
    batches.clear();

    for (int i = 0; i < otherBatchers.getCount(); ++i)
    {
        if (otherBatchers[i]->localDescriptorSet)
        {
            localSets.recvoery(otherBatchers[i]->localDescriptorSet);
            otherBatchers[i]->localDescriptorSet = nullptr;
        }
    }
    otherBatchers.clear();
}

//==============================================
DescriptorSet *UBOLocalDescrtiptorSets::getLocalDescriptorSet()
{
    DescriptorSet *localDescriptorSet = nullptr;
    if (cacheLocalDescriptorSets.getCount() > 0)
    {
        localDescriptorSet = *cacheLocalDescriptorSets.pop();
    }

    if (!localDescriptorSet)
    {
        DescriptorSetLayoutInfo info;
        info.bindings = &pipeline::localSetLayoutBindings;
        DescriptorSetLayout *localLayout = DeviceManager::getInstance()->device->createDescriptorSetLayout(info);
        DescriptorSetInfo setInfo;
        setInfo.layout = localLayout;
        localDescriptorSet = DeviceManager::getInstance()->device->createDescriptorSet(setInfo);
    }
    return localDescriptorSet;
}

void UBOLocalDescrtiptorSets::setLocalTransform(DescriptorSet *localDescriptorSet, Transform2DComponent *transform)
{

    BufferObject *localBuffer = nullptr;
    if (localDescriptorSet->hasValue(toNumber(pipeline::UBOLocal::BINDING)))
    {
        localBuffer = localDescriptorSet->getBindingValue<BufferObject>(toNumber(pipeline::UBOLocal::BINDING));
    }
    else
    {
        BufferInfo bufferInfo;
        bufferInfo.type = BufferType::UNIFORM;
        bufferInfo.usage = BufferUsage::DYNAMIC_DRAW;
        bufferInfo.size = pipeline::UBOLocal::SIZE;
        localBuffer = DeviceManager::getInstance()->device->createBuffer(bufferInfo);
        localDescriptorSet->bindBuffer(toNumber(pipeline::UBOLocal::BINDING), localBuffer);
    }

    Mat4 mat4;
    Mat3 &worldMat = transform->getWorldTransform();
    mat4.setFromMat3(worldMat);
    localBuffer->update((char *)mat4.data, 64);
}

void UBOLocalDescrtiptorSets::recvoery(DescriptorSet *localDescriptorSet)
{

    for (int i = 0; i < cacheLocalDescriptorSets.getCount(); ++i)
    {
        if (cacheLocalDescriptorSets[i] == localDescriptorSet)
        {
            return;
        }
    }

    localDescriptorSet->clear();
    cacheLocalDescriptorSets.push(localDescriptorSet);

    // printf("++++++recvoery %d \n", cacheLocalDescriptorSets.getCount());
}
