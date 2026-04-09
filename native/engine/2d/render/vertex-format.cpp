#include "vertex-format.h"

AttributeCustomSetting aPos = {
    0,
    2,
    24,
    Format::RG32F
};

AttributeCustomSetting aUv = {
    8,
    2,
    24,
    Format::RG32F
};

AttributeCustomSetting aColor = {
    16,
    4,
    24,
    Format::RGBA8UI
};

AttributeCustomSetting aTextureIndex = {
    20,
    1,
    24,
    Format::R32F
};


std::vector<Attribute> vfmtPosUvColor = {
    {"aPos", Format::RG32F,aPos},        // pos 8
    {"aUv", Format::RG32F,aUv},         // uv 8
    {"aColor", Format::RGBA8UI,aColor},    // 使用一个数字来存储RGBA四分量颜色值 color 4
    {"aTextureIndex", Format::R32F,aTextureIndex} // textureIndex 4
};

AttributeCustomSetting GaPos = {
    0,
    2,
    12,
    Format::R32F
};

AttributeCustomSetting GaColor = {
    8,
    4,
    12,
    Format::R8
};

std::vector<Attribute> vfmtPosColor = {
    {
        "aPos", 
        Format::RG32F,
        GaPos
    }, // pos 8
    {
        "aColor", 
        Format::RGBA8UI,
        GaColor
    } // 使用一个数字来存储RGBA四分量颜色值 color 4
};