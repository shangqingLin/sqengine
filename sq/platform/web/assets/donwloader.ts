import { http, HttpRequestOptions, ResponseType, ON_COMPLETE_CALL } from "../../../net/http";
import { ImageAsset, ImageSource,DownLoaderOptions} from "../../../assets/index";
import { downloaderAudioWeb } from "../audio/audio-web";

export function downloaderJson(url: string, options:DownLoaderOptions, callback?: ON_COMPLETE_CALL) {
    let htppOptions: HttpRequestOptions = Object.create(null) as HttpRequestOptions;
    htppOptions.responseType = ResponseType.JSON;
    http.get(url, htppOptions, callback);
}


function createImageSource(img: ImageSource) {
    let source = new ImageAsset(img);
    return source;
}

export function downloadImage(url: string,options:DownLoaderOptions, onComplete: ((err: Error | number | null, data?: ImageAsset | null) => void)): void {
    const img = new window.Image();

    function loadCallback(): void {
        img.removeEventListener('load', loadCallback);
        img.removeEventListener('error', errorCallback);

        let imgSource = createImageSource(img);
        onComplete(null, imgSource);
        // createImageBitmap(img, { premultiplyAlpha: 'none' }).then((result: ImageBitmap): void => {
        //     let imgSource = createImageSource(result);
        //     onComplete(null, imgSource);
        // }, (err): void => {
        //     onComplete(err, null);
        // });
    }

    function errorCallback(): void {
        img.removeEventListener('load', loadCallback);
        img.removeEventListener('error', errorCallback);
        onComplete(new Error())
    }
    img.addEventListener('load', loadCallback);
    img.addEventListener('error', errorCallback);
    img.src = url;
}

export function donwloaderArrayBuffer(url: string,options:DownLoaderOptions, callback?: ON_COMPLETE_CALL) {
    let httpOptions: HttpRequestOptions = Object.create(null) as HttpRequestOptions;
    httpOptions.responseType = ResponseType.ARRAY_BUFFER;
    http.get(url, httpOptions, callback);
}


export function downloaderText(url:string,options:DownLoaderOptions,callback?:ON_COMPLETE_CALL){
    let httpOptions:HttpRequestOptions = Object.create(null) as HttpRequestOptions;
    httpOptions.responseType = ResponseType.TEXT;
    http.get(url,httpOptions,callback);
}

export function loadAudio(url:string,options:DownLoaderOptions,callback?:ON_COMPLETE_CALL)
{
    downloaderAudioWeb(url,callback);
}

