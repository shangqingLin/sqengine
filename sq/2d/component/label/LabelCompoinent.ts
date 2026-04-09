import { Color, sqclass, editorMenu, serializable, type, Vec2 } from "../../../core/index";
import { ComponentType } from "../../../native_binding/index";
import { AssetManager, Font } from "../../../assets/index";
import UIContentComponent from "../UIContentComponent";

export enum HorizontalTextAlignment {
    LEFT = 0,
    CENTER = 1,
    RIGHT = 2
}

export enum VerticalTextAlignment {
    TOP = 0,
    CENTER = 1,
    BOTTOM = 2
}

export enum Overflow {
    NONE = 0,
    CLAMP = 1,
    SHRINK = 2,
    RESIZE_HEIGHT = 3
}

export enum CacheMode {
    NONE = 0,
    BITMAP = 1,
    CHAR = 2
}

/* editor:start */
@editorMenu("2D/LabelComponent")
/* editor:end */
@sqclass("sq.LabelComponent")
export class LabelComponent extends UIContentComponent {
    private _text: string;
    private _font: Font;

    //默认是24
    private _fontSize: number = 24;

    /**
     * 行高
     */
    private _lineHeight: number = 1;

    /**
     * 文本直接的水平距离
     */
    private _spacingX: number;

    private _enableWrapText: boolean;
    private _isBold: boolean;
    private _isItalic: boolean;
    private _isUnderline: boolean;
    private _underlineHeight: number;
    protected _horizontalAlign = HorizontalTextAlignment.CENTER;
    protected _verticalAlign = VerticalTextAlignment.CENTER;
    protected _outlineWidth = 2;
    protected _enableOutline = false;
    protected _enableShadow = false;
    protected _shadowColor = new Color(0, 0, 0, 255);
    protected _shadowOffset = new Vec2(2, 2);
    protected _shadowBlur = 2;
    protected _outlineColor = new Color(0, 0, 0, 255);
    protected _overflow: Overflow = Overflow.NONE;
    
    constructor() {
        super(ComponentType.LabelComponent);
    }

    set text(text: string) {
        if (text === null || text === undefined) {
            text = '';
        } else {
            text = text.toString();
        }
        if (this._text === text) {
            return;
        }
        this._text = text;
        this.nativeBeginOp(50);
        this.nativeWriteOpArg("str", text);
        this.nativeEndOp();
    }

    get text(): string {
        return this._text;
    }


    set fontUrl(url: string) {
        AssetManager.getInstance().load(url, (error: Error | null, font: Font) => {
            this.font = font;
        });
    }

    set font(font: Font) {
        if (this._font !== font) {
            this._font = font;
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("i32", font.getId());
            this.nativeEndOp();
        }
    }

    get font(): Font {
        return this._font;
    }

    get fontSize(): number {
        return this._fontSize || 24;
    }

    set fontSize(size: number) {
        if (this._fontSize !== size) {
            this._fontSize = size;
            this.nativeBeginOp(52);
            this.nativeWriteOpArg("f32", size);
            this.nativeEndOp();
        }
    }

    get lineHeight(): number {
        return this._lineHeight;
    }

    set lineHeight(value) {
        if (this._lineHeight === value) {
            return;
        }
        this._lineHeight = value;
        this.nativeBeginOp(53);
        this.nativeWriteOpArg("f32", value);
        this.nativeEndOp();
    }

    get spacingX(): number {
        return this._spacingX;
    }
    set spacingX(value) {
        if (this._spacingX === value) {
            return;
        }

        this._spacingX = value;
        this.nativeBeginOp(54);
        this.nativeWriteOpArg("f32", value);
        this.nativeEndOp();
    }

    /**
     * 当达到设置的宽度的时候，是否自动换行
     * 默认为false
     */
    get enableWrapText(): boolean {
        return this._enableWrapText;
    }
    set enableWrapText(value) {
        if (this._enableWrapText === value) {
            return;
        }

        this._enableWrapText = value;
        this.nativeBeginOp(55);
        this.nativeWriteOpArg("i8", value);
        this.nativeEndOp();
    }

