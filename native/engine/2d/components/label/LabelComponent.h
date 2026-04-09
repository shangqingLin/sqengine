#pragma once
#include "../../../assets/Font.h"
#include "./LabelRender.h"
#include "../UIContentComponent.h"

void bindingProcessLabelComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

/**
 * @zh 文本横向对齐类型。
 */
enum class HorizontalTextAlignment
{
    /**
     * @en Alignment left for text.
     * @zh 左对齐。
     */
    LEFT = 0,
    /**
     * @en Alignment center for text.
     *
     * @zh 中心对齐。
     */
    CENTER = 1,
    /**
     * @en Alignment right for text.
     *
     * @zh 右对齐。
     */
    RIGHT = 2,
};

/** *
 * @zh 文本垂直对齐类型。
 */
enum class VerticalTextAlignment
{
    /**
     * @zh 上对齐。
     */
    TOP = 0,
    /**     *
     * @zh 中心对齐。
     */
    CENTER = 1,
    /**
     * @zh 下对齐。
     */
    BOTTOM = 2,
};

/**
 * @zh 文本溢出行为类型。
 */
enum class Overflow
{
    /**     *
     * @zh 不做任何限制。
     */
    NONE = 0,
    /**     *
     * @zh CLAMP 模式中，当文本内容超出边界框时，多余的会被截断。
     */
    CLAMP = 1,
    /**     *
     * @zh SHRINK 模式，字体大小会动态变化，以适应内容大小。这个模式在文本刷新的时候可能会占用较多 CPU 资源。
     */
    SHRINK = 2,
    /**     *
     * @zh 在 RESIZE_HEIGHT 模式下，只能更改文本的宽度，高度是自动改变的。
     */
    RESIZE_HEIGHT = 3,
};

/**
 * @zh 文本图集缓存类型。
 */
enum class CacheMode
{
    /**
     *
     * @zh 不做任何缓存。
     */
    NONE = 0,
    /**
     *
     * @zh BITMAP 模式，将 label 缓存成静态图像并加入到动态图集，以便进行批次合并，可与使用碎图的 Sprite 进行合批。
     * （注：动态图集在 Chrome 以及微信小游戏暂时关闭，该功能无效）。
     */
    BITMAP = 1,

    /**
     * @zh CHAR 模式，将文本拆分为字符，并将字符缓存到一张单独的大小为 1024 * 1024 的图集中进行重复使用，不再使用动态图集。
     * （注：当图集满时将不再进行缓存，暂时不支持 SHRINK 自适应文本尺寸（后续完善））。
     */
    CHAR = 2,
};

class LabelComponent : public UIContentComponent
{
private:
    int fontSize = 24;
    int lineHeight = 0;
    int spacingX = 0;
    int contextWidth = 0;
    int contextHeight = 0;
    bool _isBold = false;
    bool _isItalic = false;
    bool _enableWrapText = true;
    HorizontalTextAlignment _horizontalAlign;
    VerticalTextAlignment _verticalAlign;
    Font *font = nullptr;
    std::vector<unsigned int> textCodes;
    LabelRender labelRender;
    sqstd::ByteBlockChunk *vertData = nullptr;
    Texture2d *textTexture = nullptr;

protected:
    virtual bool canRender();
    virtual void onUpdate();

public:
    friend class LabelRender;
    friend class LabelAssembler;
    LabelComponent();
    ~LabelComponent();

    inline void setTexture(Texture2d *textTexture) { this->textTexture = textTexture; };
    inline Texture2d *getTexture() { return this->textTexture; };
    inline LabelRender &getRender() { return labelRender; };

    void setFont(Font *font);
    inline Font *getFont() { return font; };

    void setText(char *text);
    inline std::vector<unsigned int> &getTextCode() { return textCodes; };

    void setHorizontalAlign(HorizontalTextAlignment align);
    inline const HorizontalTextAlignment &getHorizonAlign() { return _horizontalAlign; };

    void setVerticalAlign(VerticalTextAlignment align);
    inline const VerticalTextAlignment &getVerticalAlign() { return _verticalAlign; };

    void setFontSize(int size);
    inline const int &getFontSize() { return fontSize; };

    void setLineHeight(int height);
    inline const int &getLineHeight() { return lineHeight; };

    void setSpacingX(int value);
    inline const int &getSpacingX() { return spacingX; };

    void setIsBold(bool b);
    inline const bool &getIsBold() { return _isBold; };

    void setIsItalic(bool b);
    inline const bool &getIsItalic() { return _isItalic; };

    void enableWrapText(bool);

    
    /**
     * 设置是否显示下划线
     */
    void setIsUnderline(bool);

    /**
     * 设置下划线条的高度
     */
    void setUnderlineHeihgt(float);
};
