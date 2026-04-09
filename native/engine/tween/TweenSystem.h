#pragma once
#include "../framework/System.h"
#include <vector>
#include <unordered_map>
#include "../../bindings/binding.h"

class Tween;
class TweenSystem : public System
{
private:
     std::vector<Tween*> tweenActives;
     std::unordered_map<int,Tween*> tweenObjMap;
     bridge::NativeToJsObject natvieToJsObject;
public:
    friend class Tween;
    TweenSystem();
    static TweenSystem* getInstance();
    Tween* getOrCreateTween(int id);
    void addActive(Tween*);
    void recoveryTween(Tween*);
    virtual void init();
    virtual void update(float dt);
};
