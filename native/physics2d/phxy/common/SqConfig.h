#pragma once

namespace phxy
{

    class SqConfig
    {

    private:
        /**
         * 物理世界中是以真实的物理世界单位来处理的
         * 但屏幕单位是像素。所以这个参数用于设置多少个像素为1米。单位是： 像素/米
         */
        float lengthUnitsPerMeter = 1.0f;
        float hugValue = 0.f;
        float linearSlop = 0.f;
        float speculative_distance = 0.f;

        /**
         * 用于构建Shape的FatAABB时的margin
         * 即Shape的AABB在计算时会加上这个margin，所以Shape的FatAABB会比实际的AABB大
         */
        float aabb_margin = 0.f;
    public:
        SqConfig();
        static SqConfig* getInstance();
        void setLengthUnitsPerMeter(float lengthUnits);
        float getLengthUnitsPerMeter(void);
        inline float getHugValue() { return hugValue; };
        inline float getLinearSlop() { return linearSlop; };
        inline float getSpeculativeDistance() { return speculative_distance; };
        inline float getAABBMargin() { return aabb_margin; };
    };
}
