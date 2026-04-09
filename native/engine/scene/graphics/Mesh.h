#pragma once

#include <vector>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../../gfx/gfx.h"
#include "../../../bindings/binding.h"

/**
 * 索引绘制的设置
 */
struct DrawPrimitiveMesh
{

    gfx::Type type = gfx::Type::UINT2;

    /**
     * 取IndexBuffer哪个
     * 单位为个数，不是字节
     */
    unsigned int offset = 0;

    /**
     * 绘制多少个索引
     */
    unsigned int count = 0;

    /**
     * 如果这个数值大于0，则表示使用多实例绘制
     * 这里设置绘制的实例个数
     */
    int instance = 0;

    ~DrawPrimitiveMesh();
};

class Mesh : public bridge::JsToNativeObject
{
private:
    std::vector<VertexBuffer *> vertexBuffers;
    std::vector<int> strides;
    IndexBuffer *indexBuffer;
    InputAssembler *inputAssembler;
public:
    friend class MeshInstance;

    //一个奇怪的做法，以后再修改
    bool autoDeleteBuffer = true;

    Mesh();

    // 至少有一个虚拟函数才能进行多态
    virtual ~Mesh();

    // 清理网格上所有的数据
    virtual void clear();

    void reset();

    void addVertextBuffer(VertexBuffer *);
    void addVertextBuffer(VertexBuffer *, const std::vector<Attribute> &);
    void addVertextBuffer(VertexBuffer *, const Attribute &);

    /**
     * 设置顶点属性定义。告诉图形API VertexBuffer中的属性是如何分布的
     * 让图形API正确读取缓冲区的数据，正确地设置到顶点着色器中
     */
    void setVertexAttributeDscriptor(const Attribute &, int index);
    void setVertexAttributeDscriptor(const std::vector<Attribute> &, int index);
    inline int getVertexStride(int index) { return strides[index]; };
    void setIndexBuffer(IndexBuffer *);
    inline IndexBuffer *getIndexBuffer() { return indexBuffer; };
    inline const std::vector<VertexBuffer *> &getVertexBufers() const { return vertexBuffers; };
    inline VertexBuffer *getVertexBufer(int index) const { return vertexBuffers[index]; };
    inline InputAssembler *getInputAssembler() const { return inputAssembler; };
    inline std::vector<Attribute> &getAttributes(int index) { return inputAssembler->attrs[index]; };
    void upload() const;
};
