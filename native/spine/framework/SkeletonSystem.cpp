#include "SkeletonSystem.h"
#include "SpineComponent.h"
#include "../../engine/scene/NodeBridge.h"
static SkeletonSystem *skSystem = nullptr;
SkeletonSystem *SkeletonSystem::getInstance()
{
    return skSystem;
}

SkeletonSystem::SkeletonSystem()
{
    skSystem = this;
    bridge::NodeBridge::getInstance()->registerNodeProcessComponent(bridge::ComponentType::SpineComponent, processSpineComponent);
}

void SkeletonSystem::addComponent(SpineComponent *c)
{
    components.push_back(c);
}

void SkeletonSystem::removeComponent(SpineComponent *c)
{
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i] == c)
        {
            components.erase(components.begin() + i);
            break;
        }
    }
}

void SkeletonSystem::update(float dt)
{
    float sdt = dt *  0.001;
    for (int i = 0; i < components.size(); ++i)
    {
        components[i]->update(sdt);
    }
}