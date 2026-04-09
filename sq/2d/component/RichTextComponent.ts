import { SpriteAtlas } from "../../assets";
import { Color } from "../../core";
import { HorizontalTextAlignment, VerticalTextAlignment } from "./label/LabelCompoinent";
import UIContentComponent from "./UIContentComponent";

/**
 * 富文本组件
 */

export default class RichTextComponent extends UIContentComponent {

    protected _maxWidth = 0;
    protected _horizontalAlign = HorizontalTextAlignment.LEFT;
    protected _verticalAlign = VerticalTextAlignment.TOP;
    protected _fontSize = 40;
    protected _fontColor: Color = Color.WHITE.clone();
    protected _fontFamily = 'Arial';
    protected _font: any = null;
    protected _lineHeight: number;
    protected _text: string;
    protected _handleTouchEvent = true;
    protected _imageAtlas: SpriteAtlas | null = null;
    
    /**
     * 富文本行高。
     */
    get lineHeight(): number {
        return this._lineHeight;
    }

    set lineHeight(value) {
        if (this._lineHeight === value) {
            return;
        }
        this._lineHeight = value;
    }


    /**
    * 富文本字体大小。
    */
    get fontSize(): number {
        return this._fontSize;
    }

    set fontSize(value) {
        if (this._fontSize === value) {
            return;
        }
        this._fontSize = value;
    }

    /**
     * 富文本的最大宽度。
     */
    get maxWidth(): number {
        return this._maxWidth;
    }

    set maxWidth(value) {
        if (this._maxWidth === value) {
            return;
        }
        this._maxWidth = value;
    }

    /*
    * 富文本定制字体。
    */
    get font(): any | null {
        return this._font;
    }
    set font(value) {
        if (this._font === value) {
            return;
        }
        this._font = value;
    }

    set text(text: string) {
        this._text = text;
    }

    get text(): string {
        return this._text;
    }


    get horizontalAlign(): HorizontalTextAlignment {
        return this._horizontalAlign;
    }

    set horizontalAlign(value) {
        if (this.horizontalAlign === value) {
            return;
        }

        this._horizontalAlign = value;
    }

    get verticalAlign(): VerticalTextAlignment {
        return this._verticalAlign;
    }

    set verticalAlign(value) {
        if (this._verticalAlign === value) {
            return;
        }
        this._verticalAlign = value;
    }

    get handleTouchEvent(): boolean {
        return this._handleTouchEvent;
    }

    set handleTouchEvent(value) {
        if (this._handleTouchEvent === value) {
            return;
        }
        this._handleTouchEvent = value;
    }

    get imageAtlas(): SpriteAtlas | null {
        return this._imageAtlas;
    }

    set imageAtlas(value) {
        if (this._imageAtlas === value) {
            return;
        }

        this._imageAtlas = value
    }
}