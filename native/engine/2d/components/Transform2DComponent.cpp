#include "Transform2DComponent.h"
#include "../../core/math/Vec2.h"
#include "../../scene/Camera.h"
#include "UIContentComponent.h"

Transform2DComponent::Transform2DComponent() : TransformBaseComponent()
{
}

void Transform2DComponent::onTransformDirty()
{
    props.dirty = true;
}

Mat3 &Transform2DComponent::getWorldTransform()
{

    if (!(node->changedFlags & TransformBit::TRS))
    {
        return props.worldMat;
    }

    node->changedFlags &= ~TransformBit::TRS;
    props.dirty = false;

    Vec2 localPositionVec;
    Vec2 localRotateVec;
    Vec2 localScaleVec;

    // if(node->nativeId == 26)
    // printf("Transform2DComponent::getWorldTransform() %d \n",node->nativeId);

    // if (node->nativeId == 84)
    // {
    //     printf("transform %d x %f y %f \n", node->nativeId, getX(), getY());
    // }

    // if (node->nativeId == 84)
    // {
    //     printf("transform %d x %f y %f \n", node->nativeId, getX(), getY());
    //     Node *parent = this->node->parent;
    //     while (parent)
    //     {
    //         Transform2DComponent *t = parent->getComponent<Transform2DComponent>();
    //         printf("   parent transform %d x %f y %f \n", parent->nativeId, t->getX(), t->getY());
    //         parent = parent->parent;
    //     }
    // }

    localPositionVec.set(props.x, props.y);
    localRotateVec.set(props.rx, props.ry);
    localScaleVec.set(props.sx, props.sy);
    props.worldMat.setTRS(localPositionVec, localRotateVec, localScaleVec);

    if (node->getParent())
    {
        Transform2DComponent *pt = node->getParent()->getComponent<Transform2DComponent>();
        props.worldMat.mul2(pt->getWorldTransform(), props.worldMat);
        // pt->getWorldTransform().print();
    }

    // printf("getWorldTransform local pos: (%f %f) r:(%f %f) scale:(%f %f) \n",x,y,rx,ry,sx,sy);
    // worldMat.print();
    return props.worldMat;
}

Mat3 Transform2DComponent::getLocalTransform()
{
    Mat3 localMat;
    Vec2 localPositionVec;
    Vec2 localRotateVec;
    Vec2 localScaleVec;

    localPositionVec.set(props.x, props.y);
    localRotateVec.set(props.rx, props.ry);
    localScaleVec.set(props.sx, props.sy);
    localMat.setTRS(localPositionVec, localRotateVec, localScaleVec);
    return localMat;
}

void Transform2DComponent::setPosition(float x, float y)
{
    setX(x);
    setY(y);
}

void Transform2DComponent::setX(float x)
{

    // printf("Transform2DComponent::setX() %d %f %f \n", node->nativeId, x, props.x);
    if (props.x != x)
    {

        props.x = x;
        invalidateChildren(TransformBit::POSITION);
    }
}
void Transform2DComponent::setY(float y)
{
    // if (node->nativeId == 31)
    // {
    //     printf("   set Y %f \n",y);
    //     TRACE();
    // }

    if (props.y != y)
    {
        props.y = y;
        invalidateChildren(TransformBit::POSITION);
    }
}

void Transform2DComponent::setScaleX(float sx)
{
    if (props.sx != sx)
    {
        props.sx = sx;
        invalidateChildren(TransformBit::SCALE);
    }
}

void Transform2DComponent::setScaleY(float sy)
{
    if (props.sy != sy)
    {
        props.sy = sy;
        invalidateChildren(TransformBit::SCALE);
    }
}

void Transform2DComponent::setScale(float x, float y)
{
    setScaleX(x);
    setScaleY(y);
}

void Transform2DComponent::setRotateX(float rx)
{
    if (props.rx != rx)
    {
        props.rx = rx;
        invalidateChildren(TransformBit::ROTATION);
    }
}

void Transform2DComponent::setRotateY(float ry)
{
    if (props.ry != ry)
    {
        props.ry = ry;
        invalidateChildren(TransformBit::ROTATION);
    }
}

void Transform2DComponent::setRotate(float r)
{
    if (props.ry != r || props.rx != r)
    {
        props.ry = r;
        props.rx = r;
        invalidateChildren(TransformBit::ROTATION);
    }
}

