#ifndef GFX_DEVOCE_CAPS_H_
#define GFX_DEVOCE_CAPS_H_
#include "./define.h"

/**
 * 定义当前平台OpenGL一些特性的支持情况
 */
class DeviceCaps
{

public:
  virtual ~DeviceCaps() {};

  /**
   * 支持的纹理单元个数
   */
  int maxTextureUnits = 10;

  /**
   * 是否启用VAO
   */
  bool enableVAO = true;

  /**
   * 是否支持浮点数纹理
   */
  bool supportFloatTexture = false;

  /**
   * 顶点着色器是否支持读取纹理
   */
  bool supportVertexShaderTexture = false;

  // 在着色器绑定到Program之后，为了节省内存，我们一般都删除着色器对象的
  // 但是一些比较旧的IOS设备、或由于微信小游戏不能正确实现detachShader等这些情况下，我们都不能删除着色器对象
  bool destroyShadersImmediately = true;
  
  /**
   * 是否支持指定的纹理格式
   */
  virtual bool supperFormatTexture(Format format) = 0;

  /**
   * 一个顶点缓冲区支持的最大顶点数目
   */
  inline int getMaxVBOVertices()
  {
    // OpenGL没有API获取一个缓冲区最大容量，所以这个数值为经验所得
    return 65535;
  }
};
#endif