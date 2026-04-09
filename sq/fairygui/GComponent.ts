import { MaskComponent, MaskType, UIContentComponent } from "../2d";
import { Size, Vec2 } from "../core";
import { Application } from "../framework/Application";
import { Node, NodeEventType } from "../scene";
import { ChildrenRenderOrder, OverflowType } from "./common/FieldTypes";
import { Margin } from "./common/Margin";
import { Controller } from "./Controller";
import { IHitTest, PixelHitTest, ChildHitArea } from "./event/HitTest";
import { FUIEvent } from "./FUIEvent";
import { GGraph } from "./GGraph";
import { GGroup } from "./GGroup";
import { GImage } from "./GImage";
import { GObject } from "./GObject";
import { ScrollPane } from "./ScrollPane";
import { Transition } from "./Transition";
import { UIConfig } from "./UIConfig";
import { ByteBuffer } from "./utils/ByteBuffer";

export class GComponent extends GObject {
    public hitArea?: IHitTest;

    private _sortingChildCount: number = 0;
    private _opaque: boolean;
    private _applyingController?: Controller;
    private _rectMask?: MaskComponent;
    private _maskContent?: GObject;
    private _invertedMask?: boolean = false;

    protected override _margin: Margin;
    protected _trackBounds: boolean;
    protected _boundsChanged: boolean;

    /**
     * 控制子节点的渲染顺序
     * 1、Ascent（上升）： 表示从节点0开始渲染，所以处于最后的节点渲染到最上面
     * 2、Descent（下降）： 表示_children.length开始渲染，直到第0个，所以处于第0个节点渲染到最上面
     */
    protected _childrenRenderOrder: ChildrenRenderOrder = ChildrenRenderOrder.Ascent;
    protected _apexIndex: number = 0;

    public _buildingDisplayList: boolean;

    /**
     * 存储子节点
     */
    protected _children: Array<GObject>;

    /**
     * 存储控制器
     */
    protected _controllers: Array<Controller>;


    /**
     * 存储动画
     */
    public _transitions: Array<Transition>;

    /**
     * 因为遮罩只能对子节点有作用，为了实现遮罩我们这里
     * 再创建一个节点层
     * 
     */
    protected _container: Node;
    protected _containerUITrans: UIContentComponent;

    public _scrollPane?: ScrollPane;
    public _alignOffset: Vec2;
    public _customMask?: MaskComponent;

    public constructor() {
        super();

        this._node.name = "GComponent";
        this._children = new Array<GObject>();
        this._controllers = new Array<Controller>();
        this._transitions = new Array<Transition>();
        this._margin = new Margin();
        this._alignOffset = new Vec2();


        this._container = new Node("Container");
        (this._container as any)["$go"] = this;
        this._containerUITrans = this._container.addComponent(UIContentComponent);
        this._containerUITrans.setAnchorPoint(0, 1);
        this._node.addChild(this._container);
    }

    public override dispose(): void {
        var i: number;
        var cnt: number;

        cnt = this._transitions.length;
        for (i = 0; i < cnt; ++i) {
            var trans: Transition = this._transitions[i];
            trans.dispose();
        }

        cnt = this._controllers.length;
        for (i = 0; i < cnt; ++i) {
            var cc: Controller = this._controllers[i];
            cc.dispose();
        }
        this._scrollPane = null;

        cnt = this._children.length;
        for (i = cnt - 1; i >= 0; --i) {
            var obj: GObject = this._children[i];
            obj._parent = null;//avoid removeFromParent call
            obj.dispose();
        }

        this._boundsChanged = false;

        Application.ins.timer.clearAll(this);
        super.dispose();
    }

    public get displayListContainer(): Node {
        return this._container;
    }
    
    public addChild(child: GObject): GObject {
        this.addChildAt(child, this._children.length);
        return child;
    }

