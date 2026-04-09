#include "TweenSystem.h"
#include "TweenObjectBridge.h"
#include "Tween.h"
#include "../core/core.h"

static TweenSystem *_inst = nullptr;

TweenSystem *TweenSystem::getInstance()
{
    return _inst;
}

TweenSystem::TweenSystem() : natvieToJsObject(bridge::NativeObjectType::tween)
{
    _inst = this;
}

void TweenSystem::init()
{
    bridge::DispatchManager::getInstance()->registerObjectBridge(bridge::ObjectType::Tween, new TweenObjectBridge);
}

Tween *TweenSystem::getOrCreateTween(int id)
{
    std::unordered_map<int, Tween *>::iterator it = tweenObjMap.find(id);
    if (it != tweenObjMap.end())
    {
        SQ_ASSERT(it->second->isComplete());
        return it->second;
    }

    Tween *tween = new Tween();
    tween->id = id;
    tweenObjMap[id] = tween;
    return tween;
}

void TweenSystem::addActive(Tween *tween)
{
    tweenActives.push_back(tween);
}

void TweenSystem::update(float dt)
{
    for (int i = 0; i < tweenActives.size(); ++i)
    {
        tweenActives[i]->update(dt);
    }
    for (int i = tweenActives.size() - 1; i >= 0; --i)
    {
        if (tweenActives[i]->isComplete())
        {
            tweenActives.erase(tweenActives.begin() + i);
        }
    }
}

void TweenSystem::recoveryTween(Tween *tween)
{
    tween->recovery();
}