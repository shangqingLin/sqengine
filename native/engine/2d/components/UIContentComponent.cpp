#include "UIContentComponent.h"
#include "Transform2DComponent.h"
#include "../../core/math/Vec2.h"
#include "../../core/math/Vec3.h"
#include "../../core/math/Mat3.h"
#include "../../scene/Camera.h"
#include "../../scene/Scene.h"

UIContentComponent::UIContentComponent() : RenderComponent(),
                                           colorNum(0xffffffff), // 白色
                                           stencilStage(pipeline::StencilStage::DISABLED)

{
    
}

bool UIContentComponent::canRender()
{
    return false;
}

void UIContentComponent::setColor(Color &color)
{
    colorNum = Color::combineToNum(color);
}

unsigned int UIContentComponent::getColorNum()
{
    return colorNum;
}

void UIContentComponent::setPivotX(float pivotX)
{
    if (props.px != pivotX)
    {
        props.px = pivotX;
        setDirty();
    }
}

void UIContentComponent::setPivotY(float pivotY)
{
    if (props.py != pivotY)
    {
        props.py = pivotY;
        setDirty();
    }
}

void UIContentComponent::setAnchorX(float x)
{
    if (props.ax != x)
    {
        props.ax = x;
        setDirty();
    }
}

void UIContentComponent::setAnchorY(float y)
{
    if (props.ay != y)
    {
        props.ay = y;
        setDirty();
    }
}

void UIContentComponent::setAnchor(float x, float y)
{
    setAnchorX(x);
    setAnchorY(y);
}

void UIContentComponent::setWidth(float width)
{

    // printf("+++++++++++ setWidth node %d width: %f %f \n", node->nativeId, width, this->width);
    if (props.width != width)
    {
        props.width = width;
        setDirty();
    }
}

void UIContentComponent::setHeight(float height)
{
    if (props.height != height)
    {
        props.height = height;
        setDirty();
    }
}

float UIContentComponent::getAnchorXInPix()
{
    return props.ax * props.width;
}

float UIContentComponent::getAnchorYInPix()
{
    return props.ay * props.height;
}

float UIContentComponent::getOffsetX()
{
    return props.px ? props.px : getAnchorXInPix();
}

float UIContentComponent::getOffsetY()
{
    return props.py ? props.py : getAnchorYInPix();
}

bool UIContentComponent::validHit()
{
    return props.width != 0 && props.height != 0;
}

void UIContentComponent::setDirty()
{
    node->setChangedFlags(TransformBit::POSITION);
}

bool UIContentComponent::hitTest(Event &event)
{

    // if(event.type != EventType::mousedown) return false;

    if (!validHit())
    {
        return false;
    }

    const std::vector<Camera *> &cameras = node->getScene()->getCameras();
    Vec3 worldPoint;
    Vec2 screenPoint(event.screenX, event.screenY);

    Transform2DComponent *transform = node->getComponent<Transform2DComponent>();
    // x和y是为是为世界坐标系中的点，所以需要转换为本地坐标
    Mat3 &worldMat = transform->getWorldTransform();

    Mat3 worldMatInv;
    worldMatInv.copy(worldMat);
    worldMatInv.translate(-getOffsetX(), -getOffsetY());
    worldMatInv.invert();

    // worldMatInv.print();

    for (int i = 0; i < cameras.size(); ++i)
    {
        Camera *camera = cameras[i];

        if (camera->getLayer() != node->getLayer())
            continue;

        camera->screenToWorld(screenPoint, worldPoint);
        event.worldX = worldPoint.x;
        event.worldY = worldPoint.y;

        // printf("screenPoint:(%f,%f) world : (%f,%f)\n",screenPoint.x,screenPoint.y,worldPoint.x,worldPoint.y);

        screenPoint.x = worldPoint.x;
        screenPoint.y = worldPoint.y;

        worldMatInv.transformPoint(screenPoint, screenPoint);
        event.localX = screenPoint.x;
        event.localY = screenPoint.y;

        // printf("anchor:(%f,%f) checkpos:(%f,%f) size: (%f,%f) check: %d layer:%d\n",
        //     ax,ay,
        //     screenPoint.x,screenPoint.y,
        //     width,height,
        //     checkHit(screenPoint),camera->getLayer()
        // );

        return checkHit(screenPoint);
    }

    return false;
}

bool UIContentComponent::checkHit(Vec2 &localPoint)
{
    return localPoint.x >= 0 && localPoint.y >= 0 && localPoint.x <= props.width && localPoint.y <= props.height;
}

void processUIContentComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<UIContentComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<UIContentComponent>();
        return;
    }

    UIContentComponent *component = node->getComponent<UIContentComponent>();
    processUIContentComponent(component, type, nodeOp, op, buffer, node);
}

void processUIContentComponent(UIContentComponent *component, bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{

    if (op <= 20)
    {
        bindingProcessRenderComponent(component, type, nodeOp, op, buffer, node);
        return;
    }

    switch (op)
    {
    case 21:
    {
        float x = *buffer.popp<float>();
        float y = *buffer.popp<float>();
        component->setAnchorX(x);
        component->setAnchorY(y);
        break;
    }
    case 22:
    {
        float x = *buffer.popp<float>();
        float y = *buffer.popp<float>();
        component->setPivotX(x);
        component->setPivotY(y);
        break;
    }
    case 23:
    {
        component->setWidth(*buffer.popp<float>());
        component->setHeight(*buffer.popp<float>());
        break;
    }
    case 24:
    {
        component->setDirty();
        break;
    }
    case 25:
    {
        component->setStencil(pipeline::StencilStage(*buffer.popp<char>()));
        break;
    }
    case 30:
    {
        Color color;
        color.r = *buffer.popp<float>();
        color.g = *buffer.popp<float>();
        color.b = *buffer.popp<float>();
        color.a = *buffer.popp<float>();
        component->setColor(color);
        break;
    }
    default:
        SQ_ASSERT(false);
    }
}