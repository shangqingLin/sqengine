#pragma once
#include "../../core/math/Size.h"
#include "../../core/math/Vec2.h"
#include "../../../bindings/define.h"
#include "../../framework/component/RenderComponent.h"
#include "../../rendering/stencil-define.h"

class UIContentComponent : public RenderComponent
{
protected:
   // 这样封装方便js端直接读取此内存地址，
   // 而不去计算UIContentComponent实例中各个属性偏移,因为那样容易出错，还可能有平台和Emscripten兼容性问题
   struct
   {

      /**
       * 锚点
       * 默认所有的UI锚点都在中心点。
       * (0,0)点在左下角
       * (0,1)点在左上角
       */
      float ax{0.5f};
      float ay{0.5f};
      float px{0.f}, py{0.f},
          width{0.f}, height{0.f};

   } props; // js端直接读取此属性地址

   unsigned int colorNum;

   /**
    * 是否启动模板测试。那么接下来所有的子节点都会执行模板测试
    */
   pipeline::StencilStage stencilStage;

   virtual bool checkHit(Vec2 &localPoint);
   virtual bool validHit();
   float getAnchorXInPix();
   float getAnchorYInPix();

public:
   UIContentComponent();
   virtual bool canRender();
   void setPivotX(float pivotX);
   void setPivotY(float pivotY);
   void setAnchorX(float x);
   inline float getAnchorX() { return props.ax; };
   void setAnchorY(float y);
   inline float getAnchorY() { return props.ay; };
   void setAnchor(float x, float y);
   void setWidth(float width);
   inline float getWidth() { return props.width; };
   void setHeight(float height);
   inline float getHeight() { return props.height; };
   float getOffsetX();
   float getOffsetY();
   bool hitTest(Event &event);
   virtual void setDirty();
   virtual void setColor(Color &);
   unsigned int getColorNum();
   inline void setStencil(pipeline::StencilStage s) { stencilStage = s; };
   inline pipeline::StencilStage getStencil() { return stencilStage; };
   inline void *getPropsAdress() { return &props; };
};

void processUIContentComponent(UIContentComponent *, bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
void processUIContentComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
