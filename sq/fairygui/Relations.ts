
/**
 * 关联关系
 */

import { FUIEvent } from "./FUIEvent";
import type { GObject } from "./GObject";


export enum RelationType {
    Left_Left, //左-左
    Left_Center,//左-中
    Left_Right,	//左-右
    Center_Center, //左右居中
    Right_Left,	//右-左
    Right_Center,//右-中
    Right_Right,//右-右	
    Top_Top,//顶-顶
    Top_Middle,	//顶-中
    Top_Bottom,	//顶-底
    Middle_Middle,	//上下居中
    Bottom_Top,	//顶-底
    Bottom_Middle,//顶-中	
    Bottom_Bottom,	//底-底
    Width, //宽->宽
    Height, //高->高
    LeftExt_Left,//左延伸-左
    LeftExt_Right,//左延伸-右
    RightExt_Left,//右延伸-左
    RightExt_Right,//右延伸-右
    TopExt_Top,//顶延伸-顶
    TopExt_Bottom,//顶延伸-底
    BottomExt_Top,//底延伸-顶
    BottomExt_Bottom,//底延伸-底
    Size //
}



export class RelationDef {
    public percent: boolean = false;
    public type: RelationType = 0;
    public axis: number = 0;
    public copyFrom(source: RelationDef): void {
        this.percent = source.percent;
        this.type = source.type;
        this.axis = source.axis;
    }
}

/**
 * 一对GObject对应一个RelationItem，一个RelationItem实例中可以包含多个关联
 */
export class RelationItem {
    private _owner: GObject;
    private _target: GObject;
    private _defs: Array<RelationDef>;
    private _targetX: number;
    private _targetY: number;
    private _targetWidth: number;
    private _targetHeight: number;

    public constructor(owner: GObject) {
        this._owner = owner;
        this._defs = new Array<RelationDef>();
    }

    public get owner(): GObject {
        return this._owner;
    }

    public set target(value: GObject) {
        if (this._target != value) {
            if (this._target)
                this.releaseRefTarget(this._target);
            this._target = value;
            if (this._target)
                this.addRefTarget(this._target);
        }
    }

    public get target(): GObject {
        return this._target;
    }

    public add(relationType: RelationType, usePercent?: boolean): void {
        if (relationType === RelationType.Size) {
            this.add(RelationType.Width, usePercent);
            this.add(RelationType.Height, usePercent);
            return;
        }

        var length: number = this._defs.length;
        for (var i: number = 0; i < length; i++) {
            var def: RelationDef = this._defs[i];
            if (def.type === relationType)
                return;
        }

        this.internalAdd(relationType, usePercent);
    }

    public internalAdd(relationType: RelationType, usePercent?: boolean): void {
        if (relationType === RelationType.Size) {
            this.internalAdd(RelationType.Width, usePercent);
            this.internalAdd(RelationType.Height, usePercent);
            return;
        }

        var info: RelationDef = new RelationDef();
        info.percent = usePercent;
        info.type = relationType;
        info.axis = (relationType <= RelationType.Right_Right || relationType === RelationType.Width
            || relationType >= RelationType.LeftExt_Left && relationType <= RelationType.RightExt_Right) ? 0 : 1;
        this._defs.push(info);
    }

    public remove(relationType: RelationType): void {
        if (relationType == RelationType.Size) {
            this.remove(RelationType.Width);
            this.remove(RelationType.Height);
            return;
        }

        var dc: number = this._defs.length;
        for (var k: number = 0; k < dc; k++) {
            if (this._defs[k].type === relationType) {
                this._defs.splice(k, 1);
                break;
            }
        }
    }

    private addRefTarget(target: GObject): void {
        if (target != this._owner.parent)
            target.on(FUIEvent.XY_CHANGED, this.__targetXYChanged, this);
        target.on(FUIEvent.SIZE_CHANGED, this.__targetSizeChanged, this);
        target.on(FUIEvent.SIZE_DELAY_CHANGE, this.__targetSizeWillChange, this);

        this._targetX = this._target.x;
        this._targetY = this._target.y;

        //@ts-ignore
        this._targetWidth = this._target._width;

        //@ts-ignore
        this._targetHeight = this._target._height;
    }

