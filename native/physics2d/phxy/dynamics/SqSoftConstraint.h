#pragma once

/**
 * 让穿透的碰撞点使用弹簧系统恢复到接触面
 * 使用弹簧系统使得恢复运动很柔和，效果很好
 */

namespace phxy
{

    class SqSoftConstraint
    {

    public:
        float biasRate;
        float massScale;
        float impulseScale;
        SqSoftConstraint();
        void step(float hertz,float zeta,float dt);
    };
}