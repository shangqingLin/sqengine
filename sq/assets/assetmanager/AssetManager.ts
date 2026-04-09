import { js, EventDispatch, path, type } from "../../core/index";
import { http } from "../../net/http";
import Asset from "../assets/Asset";
import { downloader } from "./downloader";
import parser from "./parser/parser";
import { AssetInfo, Config } from "./config";
import { initializeAssetManager } from "../../platform/index";


export enum AssetLoadType {
    BUFFER = 1,
    TEXT,
    JSON
}

/**
 * Asset实例管理类，管理整个场景的资源
 * @constructor
 */
export class AssetManager extends EventDispatch {

    static basePath: string = "";
    static assetRootPath: string = "";
    private static _assetManagerInstance: AssetManager;

    /**
     * 记录当前所有已经加载的资源
     * @private
     */
    private urlMap: { [key: string]: Asset };
    private uuidMap: { [key: string]: Asset };
    private numIdMap: { [key: number]: Asset };

    private loadingAssetMap: { [key: string]: Array<(err: Error | null, data?: any) => void> };
    private config: Config;
    static getInstance() {
        if (!this._assetManagerInstance) {
            this._assetManagerInstance = new AssetManager();
        }
        return this._assetManagerInstance;
    }

    async initialize(basePath: string) {
        return this.loadConfig(basePath)
            .then(() => {
                return initializeAssetManager();
            });
    }

    private async loadConfig(basePath: string) {
        if (this.urlMap) {
            let urls = Object.keys(this.urlMap);
            for (let i = 0; i < urls.length; ++i) {
                //@ts-ignore
                this.urlMap[urls[i]].destroy();
            }
        }
        this.urlMap = js.createMap() as any;
        this.uuidMap = js.createMap();
        this.numIdMap = js.createMap();
        this.loadingAssetMap = js.createMap();
        this.config = new Config();

        AssetManager.basePath = basePath;
        if (basePath[basePath.length - 1] !== "/") {
            AssetManager.basePath += "/"
        }
        AssetManager.assetRootPath = AssetManager.basePath + "assets/";
        return this.config.loadConfig(basePath + "/config.json?v=" + Date.now());
    }

    addAsset(asset: Asset) {
        if (asset.keyUrl) this.urlMap[asset.keyUrl] = asset;
        if (asset.uuid) this.uuidMap[asset.uuid] = asset;
        this.numIdMap[asset.getId()] = asset;
    }

    getAssetConfig(): Config {
        return this.config;
    }

    getAssetByGameUrl(url: string): Asset {
        return this.urlMap[url];
    }

    getAssetById(id: number): Asset {
        return this.numIdMap[id];
    }

    destroyByUUID(uuid: string) {
        var content: Asset = this.uuidMap[uuid];
        if (content) {
            this.destroyAsset(content);
        }
    }

    destroyAsset(asset: Asset) {
        asset.destroy();
    }

    //在Asset类的Destroy中调用，避免循环destroy
    private destroyAssetFromAsset(asset: Asset) {
        delete this.urlMap[asset.keyUrl];
        delete this.urlMap[asset.getId()];
        delete this.uuidMap[asset.uuid];
    }

    getAssetByUUId(uuid: string): Asset {
        return this.uuidMap[uuid];
    }

    loadById(uuid: string | Array<string>, completeCallback: (error: Error | null, asset: Asset | Array<Asset>) => void, progressCallback?: Function) {
        let isArray: boolean = Array.isArray(uuid);
        if (!isArray) {
            (uuid as Array<string>) = [uuid as string];
        }

        let urls = [];
        for (let i = 0, n = uuid.length; i < n; ++i) {
            let info: AssetInfo = this.config.getInfo(uuid[i]);
            /* debug:start */
            if (!info) {
                return console.error("config中找不到" + uuid + "的资源配置数据");
            }
            /* debug:end */
            urls.push(info.gameUrl);
        }

        if (urls.length > 0) {
            this.load(urls, (error: Error | null, result: Asset | Array<Asset>) => {
                completeCallback(error, isArray ? result : (result as Array<Asset>)[0]);
            }, progressCallback);
        } else {
            completeCallback(new Error, null);
        }
    }

