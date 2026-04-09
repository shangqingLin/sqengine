#pragma once

namespace Bit
{

    /**
     * 正确地将unsigned in的位模式存储到浮点数中
     * 不能这样哦：
     *  1、float a = float(unsigned int);
     *  2、float b = static_cast<float>(unsigned int);
     * 上面的两种方式只是数据转换而已，会改变本来的位模式的
     */
    float uintToFloatBits(unsigned int u);
    unsigned int floatToUintBits(float f);

} // namespace  Bit
