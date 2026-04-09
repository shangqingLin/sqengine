import Node from "../../scene/Node";
import { serializable, sqclass, SQJSON, type } from "../../core/index";
import { TiledMapItemData } from "./data/TiledMapItemData";
import { TiledMapGridLayerData } from "./data/TIledMapLayerData";
import TiledMapComponent from "./TiledMapComponent";

/**
 * TiledMap中的图层的基类
 */
@sqclass("sq.TiledMapBaseLayer")
export default abstract class TiledMapBaseLayer extends Node {

    @serializable
    @type(SQJSON)
    protected tiles: Array<TiledMapItemData>;

    public readonly map: TiledMapComponent;

    public static readonly showState: number = 1;
    public static readonly hideState: number = 1 << 2;
    public static readonly firstShowState: number = 1 << 3;

    /**
     * 显示整张地图
     */
    abstract showAll(): void;

    abstract showArea(x: number, y: number, width: number, height: number): void;

    //TiledMapComponent中调用
    protected onInitMap(map: TiledMapBaseLayer) {
        //@ts-ignore
        this.map = map;
    }
}