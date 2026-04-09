import { serializable, sqclass, SQJSON, type } from "../../../core/index";
import { TiledMapItemData } from "./TiledMapItemData";
import TiledLayerType from "./TiledMapLayerType";


@sqclass("sq.TiledMapLayerData")
export class TiledMapLayerData {

    @serializable
    @type(TiledLayerType)
    layerType: TiledLayerType;

    @serializable
    @type(TiledMapItemData, true)
    items?: Array<TiledMapItemData>;

    addItem(item: TiledMapItemData) {
        if (!this.items) {
            this.items = [];
        }
        this.items.push(item);
    }
}


export interface NodeBound {
    x: number;
    y: number;
    width: number;
    height: number;
    fileId: string;
    userData?:any;
}

@sqclass("sq.TiledMapObjectLayerData")
export class TiledMapObjectLayerData extends TiledMapLayerData {
    public bounds?: Array<NodeBound>;
}

@sqclass("sq.TiledMapGridLayerData")
export class TiledMapGridLayerData extends TiledMapLayerData {
}

