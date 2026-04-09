import { Transform2DComponent, UIContentComponent } from "../2d";
import { Rect, Vec2 } from "../core";
import { Component } from "../framework";
import { Node, NodeEventType } from "../scene";
import { PackageItem } from "./assest/PackageItem";
import { ObjectPropID } from "./common/FieldTypes";
import { Margin } from "./common/Margin";
import { Controller } from "./Controller";
import { BlendMode } from "./display/BlendMode";
import { FUIEvent } from "./FUIEvent";
import { GComponent } from "./GComponent";
import { GearAnimation } from "./gear/GearAnimation";
import { GearBase } from "./gear/GearBase";
import { GearColor } from "./gear/GearColor";
import { GearDisplay } from "./gear/GearDisplay";
import { GearDisplay2 } from "./gear/GearDisplay2";
import { GearFontSize } from "./gear/GearFontSize";
import { GearIcon } from "./gear/GearIcon";
import { GearLook } from "./gear/GearLook";
import { GearSize } from "./gear/GearSize";
import { GearText } from "./gear/GearText";
import { GearXY } from "./gear/GearXY";
import type { GGroup } from "./GGroup";
import { GTreeNode } from "./GTreeNode";
import { Relations } from "./Relations";

let GearClasses: Array<typeof GearBase> = [
    GearDisplay, //0
    GearXY, // 1
    GearSize, //2
    GearLook, //3
    GearColor,//4
    GearAnimation,//5
    GearText,//6
    GearIcon,//7
    GearDisplay2,//8
    GearFontSize//9
];

function createGear(owner: GObject, index: number): GearBase {
    let ret = new (GearClasses[index])();
    ret._owner = owner;
    return ret;
}


/**
 * 监听Node节点Active的情况
 */
export class GObjectPartner extends Component {
    public _emitDisplayEvents?: boolean;

    protected override onEnabled() {
        (<any>this.node)["$gobj"].onEnable();
        if (this._emitDisplayEvents)
            this.node.fire(FUIEvent.DISPLAY);
    }

    protected override onDisabled() {
        (<any>this.node)["$gobj"].onDisable();

        if (this._emitDisplayEvents)
            this.node.fire(FUIEvent.UNDISPLAY);
    }

    protected override onRemove() {
        (<any>this.node)["$gobj"].onDestroy();
    }
}

export class GObject {
    public data?: any;

    protected _node: Node;
    public name: string;

    /**
     * 这个Object所属的文件的内容，只有xml、Image、Font等有具体的文件的这个属性才有值
     */
    public readonly packageItem?: PackageItem;

    /**
     * 处理当前的GObject所有的关联
     */
    protected _relations: Relations;

    /**
     * 属性控制器。
     * 存储的顺序按照上面的GearClasses注册的顺序，在运行过程中就是通过这个顺序index执行对应属性类型的控制器
     */
    protected _gears: GearBase[];

    /**
     * 所在的组
     */
    protected _group: GGroup | null;

    protected _uiTrans: UIContentComponent;
    protected _transform: Transform2DComponent;

    protected _x: number = 0;
    protected _y: number = 0;
    protected _skewX: number = 0;
    protected _skewY: number = 0;
    protected _alpha: number = 1;
    protected _touchable: boolean = true;
    protected _grayed?: boolean;

    protected _visible: boolean = true;
    protected _internalVisible: boolean = true;
    public _underConstruct: boolean = false;
    public _parent: GComponent | null;
    protected _blendMode: BlendMode;

    //fgui文件中的“边缘”设置
    protected _margin: Margin;

    //
    public sourceWidth: number = 0;
    public sourceHeight: number = 0;
    public initWidth: number = 0;
    public initHeight: number = 0;

    //fgui中对GObject设置的大小,可以超出Min和Max设置的大小
    public _rawWidth: number = 0;
    public _rawHeight: number = 0;

    //设置到Node中最终的大小
    public _width: number = 0;
    public _height: number = 0;

    //fgui中设置的最大尺寸。
    //控制上面的_width和_height不能超出这个范围。
    public minWidth: number = 0;
    public minHeight: number = 0;
    public maxWidth: number = 0;
    public maxHeight: number = 0;


