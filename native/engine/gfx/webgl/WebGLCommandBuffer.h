#ifndef _WEBGL_COMMAND_BUFFER_H_
#define _WEBGL_COMMAND_BUFFER_H_
#include "../base/CommandBuffer.h"
#include "../base/PipelineState.h"
#include "WebGLRenderPass.h"

class WebGLCommandBuffer : public gfx::CommandBuffer
{

private:
  PipelineState *pipelineState = NULL;
  std::vector<DescriptorSet *> curGPUDescriptorSets;

  struct
  {
    gfx::WebGLRenderPass *pass{nullptr};
    Rect<float> renderArea;
    Color clearColors;
    float clearDepth;
    int clearStencil;
    bool dirty;
  } renderPassSet;

  void bindStates();

public:
  virtual void beginRenderPass(gfx::RenderPass *renderPass, Rect<float> &renderArea, Color &clearColors, float clearDepth, int clearStencil,bool forceRefresh = false);
  virtual void endRenderPass();
  virtual void bindPipelineState(PipelineState *pso);
  virtual void bindDescriptorSet(int set, DescriptorSet *descriptorSet);
  virtual void setViewport(const Rect<float> &vp);
  virtual void setScissor(const Rect<float> &rect);
  virtual void draw(const gfx::InputDraw &info);
};
#endif