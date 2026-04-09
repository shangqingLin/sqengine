#ifndef GFX_WEBGL_RENDER_PASS_H_
#define GFX_WEBGL_RENDER_PASS_H_

#include "../base/RenderPass.h"
#include "../webglbase/BaseWebGLGPUObjectDefine.h"

namespace gfx{
    class WebGLRenderPass : public RenderPass
    {
        protected:
            IGLGPUFrameBuffer frameBuffer;
        public:
            virtual void initialize(RenderPassDefine&);
            virtual ~WebGLRenderPass();
            inline const IGLGPUFrameBuffer& getFrameBuffer() { return frameBuffer; };
    };    
}
#endif