#include "Color.h"
#include <stdio.h>
#include "../common/OS.h"

Color::Color(float r, float g, float b, float a) : r(r),
                                                   g(g),
                                                   b(b),
                                                   a(a)
{
}
Color::Color() {}

Color::Color(float r, float g, float b) : r(r),
                                          g(g),
                                          b(b),
                                          a(1.f)
{
}

void Color::set(float r, float g, float b, float a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void Color::fromHEX(unsigned int hex)
{
    if (hex > 0xFFFFFF)
    { // 0xRRGGBBAA 包含透明度 ，例如：#BE7A1F80，透明度为0x80

        r = ((hex >> 24) & 0xFF) / 255.f;
        g = ((hex >> 16) & 0xFF) / 255.f;
        b = ((hex >> 8) & 0xFF) / 255.f;
        a = (hex & 0xFF) / 255.f;
    }
    else
    { // 0xRRGGBB 不包含透明度，默认为透明度为1
        r = ((hex >> 16) & 0xFF) / 255.f;
        g = ((hex >> 8) & 0xFF) / 255.f;
        b = (hex & 0xFF) / 255.f;
        a = 1.0f;
    }
}

bool Color::operator==(const Color &eColor) const
{
    return r == eColor.r && g == eColor.g && b == eColor.b && a == eColor.a;
}

unsigned int Color::combineToNum(const Color &color)
{

    unsigned int colorNum = 0;
    unsigned int tempColorNum = 0;

    // GPU中分配颜色是顺序是rgba,存储到VBO的时候需要注意字节序，低位地址应该存储高位字节
    if (isLittleEndian())
    {
        tempColorNum = (unsigned int)(color.a * 255);
        colorNum |= tempColorNum << 24;

        tempColorNum = (unsigned int)(color.b * 255);
        colorNum |= tempColorNum << 16;

        tempColorNum = (unsigned int)(color.g * 255);
        colorNum |= tempColorNum << 8;

        colorNum |= (unsigned int)(color.r * 255);
    }
    else
    {
        // 32 8,8,8,8
        tempColorNum = (unsigned int)(color.r * 255);
        colorNum |= tempColorNum << 24;

        tempColorNum = (unsigned int)(color.g * 255);
        colorNum |= tempColorNum << 16;

        tempColorNum = (unsigned int)(color.b * 255);
        colorNum |= tempColorNum << 8;

        colorNum |= (unsigned int)(color.a * 255);
    }

    return colorNum;
}