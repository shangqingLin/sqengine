#include "NodeBridge.h"
#include "../2d/components/sprite/SpriteComponent.h"
#include "../2d/components/Transform2DComponent.h"
#include "../framework/component/CameraComponent.h"
#include "../2d/components/graphics/Graphics.h"
#include "../framework/Application.h"
#include "../2d/components/label/LabelComponent.h"
#include "../2d/components/particle/Particles2DComponent.h"
#include "../framework/component/RenderComponent.h"
#include "PoolManager.h"

using namespace bridge;

static NodeBridge *instance = NULL;

NodeBridge *NodeBridge::getInstance()
{
	return instance;
}
NodeBridge::NodeBridge()
{
	instance = this;
	registerNodeProcessComponent(bridge::ComponentType::Transform2DComponent, &processTransform2DComponent);
	registerNodeProcessComponent(bridge::ComponentType::SpriteComponent, &bindingProcessSpriteComponent);
	registerNodeProcessComponent(bridge::ComponentType::CameraComponent, &processCameraCopmonent);
	registerNodeProcessComponent(bridge::ComponentType::GraphicsComponent, &processGraphicsComponent);
	// registerNodeProcessComponent(bridge::ComponentType::canvas, &processCanvasComponent);
	registerNodeProcessComponent(bridge::ComponentType::UIContentComponent, &processUIContentComponent);
	registerNodeProcessComponent(bridge::ComponentType::LabelComponent, &bindingProcessLabelComponent);
	registerNodeProcessComponent(bridge::ComponentType::Particle2DComponent, &bindingProcessParticles2DComponent);
	registerNodeProcessComponent(bridge::ComponentType::Renderer, &bindingProcessRenderComponent);
}

void NodeBridge::processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, ObjectType type)
{

	unsigned int opp = op & 0xffff;

	// printf("  processDispatch node being %d %d %d \n", nativeId,opp,buffer.getReadPos());

	switch (opp)
	{
	case 1:
	{
		Node *node = NULL;
		if (type == ObjectType::Scene)
		{
			node = new Scene();
		}
		else
		{
			node = new Node(); // PoolManager<Node>::getInstance()->get();
		}

		node->nativeId = nativeId;
		JsToNativeObjectManager::getInstance()->add(node);
		// printf("__________________create node %d %d\n",type,nativeId);
		break;
	}
	case 2:
	{
		int &childNativeId = *buffer.popp<int>();
		Node *child = JsToNativeObjectManager::getInstance()->getById<Node>(childNativeId);
		Node *parent = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		// printf("__________________add node child: %d %p parent: %d %p\n",childNativeId,child,nativeId,parent);
		parent->addChild(child);
		break;
	}
	case 3:
	{
		int &childNativeId = *buffer.popp<int>();
		Node *child = JsToNativeObjectManager::getInstance()->getById<Node>(childNativeId);
		Node *parent = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		parent->removeChild(child);
		break;
	}
	case 4:
	{
		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		bridge::ComponentType type = bridge::ComponentType(*buffer.popp<char>());
		processComponent(type, 1, 0, buffer, node);
		break;
	}
	case 5:
	{

		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		bridge::ComponentType type = bridge::ComponentType(*buffer.popp<char>());
		processComponent(type, 2, 0, buffer, node);
		break;
	}
	case 6:
	{
		char &v = *buffer.popp<char>();
		// printf("====================setVisible %d %d\n",nativeId,v);
		// visible
		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		node->setVisible(v);
		break;
	}
	case 7:
	{
		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		node->setLayer(Layers(*buffer.popp<int>()));
		break;
	}
	case 8:
	{
		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		dispatchProcessComponent(op, buffer, node);
		break;
	}
	case 10:
	{
		Scene *node = JsToNativeObjectManager::getInstance()->getById<Scene>(nativeId);
		// printf("active scene %p \n",node);
		bool active = *buffer.popp<char>();
		if (active)
		{
			Application::getInstance()->setRunScene(node);
		}
		else
		{
			node->active(false);
		}
		break;
	}
	case 11:
	{
		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		node->getEventProcess().onFromJs(NodeEventType(*buffer.popp<int>()));
		break;
	}
	case 12:
	{
		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		node->getEventProcess().offFromJs(NodeEventType(*buffer.popp<int>()));
		break;
	}
	case 13:
	{
		Scene *node = JsToNativeObjectManager::getInstance()->getById<Scene>(nativeId);
		node->active(*buffer.popp<char>());
		break;
	}
	case 14:
	{
		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		Node *insertNode = JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
		int &insertIndex = *buffer.popp<int>();
		node->addChildAt(insertNode, insertIndex);
		break;
	}
	case 100:
	{
		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
		JsToNativeObjectManager::getInstance()->remove(node);
		node->destroy();
		break;
	}
	// case 101:
	// {
	// 	Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nativeId);
	// 	node->setTestNum(*buffer.popp<int>());
	// 	break;
	// }
	default:
		SQ_ASSERT(false);
	}
}

void NodeBridge::dispatchProcessComponent(unsigned int &nodeOp, ArrayBuffer &buffer, Node *node)
{
	unsigned int op1 = nodeOp & 0xff0000;
	op1 = op1 >> 16;
	char op = (char)op1;

	unsigned int typeNum = nodeOp & 0xff000000;
	typeNum = typeNum >> 24;

	bridge::ComponentType type = bridge::ComponentType(typeNum);
	processComponent(type, 3, op, buffer, node);
}

void NodeBridge::registerNodeProcessComponent(bridge::ComponentType type, bridge::NODE_PROCESS_COMPONENT_FUN func)
{

	processComponentMap[type] = func;
}

void NodeBridge::processComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
	SQ_ASSERT(processComponentMap.find(type) != processComponentMap.end());
	// printf("process component %d op %d pos %d \n",type,op,buffer.getCurrentPos());
	processComponentMap[type](type, nodeOp, op, buffer, node);
}

#ifdef WASM
#include <emscripten.h>
// #include "../../physics-2d/index.h"
extern "C"
{
	EMSCRIPTEN_KEEPALIVE char *getNodeComponentPointer(int componentType, int nodeNativeId)
	{
		Node *node = JsToNativeObjectManager::getInstance()->getById<Node>(nodeNativeId);
		bridge::ComponentType type = bridge::ComponentType(componentType);

		char *adress = NULL;

		switch (type)
		{
		case bridge::ComponentType::UIContentComponent:
		{
			UIContentComponent *component = node->getComponent<UIContentComponent>();
			if (component)
			{
				adress = (char *)component->getPropsAdress();
			}
			break;
		}
		case bridge::ComponentType::SpriteComponent:
		{
			SpriteComponent *component = node->getComponent<SpriteComponent>();
			adress = (char *)component->getPropsAdress();
			break;
		}
		case bridge::ComponentType::LabelComponent:
		{
			LabelComponent *component = node->getComponent<LabelComponent>();
			adress = (char *)component->getPropsAdress();
			break;
		}
		case bridge::ComponentType::GraphicsComponent:
		{
			Graphics *component = node->getComponent<Graphics>();
			adress = (char *)component->getPropsAdress();
			break;
		}
		case bridge::ComponentType::Transform2DComponent:
		{
			Transform2DComponent *component = node->getComponent<Transform2DComponent>();
			if (component)
			{
				adress = (char *)component->getPropsAdress();
			}
			break;
		}
		}
		// 在JS需要正确计算偏移量哦
		// 在这里都是4字节对齐的，比如说你的属性声明为bool，但是它还是占用4个字节的
		return adress;
	};
}
#endif
