#include "CameraComponent.h"
#include "../../assets/AssetManager.h"
#include "../../scene/Layers.h"
#include "../../scene/Scene.h"

CameraComponent::CameraComponent()
{
	camera.changeTargetWindow(NULL);
}

void CameraComponent::onEnable()
{
	camera.enabled = true;
	// printf("_______________CameraComponent onEnable \n");
	camera.setNode(node);
	node->getScene()->addCamera(&camera);
}

void CameraComponent::onDisable()
{
	if (node->getScene())
	{
		node->getScene()->removeCamera(&camera);
	}
	camera.enabled = false;
}

void CameraComponent::setViewport(Rect<float> &rect)
{
	camera.setViewport(rect);
}

void CameraComponent::setClearColor(Color &color)
{
	camera.setClearColor(color);
}

void CameraComponent::screenToWorld(Vec2 &screenPos, Vec3 &out)
{
	camera.screenToWorld(screenPos, out);
}

void CameraComponent::setRenderTexture(RenderTexture *texture)
{
	renderTexture = texture;
	camera.changeTargetWindow(renderTexture->getRenderWindow());
}

void processCameraCopmonent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
	if (nodeOp == 1)
	{
		node->addComponent<CameraComponent>();
		return;
	}
	else if (nodeOp == 2)
	{
		node->removeComponent<CameraComponent>();
		return;
	}

	CameraComponent *component = node->getComponent<CameraComponent>();
	switch (op)
	{
	case 1:
		component->setFov(*buffer.popp<float>());
		break;
	case 2:
		component->setNearClip(*buffer.popp<float>());
		break;
	case 3:
		component->setFarClip(*buffer.popp<float>());
		break;
	case 4:
	{
		float &f = *buffer.popp<float>();
		component->setAspect(f);
		break;
	}
	case 5:
		component->setProjection(CameraProjection(*buffer.popp<char>()));
		break;
	case 6:
	{
		Rect<float> rect;
		rect.x = *buffer.popp<float>();
		rect.y = *buffer.popp<float>();
		rect.width = *buffer.popp<float>();
		rect.height = *buffer.popp<float>();
		component->setViewport(rect);
		break;
	}
	case 7:
		component->setClearFlag(ClearFlags(*buffer.popp<int>()));
		break;
	case 8:
	{
		Color color;
		color.r = *buffer.popp<float>();
		color.g = *buffer.popp<float>();
		color.b = *buffer.popp<float>();
		color.a = *buffer.popp<float>();
		component->setClearColor(color);
		break;
	}
	case 9:
	{
		component->setClearDepth(*buffer.popp<float>());
		break;
	}
	case 10:
		component->setClearStencil(*buffer.popp<int>());
		break;
	case 11:
		component->setOrthoHeight(*buffer.popp<float>());
		break;
	case 13:
	{
		float &x = *buffer.popp<float>();
		float &y = *buffer.popp<float>();
		Vec3 out;
		Vec2 screenPoint(x, y);
		component->screenToWorld(screenPoint, out);
		component->nativeToJs.beginOpync();
		component->nativeToJs.writeOpArg(out.x);
		component->nativeToJs.writeOpArg(out.y);
		component->nativeToJs.endOpSync();
		break;
	}
	case 14:
	{
		int &assetId = *buffer.popp<int>();
		RenderTexture *texture = static_cast<RenderTexture *>(AssetManager::getInstance()->findById(assetId));
		component->setRenderTexture(texture);
		break;
	}
	case 15:
		component->setEnable(*buffer.popp<char>());
		break;
	case 16:
	{
		int &priority = *buffer.popp<int>();
		component->setPriority(priority);
		break;
	}
	case 17:
	{
		Mat4 &mat = component->camera.getViewProjMat();
		component->nativeToJs.beginOpync();
		for (int i = 0; i < 16; ++i)
		{
			component->nativeToJs.writeOpArg(mat.data[i]);
		}
		component->nativeToJs.endOpSync();
	}
	default:
		SQ_ASSERT(false);
	}
}
