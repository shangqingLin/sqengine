#include "easing.h"
#include "../../core/core.h"

float constant(float k)
{
    return 0.f;
}

/**
 * @en A linear function, `f(k) = k`. Result correlates to input value one to one.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 线性函数，`f(k) = k`。返回值和输入值一一对应。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float linear(float k)
{
    return k;
}

/**
 * @en A quadratic function, f(k) = k * k. The interpolation starts slowly, then progressively speeds up until the end,
 * at which point it stops abruptly. Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 一个二次方的函数，f(k) = k * k。插值开始时很慢，然后逐渐加快，直到结束，并突然停止。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quadIn(float k)
{
    return k * k;
}

/**
 * @en A quadratic function, f(k) = k * (2-k). The interpolation starts abruptly and then progressively slows down towards the end. Refer to
 * [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 一个二次方的函数，f(k) = k * (2-k)。插值开始时很突然，然后在接近尾声时逐渐减慢。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quadOut(float k)
{
    return k * (2 - k);
}

/**
 * @en The interpolation starts slowly, speeds up, and then slows down towards the end. Refer to
 * [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) graphic feature.
 * @zh 插值开始时很慢，接着加快，然后在接近尾声时减慢。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quadInOut(float k)
{
    k *= 2;
    if (k < 1)
    {
        return 0.5 * k * k;
    }
    k-=1.f;
    return -0.5 * (k * (k - 2) - 1);
}

/**
 * @en Starts slowly and accelerates. Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 启动慢，加速快。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float cubicIn(float k)
{
    return k * k * k;
}

/**
 * @en Starts quickly and decelerates.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 起动迅速，减速慢。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float cubicOut(float k)
{
    k -= 1.0f;
    return k * k * k + 1;
}

/**
 * @en Accelerates the animation at the beginning, and decelerates the animation at the end.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 在开始时加速动画，在结束时减慢动画的速度。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float cubicInOut(float k)
{
    k *= 2;
    if (k < 1)
    {
        return 0.5 * k * k * k;
    }
    k -= 2;
    return 0.5 * (k * k * k + 2);
}

/**
 * @en Starts slowly and accelerates. Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 启动慢，加速快。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quartIn(float k)
{
    return k * k * k * k;
}

/**
 * @en Starts quickly and decelerates.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 起动迅速，减速慢。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quartOut(float k)
{
    k -= 1.f;
    return 1 - (k * k * k * k);
}

/**
 * @en Accelerates the animation at the beginning, and decelerates the animation at the end.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 在开始时加速动画，在结束时减慢动画的速度。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quartInOut(float k)
{
    k *= 2;
    if (k < 1)
    {
        return 0.5 * k * k * k * k;
    }
    k -= 2;
    return -0.5 * (k * k * k * k - 2);
}

/**
 * @en Starts slowly and accelerates. Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 启动慢，加速快。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quintIn(float k)
{
    return k * k * k * k * k;
}

/**
 * @en Starts quickly and decelerates.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 起动迅速，减速慢。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quintOut(float k)
{
    k -= 1.0f;
    return k * k * k * k * k + 1;
}

/**
 * @en Accelerates the animation at the beginning, and decelerates the animation at the end.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 在开始时加速动画，在结束时减慢动画的速度。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quintInOut(float k)
{
    k *= 2;
    if (k < 1)
    {
        return 0.5 * k * k * k * k * k;
    }
    k -= 2;
    return 0.5 * (k * k * k * k * k + 2);
}

/**
 * @en Smoothly accelerates the animation.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 平滑地加速动画。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float sineIn(float k)
{
    if (k == 1)
    {
        return 1;
    }
    return 1 - cos(k * Math::PI / 2);
}

/**
 * @en Smoothly decelerates the animation.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 平滑地使动画降速。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float sineOut(float k)
{
    return sin(k * Math::PI / 2);
}

/**
 * @en Smoothly accelerates the animation at the beginning, and smoothly decelerates the animation at the end.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 在开始时平滑地加速动画，在结束时平滑地减速动画。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float sineInOut(float k)
{
    return 0.5 * (1 - cos(Math::PI * k));
}

/**
 * @en Starts slowly and accelerates. Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 启动慢，加速快。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float expoIn(float k)
{
    return k == 0 ? 0 : pow(1024, k - 1);
}

/**
 * @en Starts quickly and decelerates.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 起动迅速，减速慢。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float expoOut(float k)
{
    return k == 1 ? 1 : 1 - pow(2, -10 * k);
}

/**
 * @en Accelerates the animation at the beginning, and decelerates the animation at the end.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 在开始时加速动画，在结束时减慢动画的速度。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float expoInOut(float k)
{
    if (k == 0)
    {
        return 0;
    }
    if (k == 1)
    {
        return 1;
    }

    k *= 2;
    if (k < 1)
    {
        return 0.5 * pow(1024, k - 1);
    }
    return 0.5 * (-pow(2, -10 * (k - 1)) + 2);
}

/**
 * @en Starts slowly and accelerates. Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 启动慢，加速快。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float circIn(float k)
{
    return 1 - sqrt(1 - k * k);
}

/**
 * @en Starts quickly and decelerates.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 起动迅速，减速慢。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float circOut(float k)
{
    k -= 1.f;
    return sqrt(1 - (k * k));
}

/**
 * @en Accelerates the animation at the beginning, and decelerates the animation at the end.
 * Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 在开始时加速动画，在结束时减慢动画的速度。具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float circInOut(float k)
{
    k *= 2;
    if (k < 1)
    {
        return -0.5 * (sqrt(1 - k * k) - 1);
    }
    k -= 2;
    return 0.5 * (sqrt(1 - k * k) + 1);
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float elasticIn(float k)
{
    float s;
    float a = 0.1f;
    float p = 0.4f;
    if (k == 0.f)
    {
        return 0.f;
    }
    if (k == 1.f)
    {
        return 1.f;
    }
    if (!a || a < 1.f)
    {
        a = 1.f;
        s = p / 4.f;
    }
    else
    {
        s = p * asin(1 / a) / (2 * Math::PI);
    }

    k -= 1;
    return -(a * pow(2, 10 * k) * sin((k - s) * (2 * Math::PI) / p));
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float elasticOut(float k)
{
    float s;
    float a = 0.1f;
    float p = 0.4f;
    if (k == 0.f)
    {
        return 0.f;
    }
    if (k == 1.f)
    {
        return 1.f;
    }
    if (!a || a < 1.f)
    {
        a = 1.f;
        s = p / 4.f;
    }
    else
    {
        s = p * asin(1 / a) / (2 * Math::PI);
    }
    return (a * pow(2, -10 * k) * sin((k - s) * (2 * Math::PI) / p) + 1);
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float elasticInOut(float k)
{
    float s;
    float a = 0.1f;
    float p = 0.4f;
    if (k == 0.f)
    {
        return 0.f;
    }
    if (k == 1.f)
    {
        return 1.f;
    }
    if (!a || a < 1.f)
    {
        a = 1.f;
        s = p / 4.f;
    }
    else
    {
        s = p * asin(1 / a) / (2 * Math::PI);
    }
    k *= 2;
    if (k < 1)
    {
        k -= 1;
        return -0.5 * (a * pow(2, 10 * k) * sin((k - s) * (2 * Math::PI) / p));
    }
    k -= 1;
    return a * pow(2, -10 * k) * sin((k - s) * (2 * Math::PI) / p) * 0.5 + 1;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float backIn(float k)
{
    if (k == 1.f)
    {
        return 1;
    }
    float s = 1.70158f;
    return k * k * ((s + 1) * k - s);
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float backOut(float k)
{
    if (k == 0.f)
    {
        return 0.f;
    }
    float s = 1.70158f;
    k-=1;
    return k * k * ((s + 1) * k + s) + 1;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float backInOut(float k)
{
    float s = 1.70158f * 1.525f;
    k *= 2;
    if (k < 1)
    {
        return 0.5 * (k * k * ((s + 1) * k - s));
    }
    k -= 2;
    return 0.5 * (k * k * ((s + 1) * k + s) + 2);
}

float bounceIn(float k)
{
    return 1 - bounceOut(1 - k);
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float bounceOut(float k)
{
    if (k < (1 / 2.75f))
    {
        return 7.5625f * k * k;
    }
    else if (k < (2 / 2.75f))
    {
        k -= 1.5f / 2.75f;
        return 7.5625f * k * k + 0.75f;
    }
    else if (k < (2.5f / 2.75))
    {
        k -= (2.25f / 2.75f);
        return 7.5625f * k * k + 0.9375f;
    }
    else
    {
        k -= (2.625f / 2.75f);
        return 7.5625f * k * k + 0.984375f;
    }
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float bounceInOut(float k)
{
    if (k < 0.5f)
    {
        return bounceIn(k * 2) * 0.5f;
    }
    return bounceOut(k * 2 - 1) * 0.5f + 0.5f;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float smooth(float k)
{
    if (k <= 0.f)
    {
        return 0.f;
    }
    if (k >= 1.f)
    {
        return 1.f;
    }
    return k * k * (3 - 2 * k);
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float fade(float k)
{
    if (k <= 0.f)
    {
        return 0.f;
    }
    if (k >= 1.f)
    {
        return 1.f;
    }
    return k * k * k * (k * (k * 6 - 15) + 10);
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quadOutIn(float k)
{
    if (k < 0.5f)
    {
        return quadOut(k * 2.f) / 2.f;
    }
    return quadIn(2 * k - 1) / 2.f + 0.5f;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float cubicOutIn(float k)
{
    if (k < 0.5f)
    {
        return cubicOut(k * 2.f) / 2.f;
    }
    return cubicIn(2 * k - 1) / 2.f + 0.5f;
}


/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quartOutIn(float k)
{
    if (k < 0.5f)
    {
        return quartOut(k * 2.f) / 2.f;
    }
    return quartIn(2 * k - 1) / 2.f + 0.5f;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float quintOutIn(float k)
{
    if (k < 0.5f)
    {
        return quintOut(k * 2.f) / 2.f;
    }
    return quintIn(2 * k - 1) / 2.f + 0.5f;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float sineOutIn(float k)
{
    if (k < 0.5f)
    {
        return sineOut(k * 2.f) / 2.f;
    }
    return sineIn(2 * k - 1) / 2.f + 0.5f;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float expoOutIn(float k)
{
    if (k < 0.5f)
    {
        return expoOut(k * 2.f) / 2.f;
    }
    return expoIn(2 * k - 1) / 2.f + 0.5f;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float circOutIn(float k)
{
    if (k < 0.5f)
    {
        return circOut(k * 2.f) / 2.f;
    }
    return circIn(2 * k - 1) / 2.f + 0.5f;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float elasticOutIn(float k)
{
    if (k < 0.5)
    {
        return elasticOut(k * 2) / 2;
    }
    return elasticIn(2 * k - 1) / 2 + 0.5;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float backOutIn(float k)
{
    if (k < 0.5f)
    {
        return backOut(k * 2.f) / 2.f;
    }
    return backIn(2 * k - 1) / 2.f + 0.5f;
}

/**
 * @en Refer to [this doc](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html) for graphic feature.
 * @zh 具体效果可以参考[该文档](https://docs.cocos.com/creator/manual/zh/tween/tween-function.html)。
 */
float bounceOutIn(float k)
{
    if (k < 0.5f)
    {
        return bounceOut(k * 2.f) / 2.f;
    }
    return bounceIn(2 * k - 1) / 2.f + 0.5f;
}