    private releaseRefTarget(target: GObject): void {
        if (!target.node)
            return;
        target.off(FUIEvent.XY_CHANGED, this.__targetXYChanged, this);
        target.off(FUIEvent.SIZE_CHANGED, this.__targetSizeChanged, this);
        target.off(FUIEvent.SIZE_DELAY_CHANGE, this.__targetSizeWillChange, this);
    }

    public dispose(): void {
        if (this._target) {
            this.releaseRefTarget(this._target);
            this._target = null;
        }
    }

    public get isEmpty(): boolean {
        return this._defs.length == 0;
    }

    /**
     * target GObject的XY改变时，需要重新计算target Object的定位
     * @param evt 
     * @returns 
     */
    private __targetXYChanged(evt: Event): void {
        if (this._owner.relations.handling != null || this._owner.group != null && this._owner.group._updating) {
            this._targetX = this._target.x;
            this._targetY = this._target.y;
            return;
        }

        this._owner.relations.handling = this._target;

        var ox: number = this._owner.x;
        var oy: number = this._owner.y;
        var dx: number = this._target.x - this._targetX;
        var dy: number = this._target.y - this._targetY;
        var length: number = this._defs.length;
        for (var i: number = 0; i < length; i++) {
            var info: RelationDef = this._defs[i];
            this.applyOnXYChanged(info, dx, dy);
        }
        this._targetX = this._target.x;
        this._targetY = this._target.y;

        if (ox != this._owner.x || oy != this._owner.y) {
            ox = this._owner.x - ox;
            oy = this._owner.y - oy;

            //@ts-ignore 调用GearXY
            this._owner.updateGearFromRelations(1, ox, oy);

            if (this._owner.parent) {
                var len: number = this._owner.parent._transitions.length;
                if (len > 0) {
                    for (var i: number = 0; i < len; ++i) {
                        this._owner.parent._transitions[i].updateFromRelations(this._owner.id as string, ox, oy);
                    }
                }
            }
        }
        this._owner.relations.handling = null;
    }

    /**
     * target GObject的大小改变，需要重新计算owner的定位。
     * @param evt 
     * @returns 
     */
    private __targetSizeChanged(evt: Event): void {
        if (this._owner.relations.handling != null)
            return;

        this._owner.relations.handling = this._target;

        var ox: number = this._owner.x;
        var oy: number = this._owner.y;
        var ow: number = this._owner._rawWidth;
        var oh: number = this._owner._rawHeight;
        var length: number = this._defs.length;
        for (var i: number = 0; i < length; i++) {
            var info: RelationDef = this._defs[i];
            this.applyOnSizeChanged(info);
        }
        this._targetWidth = this._target._width;
        this._targetHeight = this._target._height;

        if (ox != this._owner.x || oy != this._owner.y) {
            ox = this._owner.x - ox;
            oy = this._owner.y - oy;

            //@ts-ignore 调用GearXY
            this._owner.updateGearFromRelations(1, ox, oy);

            if (this._owner.parent) {
                var len: number = this._owner.parent._transitions.length;
                if (len > 0) {
                    for (var i: number = 0; i < len; ++i) {
                        this._owner.parent._transitions[i].updateFromRelations(this._owner.id as string, ox, oy);
                    }
                }
            }
        }

        if (ow != this._owner._rawWidth || oh != this._owner._rawHeight) {
            ow = this._owner._rawWidth - ow;
            oh = this._owner._rawHeight - oh;

            //@ts-ignore 调用GearSize
            this._owner.updateGearFromRelations(2, ow, oh);
        }

        this._owner.relations.handling = null;
    }

    private __targetSizeWillChange(evt: Event): void {
        this._owner.relations.sizeDirty = true;
    }

