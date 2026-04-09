#include "PipelineUBO.h"
#include "RenderPipeline.h"
#include "buildin-block-define.h"
#include "../core/common/number_to_string.h"
#include "../framework/Application.h"
#include "../core/sqstd/Array.h"
#include "../core/sqstd/StackTempArenaAllocator.h"

using namespace pipeline;

PipelineUBO::PipelineUBO() : globalDescriptorSet(NULL)
{
}

void PipelineUBO::initialize(Device *device, RenderPipeline *renderPipeline)
{

    {

        std::vector<const DescriptorSetLayoutBinding *> bindings;
        bindings.resize(2);

        // Camera
        bindings.push_back(&pipeline::UBOCamera::DESCRIPTOR);

        // Global
        bindings.push_back(&pipeline::UBOGlobal::DESCRIPTOR);
        DescriptorSetLayoutInfo layoutInfo;
        layoutInfo.bindings = &bindings;
        DescriptorSetLayout *layout = device->createDescriptorSetLayout(layoutInfo);
        DescriptorSetInfo setInfo;
        setInfo.layout = layout;
        globalDescriptorSet = device->createDescriptorSet(setInfo);
    }

    // Camera

    BufferInfo bufferInfo;
    bufferInfo.type = BufferType::UNIFORM;
    bufferInfo.usage = BufferUsage::DYNAMIC_DRAW;
    bufferInfo.webglSysToJsByCopy = true;
    BufferObject *cameraBuffer = device->createBuffer(bufferInfo);
    globalDescriptorSet->bindBuffer(toNumber(pipeline::UBOCamera::BINDING), cameraBuffer);

    // GLOBAL
    bufferInfo.type = BufferType::UNIFORM;
    bufferInfo.usage = BufferUsage::DYNAMIC_DRAW;
    bufferInfo.webglSysToJsByCopy = true;
    BufferObject *globalBuffer = device->createBuffer(bufferInfo);
    globalDescriptorSet->bindBuffer(toNumber(pipeline::UBOGlobal::BINDING), globalBuffer);
}

void PipelineUBO::updateGlobalUBO(Camera *camera)
{

    int bufferSize = 12;
    sqstd::Array<float> data(sqstd::StackTempArenaAllocator::getInstance());
    data.resize(3);

    // //帧时间
    data.push(Application::getInstance()->dt);
    data.push(Application::getInstance()->gameDuration);

    // //屏幕大小
    Rect<float> &rect = camera->getViewport();
    data.push(rect.width);
    data.push(rect.height);

    // printf("camera %p %f %f %f \n",camera,Application::getInstance()->dt,rect.width,rect.height);

    BufferObject *cameraBuffer = globalDescriptorSet->getBindingValue<BufferObject>(toNumber(pipeline::UBOGlobal::BINDING));
    cameraBuffer->update((char *)data.getData(), data.getCount() << 2);
}

void PipelineUBO::updateCameraUBO(Camera *camera)
{
    sqstd::Array<float> data(sqstd::StackTempArenaAllocator::getInstance());
    data.resize(16);

    Mat4 &viewProjMat = camera->getViewProjMat();

    // 设置视图投影矩阵
    for (int i = 0; i < 16; ++i)
    {
        data.push(viewProjMat.data[i]);
    }

    // printf("UBO camrae\n");
    // if(camera->getLayer() == Layers::MAP)
    // viewProjMat.toString();

    // std::string datas;
    // charBufferToFloatString(data.m_pBuffer,16,datas);
    // printf("fucufufufuff %s\n",datas.c_str());

    BufferObject *cameraBuffer = globalDescriptorSet->getBindingValue<BufferObject>(toNumber(pipeline::UBOCamera::BINDING));
    cameraBuffer->update((char *)data.getData(), data.getCount() << 2);
}

void PipelineUBO::setupCamera(Camera *camera)
{
    updateGlobalUBO(camera);
    updateCameraUBO(camera);
}

PipelineUBO::~PipelineUBO()
{
    if (globalDescriptorSet)
    {
        delete globalDescriptorSet;
        globalDescriptorSet = nullptr;
    }
}