    get isBold(): boolean {
        return this._isBold;
    }
    set isBold(value) {
        if (this._isBold === value) {
            return;
        }

        this._isBold = value;
        this.nativeBeginOp(56);
        this.nativeWriteOpArg("i8", value);
        this.nativeEndOp();
    }

    get isItalic(): boolean {
        return this._isItalic;
    }
    set isItalic(value) {
        if (this._isItalic === value) {
            return;
        }

        this._isItalic = value;
        this.nativeBeginOp(57);
        this.nativeWriteOpArg("i8", value);
        this.nativeEndOp();
    }

    get isUnderline(): boolean {
        return this._isUnderline;
    }
    set isUnderline(value) {
        if (this._isUnderline === value) {
            return;
        }

        this._isUnderline = value;
        this.nativeBeginOp(58);
        this.nativeWriteOpArg("i8", value);
        this.nativeEndOp();
    }

    /**
     * 如果开启了下划线，则这里设置下划线的高度
     */
    get underlineHeight(): number {
        return this._underlineHeight;
    }
    set underlineHeight(value) {
        if (this._underlineHeight === value) return;
        this._underlineHeight = value;
        this.nativeBeginOp(59);
        this.nativeWriteOpArg("f32", value);
        this.nativeEndOp();
    }

    /**
     * 文本内容真实的宽度
     */
    get contentWidth(): number {
        return 0;
    }

    /**
     * 文本内容真实的高度
     */
    get contentHeight(): number {
        return 0;
    }

    /**
    * 文本内容的水平对齐方式。
    */
    @type(HorizontalTextAlignment)
    @serializable
    get horizontalAlign(): HorizontalTextAlignment {
        return this._horizontalAlign;
    }
    set horizontalAlign(value) {
        if (this._horizontalAlign === value) {
            return;
        }
        this._horizontalAlign = value;
    }


    /**
     * @en
     * Vertical Alignment of label.
     *
     * @zh
     * 文本内容的垂直对齐方式。
     */
    @type(VerticalTextAlignment)
    @serializable
    get verticalAlign(): VerticalTextAlignment {
        return this._verticalAlign;
    }
    set verticalAlign(value) {
        if (this._verticalAlign === value) {
            return;
        }

        this._verticalAlign = value;
    }


    /**
     * 启用文字描边
     **/
    get enableOutline(): boolean {
        return this._enableOutline;
    }
    set enableOutline(value) {
        if (this._enableOutline === value) return;
        this._enableOutline = value;
    }

    /**
     *描边的颜色。
    */
    get outlineColor(): Color {
        return this._outlineColor;
    }
    set outlineColor(value) {
        if (this._outlineColor === value) return;
        this._outlineColor.setFormColor(value);
    }

    /**
     * 如果需要文字描述，则这里指定描边的宽度。
     */
    get outlineWidth(): number {
        return this._outlineWidth;
    }
    set outlineWidth(value) {
        if (this._outlineWidth === value) return;
        this._outlineWidth = value;
    }


    get enableShadow(): boolean {
        return this._enableShadow;
    }
    set enableShadow(value) {
        if (this._enableShadow === value) return;
        this._enableShadow = value;
    }

    /**
     * 阴影的颜色。
     */
    get shadowColor(): Color {
        return this._shadowColor;
    }
    set shadowColor(value) {
        if (this._shadowColor === value) return;
        this._shadowColor.setFormColor(value);
    }

    /**
     * 字体与阴影的偏移。
     */
    get shadowOffset(): Vec2 {
        return this._shadowOffset;
    }
    set shadowOffset(value) {
        if (this._shadowOffset === value) return;
        this._shadowOffset.set(value.x, value.y);
    }

    /**
     * 文字显示超出范围时的处理方式。
     */
    @type(Overflow)
    @serializable
    get overflow(): Overflow {
        return this._overflow;
    }
    set overflow(value) {
        if (this._overflow === value) {
            return;
        }
        this._overflow = value;
    }

}