    /**
     * owner即自身的大小改变，重新计算定位
     * @param dWidth 
     * @param dHeight 
     * @param applyPivot 
     */
    public applyOnSelfResized(dWidth: number, dHeight: number, applyPivot: boolean): void {
        var ox: number = this._owner.x;
        var oy: number = this._owner.y;

        var length: number = this._defs.length;
        for (var i: number = 0; i < length; i++) {
            var info: RelationDef = this._defs[i];
            switch (info.type) {
                case RelationType.Center_Center:
                    //自身宽度增长了dwidth，则需要往右边挪一半才能保持初始居中状态
                    this._owner.x -= (0.5 - (applyPivot ? this._owner.pivotX : 0)) * dWidth;
                    break;
                case RelationType.Right_Center:
                case RelationType.Right_Left:
                case RelationType.Right_Right:
                    this._owner.x -= (1 - (applyPivot ? this._owner.pivotX : 0)) * dWidth;
                    break;

                case RelationType.Middle_Middle:
                    this._owner.y -= (0.5 - (applyPivot ? this._owner.pivotY : 0)) * dHeight;
                    break;
                case RelationType.Bottom_Middle:
                case RelationType.Bottom_Top:
                case RelationType.Bottom_Bottom:
                    this._owner.y -= (1 - (applyPivot ? this._owner.pivotY : 0)) * dHeight;
                    break;
            }
        }

        //判断旧的位置和新的位置是否发生了变动
        if (ox != this._owner.x || oy != this._owner.y) {

            //计算发生多少的变动
            ox = this._owner.x - ox;
            oy = this._owner.y - oy;

            //@ts-ignore 调用GearXY。自身的位置发生更改了，那么位置控制器也需要执行
            this._owner.updateGearFromRelations(1, ox, oy);

            if (this._owner.parent) {
                var len: number = this._owner.parent._transitions.length;
                if (len > 0) {
                    for (var i: number = 0; i < len; ++i) {
                        this._owner.parent._transitions[i].updateFromRelations(this._owner.id as string, ox, oy);
                    }
                }
            }
        }
    }


    private applyOnXYChanged(info: RelationDef, dx: number, dy: number): void {
        var tmp: number;

        switch (info.type) {
            case RelationType.Left_Left:
            case RelationType.Left_Center:
            case RelationType.Left_Right:
            case RelationType.Center_Center:
            case RelationType.Right_Left:
            case RelationType.Right_Center:
            case RelationType.Right_Right:
                this._owner.x += dx;
                break;

            case RelationType.Top_Top:
            case RelationType.Top_Middle:
            case RelationType.Top_Bottom:
            case RelationType.Middle_Middle:
            case RelationType.Bottom_Top:
            case RelationType.Bottom_Middle:
            case RelationType.Bottom_Bottom:
                this._owner.y += dy;
                break;

            case RelationType.Width:
            case RelationType.Height:
                break;

            case RelationType.LeftExt_Left:
            case RelationType.LeftExt_Right:
                if (this._owner != this._target.parent) {
                    tmp = this._owner.xMin;
                    this._owner.width = this._owner._rawWidth - dx;
                    this._owner.xMin = tmp + dx;
                }
                else
                    this._owner.width = this._owner._rawWidth - dx;
                break;

            case RelationType.RightExt_Left:
            case RelationType.RightExt_Right:
                if (this._owner != this._target.parent) {
                    tmp = this._owner.xMin;
                    this._owner.width = this._owner._rawWidth + dx;
                    this._owner.xMin = tmp;
                }
                else
                    this._owner.width = this._owner._rawWidth + dx;
                break;

            case RelationType.TopExt_Top:
            case RelationType.TopExt_Bottom:
                if (this._owner != this._target.parent) {
                    tmp = this._owner.yMin;
                    this._owner.height = this._owner._rawHeight - dy;
                    this._owner.yMin = tmp + dy;
                }
                else
                    this._owner.height = this._owner._rawHeight - dy;
                break;

            case RelationType.BottomExt_Top:
            case RelationType.BottomExt_Bottom:
                if (this._owner != this._target.parent) {
                    tmp = this._owner.yMin;
                    this._owner.height = this._owner._rawHeight + dy;
                    this._owner.yMin = tmp;
                }
                else
                    this._owner.height = this._owner._rawHeight + dy;
                break;
        }
    }