    public addChildAt(child: GObject, index: number): GObject {
        if (!child)
            throw new Error("child is null");

        var numChildren: number = this._children.length;

        if (index >= 0 && index <= numChildren) {
            if (child.parent == this) {
                this.setChildIndex(child, index);
            }
            else {
                child.removeFromParent();
                child._parent = this;

                var cnt: number = this._children.length;
                if (child.sortingOrder != 0) {
                    this._sortingChildCount++;

                    //找到渲染顺序需要插入的位置，首个比sortingOrder大的节点的Index
                    index = this.getInsertPosForSortingChild(child);
                }
                else if (this._sortingChildCount > 0) {
                    if (index > (cnt - this._sortingChildCount))
                        index = cnt - this._sortingChildCount;
                }

                if (index == cnt)
                    this._children.push(child);
                else
                    this._children.splice(index, 0, child);

                this.onChildAdd(child, index);
                this.setBoundsChangedFlag();
            }

            return child;
        }
        else {
            throw new Error("Invalid child index");
        }
    }

    /**
     * 在当前父节点下获取比target的sortingOrder大的首个Child的index
     * @param target 
     * @returns 
     */
    private getInsertPosForSortingChild(target: GObject): number {
        var cnt: number = this._children.length;
        var i: number = 0;
        for (i = 0; i < cnt; i++) {
            var child: GObject = this._children[i];
            if (child === target)
                continue;

            if (target.sortingOrder < child.sortingOrder)
                break;
        }
        return i;
    }

    public removeChild(child: GObject, dispose?: boolean): GObject {
        var childIndex: number = this._children.indexOf(child);
        if (childIndex != -1) {
            this.removeChildAt(childIndex, dispose);
        }
        return child;
    }

    public removeChildAt(index: number, dispose?: boolean): GObject {
        if (index >= 0 && index < this.numChildren) {
            var child: GObject = this._children[index];
            child._parent = null;

            if (child.sortingOrder != 0)
                this._sortingChildCount--;

            this._children.splice(index, 1);
            child.group = null;
            this._container.removeChild(child.node);
            if (this._childrenRenderOrder == ChildrenRenderOrder.Arch)
                Application.ins.timer.callLater(this, this.buildNativeDisplayList);
            if (dispose)
                child.dispose();
            this.setBoundsChangedFlag();
            return child;
        }
        else {
            throw new Error("Invalid child index");
        }
    }

    public removeChildren(beginIndex?: number, endIndex?: number, dispose?: boolean): void {
        if (beginIndex == undefined) beginIndex = 0;
        if (endIndex == undefined) endIndex = -1;

        if (endIndex < 0 || endIndex >= this.numChildren)
            endIndex = this.numChildren - 1;

        for (var i: number = beginIndex; i <= endIndex; ++i)
            this.removeChildAt(beginIndex, dispose);
    }

    public getChildAt<T extends GObject>(index: number, classType?: Constructor<T>): T {
        if (index >= 0 && index < this.numChildren)
            return this._children[index] as T;
        else
            throw new Error("Invalid child index");
    }

    public getChild<T extends GObject>(name: string, classType?: Constructor<T>): T {
        var cnt: number = this._children.length;
        for (var i: number = 0; i < cnt; ++i) {
            if (this._children[i].name === name)
                return this._children[i] as T;
        }

        return null;
    }

    public getChildByPath<T extends GObject>(path: String, classType?: Constructor<T>): T {
        var arr: string[] = path.split(".");
        var cnt: number = arr.length;
        var gcom: GComponent = this;
        var obj: GObject;
        for (var i: number = 0; i < cnt; ++i) {
            obj = gcom.getChild(arr[i]);
            if (!obj)
                break;

            if (i != cnt - 1) {
                if (!(obj instanceof GComponent)) {
                    obj = null;
                    break;
                }
                else
                    gcom = obj;
            }
        }

        return obj as T;
    }

    public getVisibleChild(name: string): GObject {
        var cnt: number = this._children.length;
        for (var i: number = 0; i < cnt; ++i) {
            var child: GObject = this._children[i];
            if (child._finalVisible && child.name == name)
                return child;
        }

        return null;
    }

    public getChildInGroup(name: string, group: GGroup): GObject {
        var cnt: number = this._children.length;
        for (var i: number = 0; i < cnt; ++i) {
            var child: GObject = this._children[i];
            if (child.group == group && child.name == name)
                return child;
        }

        return null;
    }

    public getChildById(id: string | number): GObject {
        var cnt: number = this._children.length;
        for (var i: number = 0; i < cnt; ++i) {
            if (this._children[i].id == id)
                return this._children[i];
        }

        return null;
    }

    public getChildIndex(child: GObject): number {
        return this._children.indexOf(child);
    }

