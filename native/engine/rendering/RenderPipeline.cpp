#include "RenderPipeline.h"
#include "buildin-block-define.h"
#include "BuildInDescriptorSetManager.h"

using namespace pipeline;

RenderPipeline::RenderPipeline() {}

void RenderPipeline::initialize()
{
    BuildInDescriptorSetManager::getIntance()->initialize();
    pipelineUBO.initialize(DeviceManager::getInstance()->device, this);
}

void RenderPipeline::render(std::vector<Camera *> &cameras,PipleRenderContext& context)
{
    int size = cameras.size();

    // printf("+++++++++++++++RenderPipeline camera %d \n",cameras.size());

    if (size == 0)
        return;

    gfx::CommandBuffer *commandBuffer = DeviceManager::getInstance()->device->getCommandBuffer();
    for (int i = 0; i < size; ++i)
    {
        Camera *camera = cameras[i];
        // applyCamera(camera);
        // commandBuffer->bindDescriptorSet(toNumber(SetIndex::GLOBAL), pipelineUBO.globalDescriptorSet);
        for (int n = 0; n < flows.size(); ++n)
        {
            flows[n]->render(camera);
        }
    }
}

RenderPipeline::~RenderPipeline()
{
    for (int n = 0; n < flows.size(); ++n)
    {
        delete flows[n];
    }
    flows.clear();
}
