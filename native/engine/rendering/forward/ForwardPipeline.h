#ifndef _RENDERING_FORWARD_PIPELINE_H_
#define _RENDERING_FORWARD_PIPELINE_H_

#include "../RenderPipeline.h"

namespace pipeline{
    class ForwardPipeline : public RenderPipeline
    {        
        public:
            ForwardPipeline();
            virtual ~ForwardPipeline();
            virtual void initialize();
    };
}
#endif