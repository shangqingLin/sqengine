#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "../scene/Pass.h"

#include "EffectAsset.h"
#include "Asset.h"
#include "../scene/PassUtils.h"
#include "TextureBase.h"

class IMaterialInfo
{
public:
  std::vector<PassStates *> states;
  std::optional<MacroRecord> macroDefines;
  IMaterialInfo();
  ~IMaterialInfo();

  // 与js端的交互接口
  void pushState(PassStates *);
  void pushMacro(char *name, int); // int代表了数字、boolean
};

class Material : public Asset
{
protected:
  /**
   * 材质可以设置渲染管线状态，这个会覆盖Pass上设置的状态
   * 一个Pass对应一个IPassStates，按照Pass在Effect中顺序来
   */
  std::vector<PassStates *> states;
  std::vector<MacroRecord> marcroDefines;
  std::vector<Pass *> passes;
  EffectAsset *effect;

  void createPass();
  const UniformSamplerTexture *findSamperProperty(std::string name, int);
  const Uniform *findProperty(std::string, int);

public:
  Material(int id);
  ~Material();
  void setEffectAsset(EffectAsset *effect);
  EffectAsset *getEffectAsset() { return effect; };
  void initialize(IMaterialInfo *);

  inline const std::vector<Pass *> &getPasses() { return passes; };
  
  void setTexture(std::string name, TextureBase *texture, int passIndex = 0);
  void setTexture(std::string name, TextureBase **texture, int textArrayNum, int passIndex = 0);
  void setProperty(std::string name, MaterialProperty value);
  void setProperty(std::string name, MaterialProperty value, int passIndex);

  void setProperty(std::string name, MaterialProperty *values, int valueNum);
  void setProperty(std::string name, MaterialProperty *value, int valueNum, int passIndex);

  // const MaterialProperty getProperty(std::string name);

  // 对Js端提供的接口
  void setPropertyBridgeJsTexture(char *name, int textureAssetId);
  void setPropertyBridgeJsFloat(char *name, float value);
  void setPropertyBridgeJsMat4(char *name, void *);
};

#endif