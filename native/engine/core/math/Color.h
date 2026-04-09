#ifndef _CORE_COLOR_H_
#define _CORE_COLOR_H_

class Color
{
public:
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 0;
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);
    Color();
    void set(float r, float g, float b, float a);
    void fromHEX(unsigned int hex);

    /**
     * 将颜色值合并为一个无符号int数值，方便着色器中使用
     */
    static unsigned int combineToNum(const Color &);

    bool operator==(const Color &) const;
};

#endif