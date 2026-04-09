#ifndef _COMMAND_BUFFER_H_
#define _COMMAND_BUFFER_H_

#include "common-define.h"
#include "define.h"
#include "DescriptorSet.h"
#include "InputAssembler.h"
#include "PipelineState.h"
#include "../../core/math/Rect.h"
#include "./InputDraw.h"
#include "RenderPass.h"

namespace gfx
{
  class CommandBuffer{
    public:
      virtual void beginRenderPass(gfx::RenderPass* renderPass,Rect<float>& renderArea,Color& clearColors, float clearDepth, int clearStencil,bool forceRefresh = false) = 0;
      virtual void endRenderPass() = 0;
      virtual void bindPipelineState(PipelineState *pso) = 0;
      virtual void bindDescriptorSet(int set, DescriptorSet *descriptorSet) = 0;

      virtual void setViewport(const Rect<float> &vp) = 0;
      virtual void setScissor(const Rect<float> &rect) = 0;
      virtual void draw(const gfx::InputDraw& info) = 0;
  };
}
#endif