void Transform2DComponent::rotateBy(float r)
{
    Vec2 localPositionVec;
    Vec2 localRotateVec;
    Vec2 localScaleVec;

    localPositionVec.set(props.x, props.y);
    localRotateVec.set(props.rx, props.ry);
    localScaleVec.set(props.sx, props.sy);
    Mat3 localMat;
    localMat.setTRS(localPositionVec, localRotateVec, localScaleVec);

    Mat3 rotMat;
    rotMat.rotate(r * Math::DEG_TO_RAD);

    localMat.mul2(rotMat, localMat);

    float lrx, lry, px, py;
    localMat.getRotation(lrx, lry, false);
    localMat.getTranslation(px, py);

    // printf("rotateBy %f before %f %f %f %f after %f %f %f %f\n", r, rx, ry, x, y, lrx, lry, px, py);

    if (lrx != props.rx || lry != props.ry || px != props.x || py != props.y)
    {
        props.rx = lrx;
        props.ry = lry;
        props.x = px;
        props.y = py;
        invalidateChildren(TransformBit::ROTATION);
    }
}

void Transform2DComponent::setLocalTransform(const Mat3 &mat)
{
    float lsx, lsy, lrx, lry, px, py;
    mat.getRotation(lrx, lry, false);
    mat.getTranslation(px, py);
    mat.getScale(lsx, lsy);

    // printf("setLocalTransform %f %f %f %f %f %f\n", lsx, lsy, lrx, lry, px, py);

    if (lrx != props.rx || lry != props.ry || px != props.x || py != props.y || lsx != props.sx || lsy != props.sy)
    {
        props.rx = lrx;
        props.ry = lry;
        props.x = px;
        props.y = py;
        props.sx = lsx;
        props.sy = lsy;
        invalidateChildren(TransformBit::TRS);
    }
}

/**
 * 设置世界坐标系下的坐标
 */
void Transform2DComponent::setWorldPosition(float wx, float wy)
{
    if (node->getParent())
    {
        Mat3 invertMat;
        Transform2DComponent *pt = node->getParent()->getComponent<Transform2DComponent>();
        invertMat.copy(pt->getWorldTransform());
        invertMat.invert();
        Vec2 localPosVec;
        localPosVec.set(wx, wy);
        invertMat.transformPoint(localPosVec, localPosVec);
        this->setX(localPosVec.x);
        this->setY(localPosVec.y);
    }
    else
    {
        this->setX(wx);
        this->setY(wy);
    }
}

Vec2 Transform2DComponent::getWorldPosition()
{
    Mat3 &mat = getWorldTransform();
    Vec2 pos;
    mat.getTranslation(pos.x, pos.y);
    return pos;
}

void Transform2DComponent::setWorldTranslate(float dx, float dy)
{
    if (node->getParent())
    {
        // Mat3 invertMat = getWorldTransform();
        Mat3 invertMat = node->getParent()->getComponent<Transform2DComponent>()->getWorldTransform();
        invertMat.invert();
        Vec2 localPosVec;
        localPosVec.set(dx, dy);
        invertMat.transformVector(localPosVec, localPosVec);
        this->setX(localPosVec.x + props.x);
        this->setY(localPosVec.y + props.y);
    }
    else
    {
        this->setX(props.x + dx);
        this->setY(props.y + dy);
    }
}

void Transform2DComponent::worldPositionToLocal(float wx, float wy, Vec2 &out)
{
    Mat3 mat = this->getWorldTransform();
    UIContentComponent *c = node->getComponent<UIContentComponent>();
    if (c)
    {
        float offsetX = c->getOffsetX();
        float offsetY = c->getOffsetY();
        if (offsetX || offsetY)
        {
            mat.translate(offsetX, offsetY);
        }
    }

    mat.invert();
    out.set(wx, wy);
    mat.transformPoint(out, out);
}

void Transform2DComponent::localPositionToWorld(float x, float y, Vec2 &out)
{
    Mat3 &mat = this->getWorldTransform();
    UIContentComponent *c = node->getComponent<UIContentComponent>();
    if (c)
    {
        float offsetX = c->getOffsetX();
        float offsetY = c->getOffsetY();
        if (offsetX || offsetY)
        {
            mat.translate(-offsetX, -offsetY);
        }
    }
    out.set(x, y);
    mat.transformPoint(out, out);
}

