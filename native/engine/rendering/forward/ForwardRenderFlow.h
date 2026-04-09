#ifndef _RENDERING_FORWAR_DRENDER_FLOW_H_
#define _RENDERING_FORWAR_DRENDER_FLOW_H_

#include "../RenderFlow.h"

namespace pipeline{
    class ForwardRenderFlow : public RenderFlow
    {
        public:
            virtual void render(Camera*);
    };
    
}

#endif
