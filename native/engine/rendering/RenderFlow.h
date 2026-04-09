#ifndef _RENDERING_RENDER_FLOW_H_
#define _RENDERING_RENDER_FLOW_H_
#include "../scene/Camera.h"

namespace pipeline{

    /**
     * 还可以将RenderPiple继续拆分
     * 整个渲染管线就是RenderFlow实例的集合，不同的渲染管线就有不同的渲染流程，RenderPiple中按照顺序执行RenderFlow
     * RenderFlow中才是真正执行渲染的地方，调用渲染命令的地方
    */
    class RenderFlow
    {
        public:
            virtual ~RenderFlow(){};
            virtual void render(Camera*) = 0;
    };
}

#endif