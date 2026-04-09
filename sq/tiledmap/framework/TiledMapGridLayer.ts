import { SpriteComponent } from "../../2d/index";
import { CommonUtils, findClassSlashByClass, js, PropertyStash, serializable, sqclass, SQJSON, type } from "../../core/index";
import { TiledMapGridItemData, TiledMapItemData } from "./data/TiledMapItemData";
import TiledMapBaseLayer from "./TiledMapBaseLayer";
import Node from "../../scene/Node";
import { Deserialize } from "../../serialization";
import { TiledMapConfig } from "./TiledMapConfig";

@sqclass("sq.TiledMapGridLayer")
export default class TiledMapGridLayer extends TiledMapBaseLayer {


    @serializable
    @type(SQJSON)
    private tileAnchor: { x: number, y: number };



    override showAll(): void {
        for (let i = 0, n = this.tiles.length; i < n; ++i) {
            let tileData: TiledMapGridItemData = this.tiles[i] as TiledMapGridItemData;
            if (!(tileData.state & TiledMapGridLayer.showState)) {
                this.addToShow(tileData);
            }
            tileData.state |= TiledMapGridLayer.showState;
        }
    }

    override showArea(x: number, y: number, width: number, height: number): void {
        
        let count: number = this.tiles.length;
        for (let i = 0; i < count; ++i) {
            let tileData: TiledMapGridItemData = this.tiles[i] as TiledMapGridItemData;
            tileData.state |= TiledMapGridLayer.hideState;
        }

        //@ts-ignore
        // let config: TiledMapConfig = this.map.config;
        // console.info("-----------------showArea", x, y, width, height, " map bound: ", config.bound.x, config.bound.y, config.bound.width, config.bound.height);

        for (let i = 0; i < count; ++i) {
            let tileData: TiledMapGridItemData = this.tiles[i] as TiledMapGridItemData;

            // console.info("check ", "index: " + i, tileData.x, tileData.y, tileData.width, tileData.height);

            if (tileData.x + tileData.width < x || x + width < tileData.x) {
                continue;
            }

            if (tileData.y + tileData.height < y || y + height < tileData.y) {
                continue;
            }


            tileData.state &= ~TiledMapGridLayer.hideState;
            if (!(tileData.state & TiledMapGridLayer.showState)) {
                tileData.state |= TiledMapGridLayer.firstShowState;
            }
            tileData.state |= TiledMapGridLayer.showState;
        }


        // let showCount = 0;
        for (let i = 0; i < count; ++i) {
            let tileData: TiledMapGridItemData = this.tiles[i] as TiledMapGridItemData;

            // if ((tileData.state & TiledMapGridLayer.hideState)) {
            //     console.info("hide ", "index : " + i, tileData.x, tileData.y, tileData.width, tileData.height);
            // }

            if ((tileData.state & TiledMapGridLayer.showState) && (tileData.state & TiledMapGridLayer.hideState)) {
                tileData.state = 0;
                tileData.node.removeFromParent();
                //@ts-ignore
                this.map.spritePool.push(tileData.node);
                tileData.node = null;

                // console.info("remove ", "index: " + i, tileData.x, tileData.y);

            } else if (tileData.state & TiledMapGridLayer.firstShowState) {
                tileData.state &= ~TiledMapGridLayer.firstShowState;

                // console.info("show ", "index: " + i, tileData.x, tileData.y, tileData.width, tileData.height);
                // ++showCount;

                if (!tileData.node) {
                    this.addToShow(tileData);
                }
            }
        }
    }

    

    private addToShow(data: TiledMapGridItemData) {
        //@ts-ignore
        let spriteNode: Node = this.map.spritePool.pop();
        if (!spriteNode) {
            spriteNode = new Node();
            spriteNode.addComponent(SpriteComponent);
        }
        data.node = spriteNode;
        let sprite = spriteNode.getComponent(SpriteComponent);
        sprite.once(SpriteComponent.ONLOADED, this, this.onSpriteLoaded, data);
        sprite.spriteFrameUrl = data.spriteUrl;
        this.addChild(spriteNode);
    }


    private onSpriteLoaded(data: TiledMapGridItemData) {
        this.calculateGridCellPos(data);
    }

    private calculateGridCellPos(spriteData: TiledMapGridItemData) {
        let spriteNode = spriteData.node;
        let sprite = spriteNode.getComponent(SpriteComponent);
        let spriteFrame = sprite.spriteFrame;


        //@ts-ignore
        let config: TiledMapConfig = this.map.config;
        let sPixelsPerUnit: number = spriteFrame.spriteAtlas.pixelsPerUnit;
        //计算Sprite占用的Cell的个数,从而计算Sprite实际渲染的大小
        let useGridNumX: number = spriteFrame.rect.width / sPixelsPerUnit;
        let useGridNumY: number = spriteFrame.rect.height / sPixelsPerUnit;

        //计算每个Sprite实际渲染的大小
        //所以Sprite渲染的大小和源大小可以不一致的
        let spriteRenderW = config.gridCellSizeX * useGridNumX;
        let spriteRenderH = config.gridCellSizeY * useGridNumY;
        sprite.width = spriteRenderW;
        sprite.height = spriteRenderH;
        let ratioW = spriteRenderW / spriteFrame.rect.width;
        let ratioH = spriteRenderH / spriteFrame.rect.height;

        //Sprite的锚点原点在左下角，和Grid Cell是一样的
        //然后将Sprite的锚点位置对齐到Grid的锚点位置，就是当前这个格子内容的位置了
        let spriteAnchorX: number = spriteFrame.pivot.x * ratioW;
        let spriteAnchorY: number = spriteFrame.pivot.y * ratioH;

        //先计算格子的位置，Unity tiledmap中格子的原点在左下角
        let gridX: number = spriteData.x * config.gridCellSizeX + spriteData.x * (config.gridCellGapUnitX * config.gridCellSizeX);
        let gridY: number = spriteData.y * config.gridCellSizeY + spriteData.y * (config.gridCellGapUnitY * config.gridCellSizeX);

        //计算Grid的锚点
        gridX += this.tileAnchor.x * config.gridCellSizeX;
        gridY += this.tileAnchor.y * config.gridCellSizeY;

        
        sprite.anchorX = sprite.anchorY = 0;
        spriteNode.transform.x = gridX - spriteAnchorX;
        spriteNode.transform.y = gridY - spriteAnchorY;

        // console.info(spriteFrame.keyUrl,
        //     "Grid:(", gridX, gridY, ")",
        //     "pivot:(", spriteFrame.pivot.x, spriteFrame.pivot.y, ")",
        //     "posGrid:(", spriteData.x, spriteData.y, ")",
        //     "pos(" + spriteNode.transform.x + "," + spriteNode.transform.y + ")"
        // );
    }

    private deserialize_tiles(value: Array<Object>, deserialize: Deserialize) {
        let properties: Record<PropertyKey, PropertyStash> = findClassSlashByClass(TiledMapGridItemData).properties;
        let propertyNames: Array<string> = Object.keys(properties);
        this.tiles = new Array(value.length);
        for (let i = 0, n = value.length; i < n; ++i) {
            let tileData: TiledMapGridItemData = new TiledMapGridItemData();
            let jsonData: any = value[i];
            for (let c = 0; c < propertyNames.length; ++c) {
                let name: string = propertyNames[c];
                if (CommonUtils.isDefine(jsonData[name])) {
                    (tileData as any)[name] = jsonData[name];
                }
            }
            this.tiles[i] = tileData;
        }
    }
}