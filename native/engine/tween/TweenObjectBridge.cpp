#include "TweenObjectBridge.h"
#include "TweenSystem.h"
#include "Tween.h"

void TweenObjectBridge::processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, bridge::ObjectType type)
{
    switch (op)
    {
    case 1:
    {
        int id = *buffer.popp<int>();
        TweenDefine define;
        define.property = *buffer.popp<unsigned int>();
        int numProertyTargetValue = 0;
        for(int i = 1; i <= 7 ; ++i){
            if(define.property & ( 1 << i )){
                ++numProertyTargetValue;
            }
        }
        
        SQ_ASSERT(numProertyTargetValue > 0);
        define.propertyTargetValue = new float(numProertyTargetValue);
        for(int i = 0; i < numProertyTargetValue ; ++i){
            define.propertyTargetValue[i] = *buffer.popp<float>();
        }

        define.target = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
        define.ease = TweenEase(*buffer.popp<char>());
        define.duration = *buffer.popp<float>();
        define.eventCall = *buffer.popp<unsigned char>();
        Tween *tween = TweenSystem::getInstance()->getOrCreateTween(id);
        tween->setDefine(define);
        if (*buffer.popp<char>())
        {
            tween->start();
        }
    }
    break;

    case 2:
    {
        Tween *tween = TweenSystem::getInstance()->getOrCreateTween(*buffer.popp<int>());
        tween->start();
        break;
    }
    case 3:
    {
        Tween *tween = TweenSystem::getInstance()->getOrCreateTween(*buffer.popp<int>());
        tween->stop();
        break;
    }
    case 4:
    {
        Tween *tween = TweenSystem::getInstance()->getOrCreateTween(*buffer.popp<int>());
        tween->puase();
        break;
    }
    case 5:
    {
        Tween *tween = TweenSystem::getInstance()->getOrCreateTween(*buffer.popp<int>());
        tween->resume();
        break;
    }
    default:
        break;
    }
}