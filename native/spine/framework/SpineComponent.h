#pragma once

#include "../../bindings/define.h"
#include "../assets/Spine.h"
#include <spine/AnimationState.h>
#include <spine/Skeleton.h>
#include <spine/AnimationStateData.h>
#include "../..//engine//framework/component/RenderComponent.h"

void processSpineComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
class SpineRender;
class SpineComponent : public RenderComponent
{
private:
    bool isPuase;
    Spine *spine;
    spSkeleton* skeleton;
    spAnimationStateData* animationStateData;
    spAnimationState *animationState;
    SpineRender* spineRender;
    int currentAnimation = -1;
    bool loop;
    float speed;
    //播放帧率
    float frameDuration;
    float currentFrameDt;

    char renderType = 0;
    void clearSpine();
protected:    
    //  virtual void _render(Batcher2D *batch);
    //  virtual bool canRender();
    //  virtual void _onMaterialModified();
public:
    friend class SpineRender;
    friend class SpineGPURender;
    friend class SpineCPURender;
    SpineComponent();
    ~SpineComponent();
    virtual void onEnable();
    virtual void onDisable();
    void setSpine(Spine*);
    void setPlaySpeed(float speed);
    void play(int animIndex,bool loop = false);
    void stop();
    void puase();
    void resume();
    void update(float dt);
    void setSkin(int skinIndex);
    spSkin* getSkin();

    void setRenderType(int type);
    
    /**
     * 设置播放帧率
     */
    void setFrame(int frame);
};