    public setChildIndex(child: GObject, index: number): void {
        var oldIndex: number = this._children.indexOf(child);
        if (oldIndex === -1)
            throw new Error("Not a child of this container");

        if (child.sortingOrder != 0) //no effect
            return;

        var cnt: number = this._children.length;
        if (this._sortingChildCount > 0) {
            if (index > (cnt - this._sortingChildCount - 1))
                index = cnt - this._sortingChildCount - 1;
        }

        this._setChildIndex(child, oldIndex, index);
    }

    public setChildIndexBefore(child: GObject, index: number): number {
        var oldIndex: number = this._children.indexOf(child);
        if (oldIndex == -1)
            throw new Error("Not a child of this container");

        if (child.sortingOrder != 0) //no effect
            return oldIndex;

        var cnt: number = this._children.length;
        if (this._sortingChildCount > 0) {
            if (index > (cnt - this._sortingChildCount - 1))
                index = cnt - this._sortingChildCount - 1;
        }

        if (oldIndex < index)
            return this._setChildIndex(child, oldIndex, index - 1);
        else
            return this._setChildIndex(child, oldIndex, index);
    }

    private _setChildIndex(child: GObject, oldIndex: number, index: number): number {
        var cnt: number = this._children.length;
        if (index > cnt)
            index = cnt;

        if (oldIndex == index)
            return oldIndex;

        this._children.splice(oldIndex, 1);
        this._children.splice(index, 0, child);

        if (this._childrenRenderOrder == ChildrenRenderOrder.Ascent)
            child.node.setSiblingIndex(index);
        else if (this._childrenRenderOrder == ChildrenRenderOrder.Descent)
            child.node.setSiblingIndex(cnt - index);
        else
            Application.ins.timer.callLater(this, this.buildNativeDisplayList);

        this.setBoundsChangedFlag();

        return index;
    }

    public swapChildren(child1: GObject, child2: GObject): void {
        var index1: number = this._children.indexOf(child1);
        var index2: number = this._children.indexOf(child2);
        if (index1 == -1 || index2 == -1)
            throw new Error("Not a child of this container");
        this.swapChildrenAt(index1, index2);
    }

    /**
     * 交换两个节点的在父节点中的位置
     * @param index1 
     * @param index2 
     */
    public swapChildrenAt(index1: number, index2: number): void {
        var child1: GObject = this._children[index1];
        var child2: GObject = this._children[index2];

        this.setChildIndex(child1, index2);
        this.setChildIndex(child2, index1);
    }

    public get numChildren(): number {
        return this._children.length;
    }

    /**
     * 判断child是否在此Parent链条中
     * @param child 
     * @returns 
     */
    public isAncestorOf(child: GObject): boolean {
        if (child == null)
            return false;

        var p: GComponent = child.parent;
        while (p) {
            if (p === this)
                return true;

            p = p.parent;
        }
        return false;
    }

    /**
     * 为GObject创建一个控制器
     * @param controller 
     */
    public addController(controller: Controller): void {
        this._controllers.push(controller);
        controller.parent = this;
        this.applyController(controller);
    }

    public getControllerAt(index: number): Controller {
        return this._controllers[index];
    }

    public getController(name: string): Controller {
        var cnt: number = this._controllers.length;
        for (var i: number = 0; i < cnt; ++i) {
            var c: Controller = this._controllers[i];
            if (c.name === name)
                return c;
        }
        return null;
    }

    public removeController(c: Controller): void {
        var index: number = this._controllers.indexOf(c);
        if (index === -1)
            throw new Error("controller not exists");

        c.parent = null;
        this._controllers.splice(index, 1);

        var length: number = this._children.length;
        for (var i: number = 0; i < length; i++) {
            var child: GObject = this._children[i];
            child.handleControllerChanged(c);
        }
    }

    public get controllers(): Array<Controller> {
        return this._controllers;
    }

    private onChildAdd(child: GObject, index: number): void {

        this._container.addChild(child.node);
        // child.node.active = child._finalVisible;
        child.node.visible = child._finalVisible;

        if (this._buildingDisplayList)
            return;

        let cnt: number = this._children.length;
        if (this._childrenRenderOrder == ChildrenRenderOrder.Ascent)
            child.node.setSiblingIndex(index);
        else if (this._childrenRenderOrder == ChildrenRenderOrder.Descent)
            child.node.setSiblingIndex(cnt - index);
        else
            Application.ins.timer.callLater(this, this.buildNativeDisplayList);
    }

