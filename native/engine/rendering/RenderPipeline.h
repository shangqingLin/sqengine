#ifndef _RENDERING_RENDER_PILELINE_H_
#define _RENDERING_RENDER_PILELINE_H_
#include <vector>
#include "../scene/Camera.h"
#include "PipelineUBO.h"
#include "RenderFlow.h"

class NodeTreeManager;
namespace pipeline
{

    struct PipleRenderContext
    {
        NodeTreeManager* nodeTreeManager{nullptr};
    };

    class RenderPipeline
    {
    protected:
        PipelineUBO pipelineUBO;
        std::vector<RenderFlow *> flows;

    public:
        RenderPipeline();
        virtual ~RenderPipeline();
        virtual void initialize();
        virtual void render(std::vector<Camera *> &,PipleRenderContext& context);
    };
}

#endif