    /**
     * 锚点是否作为元件的锚点。
     * 1、如果为true，则GObject上的锚点同时影响旋转中心和Node上的原点
     * 2、如果为false，则你在GObject上设置的锚点只会作为旋转中心，而不是影响Node上的原点不会影响位置position的设置
     */
    protected _pivotAsAnchor?: boolean;

    //在Gear内部使用，标记当前的Gear Apply中不能进行其他操作
    public _gearLocked: boolean;

    public _touchDisabled?: boolean;

    private _handlingController: boolean;

    /**
     * 控制节点的渲染顺序。
     * 这个是Fgui中控制的,非引擎的；通过调整在节点树的位置从而控制渲染顺序
     */
    protected _sortingOrder: number = 0;


    protected _partner: GObjectPartner;

    //为了实现GObject与Group交互的属性
    public _sizePercentInGroup: number = 0;


    //为了配合实现GTree添加的属性
    public _treeNode?: GTreeNode;

    constructor() {
        this._node = new Node();
        (<any>this._node)["$gobj"] = this;
        this._transform = this.node.getComponent(Transform2DComponent);
        this._node.on(NodeEventType.ANCHOR_CHANGE, this, this._onNodeAnchorChange);
        this._relations = new Relations(this);
        this._gears = new Array<GearBase>(10);
        this._partner = this._node.addComponent(GObjectPartner);
        this.onCreateUIContent();
        this._uiTrans.setAnchorPoint(0, 1);
    }

    protected onCreateUIContent() {
        this._uiTrans = this._node.addComponent(UIContentComponent);
    }



    public static cast(obj: Node): GObject {
        return (<any>obj)["$gobj"];
    }

    get node(): Node {
        return this._node;
    }

    public get id(): string | number {
        return this._node.id;
    }

    public get parent(): GComponent {
        return this._parent;
    }

    public removeFromParent(): void {
        if (this._parent)
            this._parent.removeChild(this);
    }

    public findParent(): GObject {
        if (this._parent)
            return this._parent;

        //可能有些不直接在children里，但node挂着的
        let pn: Node = this._node.parent;
        while (pn) {
            let gobj = (<any>pn)["$gobj"];
            if (gobj)
                return gobj;

            pn = pn.parent;
        }
        return null;
    }

    private _onNodeAnchorChange() {

    }

    public on(type: string | number, listener: Function, target?: any): void {
        if (type == FUIEvent.DISPLAY || type == FUIEvent.UNDISPLAY)
            this._partner._emitDisplayEvents = true;

        this._node.on(type, target, listener);
    }

    public once(type: string | number, listener: Function, target?: any): void {
        if (type == FUIEvent.DISPLAY || type == FUIEvent.UNDISPLAY)
            this._partner._emitDisplayEvents = true;

        this._node.once(type, target, listener);
    }

    public off(type: string | number, listener?: Function, target?: any): void {
        this._node.off(type, target, listener);
    }

    //在各种Gear中调用
    public addDisplayLock(): number {
        var gearDisplay: GearDisplay = <GearDisplay>this._gears[0];
        if (gearDisplay && gearDisplay.controller) {
            var ret: number = gearDisplay.addLock();
            this.checkGearDisplay();

            return ret;
        }
        else
            return 0;
    }

    //在各种Gear中调用
    public releaseDisplayLock(token: number): void {
        var gearDisplay: GearDisplay = <GearDisplay>this._gears[0];
        if (gearDisplay && gearDisplay.controller) {
            gearDisplay.releaseLock(token);
            this.checkGearDisplay();
        }
    }


    private checkGearDisplay(): void {
        if (this._handlingController)
            return;

        var connected: boolean = this._gears[0] == null || (<GearDisplay>this._gears[0]).connected;
        if (this._gears[8])
            connected = (<GearDisplay2>this._gears[8]).evaluate(connected);

        if (connected != this._internalVisible) {
            this._internalVisible = connected;
            this.handleVisibleChanged();

            if (this._group && this._group.excludeInvisibles)
                this._group.setBoundsChangedFlag();
        }
    }

    public get gearXY(): GearXY {
        return <GearXY>this.getGear(1);
    }

