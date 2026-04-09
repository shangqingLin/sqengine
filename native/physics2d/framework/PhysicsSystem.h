#pragma once

#include "../../engine/framework/System.h"
#include "PhysicsWorld.h"

namespace physics2d
{
    class PhysicsSystem : public System
    {
    private:
        PhysicsWorld *world;
        int unitsPerMeter = 1;

    public:
        PhysicsSystem();
        ~PhysicsSystem();
        static PhysicsSystem *getInstance();
        PhysicsWorld *getWorld() { return world; };
        virtual void init();
        virtual void startUpdate();
        virtual void update(float dt);
        virtual void postUpdate();
        void setDebugDraw(bool enable, Node *);
        void SetDrawFlag(unsigned int flag);
        void setUnitsPerMeter(int pixleNum);
        inline int getUnitsPerMeter() { return unitsPerMeter; };
        void enableNextStep(bool b);
        void nextStep();
    };
}