void Transform2DComponent::setWorldTransform(const Mat3 &mat)
{
    if (node->getParent())
    {
        Mat3 invertMat;
        Transform2DComponent *pt = node->getParent()->getComponent<Transform2DComponent>();
        invertMat.copy(pt->getWorldTransform());
        invertMat.invert();
        invertMat.mul(mat);
        invertMat.getTranslation(props.x, props.y);
        invertMat.getRotation(props.rx, props.ry, false);
        props.sx = 1.f;
        props.sy = 1.f;
        // printf("setWorldTransform pos: %f %f scale:%f %f rotation:%f %f\n",x,y,sx,sy,rx,ry);
    }
    else
    {
        getWorldTransform().copy(mat);
        mat.getTranslation(props.x, props.y);
        mat.getRotation(props.rx, props.ry, false);
        props.sx = 1.f;
        props.sy = 1.f;
    }

    // printf("============== %f %f %f %f (%f %f %f %f %f %f) \n",x,y,sx,sy,mat.data[0],mat.data[1],mat.data[2],mat.data[3],mat.data[4],mat.data[5]);
    this->invalidateChildren(TransformBit::TRS);
}

void Transform2DComponent::print()
{
    printf("Transform2DComponent %d %f %f %f %f %f %f\n", node->changedFlags, props.x, props.y, props.sx, props.sy, props.rx, props.ry);
}

void processTransform2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{

    // printf("-----> processTransform2DComponent %d %d %d\n",node->nativeId , nodeOp , op);

    if (nodeOp == 1)
    {
        node->addComponent<Transform2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<Transform2DComponent>();
        return;
    }

    Transform2DComponent *component = node->getComponent<Transform2DComponent>();
    SQ_ASSERT(component);
    switch (op)
    {
    case 1:
    {
        float x = *buffer.popp<float>();
        float y = *buffer.popp<float>();
        component->setX(x);
        component->setY(y);
        // printf("============set pos %d %f %f \n", node->nativeId, x, y);
        break;
    }
    case 2:
    {
        float x = *buffer.popp<float>();
        float y = *buffer.popp<float>();
        component->setScaleX(x);
        component->setScaleY(y);
        break;
    }
    case 3:
    {
        float x = *buffer.popp<float>();
        float y = *buffer.popp<float>();
        component->setRotateX(x);
        component->setRotateY(y);
        break;
    }

    case 4:
    {
        component->setWorldPosition(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    }
    case 5:
    {
        component->setWorldTranslate(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    }
    case 6:
    {
        float wx = *buffer.popp<float>();
        float wy = *buffer.popp<float>();
        Vec2 out;
        component->worldPositionToLocal(wx, wy, out);
        component->nativeToJs.beginOpync();
        component->nativeToJs.writeOpArg(out.x);
        component->nativeToJs.writeOpArg(out.y);
        component->nativeToJs.endOpSync();
        break;
    }
    case 7:
    {
        float x = *buffer.popp<float>();
        float y = *buffer.popp<float>();
        Vec2 out;
        component->localPositionToWorld(x, y, out);
        component->nativeToJs.beginOpync();
        component->nativeToJs.writeOpArg(out.x);
        component->nativeToJs.writeOpArg(out.y);
        component->nativeToJs.endOpSync();
        break;
    }
    case 9:
    {
        // 计算一下矩阵
        component->getWorldTransform();
        break;
    }
    case 10:
    {

        Mat3 mat;
        mat.data[0] = *buffer.popp<float>();
        mat.data[1] = *buffer.popp<float>();
        mat.data[2] = *buffer.popp<float>();
        mat.data[3] = *buffer.popp<float>();
        mat.data[4] = *buffer.popp<float>();
        mat.data[5] = *buffer.popp<float>();
        component->setWorldTransform(mat);
        break;
    }
    case 11:
    {
        float r = *buffer.popp<float>();
        component->rotateBy(r);
        break;
    }
    case 12:
    {
        Mat3 mat;
        mat.data[0] = *buffer.popp<float>();
        mat.data[1] = *buffer.popp<float>();
        mat.data[2] = *buffer.popp<float>();
        mat.data[3] = *buffer.popp<float>();
        mat.data[4] = *buffer.popp<float>();
        mat.data[5] = *buffer.popp<float>();
        component->setLocalTransform(mat);
        break;
    }
    case 14:
    {
        // if(component->node->nativeId == 127)
        // printf("你的的阿海阿瓦色地方撒旦哦i哦 %d %d \n",component->node->testNum,component->node->nativeId);
        component->invalidateChildren(TransformBit(*buffer.popp<int>()));
        break;
    }
    default:
        SQ_ASSERT(false);
    }
}