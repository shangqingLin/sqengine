import { Vec2 } from "../core";
import { EventMouse } from "../input/Events";
import { GComponent } from "./GComponent";
import { GObject } from "./GObject";
import { ScrollPane } from "./ScrollPane";

export class GScrollBar extends GComponent {

    /**
     * 滚动条中间的那个按钮条。用于只是ScrollPanel内容有多少的
     */
    private _grip: GObject;

    /**
     * 滚动条（上下或左右）的按钮
     */
    private _arrowButton1: GObject;
    private _arrowButton2: GObject;

    /**
     * 滚动条背景
     */
    private _bar: GObject;

    private _target: ScrollPane;

    /**
     * 1、如果为true，表示这个滚动条为垂直方向的
     * 2、如果为false，表示这个滚动条为水平方向的
     */
    private _vertical: boolean;


    private _scrollPerc: number;
    private _fixedGripSize: boolean;

    private _dragOffset: Vec2;
    private _gripDragging: boolean;

    public constructor() {
        super();

        this._node.name = "GScrollBar";
        this._dragOffset = new Vec2();
        this._scrollPerc = 0;
    }

    public setScrollPane(target: ScrollPane, vertical: boolean): void {
        this._target = target;
        this._vertical = vertical;
    }

    /**
     * 设置grip的长度。
     * 用来实现ScrollPanel内容越长，按钮越短表示ScrollPanel中的内容多
     * @param value 范围是[0,1]
     */
    public setDisplayPerc(value: number) {
        if (this._vertical) {
            if (!this._fixedGripSize)
                this._grip.height = Math.floor(value * this._bar.height);

            //设置_grip的位置
            this._grip.y = this._bar.y + (this._bar.height - this._grip.height) * this._scrollPerc;

        }
        else {
            if (!this._fixedGripSize)
                this._grip.width = Math.floor(value * this._bar.width);

            this._grip.x = this._bar.x + (this._bar.width - this._grip.width) * this._scrollPerc;
        }

        //按钮长度为0或满长度时，直接隐藏
        this._grip.visible = value !== 0 && value !== 1;
    }

    /**
     * 设置滚动条的位置
     * @param val 范围是[0,1]。0处于最开始，1处于最底端
     */
    public setScrollPerc(val: number) {
        this._scrollPerc = val;
        if (this._vertical)
            this._grip.y = this._bar.y + (this._bar.height - this._grip.height) * this._scrollPerc;
        else
            this._grip.x = this._bar.x + (this._bar.width - this._grip.width) * this._scrollPerc;
    }

    public get minSize(): number {
        if (this._vertical)
            return (this._arrowButton1 ? this._arrowButton1.height : 0) + (this._arrowButton2 ? this._arrowButton2.height : 0);
        else
            return (this._arrowButton1 ? this._arrowButton1.width : 0) + (this._arrowButton2 ? this._arrowButton2.width : 0);
    }

    public get gripDragging(): boolean {
        return this._gripDragging;
    }

    private onGripTouchDown(evt: EventMouse): void {
        evt.stopPropagation();

        this._gripDragging = true;
        this._target.updateScrollBarVisible();

        this.globalToLocal(evt.worldX, evt.worldY, this._dragOffset);
        this._dragOffset.x -= this._grip.x;
        this._dragOffset.y -= this._grip.y;
    }

    private onGripTouchMove(evt: EventMouse): void {
        if (!this.onStage)
            return;

        var pt: Vec2 = this.globalToLocal(evt.worldX, evt.worldY, s_vec2);
        if (this._vertical) {
            var curY: number = pt.y - this._dragOffset.y;
            this._target.setPercY((curY - this._bar.y) / (this._bar.height - this._grip.height), false);
        }
        else {
            var curX: number = pt.x - this._dragOffset.x;
            this._target.setPercX((curX - this._bar.x) / (this._bar.width - this._grip.width), false);
        }
    }

    private onGripTouchEnd(evt: EventMouse): void {
        if (!this.onStage)
            return;

        this._gripDragging = false;
        this._target.updateScrollBarVisible();
    }

    private onClickArrow1(evt: EventMouse): void {
        evt.stopPropagation();

        if (this._vertical)
            this._target.scrollUp();
        else
            this._target.scrollLeft();
    }

    private onClickArrow2(evt: EventMouse): void {
        evt.stopPropagation();
        if (this._vertical)
            this._target.scrollDown();
        else
            this._target.scrollRight();
    }

    private onBarTouchBegin(evt: EventMouse): void {
        evt.stopPropagation();
        var pt: Vec2 = this._grip.globalToLocal(evt.worldX, evt.worldY, s_vec2);
        if (this._vertical) {
            if (pt.y < 0)
                this._target.scrollUp(4);
            else
                this._target.scrollDown(4);
        }
        else {
            if (pt.x < 0)
                this._target.scrollLeft(4);
            else
                this._target.scrollRight(4);
        }
    }
}

var s_vec2: Vec2 = new Vec2();