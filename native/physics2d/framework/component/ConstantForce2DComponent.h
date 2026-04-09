#pragma once

#include "../../../engine/framework/component/Component.h"

namespace physics2d
{

    /**
     * 当我们需要为Body施加力或力矩时，每帧物理模拟完毕之后这些力和力矩都会在物理引擎内清空的，每帧都清空
     * 所以，我们如果需要恒定地对刚体施加力或力矩，我们就可以使用此组件
     */
    class ConstantForce2DComponent : Component
    {
    private:
    public:
    };

}