#ifndef _SCENE_H_
#define _SCENE_H_

#include "Node.h"
#include "Camera.h"
#include <vector>

class Scene : public Node
{
private:
   std::vector<Camera *> cameras;

protected:
   virtual void _onHierarchyChanged();

   /**
    * 正常情况下Scene是不能添加到子节点中的，所以没必要覆盖这个方法
    * 这都是为了支持编辑器的，在编辑器中Scene支持
    */
   virtual void onSetParent(Node *parent);

public:
   Scene();
   virtual ~Scene();
   void active(bool);

   void addCamera(Camera *);
   void removeCamera(Camera *);
   void removeAllCamera();
   inline const std::vector<Camera *> &getCameras() { return cameras; };
};
#endif
