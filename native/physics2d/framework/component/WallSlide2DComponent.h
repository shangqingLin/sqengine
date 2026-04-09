#pragma once
#include "../../../engine/framework/component/Component.h"
#include "Collision2DComponent.h"

/**
 * 让一个Body固定贴着墙壁运动
 *
 */

namespace physics2d
{
    class WallSlide2DComponent : public Component
    {
    private:
        
        bool hasStopSlopEvent = false;

        //移动速度。单位：米/每秒
        float moveSpeed = 1.0f;
        char moveDirection = 0;

        /**
         * 在这个坡度范围内能够自动移动上去
         * 超出这个范围之外的坡度不能会自动停止
         * 设置的范围是[0,90]度
         */
        float slopMin = 0.f;
        float slopMax = 90.f;
        Collision2DComponent *collisionShape;
        void checkCollisionShape();
    protected:
        virtual void onInitialize();
        void onPostContact(const phxy::SqContact *const, const phxy::SqManifold *const manifold);
    public:
        WallSlide2DComponent();
        virtual ~WallSlide2DComponent();
        void moveForward();
        void moveBack();
        void stop();
        void setSpeed(float speed);
        void setSlop(float min,float max);
        void setHasEvent(bool b);
    };

}