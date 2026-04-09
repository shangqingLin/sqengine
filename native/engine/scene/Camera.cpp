#include "Camera.h"
#include "../2d/components/Transform2DComponent.h"
#include "../framework/Application.h"

Camera::Camera() : clearStencil(0),
                   clearFlag(ClearFlags::NONE),
                   clearDepth(1),
                   aspect(16 / 9),
                   orthoHeight(10.),
                   nearClip(1),
                   farClip(1000),
                   node(NULL),
                   stateFlag(0),
                   window(NULL),
                   enabled(false),
                   priority(0),
                   clearColor(0.f, 0.f, 0.f, 0.f), // 默认显示为黑色。如果 a = 0 可能不同的平台显示黑色或白色的
                   projectionType(CameraProjection::ORTHO)
{
}

void Camera::setProjection(CameraProjection projection)
{
    if (this->projectionType != projection)
    {
        this->projectionType = projection;
        stateFlag |= CameraStateFlag::PROJ_DIRTY;
    }
}

void Camera::setNode(Node *node)
{
    this->node = node;
}

float Camera::getNearClip()
{
    return nearClip;
}

void Camera::setNearClip(float v)
{
    if (nearClip != v)
    {
        nearClip = v;
        stateFlag |= CameraStateFlag::PROJ_DIRTY;
    }
}

void Camera::setFarClip(float v)
{
    if (farClip != v)
    {
        farClip = v;
        stateFlag |= CameraStateFlag::PROJ_DIRTY;
    }
}

float Camera::getFarClip()
{
    return farClip;
}

void Camera::setFov(float v)
{
    if (projectionType == CameraProjection::PERSPECTIVE && fov != v)
    {
        fov = v;
        stateFlag |= CameraStateFlag::PROJ_DIRTY;
    }
}

float Camera::getFov()
{
    return fov;
}

void Camera::setAspect(float a)
{
    if (aspect != a)
    {
        aspect = a;
        stateFlag |= CameraStateFlag::PROJ_DIRTY;
    }
}

float Camera::getAspect()
{
    return aspect;
}

void Camera::setOrthoHeight(float orthoHeight)
{
    if (this->orthoHeight != orthoHeight)
    {
        stateFlag |= CameraStateFlag::PROJ_DIRTY;
        this->orthoHeight = orthoHeight;
    }
}

ClearFlags Camera::getClearFlag()
{
    return clearFlag;
}

void Camera::setClearFlag(ClearFlags flag)
{
    clearFlag = flag;
}

float Camera::getClearDepth()
{
    return clearDepth;
}

void Camera::setClearDepth(float depth)
{
    clearDepth = depth;
}

int Camera::getClearStencil()
{
    return clearStencil;
}

void Camera::setClearStencil(int stencil)
{
    clearStencil = stencil;
}

void Camera::setViewport(Rect<float> &rect)
{
    viewPort.x = rect.x;
    viewPort.y = rect.y;
    viewPort.width = rect.width;
    viewPort.height = rect.height;
}

void Camera::setClearColor(Color color)
{
    clearColor.r = color.r;
    clearColor.g = color.g;
    clearColor.b = color.b;
    clearColor.a = color.a;
}

void Camera::setOrthoSize(float width, float height)
{
    float halfHeight = height * 0.5;
    float halfWidth = width * 0.5;
    setOrthoHeight(halfHeight);
    setAspect(width / height);
    setFov(0.0f);
    setNearClip(0.01f);
    setFarClip(1000.0f);
    Rect<float> viewport = {0.f, 0.f, width, height};
    setViewport(viewport);
}

void Camera::update()
{

    bool projMatDirty = stateFlag & CameraStateFlag::PROJ_DIRTY;
    if (projMatDirty)
    {
        if (projectionType == CameraProjection::PERSPECTIVE)
        {
            projMat.setPerspective(fov, aspect, nearClip, farClip, false);
        }
        else
        {
            float x = orthoHeight * aspect;
            // printf("camera %p %f %f %f %f %f\n",node,aspect,x,orthoHeight,nearClip,farClip);
            projMat.setOrtho(-x, x, -orthoHeight, orthoHeight, nearClip, farClip);
        }
    }

    bool matDirty = false;
    if (node)
    {
        if (node->hasChangedFags() || projMatDirty)
        {
            Transform2DComponent *t2dComponent = node->getComponent<Transform2DComponent>();
            if (t2dComponent)
            {
                Mat3 mat3 = t2dComponent->getWorldTransform();
                mat3.invert();
                viewProjMat.setFromMat3(mat3);
                viewProjMat.mul2(projMat, viewProjMat);
                matDirty = true;
                //   printf("----------------------proj\n");
                //   mat3.print();
                //   viewProjMat.toString();
            }
            else
            {
                viewProjMat.copy(projMat);
                matDirty = true;
            }
        }
    }
    else
    {
        if (projMatDirty)
        {
            viewProjMat.copy(projMat);
            matDirty = true;
        }
    }

    if (matDirty)
    {
        viewProjMatInv.copy(viewProjMat);
        viewProjMatInv.invert();
    }

    if (projMatDirty)
    {
        stateFlag &= ~CameraStateFlag::PROJ_DIRTY;
    }
}

Mat4 &Camera::getViewProjMat()
{
    update();
    return viewProjMat;
}

void Camera::screenToWorld(Vec2 &screenPos, Vec3 &out)
{
    update();

    // 因为正投影照相机是以中心为原点的坐标系，左边为x负轴，右边为x正轴。上面为y正轴，下面为y负轴
    // 所以也需要将screenPos转换到此坐标系下的像素表示，即范围[-1,1]空间，然后再将像素表示转换为OpenGL数字表示
    out.set((screenPos.x - viewPort.x) / viewPort.width * 2 - 1,
            (screenPos.y - viewPort.y) / viewPort.height * 2 - 1,
            0.0);

    // printf("screenToWorld viewport (%f %f)\n",out.x,out.y);

    if (projectionType == CameraProjection::PERSPECTIVE)
    {
    }
    else
    {

        //    printf("viewProjMatInv:(%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f) \n",
        //       viewProjMatInv.data[0],
        //       viewProjMatInv.data[1],
        //       viewProjMatInv.data[2],
        //       viewProjMatInv.data[3],
        //       viewProjMatInv.data[4],
        //       viewProjMatInv.data[5],
        //       viewProjMatInv.data[6],
        //       viewProjMatInv.data[7],
        //       viewProjMatInv.data[8],
        //       viewProjMatInv.data[9],
        //       viewProjMatInv.data[10],
        //       viewProjMatInv.data[11],
        //       viewProjMatInv.data[12],
        //       viewProjMatInv.data[13],
        //       viewProjMatInv.data[14],
        //       viewProjMatInv.data[15]
        //    );

        viewProjMatInv.transformPoint(out, out);
    }
}

void Camera::changeTargetWindow(RenderWindow *setwindow)
{
    if (window)
    {
        window->detachCamera(this);
    }
    RenderWindow *win = setwindow ? setwindow : Application::getInstance()->getRoot().mainWindow;
    window = win;
    win->attachCamera(this);
}

Layers Camera::getLayer()
{
    return node->getLayer();
}

Camera::~Camera()
{
    if (window)
    {
        window->detachCamera(this);
        window = NULL;
    }
    node = NULL;
}

bool sortCameras(Camera *camera1, Camera *camera2)
{
    return camera1->getPriority() <= camera2->getPriority();
}