    load(urls: string | Array<string>, loadType: AssetLoadType, completeCallback: (error: Error | null, asset: Asset | Array<Asset>) => void, progressCallback?: Function): void;
    load(urls: string | Array<string>, completeCallback: (error: Error | null, asset: Asset | Array<Asset>) => void, progressCallback?: Function): void;
    load(...args: any[]) {

        let urls: string | Array<string> = args[0];
        let loadType: number = 0;
        let completeCallback = null;
        let progressCallback = null;
        if (typeof args[1] === "number") {
            loadType = args[1];
            completeCallback = args[2];
            progressCallback = args[3];
        } else {
            completeCallback = args[1];
            progressCallback = args[2];
        }

        var isArray = urls instanceof Array
        var count = isArray ? urls.length : 1;
        var current = 0;
        var result: Array<Asset> = new Array(count);

        var load = function (error: Error, content: Asset) {
            if (!error) {
                if (count > 1) {
                    let index = urls.indexOf(content.keyUrl);
                    result[index] = content;
                } else {
                    result[0] = content;
                }
                if (progressCallback) {
                    progressCallback(content);
                }
            }
            ++current;
            if (count === current) {
                completeCallback(error, isArray ? result : result[0]);
            }
        }

        if (Array.isArray(urls)) {
            for (var i = 0; i < count; ++i) {
                this.loadOne(urls[i], loadType, load);
            }
        } else {
            this.loadOne(urls, loadType, load);
        }
    }

    private loadOne(url: string, loadType: AssetLoadType | null, onComplete: (error: Error | null, asset: Asset) => void) {
        if (this.urlMap[url]) {
            onComplete(null, this.getAssetByGameUrl(url));
            return;
        }

        // if (url.startsWith("res/")) {
        //     console.info(url);
        // }

        var loadingAsset: Array<(err: Error | null, data?: any) => void> = this.loadingAssetMap[url];
        if (loadingAsset) {
            loadingAsset.push(onComplete);
            return;
        }
        loadingAsset = []
        loadingAsset.push(onComplete);
        this.loadingAssetMap[url] = loadingAsset;


        let assetInfo: AssetInfo = this.config.getInfoByGameUrl(url);
        if (!assetInfo) {
            //表示这个资源不是从编辑器导出的资源，或是本地资源（例如微信小游戏包中的资源）
            assetInfo = {
                gameUrl: url,
                loadUrl: url,
                loadType: loadType
            };
        }

        let importFileLoaded: boolean = true;
        let ownerLoaed: boolean = false;
        let erros: Array<Error> = [];

        let complete = (error: Error, data: Asset | null) => {
            ownerLoaed = true;
            if (error) erros.push(error);
            if (!importFileLoaded) return;

            if (!data) data = this.getAssetByGameUrl(url);
            if (data)
                data.onLoad();

            error = erros.length > 0 ? erros[0] : null;
            for (var i = 0; i < loadingAsset.length; ++i) {
                loadingAsset[i](error, data);
            }
            delete this.loadingAssetMap[url];

        }

        //加载关联的资源
        if (assetInfo.dependAssets && assetInfo.dependAssets.length > 0 && assetInfo.autoLoadImport !== false) {
            importFileLoaded = false;
            let count = assetInfo.dependAssets.length;
            let current = 0;
            let checkImportFile = (error: Error) => {
                ++current;
                if (error) erros.push(error);
                if (current === count) {
                    importFileLoaded = true;
                    if (ownerLoaed) complete(null, null);
                }
            }
            this.loadById(assetInfo.dependAssets, checkImportFile);
        }

        /**
         * 如果loadUrl为空则表示这个资源是一个虚拟资源，没有对应的Native文件; 例如图集的Sprite就是一个虚拟资源。
         * 如果是虚拟资源，一般都会去加载dependAssets资源的
         */
        if (assetInfo.loadUrl) {
            let wholeUrl: string = AssetManager.assetRootPath + assetInfo.loadUrl;
            let ext: string = path.getExtension(wholeUrl, false).toLowerCase();
            assetInfo.ext = ext;
            assetInfo.loadType = loadType;
            downloader.download(wholeUrl, assetInfo, (error: Error | null, content: any) => {
                if (error) {
                    complete(error, null);
                    return;
                }
                parser.parse(wholeUrl, assetInfo, content, (error: Error | null, asset: Asset) => {
                    if (!error) {
                        asset.setKeyUrl(url);
                        //@ts-ignore
                        asset.assetInfo = assetInfo;

                        this.addAsset(asset);

                        /* editor:start */
                        this.fire("loaded-asset", asset);
                        /* editor:end */
                    }
                    complete(error, asset);
                });
            });
        } else {
            complete(null, null);
        }
    }

    httpGet(url: string, completeCallback: (error: number, data: any) => void) {
        if (path.isRelativePath(url)) {
            url = AssetManager.basePath + url;
        }
        http.request("get", url, null, completeCallback);
    }
}

//@ts-ignore
window.AssetManager = AssetManager;