    private buildNativeDisplayList(dt?: number): void {
        if (!isNaN(dt)) {
            let _t = <GComponent>GObject.cast(this.node);
            _t.buildNativeDisplayList();
            return;
        }

        let cnt: number = this._children.length;
        if (cnt == 0)
            return;

        let child: GObject;
        switch (this._childrenRenderOrder) {
            case ChildrenRenderOrder.Ascent:
                {
                    let j = 0;
                    for (let i = 0; i < cnt; i++) {
                        child = this._children[i];
                        child.node.setSiblingIndex(j++);
                    }
                }
                break;
            case ChildrenRenderOrder.Descent:
                {
                    let j = 0;
                    for (let i = cnt - 1; i >= 0; i--) {
                        child = this._children[i];
                        child.node.setSiblingIndex(j++);
                    }
                }
                break;

            case ChildrenRenderOrder.Arch:
                {
                    let j = 0;
                    for (let i = 0; i < this._apexIndex; i++) {
                        child = this._children[i];
                        child.node.setSiblingIndex(j++);
                    }
                    for (let i = cnt - 1; i >= this._apexIndex; i--) {
                        child = this._children[i];
                        child.node.setSiblingIndex(j++);
                    }
                }
                break;
        }
    }

    public applyController(c: Controller): void {
        this._applyingController = c;
        var child: GObject;
        var length: number = this._children.length;
        for (var i: number = 0; i < length; i++) {
            child = this._children[i];
            child.handleControllerChanged(c);
        }
        this._applyingController = null;

        c.runActions();
    }

    public applyAllControllers(): void {
        var cnt: number = this._controllers.length;
        for (var i: number = 0; i < cnt; ++i) {
            this.applyController(this._controllers[i]);
        }
    }

    public adjustRadioGroupDepth(obj: GObject, c: Controller): void {
        var cnt: number = this._children.length;
        var i: number;
        var child: GObject;
        var myIndex: number = -1, maxIndex: number = -1;
        for (i = 0; i < cnt; i++) {
            child = this._children[i];
            if (child == obj) {
                myIndex = i;
            }
            else if (("relatedController" in child)/*is button*/ && (<any>child).relatedController == c) {
                if (i > maxIndex)
                    maxIndex = i;
            }
        }
        if (myIndex < maxIndex) {
            if (this._applyingController)
                this._children[maxIndex].handleControllerChanged(this._applyingController);
            this.swapChildrenAt(myIndex, maxIndex);
        }
    }

    public getTransitionAt(index: number): Transition {
        return this._transitions[index];
    }

    public getTransition(transName: string): Transition {
        var cnt: number = this._transitions.length;
        for (var i: number = 0; i < cnt; ++i) {
            var trans: Transition = this._transitions[i];
            if (trans.name == transName)
                return trans;
        }

        return null;
    }

    public isChildInView(child: GObject): boolean {
        if (this._rectMask) {
            return child.x + child.width >= 0 && child.x <= this.width
                && child.y + child.height >= 0 && child.y <= this.height;
        }
        else if (this._scrollPane) {
            return this._scrollPane.isChildInView(child);
        }
        else
            return true;
    }

    public getFirstChildInView(): number {
        var cnt: number = this._children.length;
        for (var i: number = 0; i < cnt; ++i) {
            var child: GObject = this._children[i];
            if (this.isChildInView(child))
                return i;
        }
        return -1;
    }

    public get scrollPane(): ScrollPane {
        return this._scrollPane;
    }

    public get opaque(): boolean {
        return this._opaque;
    }

    public set opaque(value: boolean) {
        this._opaque = value;
    }

    public get margin(): Margin {
        return this._margin;
    }

    public set margin(value: Margin) {
        this._margin.copy(value);
        this.handleSizeChanged();
    }

    public get childrenRenderOrder(): ChildrenRenderOrder {
        return this._childrenRenderOrder;
    }

    public set childrenRenderOrder(value: ChildrenRenderOrder) {
        if (this._childrenRenderOrder != value) {
            this._childrenRenderOrder = value;
            this.buildNativeDisplayList();
        }
    }

    public get apexIndex(): number {
        return this._apexIndex;
    }

