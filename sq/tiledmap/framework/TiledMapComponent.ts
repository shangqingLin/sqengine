import { js, Rect, serializable, sqclass, SQJSON, type, Vec2 } from "../../core/index";
import { Component } from "../../framework/index";
import { Deserialize } from "../../serialization/deserialize";
import { TiledMapLayerData, TiledMapObjectLayerData } from "./data/TIledMapLayerData";
import TiledMapLayerType from "./data/TiledMapLayerType";
import TiledMapBaseLayer from "./TiledMapBaseLayer";
import { TiledMapConfig } from "./TiledMapConfig";
import TiledMapGridLayer from "./TiledMapGridLayer";
import TiledMapObjectLayer from "./TiledMapObjectLayer";
import Node from "../../scene/Node";
import { DynamicTree } from "../../core/utils/DynamicTree2D";
import AABB from "../../core/math/AABB";

/**
 * tilemap地图
 * 
 * 1、地图原点在坐标系第一象限左下角
 * 
 */
@sqclass("sq.TiledMapComponent")
export default class TiledMapComponent extends Component {

    @serializable
    @type(TiledMapBaseLayer, true)
    private layers: Array<TiledMapBaseLayer>;

    @serializable
    @type(SQJSON)
    private config: TiledMapConfig;

    private layersMap: { [key: string]: TiledMapBaseLayer };

    private viewport: Rect;

    //由Layer中维护这个属性
    private spritePool: Array<Node> = [];

    private fatQueryBound: AABB = new AABB();

    private objectTree: DynamicTree = new DynamicTree();
    private objectState: { [key: string]: number } = js.createMap();
    private viewX: number;
    private viewY: number;
    private mapWorldBoundInPixles: AABB = new AABB();

    private deserialize_layers(sValue: any, deserialize: Deserialize): void {
        this.layers = deserialize.deserializeObject(sValue);
        this.layersMap = Object.create(null);
        for (let i = 0, n = this.layers.length; i < n; ++i) {
            let layer = this.layers[i];
            if (layer.name) this.layersMap[layer.name] = layer;
        }
    }

    protected override onInitialize(): void {
        if (this.layers) {
            for (let i = 0, n = this.layers.length; i < n; ++i) {
                let layer = this.layers[i];
                this.node.addChild(layer);
                //@ts-ignore
                layer.onInitMap(this);
            }
        }

        let config = this.config;
        config.pixelsPerUnitX = config.pixelsPerUnitX;
        config.pixelsPerUnitY = config.pixelsPerUnitY;
        this.updateUnit();
    }

    setPixelsPerUnit(x: number, y: number) {
        if (!this.config) this.config = js.createMap();
        let config = this.config;
        if (config.pixelsPerUnitX !== x || config.pixelsPerUnitY !== x) {
            config.pixelsPerUnitX = x;
            config.pixelsPerUnitY = y;
            this.updateUnit();
        }
    }

    /**
     * 将地图拖到屏幕哪个地方显示
     * @param view 
     */
    setViewport(view: Rect) {

        let config = this.config;
        this.node.transform.x = view.x - config.bound.x * config.gridCellSizeX;
        this.node.transform.y = view.y - config.bound.y * config.gridCellSizeY;
        this.viewport = view;

        let minX = config.bound.x * config.gridCellSizeX
        let minY = config.bound.y * config.gridCellSizeY;
        let maxX = minX + config.bound.width * config.gridCellSizeX;
        let maxY = minY + config.bound.height * config.gridCellSizeY;
        this.node.transform.localToWorldPosition(minX, minY, this.mapWorldBoundInPixles.lowerBound);
        this.node.transform.localToWorldPosition(maxX, maxY, this.mapWorldBoundInPixles.upperBound);
        // console.info(">>>", this.mapWorldBoundInPixles.lowerBound.x, this.mapWorldBoundInPixles.lowerBound.y, this.mapWorldBoundInPixles.upperBound.x, this.mapWorldBoundInPixles.upperBound.y);
    }

    private updateUnit() {
        if (!this.config) this.config = js.createMap();
        let config = this.config;
        config.gridCellSizeX = config.pixelsPerUnitX * config.gridCellSizeUnitX;
        config.gridCellSizeY = config.pixelsPerUnitY * config.gridCellSizeUnitY;
    }

    hasLayer(name: string): boolean {
        return this.layersMap && !!this.layersMap[name];
    }

    layerIndex(layer: TiledMapBaseLayer): number {
        return this.node.getChildIndex(layer);
    }

    getLayer(layerName: string): TiledMapBaseLayer {
        return this.layersMap && this.layersMap[layerName];
    }

