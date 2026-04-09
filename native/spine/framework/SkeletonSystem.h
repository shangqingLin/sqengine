#ifndef _SPINE_SKELETON_SYSTEM_H_
#define _SPINE_SKELETON_SYSTEM_H_
#include <vector>
#include "../../engine/framework/System.h"

class SpineComponent;
class SkeletonSystem  : public System {
    private:
        std::vector<SpineComponent*> components;
    public:
        SkeletonSystem();
        static SkeletonSystem* getInstance();
        void addComponent(SpineComponent*);
        void removeComponent(SpineComponent*);
        virtual void update(float dt);
};

#endif