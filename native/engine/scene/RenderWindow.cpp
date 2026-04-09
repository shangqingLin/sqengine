#include "RenderWindow.h"
#include "Camera.h"
#include "../gfx/DeviceManager.h"
#include "../core/base/config.h"

RenderWindow::RenderWindow() : width(0), height(0), renderPass(nullptr), depthStencilTexture(nullptr)
{
}

void RenderWindow::initialize(RenderWindowDefine &define)
{
    SQ_ASSERT(define.width > 0);
    SQ_ASSERT(define.height > 0);

    Device *device = DeviceManager::getInstance()->device;
    width = define.width;
    height = define.height;

    gfx::RenderPassDefine renderPass;
    renderPass.defaultRenderPass = define.defaultRenderPass;
    renderPass.colorAttachments = define.colorAttachemts;
    if (define.depthStencilAttachment.has_value())
    {
        renderPass.depthStencilAttachment = define.depthStencilAttachment.value();
    }

    if (!define.defaultRenderPass)
    {
        TextureInfo textureInfo;
        textureInfo.width = this->width;
        textureInfo.height = this->height;

        int size = define.colorAttachemts.size();
        colorTextures.reserve(size);
        renderPass.colorTextures.reserve(size);
        for (int i = 0; i < size; ++i)
        {
            textureInfo.format = define.colorAttachemts[i].format;
            gfx::Texture *texture = device->createTexture(textureInfo);
            colorTextures.push_back(texture);
            renderPass.colorTextures.push_back(texture);
        }

        if (define.depthStencilAttachment.has_value())
        {
            textureInfo.format = define.depthStencilAttachment.value().format;
            this->depthStencilTexture = device->createTexture(textureInfo);
            renderPass.depthStencilTexture = depthStencilTexture;
        }
    }
    this->renderPass = device->createRenderPass(renderPass);
}

void RenderWindow::attachCamera(Camera *camera)
{
    for (int i = 0; i < cameras.size(); ++i)
    {
        if (cameras[i] == camera)
        {
            return;
        }
    }
    cameras.push_back(camera);
}

void RenderWindow::detachCamera(Camera *camera)
{
    for (int i = 0; i < cameras.size(); ++i)
    {
        if (cameras[i] == camera)
        {
            cameras.erase(cameras.begin() + i);
            return;
        }
    }
}

void RenderWindow::extractRenderCameras(std::vector<Camera *> &result)
{
    for (int i = 0; i < cameras.size(); ++i)
    {
        if (cameras[i]->enabled)
        {
            result.push_back(cameras[i]);
        }
    }
}

RenderWindow::~RenderWindow()
{
    for (int i = 0; i < colorTextures.size(); ++i)
    {
        delete colorTextures[i];
    }
    colorTextures.clear();

    if (depthStencilTexture)
    {
        delete depthStencilTexture;
        depthStencilTexture = nullptr;
    }
    if (this->renderPass)
    {
        delete this->renderPass;
        this->renderPass = nullptr;
    }
}