#pragma once
#include "../../../../bindings/binding.h"
#include "../../../core/core.h"
#include "../UIContentComponent.h"
#include "../../../core/sqstd/Array.h"

void processGraphicsComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

class Graphics : public UIContentComponent
{
private:
    sqstd::Array<d2::Path *> poolPath;
    std::vector<d2::Path *> paths;
    d2::Path *activePath = NULL;
    bool boundDirty = false;
    bool onAddToTransform = false;
    Bound2 bound;
    bridge::NativeToJsObject nativeToJs;

    bool dirty = false;
    sqstd::ByteBlockChunk *vertData = nullptr;
    sqstd::Array<unsigned short> triangles;

protected:
    virtual bool canRender();
    virtual void onUpdate();
    virtual bool checkHit(Vec2 &localPoint);
    virtual bool validHit();
    void beginPath();
    void drawEnd();
    virtual void onStart();

public:
    friend class GraphicsAssembler;
    friend void processGraphicsComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    Graphics();
    virtual ~Graphics();

    const Bound2 &getBound();
    void queryGemoetry(float localX, float localY, d2::QueryShapeResult &);
    void drawCircle(float centerX, float centerY, float r);
    void drawEllipse(float cx, float cy, float rx, float ry);
    void drawRect(float x, float y, float w, float h);
    void drawCapsule(float c1x, float c1y, float c2x, float c2y, float radius);
    void moveTo(float x, float y);
    void lineTo(float x, float y);
    /**
     * 更新在线模式绘制的图形中的点的坐标。
     * 这样不用清除整个图形就可以实现修改点的位置，提升绘制效率
     */
    void updateLinePoint(int pathIndex, int pointIndex, float x, float y);

    /**
     * 往线模式绘制的图形中添加点
     */
    void insertAfterLinePoint(int pathIndex, int insertAfterpointIndex, float x, float y);

    /**
     * 线模式绘制删除指定的点
     */
    void deleteLinePoint(int pathIndex, int pointIndex);

    void close();
    /**
     * 绘制实体形状
     */
    void fill();

    /**
     * 绘制线框
     */
    void stroke();

    void fill(d2::FillStyle &fillStyle);

    void stroke(d2::StrokeStyle &strokeStyle);

    void clear();
};
