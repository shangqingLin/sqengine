#include "Graphics.h"
#include "../../assembler/GraphicsAssembler.h"
#include "../../../assets/AssetManager.h"
#include "../../../assets/BuildinResManager.h"
#include "../../../scene/Node.h"

Graphics::Graphics() : UIContentComponent() {}

void Graphics::onStart()
{
	if (!getMaterial())
	{
		setMaterial(BuildinResManager::getMaterial("default-graphics"));
	}
	// printf(">>>>>>>>>>>>>>>>>>>start %p %d \n",node,node->nativeId);
}

bool Graphics::canRender()
{

	// if (node->nativeId == 14)
	// printf("=============== Graphics::canRender %p  %d %d %p \n",node, node->nativeId, paths.size(), getMaterial());

	return paths.size() > 0 && getMaterial() != nullptr;
}

bool Graphics::validHit()
{
	return paths.size() > 0;
}

bool Graphics::checkHit(Vec2 &localPoint)
{
	const Bound2 &b = getBound();
	if (!b.containsPoint(localPoint.x, localPoint.y))
	{
		return false;
	}
	for (int i = 0; i < paths.size(); ++i)
	{

		if (paths[i]->containsPoint(localPoint.x, localPoint.y))
		{
			return true;
		}
	}
	return false;
}

const Bound2 &Graphics::getBound()
{
	if (boundDirty)
	{
		boundDirty = false;
		bound.set(0, 0, 0, 0);
		Bound2 b;
		for (int i = 0; i < paths.size(); ++i)
		{
			paths[i]->getBound(b);
			bound.addBound(b);
		}
	}
	return bound;
}

void Graphics::queryGemoetry(float localX, float localY, d2::QueryShapeResult &result)
{
	for (int i = 0; i < paths.size(); ++i)
	{
		paths[i]->queryGemoetry(localX, localY, result);
		if (result.gemoetryIndex != -1)
		{
			result.pathIndex = i;
			return;
		}
	}
}

void Graphics::beginPath()
{
	if (!activePath)
	{

		if (poolPath.getCount() > 0)
		{
			activePath = *poolPath.pop();
		}
		else
		{
			activePath = new d2::Path();
		}
		paths.push_back(activePath);
		dirty = true;
	}
}

void Graphics::moveTo(float x, float y)
{
	beginPath();
	activePath->moveTo(x, y);
}

void Graphics::lineTo(float x, float y)
{
	activePath->lineTo(x, y);
}

void Graphics::updateLinePoint(int pathIndex, int pointIndex, float x, float y)
{
	d2::Path *path = paths[pathIndex];
	path->updateLinePoint(pointIndex, x, y);
	boundDirty = true;
	dirty = true;
}

void Graphics::insertAfterLinePoint(int pathIndex, int insertAfterpointIndex, float x, float y)
{
	d2::Path *path = paths[pathIndex];
	path->insertAfterLinePoint(insertAfterpointIndex, x, y);
	boundDirty = true;
	dirty = true;
}

void Graphics::deleteLinePoint(int pathIndex, int pointIndex)
{
	d2::Path *path = paths[pathIndex];
	path->deleteLinePoint(pointIndex);
	boundDirty = true;
	dirty = true;
}

void Graphics::drawCircle(float centerX, float centerY, float r)
{
	SQ_ASSERT(r >= 0);
	beginPath();
	activePath->drawCircle(centerX, centerY, r);
}

void Graphics::drawEllipse(float cx, float cy, float rx, float ry)
{
	SQ_ASSERT(rx >= 0 && ry >= 0);
	beginPath();
	activePath->drawEllipse(cx, cy, rx, ry);
}

void Graphics::drawRect(float x, float y, float w, float h)
{
	SQ_ASSERT(w >= 0 && h >= 0);
	beginPath();
	activePath->drawRect(x, y, w, h);
}

void Graphics::drawCapsule(float c1x, float c1y, float c2x, float c2y, float radius)
{
	beginPath();
	Vec2 dist(c1x - c2x, c1y - c2y);
	if (dist.lengthSqr() < 0.001)
	{
		activePath->drawCircle(c1x, c1y, radius);
	}
	else
	{
		activePath->drawCapsule(c1x, c1y, c2x, c2y, radius);
	}
}

void Graphics::fill()
{
	if (activePath)
	{
		activePath->fill();
		drawEnd();
	}
}

void Graphics::stroke()
{
	if (activePath)
	{
		activePath->stroke();
		drawEnd();
	}
}

void Graphics::fill(d2::FillStyle &fillStyle)
{
	if (activePath)
	{
		activePath->fill(fillStyle);
		drawEnd();
	}
}

void Graphics::stroke(d2::StrokeStyle &strokeStyle)
{
	if (activePath)
	{
		activePath->stroke(strokeStyle);
		drawEnd();
	}
}

void Graphics::drawEnd()
{
	activePath = NULL;
	boundDirty = true;
}

void Graphics::close()
{
	activePath->close();
}

void Graphics::clear()
{

	if (poolPath.getCapacity() < paths.size())
	{
		poolPath.resize(paths.size());
	}

	for (int i = 0; i < paths.size(); ++i)
	{
		paths[i]->clear();
		poolPath.push(paths[i]);
	}
	paths.clear();

	if (vertData)
	{
		triangles.clear();
		vertData->recycle();
		vertData = nullptr;
	}
	boundDirty = true;
}

