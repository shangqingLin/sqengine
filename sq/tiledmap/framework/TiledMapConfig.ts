import { Rect } from "../../core";
import AABB from "../../core/math/AABB";

export interface TiledMapConfig {

    /**
     * 地图的包围盒
     */
    bound: Rect;
    
    //一个单元的大小，以像素为单位
    pixelsPerUnitX: number;
    pixelsPerUnitY: number;

    /**
     * 每个Cell的大小：以单元格为单位。
     */
    gridCellSizeUnitX: number;
    gridCellSizeUnitY: number;

    /**
     * 一个Cell实际的像素大小为：
     *  gridCellSizeX = pixelsPerUnitX * gridCellSizeUnitX;
     *  gridCellSizeY = pixelsPerUnitY * gridCellSizeUnitY;
     */
    gridCellSizeX: number;
    gridCellSizeY: number;

    gridCellGapUnitX: number;
    gridCellGapUnitY: number;
}


