#include "PyhsicsSystemBridge.h"
#include "engine/framework/Application.h"
#include "PhysicsSystem.h"

using namespace bridge;

void physics2d::PyhsicsSystemBridge::processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, ObjectType type)
{
  physics2d::PhysicsSystem *system = Application::getInstance()->getSystem<physics2d::PhysicsSystem>();
  switch (op)
  {
  case 1:
  {
    bool eanble = *buffer.popp<bool>();
    Node *debugNode = nullptr;
    if (eanble)
    {
      debugNode = JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
    }

    system->setDebugDraw(eanble, debugNode);
    break;
  }
  case 2:
  {
    system->setUnitsPerMeter(*buffer.popp<int>());
    break;
  }
  case 3:
  {
    system->enableNextStep(*buffer.popp<char>());
    break;
  }
  case 4:
  {
    system->nextStep();
    break;
  }
  case 5:
  {
    system->SetDrawFlag(*buffer.popp<unsigned int>());
    break;
  }
  }
}