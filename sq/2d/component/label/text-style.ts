
import { Color } from '../../../core';

export class TextStyle {
    // ---------------ttf extra part-----------------
    // bold // style
    public isBold = false; // ttf
    // Italic // style
    public isItalic = false; // ttf
    // under line // style
    public isUnderline = false; // ttf
    public underlineHeight = 1; // ttf
    // outline style
    public isOutlined = false; // both // ttf & char
    public outlineColor = Color.WHITE.clone(); // both // ttf & char
    public outlineWidth = 1; // both // ttf & char
    // shadowStyle
    public hasShadow = false; // ttf
    public shadowColor = Color.BLACK.clone(); // ttf
    public shadowBlur = 2; // ttf
    public shadowOffsetX = 0; // ttf
    public shadowOffsetY = 0; // ttf

    public color = Color.WHITE.clone(); // both

    public fontSize = 40; // input fonSize // both
    public actualFontSize = 0; // both

    public isSystemFontUsed = false; // both // ttf & char

    // -----------------------bitMap extra part-------------------------

    public originFontSize = 0; //Layout // both
    public bmfontScale = 1.0;// both

    // font info // todo merge to font
    public fontFamily = 'Arial'; // both
    public fontDesc = ''; // both

    // -----------------------bitMap extra part-------------------------

    public fontScale = 1;

    public reset (): void {
        this.isBold = false;
        this.isItalic = false;
        this.isUnderline = false;
        this.underlineHeight = 1;
        this.isOutlined = false;
        this.outlineColor.set();
        this.outlineWidth = 1;
        this.hasShadow = false;
        this.shadowColor.set();
        this.shadowBlur = 2;
        this.shadowOffsetX = 0;
        this.shadowOffsetY = 0;
    }
}