    private applyOnSizeChanged(info: RelationDef): void {
        var pos: number = 0, pivot: number = 0, delta: number = 0;
        var v: number, tmp: number;

        if (info.axis == 0) {
            if (this._target != this._owner.parent) {
                pos = this._target.x;
                if (this._target.pivotAsAnchor)
                    pivot = this._target.pivotX;
            }

            if (info.percent) {
                if (this._targetWidth != 0)
                    delta = this._target._width / this._targetWidth;
            }
            else
                delta = this._target._width - this._targetWidth;
        }
        else {
            if (this._target != this._owner.parent) {
                pos = this._target.y;
                if (this._target.pivotAsAnchor)
                    pivot = this._target.pivotY;
            }

            if (info.percent) {
                if (this._targetHeight != 0)
                    delta = this._target._height / this._targetHeight;
            }
            else
                delta = this._target._height - this._targetHeight;
        }

        switch (info.type) {
            case RelationType.Left_Left:
                if (info.percent)
                    this._owner.xMin = pos + (this._owner.xMin - pos) * delta;
                else if (pivot != 0)
                    this._owner.x += delta * (-pivot);
                break;
            case RelationType.Left_Center:
                if (info.percent)
                    this._owner.xMin = pos + (this._owner.xMin - pos) * delta;
                else
                    this._owner.x += delta * (0.5 - pivot);
                break;
            case RelationType.Left_Right:
                if (info.percent)
                    this._owner.xMin = pos + (this._owner.xMin - pos) * delta;
                else
                    this._owner.x += delta * (1 - pivot);
                break;
            case RelationType.Center_Center:
                if (info.percent)
                    this._owner.xMin = pos + (this._owner.xMin + this._owner._rawWidth * 0.5 - pos) * delta - this._owner._rawWidth * 0.5;
                else
                    this._owner.x += delta * (0.5 - pivot);
                break;
            case RelationType.Right_Left:
                if (info.percent)
                    this._owner.xMin = pos + (this._owner.xMin + this._owner._rawWidth - pos) * delta - this._owner._rawWidth;
                else if (pivot != 0)
                    this._owner.x += delta * (-pivot);
                break;
            case RelationType.Right_Center:
                if (info.percent)
                    this._owner.xMin = pos + (this._owner.xMin + this._owner._rawWidth - pos) * delta - this._owner._rawWidth;
                else
                    this._owner.x += delta * (0.5 - pivot);
                break;
            case RelationType.Right_Right:
                if (info.percent)
                    this._owner.xMin = pos + (this._owner.xMin + this._owner._rawWidth - pos) * delta - this._owner._rawWidth;
                else
                    this._owner.x += delta * (1 - pivot);
                break;

            case RelationType.Top_Top:
                if (info.percent)
                    this._owner.yMin = pos + (this._owner.yMin - pos) * delta;
                else if (pivot != 0)
                    this._owner.y += delta * (-pivot);
                break;
            case RelationType.Top_Middle:
                if (info.percent)
                    this._owner.yMin = pos + (this._owner.yMin - pos) * delta;
                else
                    this._owner.y += delta * (0.5 - pivot);
                break;
            case RelationType.Top_Bottom:
                if (info.percent)
                    this._owner.yMin = pos + (this._owner.yMin - pos) * delta;
                else
                    this._owner.y += delta * (1 - pivot);
                break;
            case RelationType.Middle_Middle:
                if (info.percent)
                    this._owner.yMin = pos + (this._owner.yMin + this._owner._rawHeight * 0.5 - pos) * delta - this._owner._rawHeight * 0.5;
                else
                    this._owner.y += delta * (0.5 - pivot);
                break;
            case RelationType.Bottom_Top:
                if (info.percent)
                    this._owner.yMin = pos + (this._owner.yMin + this._owner._rawHeight - pos) * delta - this._owner._rawHeight;
                else if (pivot != 0)
                    this._owner.y += delta * (-pivot);
                break;
            case RelationType.Bottom_Middle:
                if (info.percent)
                    this._owner.yMin = pos + (this._owner.yMin + this._owner._rawHeight - pos) * delta - this._owner._rawHeight;
                else
                    this._owner.y += delta * (0.5 - pivot);
                break;
            case RelationType.Bottom_Bottom:
                if (info.percent)
                    this._owner.yMin = pos + (this._owner.yMin + this._owner._rawHeight - pos) * delta - this._owner._rawHeight;
                else
                    this._owner.y += delta * (1 - pivot);
                break;

            case RelationType.Width:
                if (this._owner._underConstruct && this._owner == this._target.parent)
                    v = this._owner.sourceWidth - this._target.initWidth;
                else
                    v = this._owner._rawWidth - this._targetWidth;
                if (info.percent)
                    v = v * delta;
                if (this._target == this._owner.parent) {
                    if (this._owner.pivotAsAnchor) {
                        tmp = this._owner.xMin;
                        this._owner.setSize(this._target._width + v, this._owner._rawHeight, true);
                        this._owner.xMin = tmp;
                    }
                    else
                        this._owner.setSize(this._target._width + v, this._owner._rawHeight, true);
                }
                else
                    this._owner.width = this._target._width + v;
                break;
            case RelationType.Height:
                if (this._owner._underConstruct && this._owner == this._target.parent)
                    v = this._owner.sourceHeight - this._target.initHeight;
                else
                    v = this._owner._rawHeight - this._targetHeight;
                if (info.percent)
                    v = v * delta;
                if (this._target == this._owner.parent) {
                    if (this._owner.pivotAsAnchor) {
                        tmp = this._owner.yMin;
                        this._owner.setSize(this._owner._rawWidth, this._target._height + v, true);
                        this._owner.yMin = tmp;
                    }
                    else
                        this._owner.setSize(this._owner._rawWidth, this._target._height + v, true);
                }
                else
                    this._owner.height = this._target._height + v;
                break;

            case RelationType.LeftExt_Left:
                tmp = this._owner.xMin;
                if (info.percent)
                    v = pos + (tmp - pos) * delta - tmp;
                else
                    v = delta * (-pivot);
                this._owner.width = this._owner._rawWidth - v;
                this._owner.xMin = tmp + v;
                break;
            case RelationType.LeftExt_Right:
                tmp = this._owner.xMin;
                if (info.percent)
                    v = pos + (tmp - pos) * delta - tmp;
                else
                    v = delta * (1 - pivot);
                this._owner.width = this._owner._rawWidth - v;
                this._owner.xMin = tmp + v;
                break;
            case RelationType.RightExt_Left:
                tmp = this._owner.xMin;
                if (info.percent)
                    v = pos + (tmp + this._owner._rawWidth - pos) * delta - (tmp + this._owner._rawWidth);
                else
                    v = delta * (-pivot);
                this._owner.width = this._owner._rawWidth + v;
                this._owner.xMin = tmp;
                break;
            case RelationType.RightExt_Right:
                tmp = this._owner.xMin;
                if (info.percent) {
                    if (this._owner == this._target.parent) {
                        if (this._owner._underConstruct)
                            this._owner.width = pos + this._target._width - this._target._width * pivot +
                                (this._owner.sourceWidth - pos - this._target.initWidth + this._target.initWidth * pivot) * delta;
                        else
                            this._owner.width = pos + (this._owner._rawWidth - pos) * delta;
                    }
                    else {
                        v = pos + (tmp + this._owner._rawWidth - pos) * delta - (tmp + this._owner._rawWidth);
                        this._owner.width = this._owner._rawWidth + v;
                        this._owner.xMin = tmp;
                    }
                }
                else {
                    if (this._owner == this._target.parent) {
                        if (this._owner._underConstruct)
                            this._owner.width = this._owner.sourceWidth + (this._target._width - this._target.initWidth) * (1 - pivot);
                        else
                            this._owner.width = this._owner._rawWidth + delta * (1 - pivot);
                    }
                    else {
                        v = delta * (1 - pivot);
                        this._owner.width = this._owner._rawWidth + v;
                        this._owner.xMin = tmp;
                    }
                }
                break;
            case RelationType.TopExt_Top:
                tmp = this._owner.yMin;
                if (info.percent)
                    v = pos + (tmp - pos) * delta - tmp;
                else
                    v = delta * (-pivot);
                this._owner.height = this._owner._rawHeight - v;
                this._owner.yMin = tmp + v;
                break;
            case RelationType.TopExt_Bottom:
                tmp = this._owner.yMin;
                if (info.percent)
                    v = pos + (tmp - pos) * delta - tmp;
                else
                    v = delta * (1 - pivot);
                this._owner.height = this._owner._rawHeight - v;
                this._owner.yMin = tmp + v;
                break;
            case RelationType.BottomExt_Top:
                tmp = this._owner.yMin;
                if (info.percent)
                    v = pos + (tmp + this._owner._rawHeight - pos) * delta - (tmp + this._owner._rawHeight);
                else
                    v = delta * (-pivot);
                this._owner.height = this._owner._rawHeight + v;
                this._owner.yMin = tmp;
                break;
            case RelationType.BottomExt_Bottom:
                tmp = this._owner.yMin;
                if (info.percent) {
                    if (this._owner == this._target.parent) {
                        if (this._owner._underConstruct)
                            this._owner.height = pos + this._target._height - this._target._height * pivot +
                                (this._owner.sourceHeight - pos - this._target.initHeight + this._target.initHeight * pivot) * delta;
                        else
                            this._owner.height = pos + (this._owner._rawHeight - pos) * delta;
                    }
                    else {
                        v = pos + (tmp + this._owner._rawHeight - pos) * delta - (tmp + this._owner._rawHeight);
                        this._owner.height = this._owner._rawHeight + v;
                        this._owner.yMin = tmp;
                    }
                }
                else {
                    if (this._owner == this._target.parent) {
                        if (this._owner._underConstruct)
                            this._owner.height = this._owner.sourceHeight + (this._target._height - this._target.initHeight) * (1 - pivot);
                        else
                            this._owner.height = this._owner._rawHeight + delta * (1 - pivot);
                    }
                    else {
                        v = delta * (1 - pivot);
                        this._owner.height = this._owner._rawHeight + v;
                        this._owner.yMin = tmp;
                    }
                }
                break;
        }
    }
}