void Graphics::onUpdate()
{
	if (dirty)
	{
		dirty = false;
		GraphicsAssembler::getInstance()->updateRenderData(this);
	}
}

Graphics::~Graphics()
{
	for (int i = 0; i < paths.size(); ++i)
	{
		delete paths[i];
	}
	paths.clear();

	if (vertData)
	{
		vertData->recycle();
		vertData = nullptr;
	}
}

void processGraphicsComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{

	// printf("     processGraphicsComponent %d %d %d \n",op,nodeOp,buffer.getReadPos());

	if (nodeOp == 1)
	{
		node->addComponent<Graphics>();
		return;
	}
	else if (nodeOp == 2)
	{
		node->removeComponent<Graphics>();
		return;
	}

	Graphics *component = node->getComponent<Graphics>();
	SQ_ASSERT(component);
	if (op < 50)
	{
		processUIContentComponent(component, type, nodeOp, op, buffer, node);
		return;
	}
	switch (op)
	{
	case 50:
	{
		float &x = *buffer.popp<float>();
		float &y = *buffer.popp<float>();
		component->moveTo(x, y);
		break;
	}
	case 51:
	{
		float &x = *buffer.popp<float>();
		float &y = *buffer.popp<float>();
		component->lineTo(x, y);
		break;
	}

	case 52:
	{
		int &pathIndex = *buffer.popp<int>();
		int &pointIndex = *buffer.popp<int>();
		float &x = *buffer.popp<float>();
		float &y = *buffer.popp<float>();
		component->updateLinePoint(pathIndex, pointIndex, x, y);
		break;
	}

	case 53:
	{
		char &state = *buffer.popp<char>();
		if (state == 0)
		{
			component->fill();
		}
		else
		{
			d2::FillStyle style;
			if (state & 1)
			{
				Color color(
					*buffer.popp<unsigned char>() / 255.f,
					*buffer.popp<unsigned char>() / 255.f,
					*buffer.popp<unsigned char>() / 255.f,
					*buffer.popp<unsigned char>() / 255.f);
				style.color = Color::combineToNum(color);
			}
			component->fill(style);
		}
		break;
	}
	case 54:
	{
		char &state = *buffer.popp<char>();
		if (state == 0)
		{
			component->stroke();
		}
		else
		{
			d2::StrokeStyle style;
			if (state & 1)
			{
				Color color(
					*buffer.popp<unsigned char>() / 255.f,
					*buffer.popp<unsigned char>() / 255.f,
					*buffer.popp<unsigned char>() / 255.f,
					*buffer.popp<unsigned char>() / 255.f);

				// int r =	*buffer.popp<unsigned char>();
				// int g =	*buffer.popp<unsigned char>();
				// int b =	*buffer.popp<unsigned char>();
				// int a =	*buffer.popp<unsigned char>();
				// printf("Stroke Color %d %d %d %d \n", r, g, b, a);

				style.color = Color::combineToNum(color);
			}
			if (state & 2)
			{
				style.width = *buffer.popp<float>();
			}
			component->stroke(style);
		}
		break;
	}
	case 55:
	{
		component->drawCircle(*buffer.popp<float>(), *buffer.popp<float>(), *buffer.popp<float>());
		break;
	}
	case 56:
	{
		component->drawEllipse(*buffer.popp<float>(), *buffer.popp<float>(), *buffer.popp<float>(), *buffer.popp<float>());
		break;
	}
	case 57:
	{
		component->drawRect(*buffer.popp<float>(), *buffer.popp<float>(), *buffer.popp<float>(), *buffer.popp<float>());
		break;
	}
	case 58:
	{
		component->clear();
		break;
	}

	case 59:
	{
		component->close();
		break;
	}
	case 60:
	{
		float &localX = *buffer.popp<float>();
		float &localY = *buffer.popp<float>();
		d2::QueryShapeResult result;
		component->queryGemoetry(localX, localY, result);

		component->nativeToJs.beginOpync();
		component->nativeToJs.writeOpArg(result.pathIndex);
		component->nativeToJs.writeOpArg(result.gemoetryIndex);
		component->nativeToJs.writeOpArg(result.linePoint1Index);
		component->nativeToJs.writeOpArg(result.linePoint2Index);
		component->nativeToJs.endOpSync();
		break;
	}
	case 61:
	{
		int &pathIndex = *buffer.popp<int>();
		int &insertIndex = *buffer.popp<int>();
		float &x = *buffer.popp<float>();
		float &y = *buffer.popp<float>();
		component->insertAfterLinePoint(pathIndex, insertIndex, x, y);
		break;
	}
	case 62:
	{
		int &pathIndex = *buffer.popp<int>();
		int &pointIndex = *buffer.popp<int>();
		component->deleteLinePoint(pathIndex, pointIndex);
		break;
	}
	case 63:
	{
		component->drawCapsule(
			*buffer.popp<float>(),
			*buffer.popp<float>(),
			*buffer.popp<float>(),
			*buffer.popp<float>(),
			*buffer.popp<float>());
		break;
	}
	default:
		SQ_ASSERT(false);
	}
}
