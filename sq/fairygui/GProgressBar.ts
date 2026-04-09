import { math } from "../core";
import { FillMethod, ObjectPropID, ProgressTitleType } from "./common/FieldTypes";
import { GComponent } from "./GComponent";
import { GImage } from "./GImage";
import { GLoader } from "./GLoader";
import { GObject } from "./GObject";
import { EaseType } from "./tween/EaseType";
import { GTween } from "./tween/GTween";
import { GTweener } from "./tween/GTweener";

export class GProgressBar extends GComponent {

    /**
     * 最大和最小的值范围，默认是0到100
     */
    private _min: number = 0;
    private _max: number = 0;

    /**
     * 当前进度值
     */
    private _value: number = 0;

    /**
     * 进度文本显示的类型
     * 
     */
    private _titleType: ProgressTitleType;

    /**
     * 是否是反向推进。默认是进度是从左往右
     */
    private _reverse: boolean;

    //进度文本组件（GTextField）
    private _titleObject: GObject;
    private _aniObject: GObject;

    //如果进度条是水平方向的，则这个对象是水平方向缩放的那部分
    private _barObjectH: GObject;

    //如果进度条是垂直方向的，则这个对象是垂直方向缩放的那部分
    private _barObjectV: GObject;

    //如果是水平方向的进度条，这个是_barObjectH的宽度
    private _barMaxWidth: number = 0;

    //宽度起始值
    private _barStartX: number = 0;

    //_barMaxWidth和整条进度条组件相差多少长度，即 _barMaxWidthDelta =this.width - this._barMaxWidth
    private _barMaxWidthDelta: number = 0;


    //如果是垂直方向的进度条，这个是_barObjectV的高度
    private _barMaxHeight: number = 0;

    //高度的起始值
    private _barStartY: number = 0;

    //即_barMaxHeightDelta = this.height - this._barMaxHeight
    private _barMaxHeightDelta: number = 0;

    public constructor() {
        super();

        this._node.name = "GProgressBar";
        this._titleType = ProgressTitleType.Percent;
        this._value = 50;
        this._max = 100;
    }

    public get titleType(): ProgressTitleType {
        return this._titleType;
    }

    public set titleType(value: ProgressTitleType) {
        if (this._titleType != value) {
            this._titleType = value;
            this.update(this._value);
        }
    }

    public get min(): number {
        return this._min;
    }

    public set min(value: number) {
        if (this._min != value) {
            this._min = value;
            this.update(this._value);
        }
    }

    public get max(): number {
        return this._max;
    }

    public set max(value: number) {
        if (this._max != value) {
            this._max = value;
            this.update(this._value);
        }
    }

    public get value(): number {
        return this._value;
    }

    public set value(value: number) {

        if (this._value != value) {
            GTween.kill(this, false, this.update);

            this._value = value;
            this.update(value);
        }
    }

    public tweenValue(value: number, duration: number): GTweener {
        var oldValule: number;

        var tweener: GTweener = GTween.getTween(this, this.update);
        if (tweener) {
            oldValule = tweener.value.x;
            tweener.kill();
        }
        else
            oldValule = this._value;

        this._value = value;
        return GTween.to(oldValule, this._value, duration).setTarget(this, this.update).setEase(EaseType.Linear);
    }

    public update(newValue: number): void {
        var percent: number = math.clamp01((newValue - this._min) / (this._max - this._min));
        if (this._titleObject) {
            switch (this._titleType) {
                case ProgressTitleType.Percent:
                    this._titleObject.text = Math.floor(percent * 100) + "%";
                    break;

                case ProgressTitleType.ValueAndMax:
                    this._titleObject.text = Math.floor(newValue) + "/" + Math.floor(this._max);
                    break;

                case ProgressTitleType.Value:
                    this._titleObject.text = "" + Math.floor(newValue);
                    break;

                case ProgressTitleType.Max:
                    this._titleObject.text = "" + Math.floor(this._max);
                    break;
            }
        }

        var fullWidth: number = this.width - this._barMaxWidthDelta;
        var fullHeight: number = this.height - this._barMaxHeightDelta;
        if (!this._reverse) {
            if (this._barObjectH) {
                if (!this.setFillAmount(this._barObjectH, percent))
                    this._barObjectH.width = Math.round(fullWidth * percent);
            }
            if (this._barObjectV) {
                if (!this.setFillAmount(this._barObjectV, percent))
                    this._barObjectV.height = Math.round(fullHeight * percent);
            }
        }
        else {
            if (this._barObjectH) {
                if (!this.setFillAmount(this._barObjectH, 1 - percent)) {
                    this._barObjectH.width = Math.round(fullWidth * percent);
                    this._barObjectH.x = this._barStartX + (fullWidth - this._barObjectH.width);
                }

            }
            if (this._barObjectV) {
                if (!this.setFillAmount(this._barObjectV, 1 - percent)) {
                    this._barObjectV.height = Math.round(fullHeight * percent);
                    this._barObjectV.y = this._barStartY + (fullHeight - this._barObjectV.height);
                }
            }
        }
        if (this._aniObject)
            this._aniObject.setProp(ObjectPropID.Frame, Math.floor(percent * 100));
    }

    private setFillAmount(bar: GObject, percent: number): boolean {
        if (((bar instanceof GImage) || (bar instanceof GLoader)) && bar.fillMethod != FillMethod.None) {
            bar.fillAmount = percent;
            return true;
        }
        else
            return false;
    }


    protected override handleSizeChanged(): void {
        super.handleSizeChanged();

        if (this._barObjectH)
            this._barMaxWidth = this.width - this._barMaxWidthDelta;
        if (this._barObjectV)
            this._barMaxHeight = this.height - this._barMaxHeightDelta;
        if (!this._underConstruct)
            this.update(this._value);
    }
}