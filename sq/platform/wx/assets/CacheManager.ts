import { AssetManager } from "../../../assets/index";
import js from "../../../core/utils/js"
import fs from "./fs";

/**
 * 资源缓存管理器
 */
class CacheManager {

    /**
     * 本地用户缓存目录下的文件
     */
    private userLocalFileMaps: { [key: string]: any }

    /**
     * 用户临时缓存目录下的文件
     */
    private tempLocalFilesMap: { [key: string]: any };

    private waitCache: Array<string> = [];
    private timerOutHandler: number;
    initialize() {
        this.userLocalFileMaps = js.createMap();
        this.tempLocalFilesMap = js.createMap();
        let userCacheRootPath: string = fs.getUserDataPath();
        let saveFilePaths = fs.getFileListSync(userCacheRootPath);
        let needDelete = [];
        for (let i = 0, n = saveFilePaths.length; i < n; ++i) {
            let saveFilePath = saveFilePaths[i];
            let fileName = saveFilePath.substring(saveFilePath.lastIndexOf("/") + 1);

            //不在版本记录里面，证明资源已经删除了，那么就从本地缓存中删除，节省空间
            let isVaild = !!AssetManager.getInstance().getAssetConfig().getInfoByNativePath(fileName);
            if (isVaild) {
                this.userLocalFileMaps[AssetManager.assetRootPath + fileName] = userCacheRootPath + "/" + saveFilePath;
            } else {
                needDelete.push(saveFilePath);
            }
        }

        for (let i = 0, n = needDelete.length; i < n; ++i) {
            fs.deleteFile(needDelete[i]);
        }
    }


    /**
     * 	微信系统不支持直接从网络读取文件存储到本地系统
     *	需要先将文件从网络下载到本地临时目录后，再复制到本地目录
     */
    saveToUserLocalCache(saveFilePath: string, tempFilePath: string) {
        if (fs.userLocalFull) {
            return;
        }
        this.waitCache.push(saveFilePath);
        this.waitCache.push(tempFilePath);
        window.clearTimeout(this.timerOutHandler);
        this.timerOutHandler = window.setTimeout(this._cache.bind(this), 500);
    }

    private _cache() {
        this.timerOutHandler = 0;
        for (let i = 0, n = this.waitCache.length; i < n; i += 2) {
            let tempFilePath: string = this.waitCache.pop();
            let httpUrl: string = this.waitCache.pop();

            //直接使用资源名字作为本地存储的文件名，不会产生同名的情况的，因为编辑器已经保证
            //所有资源的名字是唯一的
            let name = httpUrl.substring(httpUrl.lastIndexOf("/"));
            let saveFilePath = fs.getUserDataPath() + "/" + name;
            fs.saveFile(tempFilePath, saveFilePath);
        }
    }


    getUserLocalCacheUrl(httpUrl: string): string {
        return this.userLocalFileMaps[httpUrl];
    }

    addToTempCache(httpUrl: string, tempUrl: string): void {
        this.tempLocalFilesMap[httpUrl] = tempUrl;;
    }

    getTempLocalCacheUrl(httpUrl: string): string {
        return this.tempLocalFilesMap[httpUrl];
    }
}

export default new CacheManager();