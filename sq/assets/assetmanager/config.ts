import { http } from "../../net/http";
import { AssetLoadType } from "./AssetManager";


interface AssetInfo {
    /**
     * 资源的相对于路径，是相对于编辑器assets目录的路径。
     * 用于游戏应用内使用，也是资源的唯一标记
     */
    gameUrl: string;

    /**
     * 也是资源的唯一标记，也可以使用这个ID来加载资源
     * 如果不是编辑器中导出的资源则这个为空
     */
    uuid?: string;

    /**
     * 真实的加载路径：
     * 1、因为发布之后和在编辑器中的路径可以不一样的（比如png变成astc、或为了热更添加了版本号更名了）
     *    这里保存发布之后的路径，资源真正加载的就是这个路径。
     * 
     * 2、这个属性可以为null，如果为空则表示这个资源是一个虚拟资源，没有对应的Native文件
     *    例如图集的Sprite就是一个虚拟资源。
     */
    loadUrl?: string;

    /**
     * 保存这个资源所关联的资源
     * 当当前这个资源加载的时候，它所关联的资源就会自动被加载
     * 这个存储的为config.json中的uuid
     */
    dependAssets?: Array<string>;

    /**
     * 加载主资源时，importFiles是否自动加载
     * 默认是自动加载的
     */
    autoLoadImport?: boolean;

    /**
     * 文件后缀
     */
    ext?: string;

    loadType?: AssetLoadType;
}

export type {
    AssetInfo
};

export class Config {
    private configInfoMap: { [key: string]: AssetInfo };
    async loadConfig(configUrl: string) {
        return new Promise<void>((resolve, reject) => {
            http.get(configUrl, (error, content) => {
                if (error) {
                    reject(error);
                } else {
                    this.configInfoMap = content;
                    let uuids = Object.keys(content);
                    for (let i = 0, n = uuids.length; i < n; ++i) {
                        let info: AssetInfo = content[uuids[i]];
                        info.uuid = uuids[i];
                        content[info.gameUrl] = info;
                        if (info.loadUrl === info.loadUrl) content[info.loadUrl] = info;
                    }
                    resolve();
                }
            });
        });
    }
    public getInfo(uuid: string): AssetInfo {
        return this.configInfoMap[uuid];
    }

    public getInfoByGameUrl(path: string): AssetInfo {
        return this.configInfoMap[path];
    }

    public getInfoByNativePath(nativePath: string): AssetInfo {
        return this.configInfoMap[nativePath];
    }
}