    public get gearSize(): GearSize {
        return <GearSize>this.getGear(2);
    }

    public get gearLook(): GearLook {
        return <GearLook>this.getGear(3);
    }

    public get relations(): Relations {
        return this._relations;
    }

    public addRelation(target: GObject, relationType: number, usePercent?: boolean): void {
        this._relations.add(target, relationType, usePercent);
    }

    public removeRelation(target: GObject, relationType: number): void {
        this._relations.remove(target, relationType);
    }

    //在Relations中调用
    private updateGearFromRelations(index: number, dx: number, dy: number): void {
        if (this._gears[index])
            this._gears[index].updateFromRelations(dx, dy);
    }

    public get x(): number {
        return this._x;
    }

    public set x(value: number) {
        this.setPosition(value, this._y);
    }

    public get y(): number {
        return this._y;
    }

    public set y(value: number) {
        this.setPosition(this._x, value);
    }


    public setPosition(xv: number, yv: number): void {
        if (this._x != xv || this._y != yv) {
            var dx: number = xv - this._x;
            var dy: number = yv - this._y;
            this._x = xv;
            this._y = yv;

            this.handlePositionChanged();

            //用于GGroup
            this.moveChildren(dx, dy);

            this.updateGear(1);

            if (this._parent && !("setVirtual" in this._parent)/*not list*/) {
                this._parent.setBoundsChangedFlag();
                if (this._group)
                    this._group.setBoundsChangedFlag(true);
                this._node.fire(FUIEvent.XY_CHANGED, this);
            }
        }
    }

    protected moveChildren(dx: number, dy: number) {
        //用于GGroup
    }

    protected handleAnchorChanged(): void {
        this.handlePositionChanged();
    }

    protected handlePositionChanged(): void {
        var xv: number = this._x;

        /**
         * fgui是以左上角为原点，向右为x轴正，向下为y轴正
         */
        var yv: number = -this._y;

        // console.info(this.name, this._x, this._y);

        //如果锚点不作为定位点，那么还是相对于左上角进行定位
        if (!this._pivotAsAnchor) {
            xv += this._uiTrans.anchorX * this._width;
            yv -= (1 - this._uiTrans.anchorY) * this._height;
        }

        this._transform.x = xv;
        this._transform.y = yv;
    }

    protected updateGear(index: number): void {
        if (this._underConstruct || this._gearLocked)
            return;

        var gear: GearBase = this._gears[index];
        if (gear && gear.controller)
            gear.updateState();
    }

    public getGear(index: number): GearBase {
        var gear: GearBase = this._gears[index];
        if (!gear)
            this._gears[index] = gear = createGear(this, index);
        return gear;
    }

    public checkGearController(index: number, c: Controller): boolean {
        return this._gears[index] && this._gears[index].controller === c;
    }

    public ensureSizeCorrect(): void { }

    public get width(): number {
        this.ensureSizeCorrect();
        if (this._relations.sizeDirty)
            this._relations.ensureRelationsSizeCorrect();
        return this._width;
    }

    public set width(value: number) {
        this.setSize(value, this._rawHeight);
    }

    public get height(): number {
        this.ensureSizeCorrect();
        if (this._relations.sizeDirty)
            this._relations.ensureRelationsSizeCorrect();
        return this._height;
    }

    public set height(value: number) {
        this.setSize(this._rawWidth, value);
    }

