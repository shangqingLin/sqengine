import { GComponent } from "./GComponent";
import { GObject } from "./GObject";
import { GTextField } from "./GTextField";
import { GTextInput } from "./GTextInput";
import { Color } from "../core";
import { PackageItem } from "./assest/PackageItem";
import { UIPackage } from "./assest/UIPackage";
import { AudioClip } from "../assets";
import { ObjectPropID } from "./common/FieldTypes";

export class GLabel extends GComponent {
    protected _titleObject: GObject;
    protected _iconObject: GObject;
    private _sound: string;
    private _soundVolumeScale: number;

    public constructor() {
        super();

        this._node.name = "GLabel";
    }

    public override get icon(): string | null {
        if (this._iconObject)
            return this._iconObject.icon;
    }

    public override set icon(value: string | null) {
        if (this._iconObject)
            this._iconObject.icon = value;
        this.updateGear(7);
    }

    public get title(): string | null {
        if (this._titleObject)
            return this._titleObject.text;
        else
            return null;
    }

    public set title(value: string | null) {
        if (this._titleObject)
            this._titleObject.text = value;
        this.updateGear(6);
    }

    public override get text(): string | null {
        return this.title;
    }

    public override set text(value: string | null) {
        this.title = value;
    }

    public get titleColor(): Color {
        var tf: GTextField = this.getTextField();
        if (tf)
            return tf.color;
        else
            return Color.WHITE;
    }

    public set titleColor(value: Color) {
        var tf: GTextField = this.getTextField();
        if (tf)
            tf.color = value;
        this.updateGear(4);
    }

    public get titleFontSize(): number {
        var tf: GTextField = this.getTextField();
        if (tf)
            return tf.fontSize;
        else
            return 0;
    }

    public set titleFontSize(value: number) {
        var tf: GTextField = this.getTextField();
        if (tf)
            tf.fontSize = value;
    }

    public set editable(val: boolean) {
        if (this._titleObject && (this._titleObject instanceof GTextInput))
            this._titleObject.editable = val;
    }

    public get editable(): boolean {
        if (this._titleObject && (this._titleObject instanceof GTextInput))
            return this._titleObject.editable;
        else
            return false;
    }

    public getTextField(): GTextField {
        if (this._titleObject instanceof GTextField)
            return this._titleObject;
        else if ('getTextField' in this._titleObject)
            return (<any>this._titleObject).getTextField();
        else
            return null;
    }

    public override getProp(index: number): any {
        switch (index) {
            case ObjectPropID.Color:
                return this.titleColor;
            case ObjectPropID.OutlineColor:
                {
                    var tf: GTextField = this.getTextField();
                    if (tf)
                        return tf.strokeColor;
                    else
                        return 0;
                }
            case ObjectPropID.FontSize:
                return this.titleFontSize;
            default:
                return super.getProp(index);
        }
    }

    public override setProp(index: number, value: any): void {
        switch (index) {
            case ObjectPropID.Color:
                this.titleColor = value;
                break;
            case ObjectPropID.OutlineColor:
                {
                    var tf: GTextField = this.getTextField();
                    if (tf)
                        tf.strokeColor = value;
                }
                break;
            case ObjectPropID.FontSize:
                this.titleFontSize = value;
                break;
            default:
                super.setProp(index, value);
                break;
        }
    }

    private onClick_1(): void {
        if (this._sound) {
            var pi: PackageItem = UIPackage.getItemByURL(this._sound);
            if (pi) {
                // var sound: AudioClip = <AudioClip>pi.owner.getItemAsset(pi);
                // if (sound)
                //     GRoot.inst.playOneShotSound(sound, this._soundVolumeScale);
            }
        }
    }
}