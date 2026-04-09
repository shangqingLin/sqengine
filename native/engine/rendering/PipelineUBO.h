#ifndef _RENDERING_PIPELINE_UBO_H_
#define _RENDERING_PIPELINE_UBO_H_
#include "../scene/Camera.h"
#include "../gfx/gfx.h"
namespace pipeline{
    class RenderPipeline;
    class PipelineUBO
    {
    public:
        DescriptorSet *globalDescriptorSet;
        PipelineUBO();
        ~PipelineUBO();
        void initialize(Device*,RenderPipeline*);
        void updateGlobalUBO(Camera*);
        void updateCameraUBO(Camera*);
        void setupCamera(Camera*);
    };
}
#endif