import { editableProp, serializable, sqclass, SQFloat, SQString, type } from "../../../core/index";
import type Node from "../../../scene/Node";

@sqclass("sq.TiledMapItemData")
export class TiledMapItemData {}

@sqclass("sq.TiledMapGridItemData")
export class TiledMapGridItemData extends TiledMapItemData {
    @type(SQFloat)
    @serializable
    /*editor:start*/
    @editableProp(0)
    /*editor:end*/
    x: number;

    @type(SQFloat)
    @serializable
    /*editor:start*/
    @editableProp(1)
    /*editor:end*/
    y: number;

    @type(SQFloat)
    @serializable
    /*editor:start*/
    @editableProp(2)
    /*editor:end*/
    width: number;

    @type(SQFloat)
    @serializable
    /*editor:start*/
    @editableProp(3)
    /*editor:end*/
    height: number;

    //如果当前的grid是一个有显示的东西的grid，则存储显示的节点
    node?: Node;

    //如果这个grid显示一个资源（Prefab、蓝图、图片等）
    //则这里保存这个资源的uuid
    @type(SQString)
    @serializable
    assetUUId?: string;

    //或直接存储一张图片
    @type(SQString)
    @serializable
    spriteUrl?: string;

    public state: number = 0;
}

