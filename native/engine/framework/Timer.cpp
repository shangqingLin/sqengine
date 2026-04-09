#include "Timer.h"
#include <vector>
#include <cstdio>

static std::vector<TimerHandler *> _poolHandler;

TimerHandler::TimerHandler()
{
    once = false;
    useFrame = false;
    obj = nullptr;
    func = nullptr;
    exeFrame = 0;
    delayFrame = 0;
    exeTime = 0;
    delayTime = 0;
    updateStage = TIMER_UPDATE_STAGE::UPDATE;
}

void TimerHandler::clear()
{
    once = false;
    useFrame = false;
    obj = nullptr;
    func = nullptr;
    exeFrame = 0;
    delayFrame = 0;
    exeTime = 0;
    delayTime = 0;
    updateStage = TIMER_UPDATE_STAGE::UPDATE;
}

bool TimerHandler::has()
{
    return func != nullptr;
}

void TimerHandler::call(float dt)
{
    if (obj && func)
    {
        (static_cast<TimerHandler *>(obj)->*func)(dt); // 调用成员函数
    }
}

TimerHandler *TimerHandler::create(Timer *timer)
{
    TimerHandler *handler = nullptr;
    if (!_poolHandler.empty())
    {
        handler = _poolHandler.back();
        _poolHandler.pop_back();
    }
    if (!handler)
    {
        handler = new TimerHandler();
    }
    return handler;
}

Timer::Timer() : _currFrame(0),
                 _curretTime(0),
                 _clearDelta(0)
{
}

void Timer::process(float dt, TimerHandler *handler)
{
    if (handler->useFrame)
    {
        if (_currFrame >= handler->exeFrame)
        {
            handler->call(dt);
            if (handler->once)
            {
                handler->clear();
            }else{
                handler->exeFrame += handler->delayFrame;
            }
        }
    }
    else
    {
        if (_curretTime >= handler->exeTime)
        {
            handler->call(dt);
            if (handler->once)
            {
                handler->clear();
            }else{
                handler->exeTime += handler->delayTime;
            }
        }
    }
}

void Timer::update(float dt, TIMER_UPDATE_STAGE updateStage)
{
    ++_currFrame;
    _curretTime += dt;

    TimerHandler *handler;

    for (int i = 0, n = _handlers.size(); i < n; ++i)
    {
        handler = _handlers[i];
        if (handler->has() && handler->updateStage == updateStage)
        {
            process(dt, handler);
        }
    }
    _clearDelta += dt;

    if (updateStage == TIMER_UPDATE_STAGE::POST_UPDATE && _clearDelta >= 500)
    {
        _clearDelta = 0;
        for (int i = _handlers.size() - 1; i >= 0; --i)
        {
            handler = _handlers[i];
            if (!handler->has())
            {
                _handlers.erase(_handlers.begin() + i);
                _poolHandler.push_back(handler);
            }
        }
    }
}