    public set apexIndex(value: number) {
        if (this._apexIndex != value) {
            this._apexIndex = value;

            if (this._childrenRenderOrder == ChildrenRenderOrder.Arch)
                this.buildNativeDisplayList();
        }
    }

    public get mask(): GObject {
        return this._maskContent;
    }

    public set mask(value: GObject) {
        this.setMask(value, false);
    }

    public setMask(value: GObject, inverted: boolean): void {
        if (this._maskContent) {
            this._maskContent.node.off(NodeEventType.TRASNFORM_CHANGE, this, this.onMaskContentChanged);
            this._maskContent.node.off(NodeEventType.RESIZE, this, this.onMaskContentChanged);
            this._maskContent.node.off(NodeEventType.ANCHOR_CHANGE, this, this.onMaskContentChanged);
            this._maskContent.visible = true;
        }

        this._maskContent = value;
        if (this._maskContent) {
            if (!(value instanceof GImage) && !(value instanceof GGraph))
                return;

            if (!this._customMask) {
                let maskNode: Node = new Node("Mask");
                maskNode.layer = UIConfig.defaultUILayer;
                maskNode.addComponent(UIContentComponent);
                this._node.addChild(maskNode);
                if (this._scrollPane)
                    // this._container.parent.parent = maskNode;
                    maskNode.addChild(this._container.parent);
                else
                    // this._container.parent = maskNode;
                    maskNode.addChild(this._container);

                this._customMask = maskNode.addComponent(MaskComponent);
            }

            value.visible = false;
            value.node.on(NodeEventType.TRASNFORM_CHANGE, this, this.onMaskContentChanged);
            value.node.on(NodeEventType.RESIZE, this, this.onMaskContentChanged);
            value.node.on(NodeEventType.ANCHOR_CHANGE, this, this.onMaskContentChanged);

            this._invertedMask = inverted;
            if (this._node.activeInHierarchy)
                this.onMaskReady();
            else
                this.on(FUIEvent.DISPLAY, this.onMaskReady, this);

            this.onMaskContentChanged();
            if (this._scrollPane)
                this._scrollPane.adjustMaskContainer();
            else
                this._container.transform.setPosition(0, 0);
        }
        else if (this._customMask) {
            if (this._scrollPane)
                // this._container.parent.parent = this._node;
                this.node.addChild(this._container.parent);
            else
                this._node.addChild(this._container);
            // this._container.parent = this._node;
            this._customMask.node.destroy();
            this._customMask = null;

            if (this._scrollPane)
                this._scrollPane.adjustMaskContainer();
            // else
            // this._container.transform.setPosition(this._pivotCorrectX, this._pivotCorrectY);
        }
    }

    private onMaskReady() {
        this.off(FUIEvent.DISPLAY, this.onMaskReady, this);

        if (this._maskContent instanceof GImage) {
            this._customMask.type = MaskType.SPRITE_STENCIL;
            // this._customMask.alphaThreshold = 0.0001;
            this._customMask.spriteFrame = this._maskContent._content.spriteFrame;
        }
        else if (this._maskContent instanceof GGraph) {
            if (this._maskContent.type == 2)
                this._customMask.type = MaskType.GRAPHICS_ELLIPSE;
            else
                this._customMask.type = MaskType.GRAPHICS_RECT;
        }

        this._customMask.inverted = this._invertedMask;
    }

    private onMaskContentChanged() {
        let maskNode: Node = this._customMask.node;
        let maskUITrans: UIContentComponent = maskNode.getCompoentInherit(UIContentComponent);

        let contentNode: Node = this._maskContent.node;

        //@ts-ignore
        let contentUITrans: UIContentComponent = this._maskContent._uiTrans;

        let w: number = this._maskContent.width * this._maskContent.scaleX;
        let h: number = this._maskContent.height * this._maskContent.scaleY;

        maskUITrans.setContentSize(w, h);

        let left: number = contentNode.transform.x - contentUITrans.anchorX * w;
        let top: number = contentNode.transform.y - contentUITrans.anchorY * h;
        maskUITrans.setAnchorPoint(-left / maskUITrans.width, -top / maskUITrans.height);

        maskNode.transform.setPosition(this._pivotCorrectX, this._pivotCorrectY);
    }

