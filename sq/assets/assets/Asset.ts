/**
 * 资源基类
 */

import { EventDispatch, IDGen } from "../../core/index";
import { AssetInfo } from "../assetmanager/config";
import { AssetManager } from "../index";

var idGen = new IDGen();

export default class Asset extends EventDispatch {
    protected native: Module.NativeBaseObj;

    protected readonly id: number;
    public readonly keyUrl: string;
    public readonly uuid: string;
    public readonly assetInfo: AssetInfo;

    //不是引擎内的通用资源，统一存储到这里
    public readonly file?: any;

    constructor() {
        super();

        //@ts-ignore
        this.id = idGen.get();
        AssetManager.getInstance().addAsset(this);
    }

    public getId(): number {
        return this.id;
    }

    public getNative(): Module.NativeBaseObj {
        return this.native;
    }

    public destroy(): void {

        // console.info("js remove asset ",this.getId());

        //@ts-ignore
        AssetManager.getInstance().destroyAssetFromAsset(this);

        //表示已经destroy了
        if (!this.uuid && isNaN(this.id)) return;

        if (this.native) {
            window.Module.destroy(this.native);
            this.native = null;
        }

        idGen.recovery(this.id);

        //@ts-ignore
        this.id = NaN;

        //@ts-ignore
        this.uuid = null;
    }

    public setKeyUrl(url: string) {
        //@ts-ignore
        this.keyUrl = url;
        /* editor:start */
        if (this.native) {
            //@ts-ignore
            this.native.setKeyUrl(url);
        }
        /* editor:end */
    }

    /**
     * 资源加载完毕调用
     * 引擎内部调用，外面不要调用
     */
    public onLoad() { }

    /**
     * 子类实现该方法
     * 游戏中有些资源存储在磁盘上是一种格式，加载之后还需要解析为游戏可使用的格式。
     * 资源先加载存储在磁盘上的数据，然后再解析为游戏中可使用的数据，所以当资源解析之后，就可以清理掉存储在磁盘上的数据，节省内存
     */
    public clear() {

    }
}

