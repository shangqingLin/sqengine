import { SpriteSizeMode, SpriteType } from "../2d";
import { SpriteFrame } from "../assets";
import { Color } from "../core";
import { FillMethod, FillOrigin, FlipType, ObjectPropID } from "./common/FieldTypes";
import { Image } from "./display/Image";
import { GObject } from "./GObject";

export class GImage extends GObject {
    public _content: Image;

    public constructor() {
        super();
        this._node.name = "GImage";
        this._touchDisabled = true;
    }

    protected override onCreateUIContent(): void {
        this._uiTrans = this._content = this._node.addComponent(Image);
        this._content.sizeMode = SpriteSizeMode.CUSTOM;
        // this._content.trim = false;
        // console.info( "GImage Node", this._node.id);
    }

    public get color(): Color {
        return this._content.getColor();
    }

    public set color(value: Color) {
        this._content.setColor(value);
        this.updateGear(4);
    }

    public get flip(): FlipType {
        return this._content.flip;
    }

    public set flip(value: FlipType) {
        this._content.flip = value;
    }

    public get fillMethod(): FillMethod {
        return this._content.fillMethod;
    }

    public set fillMethod(value: FillMethod) {
        this._content.fillMethod = value;
    }

    public get fillOrigin(): FillOrigin {
        return this._content.fillOrigin;
    }

    public set fillOrigin(value: FillOrigin) {
        this._content.fillOrigin = value;
    }

    public get fillClockwise(): boolean {
        return this._content.fillClockwise;
    }

    public set fillClockwise(value: boolean) {
        this._content.fillClockwise = value;
    }

    public get fillAmount(): number {
        return this._content.fillAmount;
    }

    public set fillAmount(value: number) {
        this._content.fillAmount = value;
    }

    protected override handleGrayedChanged(): void {
        //this._content.grayscale = this._grayed;
    }

    public override getProp(index: number): any {
        if (index == ObjectPropID.Color)
            return this.color;
        else
            return super.getProp(index);
    }
}