export class Relations {

    private _owner: GObject;
    private _items: Array<RelationItem>;
    public sizeDirty: boolean = false;
    public handling: GObject | null;

    constructor(owner: GObject) {
        this._owner = owner;
        this._items = new Array<RelationItem>();
    }

    /**
     * 为当前的Owner Object添加一个关联
     * @param target 与之关联的GObject
     * @param relationType 
     * @param usePercent 
     * @returns 
     */
    add(target: GObject, relationType: RelationType, usePercent?: boolean) {
        var length: number = this._items.length;
        for (var i: number = 0; i < length; i++) {
            var item: RelationItem = this._items[i];
            if (item.target === target) {
                item.add(relationType, usePercent);
                return;
            }
        }

        var newItem: RelationItem = new RelationItem(this._owner);
        newItem.target = target;
        newItem.add(relationType, usePercent);
        this._items.push(newItem);
    }

    public remove(target: GObject, relationType?: number): void {
        relationType = relationType || 0;
        var cnt: number = this._items.length;
        var i: number = 0;
        while (i < cnt) {
            var item: RelationItem = this._items[i];
            if (item.target == target) {
                item.remove(relationType);
                if (item.isEmpty) {
                    item.dispose();
                    this._items.splice(i, 1);
                    cnt--;
                }
                else
                    i++;
            }
            else
                i++;
        }
    }

