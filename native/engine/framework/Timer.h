#ifndef _APP_TIMER_H_
#define _APP_TIMER_H_
#include <vector>
#include "../core/core.h"

class Timer;
enum class TIMER_UPDATE_STAGE : int
{
    UPDATE,        // 在遍历节点树之前
    AFTER_UPDATE,  // 在遍历节点树之后、执行管线渲染之前
    POST_UPDATE,   // 在渲染之后
    PHYSICS_UPDATE // 物理引擎update之后
};
SQ_ENUM_CONVERSION_OPERATOR(TIMER_UPDATE_STAGE);

class TimerHandler
{
private:
    bool once;
    int exeFrame;
    int delayFrame;
    float exeTime;
    float delayTime;
    bool useFrame;

    // 设置定时器被调用的阶段
    TIMER_UPDATE_STAGE updateStage;
    void *obj;                         // 存储对象指针
    void (TimerHandler::*func)(float); // 存储成员函数指针
public:
    TimerHandler();
    void clear();
    bool has();
    void call(float dt);

    template <typename O>
    bool equals(void *object, void (O::*function)(float))
    {
        return obj == object && func == reinterpret_cast<void (TimerHandler::*)(float)>(function);
    }

    template <typename O>
    void setFunction(O *object, void (O::*function)(float))
    {
        obj = static_cast<void *>(object);
        func = reinterpret_cast<void (TimerHandler::*)(float)>(function);
    }
    friend class Timer;
    static TimerHandler *create(Timer *);
};

/**
 * 基于帧循环的定时器管理工具
 */
class Timer
{
private:
    std::vector<TimerHandler *> _handlers;
    int _currFrame;
    float _curretTime;
    float _clearDelta;

    Timer();

    template <typename O>
    TimerHandler *_create(O *obj, void (O::*func)(float dt))
    {
        TimerHandler *handler = NULL;
        for (int i = 0; i < _handlers.size(); ++i)
        {
            TimerHandler *handler2 = _handlers[i];
            if (handler2->has() && handler2->equals<O>(obj, func))
            {
                handler = handler2;
                break;
            }
        }
        if (!handler)
        {
            handler = TimerHandler::create(this);
            handler->setFunction<O>(obj, func);
            _handlers.push_back(handler);
        }
        return handler;
    }

    template <typename O>
    TimerHandler *_createFrame(TIMER_UPDATE_STAGE updateStage, bool once, int delay, O *obj, void (O::*func)(float dt))
    {
        TimerHandler *handler = _create<O>(obj, func);
        handler->useFrame = true;
        handler->once = once;
        handler->updateStage = updateStage;
        handler->delayFrame = delay;
        handler->exeFrame = _currFrame + delay;
        return handler;
    }

    template <typename O>
    TimerHandler *_createTime(TIMER_UPDATE_STAGE updateStage, bool once, float delay, O *obj, void (O::*func)(float dt))
    {
        TimerHandler *handler = _create<O>(obj, func);
        handler->useFrame = false;
        handler->once = once;
        handler->updateStage = updateStage;
        handler->delayTime = delay;
        handler->exeTime = _curretTime + delay;
        return handler;
    }

protected:
    void process(float dt, TimerHandler *);

public:
    friend class TimerHandler;
    friend class Application;
    friend class Root;
    
    void update(float dt, TIMER_UPDATE_STAGE updateStage);

    template <typename O>
    void once(float delay, O *obj, void (O::*func)(float dt), TIMER_UPDATE_STAGE updateStage = TIMER_UPDATE_STAGE::UPDATE)
    {
        _createTime<O>(updateStage, true, delay, obj, func);
    }

    template <typename O>
    void loop(float delay, O *obj, void (O::*func)(float dt), TIMER_UPDATE_STAGE updateStage = TIMER_UPDATE_STAGE::UPDATE)
    {
        _createTime(updateStage, false, delay, obj, func);
    }

    template <typename O>
    void frameOnce(int delay, O *obj, void (O::*func)(float dt), TIMER_UPDATE_STAGE updateStage = TIMER_UPDATE_STAGE::UPDATE)
    {
        _createFrame(updateStage, true, delay, obj, func);
    }

    template <typename O>
    void frameLoop(int delay, O *obj, void (O::*func)(float dt), TIMER_UPDATE_STAGE updateStage = TIMER_UPDATE_STAGE::UPDATE)
    {
        _createFrame(updateStage, false, delay, obj, func);
    }

    template <typename O>
    void off(O *obj, void (O::*func)(float dt))
    {
        for (int i = 0; i < _handlers.size(); ++i)
        {
            TimerHandler *handler2 = _handlers[i];
            if (handler2->has() && handler2->equals<O>(obj, func))
            {
                handler2->clear();
                break;
            }
        }
    }
};
#endif