    public setSize(wv: number, hv: number, ignorePivot?: boolean): void {
        if (this._rawWidth == wv && this._rawHeight === hv) return;

        this._rawWidth = wv;
        this._rawHeight = hv;

        //将实际Node的大小控制在Min和Max的范围内
        if (wv < this.minWidth)
            wv = this.minWidth;
        if (hv < this.minHeight)
            hv = this.minHeight;
        if (this.maxWidth > 0 && wv > this.maxWidth)
            wv = this.maxWidth;
        if (this.maxHeight > 0 && hv > this.maxHeight)
            hv = this.maxHeight;

        //计算新的大小相对于旧的大小更改了多少
        var dWidth: number = wv - this._width;
        var dHeight: number = hv - this._height;
        this._width = wv;
        this._height = hv;

        this.handleSizeChanged();


        //如果当前Node的锚点不在左上角,同时这个锚点不影响定位，即还是以fgui的左上角作为节点的原点
        //则这里由于宽高改变了，需要重新将位置调整到相对于左上角进行定位。
        if ((this._uiTrans.anchorX != 0 || this._uiTrans.anchorY != 1) && !this._pivotAsAnchor && !ignorePivot)
            this.setPosition(this.x - this._uiTrans.anchorX * dWidth, this.y - (1 - this._uiTrans.anchorY) * dHeight);
        else
            this.handlePositionChanged();

        this.resizeChildren(dWidth, dHeight);

        this.updateGear(2);

        if (this._parent) {
            //@ts-ignore 大小改变影响关联，所以需要重新计算位置。
            this._relations.onOwnerSizeChanged(dWidth, dHeight, this._pivotAsAnchor || !ignorePivot);
            this._parent.setBoundsChangedFlag();
            if (this._group)
                this._group.setBoundsChangedFlag();
        }

        this._node.fire(FUIEvent.SIZE_CHANGED, this);
    }

    protected resizeChildren(dWidth: number, dHeight: number) {
        //用于GGroup
    }

    protected handleSizeChanged(): void {
        this._uiTrans.width = this._width;
        this._uiTrans.height = this._height;
    }


    public get scaleX(): number {
        return this._transform.scaleX;
    }

    public set scaleX(value: number) {
        this.setScale(value, this._transform.scaleY);
    }

    public get scaleY(): number {
        return this._transform.scaleY;
    }

    public set scaleY(value: number) {
        this.setScale(this._transform.scaleX, value);
    }

    public setScale(sx: number, sy: number) {
        if (this._transform.scaleX !== sx || this._transform.scaleY !== sy) {
            this._transform.scaleX = sx;
            this._transform.scaleY = sy;
            this.updateGear(2);
        }
    }


    public get skewX(): number {
        return this._skewX;
    }

    public get pivotX(): number {
        return this._uiTrans.anchorX;
    }

    public set pivotX(value: number) {
        this._uiTrans.anchorX = value;
    }

    public get pivotY(): number {
        return 1 - this._uiTrans.anchorY;
    }

    public set pivotY(value: number) {
        this._uiTrans.anchorY = 1 - value;
    }

    public setPivot(xv: number, yv: number, asAnchor?: boolean): void {
        if (this._uiTrans.anchorX != xv || this._uiTrans.anchorY != 1 - yv) {
            this._pivotAsAnchor = asAnchor;
            this._uiTrans.setAnchorPoint(xv, 1 - yv);
        }
        else if (this._pivotAsAnchor != asAnchor) {
            this._pivotAsAnchor = asAnchor;
            this.handlePositionChanged();
        }
    }

    public get pivotAsAnchor(): boolean {
        return this._pivotAsAnchor;
    }

    public get visible(): boolean {
        return this._visible;
    }

    public set visible(value: boolean) {
        if (this._visible != value) {
            this._visible = value;

            this.handleVisibleChanged();

            if (this._group && this._group.excludeInvisibles)
                this._group.setBoundsChangedFlag();
        }
    }


    public get xMin(): number {
        return this._pivotAsAnchor ? (this._x - this._width * this._uiTrans.anchorX) : this._x;
    }

    public set xMin(value: number) {
        if (this._pivotAsAnchor)
            this.setPosition(value + this._width * this._uiTrans.anchorX, this._y);
        else
            this.setPosition(value, this._y);
    }

    public get yMin(): number {
        return this._pivotAsAnchor ? (this._y - this._height * (1 - this._uiTrans.anchorY)) : this._y;
    }

    public set yMin(value: number) {
        if (this._pivotAsAnchor)
            this.setPosition(this._x, value + this._height * (1 - this._uiTrans.anchorY));
        else
            this.setPosition(this._x, value);
    }

    public get _finalVisible(): boolean {
        return this._visible && this._internalVisible && (!this._group || this._group._finalVisible);
    }

    public get internalVisible3(): boolean {
        return this._visible && this._internalVisible;
    }

    protected handleVisibleChanged(): void {
        this._node.visible = this._finalVisible;
        this.handleGroupVisibleChanged();

        if (this._parent)
            this._parent.setBoundsChangedFlag();
    }

