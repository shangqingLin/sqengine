#include "Tween.h"
#include "TweenSystem.h"
#include "../2d/components/Transform2DComponent.h"
#include "./tween-ease/define.h"

Tween::Tween()
{
    state = Tween::STOP;
}

TweenDefine::~TweenDefine()
{
    if (propertyTargetValue)
    {
        delete[] propertyTargetValue;
        propertyTargetValue = nullptr;
    }
}

void Tween::setDefine(TweenDefine &define)
{
    target = define.target;
    duration = define.duration;
    loop = define.loop;
    ease = define.ease;
    eventCall = define.eventCall;
    Transform2DComponent *transform = define.target->getComponent<Transform2DComponent>();
    int index = 0;
    if (define.property & toNumber(TweenPropertyFlag::X))
    {
        properties.emplace_back(TweenProperty(TweenPropertyFlag::X, transform->getX(), define.propertyTargetValue[index++]));
    }

    if (define.property & toNumber(TweenPropertyFlag::Y))
    {
        properties.emplace_back(TweenProperty(TweenPropertyFlag::Y, transform->getX(), define.propertyTargetValue[index++]));
    }

    if (define.property & toNumber(TweenPropertyFlag::SX))
    {
        properties.emplace_back(TweenProperty(TweenPropertyFlag::SX, transform->getX(), define.propertyTargetValue[index++]));
    }

    if (define.property & toNumber(TweenPropertyFlag::SY))
    {
        properties.emplace_back(TweenProperty(TweenPropertyFlag::SY, transform->getX(), define.propertyTargetValue[index++]));
    }

    if (define.property & toNumber(TweenPropertyFlag::RX))
    {
        properties.emplace_back(TweenProperty(TweenPropertyFlag::RX, transform->getX(), define.propertyTargetValue[index++]));
    }

    if (define.property & toNumber(TweenPropertyFlag::RY))
    {
        properties.emplace_back(TweenProperty(TweenPropertyFlag::RY, transform->getX(), define.propertyTargetValue[index++]));
    }

    if (define.property & toNumber(TweenPropertyFlag::ROTATION))
    {
        properties.emplace_back(TweenProperty(TweenPropertyFlag::ROTATION, transform->getX(), define.propertyTargetValue[index++]));
    }
}

void Tween::start()
{
    if (state & Tween::ACTIVE)
        return;
    state |= ~Tween::STOP;
    state |= ~Tween::COMPLETE;
    state |= Tween::ACTIVE;
    TweenSystem::getInstance()->addActive(this);

    if (eventCall & (1 << 2))
    {
        // start事件
        TweenSystem::getInstance()->natvieToJsObject.beginOp(2);
        TweenSystem::getInstance()->natvieToJsObject.writeOpArg(id);
        TweenSystem::getInstance()->natvieToJsObject.endOp();
    }
}

void Tween::stop()
{
    state |= Tween::STOP;
}

void Tween::puase()
{
    state |= Tween::STOP;
}

void Tween::resume()
{
    state |= ~Tween::STOP;
}

void Tween::update(float dt)
{
    if (this->isStop())
        return;

    Transform2DComponent *transform = target->getComponent<Transform2DComponent>();
    timeTrace += dt;
    float time = std::min(timeTrace / duration, 1.f);
    float alpha = easeMap[ease](time);
    for (int i = 0; i < properties.size(); ++i)
    {
        TweenProperty &property = properties[i];
        float value = Math::lerp(property.start, property.end, alpha);
        switch (property.propertyType)
        {
        case TweenPropertyFlag::X:
            transform->setX(value);
            break;
        case TweenPropertyFlag::Y:
            transform->setY(value);
            break;
        case TweenPropertyFlag::SX:
            transform->setScaleX(value);
            break;
        case TweenPropertyFlag::SY:
            transform->setScaleY(value);
            break;    
        case TweenPropertyFlag::RX:
            transform->setRotateX(value);
            break;
        case TweenPropertyFlag::RY:
            transform->setRotateY(value);
            break;
        case TweenPropertyFlag::ROTATION:
            transform->setRotate(value);
            break;
        }
    }

    bridge::NativeToJsObject &natvieToJsObject = TweenSystem::getInstance()->natvieToJsObject;

    if (eventCall & (1 << 3))
    {
        // update事件
        natvieToJsObject.beginOp(3);
        natvieToJsObject.writeOpArg(id);
        natvieToJsObject.writeOpArg(alpha);
        natvieToJsObject.endOp();
    }

    if (alpha == 1.0f)
    {
        if (loop)
        {
            timeTrace = 0.f;
        }
        else
        {
            if (eventCall & (1 << 1))
            {
                // 完成事件
                natvieToJsObject.beginOp(1);
                natvieToJsObject.writeOpArg(id);
                natvieToJsObject.endOp();
            }
        }
    }
}

void Tween::recovery()
{
    state |= Tween::COMPLETE;
    state |= ~Tween::ACTIVE;
    state |= Tween::STOP;
    target = nullptr;
    ease = TweenEase::linear;
    target = nullptr;
    duration = 0.f;
    loop = false;
    timeTrace = 0.f;
    eventCall = 0;
    properties.clear();
}

bool Tween::isComplete()
{
    return state & Tween::COMPLETE;
}

bool Tween::isStop()
{
    return state & Tween::STOP;
}