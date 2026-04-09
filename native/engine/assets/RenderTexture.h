#pragma once

#include "Texture2d.h"
#include "../scene/RenderWindow.h"

struct RenderTextureDefine
{
    /**
     * 大小是必须设置的
     */
    int width = 0;
    int height = 0;
    Format format = Format::RGBA8;
    gfx::AttachmentOp op = gfx::AttachmentOp::CLEAR;
    std::optional<SamplerInfo> sampler; // 纹理的采样器对象
    bool needDepthStencil = false;      // 是否需要记录深度和模板缓冲区的值
};

class RenderTexture : public Texture2d
{
private:
    RenderWindow *window;

public:
    RenderTexture();
    RenderTexture(int id);
    void initiliazeFormJS(char *);
    void initiliaze(RenderTextureDefine &);
    virtual ~RenderTexture();
    inline RenderWindow *getRenderWindow() { return window; };
    virtual void create(ITexture2DCreateInfo *);
};
