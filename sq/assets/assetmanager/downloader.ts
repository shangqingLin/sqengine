import { js } from "../../core/index"
import { downloaderText, donwloaderArrayBuffer, downloaderJson, downloadImage, loadAudio } from "../../platform/index";
import { AssetInfo } from "./config"
import { Application } from "../../framework/Application"
import { AssetLoadType, AssetManager } from "./AssetManager"


interface DownLoaderOptions {
    assetInfo: AssetInfo
}



var donwloaderRegister: {
    [key: string]: (url: string, options: DownLoaderOptions,
        complete: (error: Error | number | null, data: any) => void) => void
} = {
    "json": downloaderJson,
    "png": downloadImage,
    "jpg": downloadImage,
    "jpeg": downloadImage,
    "astc": donwloaderArrayBuffer,
    "prefab": downloaderJson,
    "scene": downloaderJson,
    "tm": downloaderJson,
    "mat": downloaderJson,
    "eff": downloaderJson,
    "text": downloaderText,
    "txt": downloaderText,
    "skel": donwloaderArrayBuffer,
    "ttf": donwloaderArrayBuffer,
    "bp": downloaderJson,
    "atlas": downloaderJson,
    '.mp3': loadAudio,
    '.ogg': loadAudio,
    '.wav': loadAudio,
    '.m4a': loadAudio,
}

class Downlaoder {

    /**
     * 当前正在加载的资源
     * @private
     */
    private loadingAssetMap: { [key: string]: Array<(err: Error | null, data?: any) => void> } = js.createMap();

    /**
     * 避免一次性过多加载资源，控制在这个变量设置的范围内
     */
    private maxLoadCount: number = 5;
    private currentLoadCount: number = 0;
    private waitForLoad: Array<any> = [];

    download(url: string, configAssetInfo: AssetInfo, onComplete: ((err: Error | null, data?: any) => void)) {
        var loadingAsset: Array<(err: Error | null, data?: any) => void> = this.loadingAssetMap[url];
        if (loadingAsset) {
            loadingAsset.push(onComplete);
            return;
        }

        if (this.currentLoadCount >= this.maxLoadCount) {
            this.waitForLoad.push(url, configAssetInfo, onComplete);
            return;
        }

        loadingAsset = [];
        loadingAsset.push(onComplete);
        this.loadingAssetMap[url] = loadingAsset;

        ++this.currentLoadCount;
        this.download2(url, configAssetInfo, (error: Error, data?: any) => {

            for (var i = 0; i < loadingAsset.length; ++i) {
                loadingAsset[i](error, data);
            }
            delete this.loadingAssetMap[url];
            --this.currentLoadCount;
            if (this.currentLoadCount < this.maxLoadCount) {
                Application.ins.timer.frameOnce(1, this, this.checkNextToLoad);
            }
        });
    }

    private download2(url: string, configAssetInfo: AssetInfo, onComplete: ((err: Error | null, data?: any) => void)) {
        let downloader = donwloaderRegister[configAssetInfo.ext];

        if (!downloader) {
            switch (configAssetInfo.loadType) {
                case AssetLoadType.BUFFER:
                    downloader = donwloaderArrayBuffer;
                    break
                case AssetLoadType.JSON:
                    downloader = downloaderJson;
                    break;
                case AssetLoadType.TEXT:
                    downloader = downloaderText;
                    break;
            }
        }
        
        if (!downloader) throw Error("找不到downloader，传入的资源类型为：" + configAssetInfo.ext);

        let maxRetryCount = 5;
        let processCount = 0;
        let errors: any;
        let done = (error: Error | number, data: any) => {
            if (error === 0 || (error && error !== 200)) {
                errors = typeof error === "number" ? new Error(error.toString()) : error;
                process();
            } else {
                onComplete(null, data);
            }
        }

        let process = () => {
            ++processCount;
            if (processCount > maxRetryCount) {
                onComplete(errors);
            } else {
                downloader(url, {
                    assetInfo: configAssetInfo
                } as DownLoaderOptions,
                    done);
            }
        }
        process();
    }

    private checkNextToLoad() {
        while (this.waitForLoad.length > 0) {
            //@ts-ignore
            this.download(this.waitForLoad.shift(), this.waitForLoad.shift(), this.waitForLoad.shift());
            if (this.currentLoadCount >= this.maxLoadCount) {
                break;
            }
        }
    }
}
const downloader = new Downlaoder();

export {
    downloader
};

export type {
    DownLoaderOptions
};