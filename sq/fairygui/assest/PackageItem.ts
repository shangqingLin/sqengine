import { Asset } from "../../assets";
import { Rect, Vec2 } from "../../core";
import { ObjectType, PackageItemType } from "../common/FieldTypes";
import { UIPackage } from "./UIPackage";
import { ByteBuffer } from "../utils/ByteBuffer";
import { Frame } from "../display/MovieClip";
import { PixelHitTestData } from "../event/HitTest";


/**
 * 一个xml文件、一张图片、字体设置等一个文件对应一个PackageItem实例
 * 
 */
export class PackageItem {
    public owner: UIPackage;

    public type: PackageItemType;
    public objectType?: ObjectType;
    public id: string;

    /**
     * 文件名字（不包括路径）。即在编辑器中命名的文件名字
     */
    public name: string;

    /**
     * 文件的大小（比如Image图片的源大小、组件的尺寸）
     */
    public width: number = 0;
    public height: number = 0;

    //路径+名字
    public file: string;
    public decoded?: boolean;
    public loading?: Array<Function>;

    //如果是xml组件文件，则存储这个xml文件内容
    public rawData?: ByteBuffer;
    public asset?: Asset;

    public highResolution?: Array<string>;
    public branches?: Array<string>;

    //image
    public scale9Grid?: Rect;
    public scaleByTile?: boolean;
    public tileGridIndice?: number;
    public smoothing?: boolean;
    public hitTestData?: PixelHitTestData;

    //movieclip
    public interval?: number;
    public repeatDelay?: number;
    public swing?: boolean;
    public frames?: Array<Frame>;

    //扩展类。表示这个组件创建的是extensionType指定的类实例
    //如果不指定该属性，则创建GComponent类实例
    public extensionType?: any;

    //skeleton
    public skeletonAnchor?: Vec2;
    // public atlasAsset?: dragonBones.DragonBonesAtlasAsset;

    public constructor() {
    }

    public load(): Asset {
        return this.owner.getItemAsset(this);
    }

    public getBranch(): PackageItem {
        if (this.branches && this.owner._branchIndex != -1) {
            var itemId: string = this.branches[this.owner._branchIndex];
            if (itemId)
                return this.owner.getItemById(itemId);
        }

        return this;
    }

    public getHighResolution(): PackageItem {
        // if (this.highResolution && UIContentScaler.scaleLevel > 0) {
        //     var itemId: string = this.highResolution[UIContentScaler.scaleLevel - 1];
        //     if (itemId)
        //         return this.owner.getItemById(itemId);
        // }

        return this;
    }

    public toString(): string {
        return this.name;
    }
}