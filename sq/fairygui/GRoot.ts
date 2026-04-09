import { UIContentComponent } from "../2d";
import { Vec2 } from "../core";
import { screen, ScreenEvent } from "../platform";
import { NodeEventType } from "../scene";
import ObjectCreateProcess from "./assest/ObjectCreateProcess";
import { PopupDirection } from "./common/FieldTypes";
import type { GObject } from "./GObject";
import { GComponent } from "./GComponent";
import { GGraph } from "./GGraph";


/**
 * 抽象了整个FGUI的根节点，这个根节点将UI绘制坐标系挪到屏幕左上角（x正轴向右，Y正轴向下，与编辑器一直）
 * 所有的FGUI节点都需要添加到这里创建的根节点之下
 */
export class GRoot extends GComponent {
    private _modalLayer: GGraph;
    private _popupStack: Array<GObject>;
    private _justClosedPopups: Array<GObject>;
    private _modalWaitPane: GObject;
    private static _inst: GRoot;

    static initialize() {
        ObjectCreateProcess.initialize();
        this.create();
    }

    public static get inst(): GRoot {
        if (!GRoot._inst)
            throw 'Call GRoot.create first!';

        return GRoot._inst;
    }

    public static create(): GRoot {
        GRoot._inst = new GRoot();
        return GRoot._inst;
    }

    public constructor() {
        super();
        this._node.name = "GRoot";
        this.opaque = false;
        this._popupStack = new Array<GObject>();
        this._justClosedPopups = new Array<GObject>();
        this.onCreateCoord();
        screen.on(ScreenEvent.RENDER_SIZE_CHANGE, this, this.onCreateCoord);
        this.node.on(NodeEventType.NODE_ADD_PARENT, this, this.onCreateCoord);
        this.node.on(NodeEventType.NODE_REMOVE_PARENT, this, this.onCreateCoord);
    }

    private onCreateCoord() {

        //构建fgui坐标系：原点在左上角，并且这个左上角的点在父节点的锚点处
        //如果没有父节点，则这个左上角的点在屏幕左上角
        let renderSize = screen.getRenderSize();
        this.setSize(renderSize.width, renderSize.height);
        let anchorPoint = Vec2.temp1Vec2;
        anchorPoint.set(0.5, 0.5);
        if (this.node.parent) {
            let uicontent = this.node.parent.getComponent(UIContentComponent);
            if (uicontent) {
                anchorPoint.set(uicontent.anchorX, uicontent.anchorY);
            }
        }
        this.node.transform.setPosition(-this._width * anchorPoint.x, this._height * (1 - anchorPoint.y));
    }
    public override handlePositionChanged() {
        //nothing here
    }

    public getPopupPosition(popup: GObject, target?: GObject, dir?: PopupDirection | boolean, result?: Vec2): Vec2 {
        let pos = result || new Vec2();
        var sizeW: number = 0, sizeH: number = 0;
        if (target) {
            pos = target.localToGlobal();
            this.globalToLocal(pos.x, pos.y, pos);
            let pos2 = target.localToGlobal(target.width, target.height);
            this.globalToLocal(pos2.x, pos2.y, pos2);
            sizeW = pos2.x - pos.x;
            sizeH = pos2.y - pos.y;
        }
        else {
            pos = this.globalToLocal(pos.x, pos.y);
        }

        if (pos.x + popup.width > this.width)
            pos.x = pos.x + sizeW - popup.width;
        pos.y += sizeH;
        if (((dir === undefined || dir === PopupDirection.Auto) && pos.y + popup.height > this.height)
            || dir === false || dir === PopupDirection.Up) {
            pos.y = pos.y - sizeH - popup.height - 1;
            if (pos.y < 0) {
                pos.y = 0;
                pos.x += sizeW / 2;
            }
        }

        return pos;
    }

    public showPopup(popup: GObject, target?: GObject | null, dir?: PopupDirection | boolean): void {
        if (this._popupStack.length > 0) {
            var k: number = this._popupStack.indexOf(popup);
            if (k != -1) {
                for (var i: number = this._popupStack.length - 1; i >= k; i--)
                    this.removeChild(this._popupStack.pop());
            }
        }
        this._popupStack.push(popup);

        if (target) {
            var p: GObject = target;
            while (p) {
                if (p.parent == this) {
                    if (popup.sortingOrder < p.sortingOrder) {
                        popup.sortingOrder = p.sortingOrder;
                    }
                    break;
                }
                p = p.parent;
            }
        }

        this.addChild(popup);
        this.adjustModalLayer();

        let pt = this.getPopupPosition(popup, target, dir);
        popup.setPosition(pt.x, pt.y);
    }

    public togglePopup(popup: GObject, target?: GObject, dir?: PopupDirection | boolean): void {
        if (this._justClosedPopups.indexOf(popup) != -1)
            return;

        this.showPopup(popup, target, dir);
    }

    public hidePopup(popup?: GObject): void {
        if (popup) {
            var k: number = this._popupStack.indexOf(popup);
            if (k != -1) {
                for (var i: number = this._popupStack.length - 1; i >= k; i--)
                    this.closePopup(this._popupStack.pop());
            }
        }
        else {
            var cnt: number = this._popupStack.length;
            for (i = cnt - 1; i >= 0; i--)
                this.closePopup(this._popupStack[i]);
            this._popupStack.length = 0;
        }
    }

    public get hasAnyPopup(): boolean {
        return this._popupStack.length != 0;
    }

    private closePopup(target: GObject): void {
        if (target.parent) {
            if (target instanceof Window)
                target.hide();
            else
                this.removeChild(target);
        }
    }


    private adjustModalLayer(): void {
        var cnt: number = this.numChildren;

        if (this._modalWaitPane && this._modalWaitPane.parent)
            this.setChildIndex(this._modalWaitPane, cnt - 1);

        for (var i: number = cnt - 1; i >= 0; i--) {
            var g: GObject = this.getChildAt(i);
            if ((g instanceof Window) && g.modal) {
                if (this._modalLayer.parent == null)
                    this.addChildAt(this._modalLayer, i);
                else
                    this.setChildIndexBefore(this._modalLayer, i);
                return;
            }
        }

        if (this._modalLayer.parent)
            this.removeChild(this._modalLayer);
    }
}

window.GRoot = GRoot;