    public get _pivotCorrectX(): number {
        return -this.pivotX * this._width + this._margin.left;
    }

    public get _pivotCorrectY(): number {
        return this.pivotY * this._height - this._margin.top;
    }

    public get baseUserData(): string {
        var buffer: ByteBuffer = this.packageItem.rawData;
        buffer.seek(0, 4);
        return buffer.readS();
    }

    // protected setupScroll(buffer: ByteBuffer): void {
    //     this._scrollPane = this._node.addComponent(ScrollPane);
    //     this._scrollPane.setup(buffer);
    // }

    // protected setupOverflow(overflow: OverflowType): void {
    //     if (overflow == OverflowType.Hidden)
    //         this._rectMask = this._container.addComponent(MaskComponent);

    //     if (!this._margin.isNone)
    //         this.handleSizeChanged();
    // }

    setOverflow(overflow: OverflowType) {
        if (overflow == OverflowType.Scroll) {
            this._scrollPane = this._node.addComponent(ScrollPane);
        } else {
            if (overflow == OverflowType.Hidden) {
                //滚动区域是隐藏的，那么超出组件区域大小的内容使用一个遮罩来裁剪
                this._rectMask = this._container.addComponent(MaskComponent);
            }

            //有边缘设置
            if (!this._margin.isNone)
                this.handleSizeChanged();
        }
    }

    protected override handleAnchorChanged(): void {
        super.handleAnchorChanged();

        if (this._customMask)
            this._customMask.node.transform.setPosition(this._pivotCorrectX, this._pivotCorrectY);
        else if (this._scrollPane)
            this._scrollPane.adjustMaskContainer();
        // else
        // this._container.transform.setPosition(this._pivotCorrectX + this._alignOffset.x, this._pivotCorrectY - this._alignOffset.y);
    }

    protected override handleSizeChanged(): void {
        super.handleSizeChanged();

        if (this._customMask)
            this._customMask.node.transform.setPosition(this._pivotCorrectX, this._pivotCorrectY);
        else if (!this._scrollPane)
            this._container.transform.setPosition(this._pivotCorrectX, this._pivotCorrectY);

        if (this._scrollPane)
            this._scrollPane.onOwnerSizeChanged();
        else
            this._containerUITrans.setContentSize(this.viewWidth, this.viewHeight);
    }

    protected override handleGrayedChanged(): void {
        var c: Controller = this.getController("grayed");
        if (c) {
            c.selectedIndex = this.grayed ? 1 : 0;
            return;
        }

        var v: boolean = this.grayed;
        var cnt: number = this._children.length;
        for (var i: number = 0; i < cnt; ++i) {
            this._children[i].grayed = v;
        }
    }

    public override handleControllerChanged(c: Controller): void {
        super.handleControllerChanged(c);

        if (this._scrollPane)
            this._scrollPane.handleControllerChanged(c);
    }

    /**
     * 标记一下GComponent的AABB包围盒大小更改
     * @returns 
     */
    public setBoundsChangedFlag(): void {
        if (!this._scrollPane && !this._trackBounds)
            return;

        if (!this._boundsChanged) {
            this._boundsChanged = true;
            Application.ins.timer.callLater(this, this.refresh);
        }
    }

    /**
     * 刷新 AABB 的大小
     * @param dt 
     * @returns 
     */
    private refresh(dt?: number): void {
        if (!isNaN(dt)) {
            let _t = <GComponent>GObject.cast(this.node);
            _t.refresh();
            return;
        }

        if (this._boundsChanged) {
            var len: number = this._children.length;
            if (len > 0) {
                for (var i: number = 0; i < len; i++) {
                    var child: GObject = this._children[i];
                    child.ensureSizeCorrect();
                }
            }

            this.updateBounds();
        }
    }

    public ensureBoundsCorrect(): void {
        var len: number = this._children.length;
        for (var i: number = 0; i < len; i++) {
            var child: GObject = this._children[i];
            child.ensureSizeCorrect();
        }

        if (this._boundsChanged)
            this.updateBounds();
    }

