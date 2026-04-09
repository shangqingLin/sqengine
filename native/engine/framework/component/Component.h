#pragma once
#include "../../scene/Node.h"

/**
 * 节点组件基类
 */
class Component
{
protected:
   int state = 0;

public:
   static const int START = 1;
   static const int ENABLE = 1 << 1;
   static const int DISABLE = 1 << 2;

   friend class NodeTreeManager;
   friend class Node;

   Node *node = NULL;

   // 为了完成多态，基类必须有一个虚函数
   virtual ~Component()
   {
      node = nullptr;
   };

   inline int getState() { return state; };

   /**
    * 创建Component的时候执行
    * 整个生命周期调用一次
    */
   virtual void onAwake() {};

   /**
    * 帧循环中执行
    *
    */
   virtual void onEnable() {

   };

   /**
    * 帧循环中执行
    * 首次Enable执行，在onEnable之后执行。
    * 整个生命周期调用一次
    */
   virtual void onStart() {};

   /**
    * 帧循环中执行
    */
   virtual void onDisable() {};

   virtual void onUpdate() {};

   virtual void onDestroy() {};
};
