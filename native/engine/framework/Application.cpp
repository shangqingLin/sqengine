#include "Application.h"
#include "../../bindings/DispatchManager.h"
#include "../scene/NodeNativeToJsBridge.h"
#include "../scene/graphics/GraphicsBridge.h"
#include "../scene/NodeBridge.h"
#include "../scene/CommandBuffer.h"
#include "../core/sqstd/sqstd.h"
#include "../core/common/Date.h"

static Application *ins = nullptr;

Application::Application() : scene(nullptr), root(this), gameDuration(0.f)
{
    ins = this;
}

Application *Application::getInstance()
{
    return ins;
}

void Application::initialize()
{
    // 1 M = 1024 x 1024
    sqstd::StackTempArenaAllocator::preAllocation(1048576);

    bridge::DispatchManager::getInstance()->initialize();
    bridge::NodeBridge *nodeBridge = new bridge::NodeBridge();
    bridge::DispatchManager::getInstance()->registerObjectBridge(bridge::ObjectType::Node, nodeBridge);
    bridge::DispatchManager::getInstance()->registerObjectBridge(bridge::ObjectType::Scene, nodeBridge);
    bridge::DispatchManager::getInstance()->registerObjectBridge(bridge::ObjectType::CommandBufferBridge, new CommandBufferBridge());
    bridge::DispatchManager::getInstance()->registerObjectBridge(bridge::ObjectType::Mesh, new MeshGraphicsBridge());
    bridge::DispatchManager::getInstance()->registerObjectBridge(bridge::ObjectType::VertexBuffer, new VertexBufferGraphicsBridge());
    bridge::DispatchManager::getInstance()->registerObjectBridge(bridge::ObjectType::IndexBuffer, new IndexBufferGraphicsBridge());

    root.initialize();
    root.setPipleline(NULL);
    NodeNativeToJsBridge::initialize();
}

void Application::update()
{

    // printf("___________________________update\n");

    frameStartTime = getDateNow();
    for (int i = 0; i < systems.size(); ++i)
    {
        systems[i]->startUpdate();
    }

    // root.dt = bridge::DispatchManager::getInstance()->readFrameTime();
    // root.dtSecond = root.dt * 0.001f;
    dt = bridge::DispatchManager::getInstance()->readFrameTime();
    dtSecond = dt * 0.001f;
    gameDuration += dtSecond;

    bridge::DispatchManager::getInstance()->frameBegin();
    bridge::DispatchManager::getInstance()->dispatchJsToNative();

    if (scene)
    {
        root.updateEnable();
        root.start();
    }

    timer.update(dt, TIMER_UPDATE_STAGE::UPDATE);
    for (int i = 0; i < systems.size(); ++i)
    {
        systems[i]->update(dt);
    }

    if (scene)
    {
        root.updateDisable();
        root.update(dt);
    }

    Node::resetHasChangedFlags();
    bridge::DispatchManager::getInstance()->frameEnd();
    // 确保临时分配器的内存被清理，没有被错误使用
    SQ_ASSERT(sqstd::StackTempArenaAllocator::getInstance()->getAllocation() == 0);
}

long Application::getFromFrameStartTime()
{
    return getDateNow() - frameStartTime;
}

void Application::postUpdate()
{
    for (int i = 0; i < systems.size(); ++i)
    {
        systems[i]->postUpdate();
    }
    timer.update(0, TIMER_UPDATE_STAGE::POST_UPDATE);
}

void Application::registerSystem(System *system)
{
    systems.push_back(system);
    system->init();
}

void Application::setRunScene(Scene *scene)
{
    if (this->scene)
    {
        this->scene->active(false);
    }
    this->scene = scene;
    this->scene->active(true);
    this->emit<Application::CHANGE_SCENE>();
}

void Application::setRenderPipeline(pipeline::RenderPipeline *pipeline)
{
    root.setPipleline(pipeline);
}
