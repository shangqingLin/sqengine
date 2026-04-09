#include "Scene.h"
#include "../framework/Application.h"
#include "../framework/component/Component.h"

Scene::Scene()
{
  _activeInHierarchy = false;
  scene = this;
}

void Scene::active(bool b)
{
  if (_activeInHierarchy == b)
    return;
  _activeInHierarchy = b;
  _onHierarchyChanged();
}

void Scene::onSetParent(Node *parent)
{
  // if (this->parent == parent)
  //   return;
  // this->parent = parent;
  // if (this->parent && this->parent->scene && (!this->scene || this->scene != this->parent->scene))
  // {
  //   walk(upateFromParent);
  // }
}

void Scene::_onHierarchyChanged()
{
  for (int i = 0; i < components.size(); ++i)
  {
    Component *ptr = components[i];
    if (_activeInHierarchy)
    {
      ptr->onEnable();
    }
    else
    {
      ptr->onDisable();
    }
  }
  for (int j = 0; j < children.size(); ++j)
  {
    children[j]->_onHierarchyChanged();
  }
}

void Scene::addCamera(Camera *camera)
{
  for (int i = 0; i < cameras.size(); ++i)
  {
    if (cameras[i] == camera)
    {
      return;
    }
  }
  cameras.push_back(camera);
}

void Scene::removeCamera(Camera *camera)
{
  for (int i = 0; i < cameras.size(); ++i)
  {
    if (cameras[i] == camera)
    {
      cameras.erase(cameras.begin() + i);
      return;
    }
  }
}

void Scene::removeAllCamera()
{
  cameras.clear();
}

Scene::~Scene() {}