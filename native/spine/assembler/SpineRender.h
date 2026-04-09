#ifndef _SPINE_RENDER_H_
#define _SPINE_RENDER_H_

#include "../framework/SpineComponent.h"
#include <engine/scene/graphics/IndexBuffer.h>

class SpineRender
{
protected:
    bool checkSlotHidden(spSlot *slot);

    /**
     * 1、创建并缓存indices数据
     * 2、计算需要多少顶点和索引数据
     */
    void prepareRender(SpineComponent*,IndexBuffer*,bool&,int&);
public:
    virtual ~SpineRender(){}
    virtual void render(SpineComponent*,float dt) = 0;
};
#endif