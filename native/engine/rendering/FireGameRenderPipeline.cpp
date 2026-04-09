#include "FireGameRenderPipeline.h"
#include "../gfx/DeviceManager.h"
#include "../scene/NodeTreeManager.h"
#include "./buildin-block-define.h"
#include "../gfx/webgl2/WebGL2RenderPass.h"
#include "../assets/AssetManager.h"
#include "BuildInMeshDataManager.h"

using namespace pipeline;

void FireGameRenderPipeline::renderMeshInstance(MeshInstance *batch, gfx::CommandBuffer *commandBuffer)
{

    Mesh *mesh = batch->getMesh();
    ERR_FAIL_NULL_V(mesh);

    const sqstd::Array<Pass> &passes = batch->getPasses();
    int passSize = passes.getCount();
    SQ_ASSERT(passSize > 0);

    // printf("renderMeshInstance %p \n",batch);

    // printf("material %p passSize: %d\n",material,passSize);
    Device *device = DeviceManager::getInstance()->getDevice();

    mesh->upload();

    DrawPrimitiveMesh &primitive = batch->primitive;
    SQ_ASSERT(primitive.count > 0);

    gfx::InputDraw gpuDraw;
    gpuDraw.firstIndex = primitive.offset;
    gpuDraw.indexCount = primitive.count;
    gpuDraw.instanceCount = primitive.instance;
    gpuDraw.glIndexType = primitive.type;

    // printf(">>>>>> %d \n",primitive.count);

    IPipelineStateInfo state;
    for (int s = 0; s < passSize; ++s)
    {
        // printf(" === draw begin === \n");
        Pass *pass = &passes[s];
        state.blendState = &pass->getBlendState();
        state.depthState = &pass->getDepthState();
        state.stencilState = &pass->getStencilState();
        state.rasterizerState = &pass->getRasterizerState();
        state.shader = pass->getShaderVariant();
        state.primitive = pass->getPrimitive();
        state.assembler = mesh->getInputAssembler();
        PipelineState *pipleState = device->createPipelineState(state);
        commandBuffer->bindPipelineState(pipleState);
        commandBuffer->bindDescriptorSet(toNumber(SetIndex::MATERIAL), pass->getDescriptorSet());
        if (batch->localDescriptorSet)
            commandBuffer->bindDescriptorSet(toNumber(SetIndex::LOCAL), batch->localDescriptorSet);

        // printf("draw begin material %p shader %p \n",material,state.shader);
        commandBuffer->draw(gpuDraw);
        // printf(" --draw end \n ");
    }
}

void FireGameRenderPipeline::beginRenderPass(Camera *camera, gfx::CommandBuffer *commandBuffer, bool forceRefresh)
{

    SQ_ASSERT(camera->getRenderWindow());

    // printf(" beginRenderPass layer %d \n",camera->getLayer());

    pipelineUBO.setupCamera(camera);
    commandBuffer->bindDescriptorSet(toNumber(SetIndex::GLOBAL), pipelineUBO.globalDescriptorSet);
    commandBuffer->beginRenderPass(
        camera->getRenderWindow()->getRenderPass(),
        camera->getViewport(),
        camera->getClearColor(),
        camera->getClearDepth(),
        camera->getClearStencil(),
        forceRefresh);
}

void FireGameRenderPipeline::enableWashRender(bool b, int waterMarkMaterialId)
{
    enabelWashRender = b;
    washFirst = b;
    if (b)
    {
        waterMarkMaterial = (Material *)AssetManager::getInstance()->findById(waterMarkMaterialId);
    }
    else
    {
        waterMarkMaterial = nullptr;
    }
}

void FireGameRenderPipeline::renderWashEffect(std::vector<Camera *> &cameras, sqstd::Array<MeshInstance *> &drawCalls)
{
    Device *device = DeviceManager::getInstance()->getDevice();
    gfx::CommandBuffer *commandBuffer = device->getCommandBuffer();

    Camera *gameDataCamera = nullptr;
    for (int i = 0; i < cameras.size(); ++i)
    {
        Camera *camera = cameras[i];
        if (camera->getLayer() == Layers::SCENE_OBJECT)
        {
            gameDataCamera = camera;
            break;
        }
    }

    gfx::RenderPass *renderPass = gameDataCamera->getRenderWindow()->getRenderPass();
    if (washFirst)
    {
        // gfx::WebGL2RenderPass* ps2 = dynamic_cast<gfx::WebGL2RenderPass*>(renderPass);
        // printf("++++++++++++++++++++++++buffer %d \n",ps2->getFrameBuffer().glFramebuffer);
        
        // 第一次执行先清除屏幕数据
        renderPass->colorAttachments[0].op = gfx::AttachmentOp::CLEAR;
    }
    else
    {
        // 接下来的渲染保持帧缓冲区的数据不清除
        renderPass->colorAttachments[0].op = gfx::AttachmentOp::LOAD;
    }

    beginRenderPass(gameDataCamera, commandBuffer, washFirst);
    washFirst = false;

    // 进行一次全屏渲染，主要目的是读取水体纹理中的像素，看看哪个像素被水体覆盖了
    // 所以执行该方法之前，必须先执行水体渲染,即Layers.FLUID必须先渲染
    MeshInstance *markFluidMeshInstance = batch.createMeshInstance();
    markFluidMeshInstance->setMesh(pipeline::BuildInMeshDataManager::getIntance()->getScreenMesh());
    markFluidMeshInstance->primitive.count = 6;
    markFluidMeshInstance->primitive.offset = 0;

    // waterMarkMaterial里面需要持有对水体纹理的引用
    markFluidMeshInstance->setMaterial(waterMarkMaterial);
    renderMeshInstance(markFluidMeshInstance, commandBuffer);

    // 渲染内容，配合Blend与有水体覆盖的像素进行判断，有水体覆盖的像素显示这里的内容，没有水体的则不显示
    for (int n = 0; n < drawCalls.getCount(); ++n)
    {
        MeshInstance *batch = drawCalls[n];
        if (batch->layer == Layers::SCENE_OBJECT)
        {
            renderMeshInstance(batch, commandBuffer);
        }
    }
}