    getLayers(): Readonly<Array<TiledMapBaseLayer>> {
        return this.layers;
    }

    addLayer(name: string, data: TiledMapLayerData, index?: number): TiledMapBaseLayer {
        if (!this.layers) {
            this.layers = [];
            this.layersMap = Object.create(null);
        }

        /* debug:start */
        if (this.layersMap[name]) {
            throw new Error("层的名字已经存在");
        }
        /* debug:end */

        let layer: TiledMapBaseLayer;
        switch (data.layerType) {
            case TiledMapLayerType.TILED:
                layer = new TiledMapGridLayer();
                break;
            case TiledMapLayerType.OBJECT:
                layer = new TiledMapObjectLayer();
                break;
        }
        layer.name = name;

        //@ts-ignore
        layer.tiles = data.items;
        //@ts-ignore
        layer.onInitMap(this);

        if (index !== undefined) {
            this.layers.splice(index, 0, layer);
            this.node.addChildAt(layer, index);
        } else {
            this.layers.push(layer);
            this.node.addChild(layer);
        }
        this.layersMap[name] = layer;
        return layer;
    }

    removeLayer(nameOrNode: string | Node, destroy: boolean = true): TiledMapBaseLayer {
        if (nameOrNode instanceof Node) {
            nameOrNode = nameOrNode.name;
        }
        let layer = this.layersMap[nameOrNode];
        if (layer) {
            delete this.layersMap[nameOrNode];
            let index = this.layers.indexOf(layer);
            this.layers.splice(index, 1);
            if (destroy) layer.destroy();
            else this.node.removeChild(layer);
        }
        return layer;
    }

    /**
     * 
     *(mapX，mapY) : 和moveTo一样，以地图左下角为原点的坐标点
     * 
     */
    moveLimitCalculate(mapX: number, mapY: number, result: Vec2): Vec2 {

        let mapWorldBoundInPixles = this.mapWorldBoundInPixles;

        // console.info(mapX, mapY,
        //     mapWorldBoundInPixles.lowerBound.x, mapWorldBoundInPixles.lowerBound.y,
        //     mapWorldBoundInPixles.upperBound.x, mapWorldBoundInPixles.upperBound.y,
        // );

        let moveMinX = mapWorldBoundInPixles.lowerBound.x + mapX;
        let moveMinY = mapWorldBoundInPixles.lowerBound.y + mapY;
        let moveMaxX = moveMinX + this.viewport.width;
        let moveMaxY = moveMinY + this.viewport.height;

        if (mapWorldBoundInPixles.lowerBound.x <= moveMinX && moveMaxX <= mapWorldBoundInPixles.upperBound.x) {
            result.x = mapX;
        } else {

            if (moveMaxX > mapWorldBoundInPixles.upperBound.x) {
                //当前想要移动的距离超出了最大的范围
                result.x = mapWorldBoundInPixles.upperBound.x - this.viewport.width - mapWorldBoundInPixles.lowerBound.x;
                // console.info("????? ", result.x, mapX, moveMaxX, mapWorldBoundInPixles.upperBound.x);
            } else {
                //那就是超出最小范围了
                result.x = 0;
            }
        }

        if (mapWorldBoundInPixles.lowerBound.y <= moveMinY && moveMaxY <= mapWorldBoundInPixles.upperBound.y) {
            result.y = mapY;
        } else {
            if (moveMaxY > mapWorldBoundInPixles.upperBound.y) {
                result.y = mapWorldBoundInPixles.upperBound.y - this.viewport.height - mapWorldBoundInPixles.lowerBound.y;
            } else {
                result.y = 0;
            }
        }

        // console.info("move ", result.x, result.y);
        return result;
    }

    /**
     * 整张地图全部显示出来
     */
    showAll() {
        for (let i = 0, n = this.layers.length; i < n; ++i) {
            this.layers[i].showAll();
        }
    }


