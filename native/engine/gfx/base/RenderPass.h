#pragma once
#include <optional>
#include <vector>
#include "define.h"
#include "Texture.h"

namespace gfx
{

    enum class AttachmentOp
    {

        /**
         * 保留原样,不做任何操作
         */
        LOAD,

        /**
         * 清除原有数据，使用Camera上的值填充
         */
        CLEAR,

        DISCARD
    };

    /**
     * 颜色缓冲区
     */
    struct ColorAttachment
    {
        Format format = Format::UNKNOWN;
        AttachmentOp op = AttachmentOp::CLEAR;
    };

    /**
     * 深度模版缓冲区
     */
    struct DepthStencilAttachment
    {
        Format format = Format::UNKNOWN;
        AttachmentOp depthOp = AttachmentOp::CLEAR;
        AttachmentOp stencilOp = AttachmentOp::CLEAR;
    };

    struct RenderPassDefine
    {
        std::vector<gfx::Texture *> colorTextures;
        std::vector<ColorAttachment> colorAttachments;
        gfx::Texture *depthStencilTexture = NULL;
        DepthStencilAttachment depthStencilAttachment;

        /**
         * 是否为默认缓冲区
         */
        bool defaultRenderPass = false;
    };

    /**
     * RenderPass是现代图形API（Vukan、Meta）的一个核心概念
     * 其功能类似于OpenGL的FrameBuffer，但要比OPenGL的FrameBuffer强大。
     * 为了统一，这种功能统一抽象为RenderPass
     */
    class RenderPass
    {
    public:
        std::vector<ColorAttachment> colorAttachments;
        DepthStencilAttachment depthStencilAttachment;
        virtual void initialize(RenderPassDefine &) = 0;
        virtual ~RenderPass() {};
    };

}
