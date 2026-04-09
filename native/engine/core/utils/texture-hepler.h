#ifndef _UTILS_TEXTURE_HEPLER_H_
#define _UTILS_TEXTURE_HEPLER_H_

namespace utils
{

    /**
     * 计算需要申请 pixlesNum 个像素的纹理需要的长度和宽度。
     * 这里不会出现某个维度特别长，另外一个特别短的情况。即你输入四个像素，不会出现1*4的情况，而是变为2*2
     *
     * @param powerOfTwo 是否需要2的幂次方
     */
    void calculateTextureSize(int pixlesNum, int &width, int &heigt, const bool powerOfTwo);

    /**
     * 宽度满足widthMultiple的倍数
     */
    void calculateTextureSize(int pixlesNum, int &width, int &heigt, int widthMultiple);

    void calculateTextureSize(int pixlesNum, int result[2], int widthMultiple);
    void calculateTextureSize(int pixlesNum, int result[2], const bool powerOfTwo);
}

#endif
