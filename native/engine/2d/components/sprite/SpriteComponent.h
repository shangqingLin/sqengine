#pragma once
#include "../UIContentComponent.h"
#include "../../../assets/Texture2d.h"
#include "../../../../bindings/define.h"
#include "./assembler/ISpriteAssembly.h"
#include "../../../assets/SpriteFrame.h"

void bindingProcessSpriteComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

enum class SpriteType
{
    /**
     * 普通类型。
     */
    SIMPLE = 0,
    /**
     * 切片（九宫格）类型。
     */
    SLICED = 1,
    /**
     * @zh  平铺类型
     */
    TILED = 2,
    /**
     * 填充类型。
     */
    FILLED = 3,

    /**
     * 自定义网格类型。即顶点数据由用户指定。
     */
    CUSTOM_MESH = 4,
};

SQ_ENUM_CONVERSION_OPERATOR(SpriteType);

enum class FillType
{
    /**
     * 水平方向填充。
     */
    HORIZONTAL = 0,

    /**
     * 垂直方向填充。
     */
    VERTICAL = 1,

    /*
     * 径向填充
     */
    RADIAL = 2,
};
SQ_ENUM_CONVERSION_OPERATOR(FillType);

class SpriteComponent : public UIContentComponent
{
protected:
    struct VertexListRenderData
    {

        int vertexNum{0};

        // 16个顶点
        float vertexList[32];

        // 自定义顶点Mesh
        float *customVertexList{nullptr};
        int customVertexListCount{0};
        unsigned short *customIndexList{nullptr};
        int customIndexListCount{0};
        int customIndexNum{0};

        /**
         * 1、1 << 1 顶点
         * 2、1 << 2 uv
         * 3、1 << 3 color
         */
        int dirtyState{0};

        int cacheFlagChangedVersion{0};
        int cacheTextureIndex{-1};

        inline bool dirtyVertex() { return dirtyState & 1 << 1; };
        inline bool dirtyUv() { return dirtyState & 1 << 2; };
        inline bool dirtyColor() { return dirtyState & 1 << 3; };

        inline void clearDirtyVertex() { dirtyState &= ~(1 << 1); };
        inline void clearDirtyUv() { dirtyState &= ~(1 << 2); };
        inline void clearDirtyColor() { dirtyState &= ~(1 << 3); };

        inline void setDirtyVertex() { dirtyState |= 1 << 1; };
        inline void setDirtyUv() { dirtyState |= 1 << 2; };
        inline void setDirtyColor() { dirtyState |= 1 << 3; };

        void clearCustomData()
        {
            if (customVertexList != nullptr)
            {
                delete[] customVertexList;
                customVertexList = nullptr;
                customVertexListCount = 0;
            }

            if (customIndexList != nullptr)
            {
                delete[] customIndexList;
                customIndexList = nullptr;
                customIndexListCount = 0;
            }
        }

        ~VertexListRenderData()
        {
            clearCustomData();
        }
    };

    VertexListRenderData vertexRenderData;
    std::optional<SpriteFrame> spriteFrame;
    sqstd::ByteBlockChunk *vertexData = nullptr;
    SpriteType type;
    FillType fillType;

    /**
     * fillStart和fillRange决定了显示的区域，其显示的范围是[fillStart,fillRange]
     * 比如你设置的HORIZONTAL：
     * 1、则0表示最左边，即从左到右渲染。比如fillStart = 0,fillRange = 0.5，则渲染区域是0到0.5，结果是显示图片的一半
     *
     * fillStart 值范围是[0,1]
     * fillRange 值范围是[-1,1]
     * 其中fillRange可以支持小于0，表示从反方向渲染，比如你设置fillStart = 1,fillRange = -0.5，则表示从右到左渲染
     */
    float fillStart;
    float fillRange;
    Vec2 fillCenter;
    ISpriteAssembly *assembly;

    void updateAssembly();

protected:
    virtual bool canRender();
    virtual void onStart();
    virtual void setDirty();

public:
    friend class SpriteSimpleAssembly;
    friend class SpriteSlicedAssembly;
    friend class SpriteBarFilledAssembly;
    friend class SpriteRadialAssembly;
    friend class SpriteCustomMeshAssembly;

    friend void bindingProcessSpriteComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

    SpriteComponent();
    virtual ~SpriteComponent();
    void setSpriteFrame(const SpriteFrame &);
    inline Texture2d *getTexture() { return spriteFrame.has_value() ? spriteFrame.value().texture : nullptr; };
    virtual void setMesh(Mesh *mesh);
    void setSpriteType(SpriteType);
    void setFillType(FillType);
    void setFillRange(float);
    void setFillStart(float);
    void setFillCenter(const Vec2 &);

    /**
     * 用户自定义Mesh时，调用此方法设置顶点数据
     * @param vertexData 顶点数据，格式由用户自定义
     * @param uvData uv数据，格式由用户自定义
     * @param vertexNum 顶点数量
     * @note 只有当SpriteType为CUSTOM_MESH时，此方法才有效。
     * 调用此方法之前需要先将SpriteType设置为CUSTOM_MESH，否则会触发断言。
     * 因为只有CUSTOM_MESH类型才允许用户自定义顶点数据，其他类型的顶点数据由引擎内部根据SpriteFrame自动生成，用户不应该干预。
     */
    void setMeshData(const float *vertexData,const float *uvData, int vertexNum,const unsigned short *indexData, int indexNum);
    virtual void setColor(Color &);
    inline ISpriteAssembly *getAssembley() { return assembly; };
};
