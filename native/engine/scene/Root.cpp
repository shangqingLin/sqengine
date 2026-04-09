#include "Root.h"
#include "../rendering/forward/ForwardPipeline.h"
#include "../framework/Application.h"
#include "../core/base/config.h"
#include <algorithm>
#include "NodeBridge.h"
#include "CommandBuffer.h"
#include "./graphics/GraphicsBridge.h"
#include "./NodeTreeManager.h"
#include "../2d/render/BatcherRenderData.h"
#include "../platform/Screen.h"

Root::Root(Application *app) : pipleLine(NULL)
{
}

void Root::initialize()
{
    RenderWindowDefine define;
    const Size<int> &size = screen->getRenderSize();
    define.width = size.width > 0 ? size.width : 1334;
    define.height = size.height > 0 ? size.height : 750;
    define.defaultRenderPass = true;
    define.colorAttachemts.assign(1, gfx::ColorAttachment());
    define.colorAttachemts[0].format = Format::RGBA8;
    define.depthStencilAttachment = gfx::DepthStencilAttachment();
    define.depthStencilAttachment.value().format = Format::DEPTH_STENCIL;
    mainWindow = createRenderWindow(define);
}

void Root::start()
{
    NodeTreeManager::getInstnace()->start();
}

void Root::updateEnable()
{
    NodeTreeManager::getInstnace()->updateEnable();
}

void Root::updateDisable()
{
    NodeTreeManager::getInstnace()->updateDisable();
}

void Root::update(float dt)
{
    BatcherRenderData::getInstance()->reset();

    Application::getInstance()->timer.update(dt, TIMER_UPDATE_STAGE::AFTER_UPDATE);

    for (int i = 0; i < renderWindows.size(); ++i)
    {
        RenderWindow *win = renderWindows[i];
        win->extractRenderCameras(renderCameraList);
    }
    sort(renderCameraList.begin(), renderCameraList.end(), sortCameras);

    // printf("Root update camera %d %d \n",renderWindows.size(), renderCameraList.size());

    NodeTreeManager::getInstnace()->update();

    if (this->pipleLine && !renderCameraList.empty())
    {
        pipeline::PipleRenderContext context;
        context.nodeTreeManager = NodeTreeManager::getInstnace();
        this->pipleLine->render(renderCameraList, context);
    }

    renderCameraList.clear();
}

void Root::setPipleline(pipeline::RenderPipeline *pipeline)
{
    if (this->pipleLine)
    {
        delete this->pipleLine;
    }

    if (pipeline)
    {
        this->pipleLine = pipeline;
        this->pipleLine->initialize();
    }
    else
    {
        this->pipleLine = new pipeline::ForwardPipeline();
        this->pipleLine->initialize();
    }
}

RenderWindow *Root::createRenderWindow(RenderWindowDefine &define)
{
    RenderWindow *win = new RenderWindow();
    win->initialize(define);
    renderWindows.push_back(win);
    return win;
}

void Root::removeRenderWindow(RenderWindow *window)
{

    for (int i = 0; i < renderWindows.size(); ++i)
    {
        if (renderWindows[i] == window)
        {
            renderWindows.erase(renderWindows.begin() + i);
            delete window;
            return;
        }
    }
}