    public contains(target: GObject): boolean {
        var length: number = this._items.length;
        for (var i: number = 0; i < length; i++) {
            var item: RelationItem = this._items[i];
            if (item.target == target)
                return true;
        }
        return false;
    }

    public clearFor(target: GObject): void {
        var cnt: number = this._items.length;
        var i: number = 0;
        while (i < cnt) {
            var item: RelationItem = this._items[i];
            if (item.target == target) {
                item.dispose();
                this._items.splice(i, 1);
                cnt--;
            }
            else
                i++;
        }
    }

    public clearAll(): void {
        var length: number = this._items.length;
        for (var i: number = 0; i < length; i++) {
            var item: RelationItem = this._items[i];
            item.dispose();
        }
        this._items.length = 0;
    }

    public ensureRelationsSizeCorrect(): void {
        if (this._items.length == 0)
            return;

        this.sizeDirty = false;
        var length: number = this._items.length;
        for (var i: number = 0; i < length; i++) {
            var item: RelationItem = this._items[i];
            //@ts-ignore
            item.target.ensureSizeCorrect();
        }
    }

    /**
     * 在GObject中调用。
     * 关联的主要作用就是用来做相对其他的GObject进行定位，而定位主要是受到GObject的大小和位置的影响。
     * 所以，当前owner的GObject大小更改时就会触发这个函数调用，重新计算定位
     * @param dWidth 
     * @param dHeight 
     * @param applyPivot 
     * @returns 
     */
    private onOwnerSizeChanged(dWidth: number, dHeight: number, applyPivot: boolean): void {
        if (this._items.length == 0)
            return;

        var length: number = this._items.length;
        for (var i: number = 0; i < length; i++) {
            var item: RelationItem = this._items[i];
            item.applyOnSelfResized(dWidth, dHeight, applyPivot);
        }
    }

    public dispose(): void {
        this.clearAll();
    }

}