    protected handleGroupVisibleChanged() {
        //用于GGroup
    }


    public set group(value: GGroup) {
        if (this._group != value) {
            if (this._group)
                this._group.setBoundsChangedFlag();
            this._group = value;
            if (this._group)
                this._group.setBoundsChangedFlag();
        }
    }

    public get group(): GGroup {
        return this._group;
    }

    public handleControllerChanged(c: Controller): void {
        this._handlingController = true;
        for (var i: number = 0; i < 10; i++) {
            var gear: GearBase = this._gears[i];
            if (gear && gear.controller == c)
                gear.apply();
        }
        this._handlingController = false;

        this.checkGearDisplay();
    }



    public get touchable(): boolean {
        return this._touchable;
    }

    public set touchable(value: boolean) {
        if (this._touchable != value) {
            this._touchable = value;
            this.updateGear(3);
        }
    }

    public get grayed(): boolean {
        return this._grayed;
    }

    public set grayed(value: boolean) {
        if (this._grayed != value) {
            this._grayed = value;
            this.handleGrayedChanged();
            this.updateGear(3);
        }
    }

    protected handleGrayedChanged(): void {

    }

    public get enabled(): boolean {
        return !this._grayed && this._touchable;
    }

    public set enabled(value: boolean) {
        this.grayed = !value;
        this.touchable = value;
    }

    public get rotation(): number {
        return -this._node.transform.rotateX;
    }

    public set rotation(value: number) {
        value = -value;
        if (this._node.transform.rotateX != value) {
            this._node.transform.rotate = value;
            this.updateGear(3);
        }
    }

    public get alpha(): number {
        return this._alpha;
    }

    public set alpha(value: number) {
        if (this._alpha != value) {
            this._alpha = value;

            // this._uiOpacity.opacity = this._alpha * 255;

            this.handleAlphaChanged();

            this.updateGear(3);
        }
    }

    protected handleAlphaChanged() {
        //用于GGroup
    }

    public get sortingOrder(): number {
        return this._sortingOrder;
    }

    public set sortingOrder(value: number) {
        if (value < 0)
            value = 0;
        if (this._sortingOrder != value) {
            var old: number = this._sortingOrder;
            this._sortingOrder = value;
            if (this._parent)
                this._parent.childSortingOrderChanged(this, old, this._sortingOrder);
        }
    }

    public get text(): string | null {
        return null;
    }

    public set text(value: string | null) {
    }

    public get icon(): string | null {
        return null;
    }

    public set icon(value: string | null) {
    }

    public get blendMode(): BlendMode {
        return this._blendMode;
    }

    public set blendMode(value: BlendMode) {
        if (this._blendMode != value) {
            this._blendMode = value;
            // BlendModeUtils.apply(this._node, value);
        }
    }

    public get actualWidth(): number {
        return this.width * Math.abs(this._node.transform.scaleX);
    }

    public get actualHeight(): number {
        return this.height * Math.abs(this._node.transform.scaleY);
    }

    public get onStage(): boolean {
        return this._node && this._node.activeInHierarchy;
    }

    public get resourceURL(): string | null {
        if (this.packageItem)
            return "ui://" + this.packageItem.owner.id + this.packageItem.id;
        else
            return null;
    }

    public localToGlobal(ax?: number, ay?: number, result?: Vec2): Vec2 {

        ax = ax || 0;
        ay = ay || 0;
        ay = -ay;
        if (!this._pivotAsAnchor) {
            ax -= this._uiTrans.anchorX * this._width;
            ay += (1 - this._uiTrans.anchorY) * this._height;
        }
        this._transform.localToWorldPosition(ax, ay, Vec2.temp1Vec2);
        Vec2.temp1Vec2.y = window.GRoot.inst.height - Vec2.temp1Vec2.y;
        result = result || new Vec2();
        result.set(Vec2.temp1Vec2.x, Vec2.temp1Vec2.y);

        // ax = ax || 0;
        // ay = ay || 0;
        // s_vec3.x = ax;
        // s_vec3.y = -ay;
        // if (!this._pivotAsAnchor) {
        //     s_vec3.x -= this._uiTrans.anchorX * this._width;
        //     s_vec3.y += (1 - this._uiTrans.anchorY) * this._height;
        // }
        // this._uiTrans.convertToWorldSpaceAR(s_vec3, s_vec3);
        // s_vec3.y = Decls.GRoot.inst.height - s_vec3.y;

        // result = result || new Vec2();
        // result.x = s_vec3.x;
        // result.y = s_vec3.y;
        return result;
    }

