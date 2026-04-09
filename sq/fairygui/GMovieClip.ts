import { SpriteSizeMode } from "../2d";
import { Color } from "../core";
import { ObjectPropID } from "./common/FieldTypes";
import { MovieClip } from "./display/MovieClip";
import { GObject } from "./GObject";

export class GMovieClip extends GObject {
    public _content: MovieClip;

    public constructor() {
        super();
        this._node.name = "GMovieClip";
        this._touchDisabled = true;
    }

    protected override onCreateUIContent(): void {
        this._uiTrans = this._content = this._node.addComponent(MovieClip);
        this._content.sizeMode = SpriteSizeMode.CUSTOM;
        // this._content.trim = false;
        this._content.setPlaySettings();
    }

    public get color(): Color {
        return this._content.getColor();
    }

    public set color(value: Color) {
        this._content.setColor(value);
        this.updateGear(4);
    }

    public get playing(): boolean {
        return this._content.playing;
    }

    public set playing(value: boolean) {
        if (this._content.playing != value) {
            this._content.playing = value;
            this.updateGear(5);
        }
    }

    public get frame(): number {
        return this._content.frame;
    }

    public set frame(value: number) {
        if (this._content.frame != value) {
            this._content.frame = value;
            this.updateGear(5);
        }
    }

    public get timeScale(): number {
        return this._content.timeScale;
    }

    public set timeScale(value: number) {
        this._content.timeScale = value;
    }

    public rewind(): void {
        this._content.rewind();
    }

    public syncStatus(anotherMc: GMovieClip): void {
        this._content.syncStatus(anotherMc._content);
    }

    public advance(timeInSeconds: number): void {
        this._content.advance(timeInSeconds);
    }

    //从start帧开始，播放到end帧（-1表示结尾），重复times次（0表示无限循环），循环结束后，停止在endAt帧（-1表示参数end）
    public setPlaySettings(start?: number, end?: number, times?: number, endAt?: number, endCallback?: (() => void) | null): void {
        this._content.setPlaySettings(start, end, times, endAt, endCallback);
    }

    protected override handleGrayedChanged(): void {
        this._content.gray = this._grayed;
    }

    protected override handleSizeChanged(): void {
        super.handleSizeChanged();

        //不知道原因，尺寸改变必须调用一次这个，否则大小不对
        this._content.sizeMode = SpriteSizeMode.CUSTOM;
    }

    public override getProp(index: number): any {
        switch (index) {
            case ObjectPropID.Color:
                return this.color;
            case ObjectPropID.Playing:
                return this.playing;
            case ObjectPropID.Frame:
                return this.frame;
            case ObjectPropID.TimeScale:
                return this.timeScale;
            default:
                return super.getProp(index);
        }
    }

    public override setProp(index: number, value: any): void {
        switch (index) {
            case ObjectPropID.Color:
                this.color = value;
                break;
            case ObjectPropID.Playing:
                this.playing = value;
                break;
            case ObjectPropID.Frame:
                this.frame = value;
                break;
            case ObjectPropID.TimeScale:
                this.timeScale = value;
                break;
            case ObjectPropID.DeltaTime:
                this.advance(value);
                break;
            default:
                super.setProp(index, value);
                break;
        }
    }
}