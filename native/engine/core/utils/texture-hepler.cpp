#include "texture-hepler.h"
#include <cmath>
#include <stdio.h>

// 计算大于等于n的最小2的幂次方
static int nextPowerOfTwo(const int n)
{
    return std::pow(2, std::ceil(std::log2(n)));
}

static void calculateTextureSizePowOfTwoOfArithmetic(const int pixelsNum, int &width, int &height)
{
    // 计算最小的总面积
    int totalPixels = nextPowerOfTwo(pixelsNum);

    // 初始宽度为2的幂次方，从sqrt(totalPixels)向上取幂
    width = nextPowerOfTwo(std::sqrt(totalPixels));
    height = totalPixels / width;

    // 确保高度也是2的幂次方
    height = nextPowerOfTwo(height);
}

// 判断一个数是否是2的幂次方
static bool isPowerOfTwo(int x)
{
    return (x > 0) && ((x & (x - 1)) == 0);
}

static void calculateTextureSizePowOfTwoOfWhile(int n, int &width, int &height)
{
    width = 1; // 从最小的2的幂次方开始
    while (width < n)
    {
        int height = (n + width - 1) / width; // 向上取整
        // 将高度调整为2的幂次方
        height = std::pow(2, std::ceil(std::log2(height)));
        if (isPowerOfTwo(width) && isPowerOfTwo(height))
        {
            if (width * height >= n)
            {
                return;
            }
        }
        width *= 2; // 增加宽度
    }
    height = 1; // 单行纹理（特例）
}

static void calculateTextureSizePowOfTwo(const int pixelsNum, int &width, int &height)
{

    /**
     * 使用while循环的版本，对于输入规模pixlesNum比较小的话，可以使用这个，因为没有很多的算术运算
     * 对于pixlesNum比较大的话，使用算术运算的效率比较高一点
     */
    if (pixelsNum <= 1000)
    {
        calculateTextureSizePowOfTwoOfWhile(pixelsNum, width, height);
    }
    else
    {
        calculateTextureSizePowOfTwoOfArithmetic(pixelsNum, width, height);
    }
}

static void calculateTextureSizeNotPowOfTwo(const int pixelsNum, int &width, int &height)
{
    // 初始宽度
    width = std::sqrt(pixelsNum);

    // 对应的高度
    height = (pixelsNum + width - 1) / width; // 等价于 ceil(pixelsNum / width)

    // 要存下pixlesNum个像素，那么width * height的面积必须大于等于pixlesNum，并且是最小的
    //  检查是否满足条件
    if (width * height < pixelsNum)
    {
        // 宽度不足时直接增加1，重新计算高度
        width++;
        height = (pixelsNum + width - 1) / width;
    }
}

void utils::calculateTextureSize(int numPixels, int &width, int &heigt, const bool powerOfTwo)
{
    if (powerOfTwo)
    {
        calculateTextureSizePowOfTwo(numPixels, width, heigt);
    }
    else
    {
        calculateTextureSizeNotPowOfTwo(numPixels, width, heigt);
    }
}

void utils::calculateTextureSize(int pixlesNum, int &width, int &height, int widthMultiple)
{
    width = std::sqrt(pixlesNum);
    int left = width % widthMultiple;
    width += left;
    height = (pixlesNum + width - 1) / width;
    if (width * height < pixlesNum)
    {
        ++height;
    }
}

void utils::calculateTextureSize(int pixlesNum, int result[2], int widthMultiple)
{
    utils::calculateTextureSize(pixlesNum, result[0], result[1], widthMultiple);
}

void utils::calculateTextureSize(int pixlesNum, int result[2], const bool powerOfTwo)
{
    utils::calculateTextureSize(pixlesNum, result[0], result[1], powerOfTwo);
}