    /**
     * 根据位置和显示区域的大小，计算只需要显示的区域中指定的内容
     * 这里的坐标系是屏幕中心为原点，x轴向左为正，y轴向上为正。单位是像素
     * 
     * 这里TiledMap MoveTo不对内部的节点设置位置来实现Move To，而是通过Camera
     * 这个Camera是外部设置的，只是TiledMap渲染到这个Camera上而已，由Camera视点位置决定地图移动
     * 这里只对地图进行裁剪
     * 
     * (mapX,mapY)指定的是地图局部坐标点，是以地图左下角为原点的坐标点
     * @param mapX 
     * @param mapY
     */
    moveTo(mapX: number, mapY: number) {
        if (!this.viewport) return console.error("先设置viewport");
        if (this.viewX === mapX && this.viewY === mapY) return;

        // var a = new Vec2(x - this.viewX, y - this.viewY);

        this.viewX = mapX;
        this.viewY = mapY;

        let checkBound: AABB = AABB.TEMP;
        checkBound.lowerBound.x = mapX;
        checkBound.lowerBound.y = mapY;
        checkBound.upperBound.x = mapX + this.viewport.width;
        checkBound.upperBound.y = mapY + this.viewport.height;
        let fatQueryBound = this.fatQueryBound;

        // console.info("==========", AABB.Contains(this.fatQueryBound, checkBound), a.length(), this.viewport.width, this.viewport.height);
        // console.info(this.fatQueryBound.toString(), checkBound.toString());

        //避免外部一点点的小移动就来执行下面如此复杂的任务。
        //移动不超过一定范围是不会执行如下逻辑的
        if (AABB.Contains(fatQueryBound, checkBound)) {
            // console.info("++++++++in fat");
            return;
        }

        let config: TiledMapConfig = this.config;
        let padding = config.gridCellSizeY;

        fatQueryBound.lowerBound.x = mapX - padding;
        fatQueryBound.lowerBound.y = mapY - padding;
        fatQueryBound.upperBound.x = mapX + this.viewport.width + padding;
        fatQueryBound.upperBound.y = mapY + this.viewport.height + padding;

        //可以想一下当x=y=0时，刚好就是地图左下角坐标，所以这里相当于将x和y转换到地图的坐标系下
        let xGrid = fatQueryBound.lowerBound.x / config.gridCellSizeX + config.bound.x;
        let yGrid = fatQueryBound.lowerBound.y / config.gridCellSizeY + config.bound.y;
        let wGrid = (this.viewport.width + padding) / config.gridCellSizeX;
        let hGrid = (this.viewport.height + padding) / config.gridCellSizeY;

        for (let i = 0, n = this.layers.length; i < n; ++i) {
            if (this.layers[i] instanceof TiledMapGridLayer) {
                this.layers[i].showArea(xGrid, yGrid, wGrid, hGrid);
            }
        }

        let nodes: Array<string> = Object.keys(this.objectState);
        for (let i = 0, n = nodes.length; i < n; ++i) {
            let state: number = this.objectState[nodes[i]];
            if (state !== 0) {
                this.objectState[nodes[i]] |= TiledMapBaseLayer.hideState;
            }
        }

        AABB.TEMP.lowerBound.x = xGrid;
        AABB.TEMP.lowerBound.y = yGrid;
        AABB.TEMP.upperBound.x = xGrid + wGrid;
        AABB.TEMP.upperBound.y = yGrid + hGrid;

        this.objectTree.Query(AABB.TEMP, (proxyId: number, userData: any) => {
            if (userData && userData.layer) {
                if (this.objectState[proxyId] === undefined) {
                    this.objectState[proxyId] = 0;
                }
                if (!(this.objectState[proxyId] & TiledMapBaseLayer.showState)) {
                    this.objectState[proxyId] |= TiledMapBaseLayer.firstShowState;
                }
                this.objectState[proxyId] |= TiledMapBaseLayer.showState;
                this.objectState[proxyId] &= ~TiledMapBaseLayer.hideState;
            }
            return true;
        });

        nodes = Object.keys(this.objectState);
        for (let i = 0, n = nodes.length; i < n; ++i) {
            let proxyId = nodes[i];
            let state: number = this.objectState[proxyId];

            // console.info(" ==== obj check  : ", "proxy", proxyId, "state", state, state & TiledMapBaseLayer.hideState);

            if (state & TiledMapBaseLayer.firstShowState) {
                this.objectState[proxyId] &= ~TiledMapBaseLayer.firstShowState;
                let userData = this.objectTree.getUserData(Number(proxyId));
                // console.info("==================obj show : ", userData.fileId);
                (userData.layer as TiledMapObjectLayer).showItem(userData.fileId);
            } else if (state & TiledMapBaseLayer.hideState) {
                this.objectState[proxyId] = 0;
                // console.info("==================obj hide : ", proxyId);
                let userData = this.objectTree.getUserData(Number(proxyId));
                (userData.layer as TiledMapObjectLayer).hideItem(userData.fileId);
            }
        }
    }

    protected override onRemove(): void {
        super.onRemove();
        for (let i = 0, n = this.spritePool.length; i < n; ++i) {
            this.spritePool[i].destroy();
        }
        this.spritePool = null;
    }

}