#ifndef _CAMERA_COMPONENT_H_
#define _CAMERA_COMPONENT_H_

#include "./Component.h"
#include "../../scene/Camera.h"
#include "../../core/math/Rect.h"
#include "../../../bindings/binding.h"
#include "../../assets/RenderTexture.h"

void processCameraCopmonent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

class CameraComponent : public Component
{
private:
  Camera camera;
  RenderTexture *renderTexture = NULL;
  bridge::NativeToJsObject nativeToJs;

public:
  friend void processCameraCopmonent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

  CameraComponent();

  virtual void onEnable();
  virtual void onDisable();

  inline void setEnable(bool enable)
  {
    if (enable)
      onEnable();
    else
      onDisable();
  };
  inline void setProjection(CameraProjection projection) { camera.setProjection(projection); };
  inline float getNearClip() { return camera.getNearClip(); };
  inline void setNearClip(float v) { camera.setNearClip(v); };

  inline void setFarClip(float v) { camera.setFarClip(v); };
  inline float getFarClip() { return camera.getFarClip(); };

  inline void setFov(float v) { camera.setFov(v); };
  inline float getFov() { return camera.getFov(); };

  void setAspect(float a) { camera.setAspect(a); };
  inline float getAspect() { return camera.getAspect(); };

  inline ClearFlags getClearFlag() { return camera.getClearFlag(); };
  inline void setClearFlag(ClearFlags flag) { camera.setClearFlag(flag); };

  void setClearColor(float r, float g, float b, float a);
  inline const Color getClearColor() { return camera.getClearColor(); };

  inline const float getClearDepth() { return camera.getClearDepth(); };
  inline void setClearDepth(float depth) { camera.setClearDepth(depth); };

  inline int getClearStencil() { return camera.getClearStencil(); };
  inline void setClearStencil(int stencil) { camera.setClearStencil(stencil); };

  inline void setOrthoHeight(float orthoHeight) { camera.setOrthoHeight(orthoHeight); };
  inline float getOrthoHeight() { return camera.getOrthoHeight(); };
  
  inline void setPriority(int priority) { camera.setPriority(priority); };
  inline int getPriority() { return camera.getPriority(); };

  void setViewport(Rect<float> &);
  void setClearColor(Color &);

  void screenToWorld(Vec2 &screenPos, Vec3 &out);
  void setRenderTexture(RenderTexture *);
  inline const RenderTexture *getRenderTexture() { return renderTexture; }
};

#endif