    protected updateBounds(): void {
        var minX: number = 0, minY: number = 0, width: number = 0, height: number = 0;
        var len: number = this._children.length;
        if (len > 0) {
            minX = Number.POSITIVE_INFINITY, minY = Number.POSITIVE_INFINITY;
            var maxX: number = Number.NEGATIVE_INFINITY, maxY: number = Number.NEGATIVE_INFINITY;
            var tmp: number = 0;
            var i: number = 0;

            for (var i: number = 0; i < len; i++) {
                var child: GObject = this._children[i];
                tmp = child.x;
                if (tmp < minX)
                    minX = tmp;

                tmp = child.y;
                if (tmp < minY)
                    minY = tmp;

                tmp = child.x + child.actualWidth;
                if (tmp > maxX)
                    maxX = tmp;

                tmp = child.y + child.actualHeight;
                if (tmp > maxY)
                    maxY = tmp;
            }
            width = maxX - minX;
            height = maxY - minY;
        }

        this.setBounds(minX, minY, width, height);
    }

    public setBounds(ax: number, ay: number, aw: number, ah: number = 0): void {
        this._boundsChanged = false;

        if (this._scrollPane)
            this._scrollPane.setContentSize(Math.round(ax + aw), Math.round(ay + ah));
    }

    public get viewWidth(): number {
        if (this._scrollPane)
            return this._scrollPane.viewWidth;
        else
            return this.width - this._margin.left - this._margin.right;
    }

    public set viewWidth(value: number) {
        if (this._scrollPane)
            this._scrollPane.viewWidth = value;
        else
            this.width = value + this._margin.left + this._margin.right;
    }

    public get viewHeight(): number {
        if (this._scrollPane)
            return this._scrollPane.viewHeight;
        else
            return this.height - this._margin.top - this._margin.bottom;
    }

    public set viewHeight(value: number) {
        if (this._scrollPane)
            this._scrollPane.viewHeight = value;
        else
            this.height = value + this._margin.top + this._margin.bottom;
    }

    public getSnappingPosition(xValue: number, yValue: number, resultPoint?: Vec2): Vec2 {
        if (!resultPoint)
            resultPoint = new Vec2();

        var cnt: number = this._children.length;
        if (cnt == 0) {
            resultPoint.x = 0;
            resultPoint.y = 0;
            return resultPoint;
        }

        this.ensureBoundsCorrect();

        var obj: GObject = null;
        var prev: GObject = null;
        var i: number = 0;
        if (yValue != 0) {
            for (; i < cnt; i++) {
                obj = this._children[i];
                if (yValue < obj.y) {
                    if (i == 0) {
                        yValue = 0;
                        break;
                    }
                    else {
                        prev = this._children[i - 1];
                        if (yValue < prev.y + prev.actualHeight / 2) //top half part
                            yValue = prev.y;
                        else //bottom half part
                            yValue = obj.y;
                        break;
                    }
                }
            }

            if (i == cnt)
                yValue = obj.y;
        }

        if (xValue != 0) {
            if (i > 0)
                i--;
            for (; i < cnt; i++) {
                obj = this._children[i];
                if (xValue < obj.x) {
                    if (i == 0) {
                        xValue = 0;
                        break;
                    }
                    else {
                        prev = this._children[i - 1];
                        if (xValue < prev.x + prev.actualWidth / 2) //top half part
                            xValue = prev.x;
                        else //bottom half part
                            xValue = obj.x;
                        break;
                    }
                }
            }

            if (i == cnt)
                xValue = obj.x;
        }

        resultPoint.x = xValue;
        resultPoint.y = yValue;
        return resultPoint;
    }

    public childSortingOrderChanged(child: GObject, oldValue: number, newValue: number = 0): void {
        if (newValue == 0) {
            this._sortingChildCount--;
            this.setChildIndex(child, this._children.length);
        }
        else {
            if (oldValue == 0)
                this._sortingChildCount++;

            var oldIndex: number = this._children.indexOf(child);
            var index: number = this.getInsertPosForSortingChild(child);
            if (oldIndex < index)
                this._setChildIndex(child, oldIndex, index - 1);
            else
                this._setChildIndex(child, oldIndex, index);
        }
    }

    protected onConstruct(): void {

    }

    protected override onEnable(): void {
        let cnt: number = this._transitions.length;
        for (let i: number = 0; i < cnt; ++i)
            this._transitions[i].onEnable();
    }

    protected override onDisable(): void {
        let cnt: number = this._transitions.length;
        for (let i: number = 0; i < cnt; ++i)
            this._transitions[i].onDisable();
    }
}