void FireGameRenderPipeline::render(std::vector<Camera *> &cameras, PipleRenderContext &context)
{
    if (cameras.size() == 0)
        return;

    sqstd::Array<MeshInstance *> drawCalls(sqstd::StackTempArenaAllocator::getInstance());
    drawCalls.resize(200);
    batch.walkScene(drawCalls);

    // printf("batch count %d \n",drawCalls.getCount());

    batch.commitOver();

    Device *device = DeviceManager::getInstance()->getDevice();
    gfx::CommandBuffer *commandBuffer = device->getCommandBuffer();

    if (drawCalls.getCount() == 0)
    {
        batch.reset();

        // 前一帧有内容，但这帧没有内容，所以需要清除上一帧的内容
        for (int i = 0; i < cameras.size(); ++i)
        {
            Camera *camera = cameras[i];
            if (camera->getLayer() == Layers::UI)
            {
                beginRenderPass(camera, commandBuffer, true);
                break;
            }
        }
        return;
    }

    // 那些自带Camera的MeshInstance
    for (int n = 0; n < drawCalls.getCount(); ++n)
    {
        MeshInstance *batch = drawCalls[n];
        if (batch->getCamera())
        {
            beginRenderPass(batch->getCamera(), commandBuffer, false);
            renderMeshInstance(batch, commandBuffer);
        }
    }

    //  printf("============begin GAME_OBJECT_INFO \n");
    // 优先渲染GameObject Info到一张场景Texture上
    for (int i = 0; i < cameras.size(); ++i)
    {
        Camera *camera = cameras[i];
        if (camera->getLayer() == Layers::FLUID)
        {

            // printf("===user camera %p \n",camera);

            /**
             * 上一帧有对象渲染到GAME_OBJECT_INFO_BLEND纹理中
             * 但当前帧没有，则这个纹理还存储上一帧旧的数据，所以即使当前帧没有对象渲染也需要清空一下RenderTexture中存储的数据
             * prevRenderGameObjectInfo 属性就是用来实现此功能
             */
            beginRenderPass(camera, commandBuffer, prevRenderGameObjectInfo);
            prevRenderGameObjectInfo = false;
            for (int n = 0; n < drawCalls.getCount(); ++n)
            {
                MeshInstance *batch = drawCalls[n];
                if (Layers::FLUID == batch->layer)
                {
                    prevRenderGameObjectInfo = true;
                    // printf("====render objinfo\n");
                    renderMeshInstance(batch, commandBuffer);
                    //   printf("====render objinfo eeeeed \n");
                }
            }
            break;
        }
    }

    if (enabelWashRender)
    {
        this->renderWashEffect(cameras, drawCalls);
    }
    // printf("============begin GAME_OBJECT_INFO_BLEND \n");
    for (int i = 0; i < cameras.size(); ++i)
    {
        Camera *camera = cameras[i];
        if (camera->getLayer() == Layers::FLMAE)
        {

            /**
             * 上一帧有对象渲染到GAME_OBJECT_INFO_BLEND纹理中
             * 但当前帧没有，则这个纹理还存储上一帧旧的数据，所以即使当前帧没有对象渲染也需要清空一下RenderTexture中存储的数据
             * prevRenderGameObjectInfoBlend 属性就是用来实现此功能
             */
            beginRenderPass(camera, commandBuffer, prevRenderGameObjectInfoBlend);
            // printf("======GAME_OBJECT_INFO_BLEND %d \n",prevRenderGameObjectInfoBlend);

            prevRenderGameObjectInfoBlend = false;
            for (int n = 0; n < drawCalls.getCount(); ++n)
            {
                MeshInstance *batch = drawCalls[n];
                if (Layers::FLMAE == batch->layer)
                {
                    prevRenderGameObjectInfoBlend = true;
                    renderMeshInstance(batch, commandBuffer);
                }
            }
            break;
        }
    }

    // printf("======begin normal render \n");

    for (int i = 0; i < cameras.size(); ++i)
    {
        Camera *camera = cameras[i];

        if (camera->getLayer() == Layers::FLMAE || camera->getLayer() == Layers::FLUID)
        {
            continue;
        }

        // printf("+++=camera %p \n",camera);
        // if (camera->getLayer() == Layers::BACKGROUND_FRA)
        // {
        // gfx::WebGL2RenderPass *pass = (gfx::WebGL2RenderPass *)(camera->getRenderWindow()->getRenderPass());
        // printf("camera frame buffer %d layer %d \n", pass->getFrameBuffer().glFramebuffer,camera->getLayer());
        // }

        beginRenderPass(camera, commandBuffer, false);
        for (int n = 0; n < drawCalls.getCount(); ++n)
        {
            MeshInstance *batch = drawCalls[n];
            if (camera->getLayer() == batch->layer)
            {
                renderMeshInstance(batch, commandBuffer);
                // printf("++++++normal %d  \n", batch->layer);
            }
        }
    }
    batch.reset();
}