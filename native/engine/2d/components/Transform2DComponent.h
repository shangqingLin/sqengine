#ifndef _TRANSFORM_2D_COMPONENT_
#define _TRANSFORM_2D_COMPONENT_
#include <optional>
#include <functional>
#include "../../core/math/Mat3.h"
#include "../../framework/component/TransformBaseComponent.h"
#include "../../input/EventType.h"
#include "../../../bindings/binding.h"

void processTransform2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

typedef std::function<void(const Vec2 &, bool &)> HIT_TEST_FUN;

/**
 * 旋转：
 * 负数表示顺时针旋转
 * 正数表示逆时针旋转
 */
class Transform2DComponent : public TransformBaseComponent
{
private:
    // 这样封装方便js端直接读取此内存地址，
    // 而不去计算Transform2DComponent实例中各个属性偏移,因为那样容易出错，还可能有平台和Emscripten兼容性问题
    struct
    {
        float x{0.f}, y{0.f}, sx{1.0f}, sy{1.0f}, rx{0.f}, ry{0.f};
        Mat3 worldMat;

        // JS端通过读取这个来得到C++端是否脏数据了
        bool dirty = false;

    } props; // js端直接拿到这个属性的地址

    bridge::NativeToJsObject nativeToJs;

    void onTransformDirty();

public:
    friend class Node;
    friend void processTransform2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    Transform2DComponent();

    Mat3 &getWorldTransform();
    Mat3 getLocalTransform();
    void setX(float x);
    void setY(float y);
    inline float getX() { return props.x; };
    inline float getY() { return props.y; };
    inline Vec2 getLocalPosition() { return Vec2(getX(), getY()); };
    void setPosition(float x, float y);
    void setScaleX(float sx);
    void setScaleY(float sy);
    inline float getScaleX() { return props.sx; };
    inline float getScaleY() { return props.sy; };
    void setScale(float x, float y);
    void setRotateX(float rx);
    void setRotateY(float ry);
    void setRotate(float r);

    /**
     * 相当于再乘以一个旋转矩阵
     */
    void rotateBy(float);
    void setLocalTransform(const Mat3 &);
    void setWorldPosition(float wx, float wy);
    Vec2 getWorldPosition();
    void setWorldTranslate(float dx, float dy);
    void worldPositionToLocal(float wx, float wy, Vec2 &);
    void localPositionToWorld(float x, float y, Vec2 &);
    void setWorldTransform(const Mat3 &);
    inline void *getPropsAdress() { return &props; };
    void print();
};

#endif