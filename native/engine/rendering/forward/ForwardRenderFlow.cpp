#include "ForwardRenderFlow.h"
#include "../buildin-block-define.h"
#include "../../gfx/gfx.h"
#include "../../core/core.h"

using namespace pipeline;

void ForwardRenderFlow::render(Camera *camera)
{
    
    /*
    Device *device = DeviceManager::getInstance()->device;
    gfx::CommandBuffer *cmd = device->getCommandBuffer();

    // printf(":render %f %f %f %f\n",camera->getViewport().x,camera->getViewport().y,camera->getViewport().width,camera->getViewport().height);
    // printf(":render renderWindow %d %p \n",camera->getNode()->nativeId,camera->getRenderWindow());
    SQ_ASSERT(camera->getRenderWindow());
    cmd->beginRenderPass(
        camera->getRenderWindow()->getRenderPass(),
        camera->getViewport(),
        camera->getClearColor(),
        camera->getClearDepth(),
        camera->getClearStencil());

    RenderScene *scene = camera->getScene();
    const std::vector<MeshInstance *> &batches = scene->getBatches();
    gfx::CommandBuffer *commandBuffer = device->getCommandBuffer();

    IPipelineStateInfo state;

    // printf("ForwardRenderFlow scene %p draw batch %d \n", scene, batches.size());

    for (int i = 0; i < batches.size(); ++i)
    {
        MeshInstance *batch = batches[i];

        //  printf("%d check: %d camera layer %u,batch layer %d \n",i,batch->primitive.has_value(), toNumber(camera->getLayer()));

        SQ_ASSERT(batch->primitive.has_value());

        // 应用Layers
        // if (!batch->primitive.has_value())
        // {
        //     continue;
        // }

        if (batch->getCamera())
        {
            if (batch->getCamera() != camera)
            {
                continue;
            }
        }
        else if (camera->getLayer() != batch->layer)
        {
            continue;
        }

        // printf("ForwardRenderFlow i %d camera %p cameranode: %d drawCount %d drawOffset %d cLayer %d bLayer %d\n",
        //     i,
        //     batch->getCamera(),
        //     camera->getNode() ? camera->getNode()->nativeId : -2,
        //     batch->primitive.value().count,
        //     batch->primitive.value().offset,
        //     toNumber(camera->getLayer()) ,
        //     toNumber(batch->layer));

        // printf("draw %d batch %p \n",i,batch);
        
        std::shared_ptr<Mesh> mesh = batch->getMesh();

        ERR_FAIL_NULL_V(mesh.get());

        mesh->upload();

        DrawPrimitiveMesh &primitive = batch->primitive.value();
        if (!primitive.gpuDraw)
        {
            primitive.gpuDraw = new gfx::InputDraw();
        }
        primitive.gpuDraw->firstIndex = primitive.offset;
        primitive.gpuDraw->indexCount = primitive.count;
        primitive.gpuDraw->instanceCount = primitive.instance;
        primitive.gpuDraw->glIndexType = primitive.type;

        Material *material = batch->getMaterial();
        ERR_FAIL_NULL_V(material);

        int passSize = material->passes.size();

        // printf("material %p passSize: %d\n",material,passSize);

        SQ_ASSERT(passSize > 0);

        for (int s = 0; s < passSize; ++s)
        {
            // printf(" === draw begin === \n");
            Pass *pass = material->passes[s];
            state.blendState = &pass->getBlendState();
            state.depthState = &pass->getDepthState();
            state.stencilState = &pass->getStencilState();
            state.rasterizerState = &pass->getRasterizerState();
            state.shader = pass->getShaderVariant();
            state.primitive = pass->getPrimitive();
            state.assembler = mesh->getInputAssembler();
            PipelineState *pipleState = device->createPipelineState(state);
            commandBuffer->bindPipelineState(pipleState);
            commandBuffer->bindDescriptorSet(toNumber(SetIndex::MATERIAL), pass->descriptorSet);

            if (batch->localDescriptorSet)
                commandBuffer->bindDescriptorSet(toNumber(SetIndex::LOCAL), batch->localDescriptorSet);
            commandBuffer->draw(*primitive.gpuDraw);
        }
    }

    // printf("----draw end\n");

    cmd->endRenderPass();*/
}
