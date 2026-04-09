#ifndef _APPLICATION_H_
#define _APPLICATION_H_
#include "../scene/Root.h"
#include "../scene/Scene.h"
#include "System.h"
#include "../core/event/EventDispatcher.h"
#include "Timer.h"
#include "../core/core.h"
#include "../rendering/RenderPipeline.h"

class Application
{

    IMPL_EVENT_TARGET(Application)
    DECLARE_TARGET_EVENT_BEGIN(Application)
    TARGET_EVENT_ARG0(CHANGE_SCENE)
    DECLARE_TARGET_EVENT_END()

private:
    std::vector<System *> systems;
    Scene *scene;
    Root root;
    long frameStartTime = 0;

public:
    /**
     * 帧时间：毫秒
     */
    float dt;

    /**
     * 帧时间：秒
     */
    float dtSecond;

    /**
     * 游戏从启动开始累积的时间，单位是秒
     */
    float gameDuration;

    Timer timer;

    Application();
    static Application *getInstance();
    void initialize();
    void update();
    void postUpdate();
    void registerSystem(System *);
    inline Root &getRoot() { return root; };
    template <typename T>
    T *getSystem()
    {
        for (int i = 0; i < systems.size(); ++i)
        {
            T *system = dynamic_cast<T *>(systems[i]);
            if (system)
            {
                return system;
            }
        }
        return nullptr;
    }
    void setRunScene(Scene *scene);
    inline Scene *getRunScene() { return scene; }
    void setRenderPipeline(pipeline::RenderPipeline *);

    /**
     * 当前当前时间到帧开始的时间间隔
     */
    long getFromFrameStartTime();
};

#endif