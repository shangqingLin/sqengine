#pragma once
#include "../core/math/Rect.h"
#include "../core/math/Color.h"
#include "../core/math/Mat4.h"
#include "Node.h"
#include "RenderWindow.h"

enum class CameraProjection
{
    /**
     * @en Orthogonal projection type
     * @zh 正交投影类型
     */
    ORTHO,
    /**
     * @en Perspective projection type
     * @zh 透视投影类型
     */
    PERSPECTIVE
};

enum CameraStateFlag
{
    PROJ_DIRTY = 1 << 1
};

/**
 *  渲染场景中的相机对象
 */
class Camera
{

private:
    CameraProjection projectionType;
    Node *node;

    bool enabled;

    float nearClip;
    float farClip;

    int stateFlag;

    /**
     * 只有透视投影才有的
     */
    float fov;

    // 近平面宽高比（宽/高=aspect）
    float aspect;

    /**
     * 正交投影近平面的高度
     * 通过这个高度和aspect计算近平面的宽度
     */
    float orthoHeight;

    /**
     * 视口设置
     * x和y为屏幕渲染的起点
     * 宽高为屏幕渲染分辨率，所以必须将Camera设置为屏幕实际的分辨率
     */
    Rect<float> viewPort;

    int clearStencil;
    float clearDepth;
    // todo 还没有用到
    ClearFlags clearFlag;
    Color clearColor;

    /**
     * 在同一个场景中所有照相机的渲染顺序，数值越小越优先渲染
     */
    int priority;

    /**
     * 投影矩阵
     */
    Mat4 projMat;

    /**
     * 视点投影矩阵
     */
    Mat4 viewProjMat;

    /**
     * 视点投影矩阵的逆矩阵
     */
    Mat4 viewProjMatInv;

    /**
     * 每个Camera必须绑定一个window
     * 如果你没有设置window，则绑定到默认的window上
     * 默认的window上的内容会渲染到默认的帧缓冲区（即屏幕上显示的帧缓冲区）
     */
    RenderWindow *window;

    void update();

public:
    friend class RenderWindow;
    friend class CameraComponent;

    Camera();
    ~Camera();

    void setNode(Node *);
    inline Node *getNode() { return node; };

    void setProjection(CameraProjection projection);

    float getNearClip();
    void setNearClip(float v);

    void setFarClip(float v);
    float getFarClip();

    void setFov(float v);
    float getFov();

    void setAspect(float);
    float getAspect();

    void setOrthoHeight(float orthoHeight);
    inline float getOrthoHeight() { return orthoHeight; };

    ClearFlags getClearFlag();
    void setClearFlag(ClearFlags flag);

    float getClearDepth();
    void setClearDepth(float depth);

    int getClearStencil();
    void setClearStencil(int stencil);

    inline void setPriority(int priority) { this->priority = priority; };
    inline int getPriority() { return priority; };

    Mat4 &getViewProjMat();

    /**
     * 屏幕像素坐标的点转换到世界坐标系中表示的点
     */
    void screenToWorld(Vec2 &screenPos, Vec3 &out);

    void changeTargetWindow(RenderWindow *);
    inline RenderWindow *getRenderWindow() { return window; };

    void setViewport(Rect<float> &);

    //快捷全屏平行投影照相机的设置
    void setOrthoSize(float width,float height);
    inline Rect<float> &getViewport() { return viewPort; };
    void setClearColor(Color);
    inline Color &getClearColor() { return clearColor; };
    Layers getLayer();
};

bool sortCameras(Camera *camera1, Camera *camera2);