    public globalToLocal(ax?: number, ay?: number, result?: Vec2): Vec2 {
        ax = ax || 0;
        ay = ay || 0;
        ay = window.GRoot.inst.height - ay;
        this._transform.worldPositionToLocal(ax, ay, Vec2.temp1Vec2);
        if (!this._pivotAsAnchor) {
            Vec2.temp1Vec2.x += this._uiTrans.anchorX * this._width;
            Vec2.temp1Vec2.y -= (1 - this._uiTrans.anchorY) * this._height;
        }
        result = result || new Vec2();
        result.set(Vec2.temp1Vec2.x,Vec2.temp1Vec2.y);
        
        // s_vec3.x = ax;
        // s_vec3.y = Decls.GRoot.inst.height - ay;
        // this._uiTrans.convertToNodeSpaceAR(s_vec3, s_vec3);
        // if (!this._pivotAsAnchor) {
        //     s_vec3.x += this._uiTrans.anchorX * this._width;
        //     s_vec3.y -= (1 - this._uiTrans.anchorY) * this._height;
        // }

        // result = result || new Vec2();
        // result.x = s_vec3.x;
        // result.y = -s_vec3.y;
        return result;
    }

    public localToGlobalRect(ax?: number, ay?: number, aw?: number, ah?: number, result?: Rect): Rect {
        ax = ax || 0;
        ay = ay || 0;
        aw = aw || 0;
        ah = ah || 0;
        result = result || new Rect();
        var pt: Vec2 = this.localToGlobal(ax, ay);
        result.x = pt.x;
        result.y = pt.y;
        pt = this.localToGlobal(ax + aw, ay + ah, pt);
        result.xMax = pt.x;
        result.yMax = pt.y;
        return result;
    }

    public globalToLocalRect(ax?: number, ay?: number, aw?: number, ah?: number, result?: Rect): Rect {
        ax = ax || 0;
        ay = ay || 0;
        aw = aw || 0;
        ah = ah || 0;
        result = result || new Rect();
        var pt: Vec2 = this.globalToLocal(ax, ay);
        result.x = pt.x;
        result.y = pt.y;
        pt = this.globalToLocal(ax + aw, ay + ah, pt);
        result.xMax = pt.x;
        result.yMax = pt.y;
        return result;
    }


    public getProp(index: number): any {
        switch (index) {
            case ObjectPropID.Text:
                return this.text;
            case ObjectPropID.Icon:
                return this.icon;
            case ObjectPropID.Color:
                return null;
            case ObjectPropID.OutlineColor:
                return null;
            case ObjectPropID.Playing:
                return false;
            case ObjectPropID.Frame:
                return 0;
            case ObjectPropID.DeltaTime:
                return 0;
            case ObjectPropID.TimeScale:
                return 1;
            case ObjectPropID.FontSize:
                return 0;
            case ObjectPropID.Selected:
                return false;
            default:
                return undefined;
        }
    }

    public setProp(index: number, value: any): void {
        switch (index) {
            case ObjectPropID.Text:
                this.text = value;
                break;

            case ObjectPropID.Icon:
                this.icon = value;
                break;
        }
    }

    protected onEnable() {
    }

    protected onDisable() {
    }

    protected onUpdate() {
    }

    protected onDestroy() {
    }

    /**
     * GObject不使用时必须调用此函数销毁该节点
     * @returns 
     */
    public dispose(): void {
        let n = this._node;
        if (!n)
            return;

        this.removeFromParent();
        this._relations.dispose();
        this._node = null;
        n.destroy();
        for (var i: number = 0; i < 10; i++) {
            var gear: GearBase = this._gears[i];
            if (gear)
                gear.dispose();
        }
    }

}