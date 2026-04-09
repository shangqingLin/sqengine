#ifndef GFX_WEBGL2_RENDER_PASS_H_
#define GFX_WEBGL2_RENDER_PASS_H_

#include "../base/RenderPass.h"
#include "WebGL2GPUObjectDefine.h"

namespace gfx
{
    class WebGL2RenderPass : public RenderPass
    {
    protected:
        IGLGPUFrameBuffer frameBuffer;
    public:
        virtual void initialize(RenderPassDefine &define);
        virtual ~WebGL2RenderPass();
        inline const IGLGPUFrameBuffer& getFrameBuffer() { return frameBuffer; };
    };
}
#endif