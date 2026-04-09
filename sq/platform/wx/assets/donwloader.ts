import { DownLoaderOptions,ImageAsset } from "../../../assets/index";
import { AudioPlayer } from "../../common/audio";
import cacheManager from "./CacheManager";
import fs from "./fs"

const REGEX = /^https?:\/\/.*/;

function transformUrl(url: string) {
    let inCache = false;

    //判断当前的路径是否为本地缓存路径
    const isInUserDataPath = url.startsWith(fs.getUserDataPath());
    if (isInUserDataPath) {
        inCache = true;
    } else if (REGEX.test(url)) { //正常的http请求的路径
        const cacheUrl = cacheManager.getUserLocalCacheUrl(url);
        if (cacheUrl) {
            inCache = true;
            url = cacheUrl;
        } else {
            const tempUrl = cacheManager.getTempLocalCacheUrl(url);
            if (tempUrl) {
                inCache = true;
                url = tempUrl;
            }
        }
    } else {
        //那么这个路径应该是项目中的路径了，即资源是直接和代码包一起的资源，不是在远程服务器上
        inCache = true;
    }
    return { url, inCache };
}

/**
 * 1、在微信平台上的所有资源都先使用downloadFile下载到本地临时缓存目录
 * 然后再从临时目录中读取，这样就完成了加载资源
 * 
 * 2、
 * @param url 
 * @param func 
 * @param options 
 * @param onFileProgress 
 * @param onComplete 
 */
function download(url: string, func:(path:string,callback:(error:Error|null,data:any)=>void)=>void,onComplete:(error:Error|null,data:any)=>void) {
    const result = transformUrl(url);
    // console.info(url,result.url);
    if (result.inCache) {
        func(result.url,onComplete);
    } else {
        fs.downloadFile(url,(err:Error|null, tempFilePath:string) => {
            if (err) {
                onComplete(err, null);
                return;
            }
            func(tempFilePath,(err, data) => {
                if (!err) {
                    cacheManager.addToTempCache(url, tempFilePath);
                    cacheManager.saveToUserLocalCache(url, tempFilePath);
                }
                onComplete(err, data);
            });
        });
    }
}


function parseArrayBuffer (url:string,onComplete:(error:Error|null,data:ArrayBuffer)=>void) {
    fs.readArrayBuffer(url, onComplete);
}

function parseText (url:string,onComplete:(error:Error|null,data:string)=>void) {
    fs.readText(url, onComplete);
}

function parseJson (url:string,onComplete:(error:Error|null,data:object)=>void) {
    fs.readJson(url, onComplete);
}

export function downloaderJson(url: string, options:DownLoaderOptions,callback?: (error: Error | null, data: object) => void) {
    download(url,parseJson,callback);
}

export function donwloaderArrayBuffer(url: string,options:DownLoaderOptions,callback?: (error: Error | null, data: ArrayBuffer) => void) {
    download(url,parseArrayBuffer,callback);
}


export function downloaderText(url: string,options:DownLoaderOptions, callback: (error: Error | null, data: object) => void) {
    download(url,parseText,callback);
}


function loadImage(url: string,onComplete: ((err: Error | null, data?: ImageAsset | null) => void))
{
    const img = new window.Image();
    function loadCallback(): void {
        let source = new ImageAsset(img);
        onComplete(null,source);
    }
    function errorCallback(): void {
        onComplete(new Error())
    }
    img.onload = loadCallback;
    img.onerror = errorCallback;
    img.src = url;
}

export function downloadImage(url: string,options:DownLoaderOptions,onComplete: ((err: Error | null, data?: ImageAsset | null) => void)): void {
     download(url,loadImage,onComplete);
}

export function loadAudio(url:string,options:DownLoaderOptions,onComplete: ((err: Error | null, data?: AudioPlayer | null) => void))
{
}

