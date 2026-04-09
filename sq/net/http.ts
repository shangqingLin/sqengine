import {url as urlUtils,path} from "../core/index"


type ON_COMPLETE_CALL = (status:number,data:any)=>void;

enum ContentType {
    FORM_URLENCODED = "application/x-www-form-urlencoded",
    GIF = "image/gif",
    JPEG =  "image/jpeg",
    DDS =  "image/dds",
    JSON = "application/json",
    PNG = "image/png",
    TEXT = "text/plain",
    XML = "application/xml",
    WAV = "audio/x-wav",
    OGG = "audio/ogg",
    MP3 = "audio/mpeg",
    MP4 = "audio/mp4",
    AAC = "audio/aac",
    BIN = "application/octet-stream"
}

enum ResponseType {
    TEXT =  'text',
    ARRAY_BUFFER = 'arraybuffer',
    BLOB =  'blob',
    DOCUMENT = 'document',
    JSON = "json"
}


interface HttpRequestOptions {
    async?:boolean,
    postdata?:any,
    headers?:any,
    callback?:ON_COMPLETE_CALL,
    query?:any
    cache?:boolean,
    withCredentials?:boolean,
    responseType?:ResponseType
}


class Http {
    post (url:string, data:any, options:HttpRequestOptions, callback:ON_COMPLETE_CALL) {
        if (typeof options === "function") {
            callback = options;
            options = Object.create(null) as HttpRequestOptions;
        }
        options.postdata = data;
        return this.request("POST", url, options, callback);
    }

    public put(url:string, data:any, options:HttpRequestOptions | ON_COMPLETE_CALL,callback:ON_COMPLETE_CALL):XMLHttpRequest {
        if (typeof options === "function") {
            callback = options;
            options = Object.create(null) as HttpRequestOptions;
        }
        options.postdata = data;
        return this.request("PUT", url, options, callback);
    }

    get(url:string, options:HttpRequestOptions | ON_COMPLETE_CALL | ON_COMPLETE_CALL, callback?:ON_COMPLETE_CALL):XMLHttpRequest {
        if (typeof options === "function") {
            callback = options;
            options = Object.create(null) as HttpRequestOptions;
        }
        return this.request("GET", url, options, callback);
    }
    del (url:string, options:any, callback:ON_COMPLETE_CALL) {
        if (typeof options === "function") {
            callback = options;
            options = {};
        }
        return this.request("DELETE", url, options, callback);
    }

    request (method:string, url:string, options:HttpRequestOptions, callback:ON_COMPLETE_CALL) : XMLHttpRequest {
        var query, postdata, xhr:XMLHttpRequest;
        var errored = false;

        options = options || Object.create(null);

        if (typeof options === "function") {
            callback = options;
            options = Object.create(null) as HttpRequestOptions;
        }

        // store callback
        options.callback = callback;

        // setup defaults
        if (options.async == null) {
            options.async = true;
        }
        if (options.headers == null) {
            options.headers = {};
        }

        if (options.postdata != null) {
            if (options.postdata instanceof Document) {
                // It's an XML document, so we can send it directly.
                // XMLHttpRequest will set the content type correctly.
                postdata = options.postdata;
            } else if (options.postdata instanceof FormData) {
                postdata = options.postdata;
            } else if (options.postdata instanceof Object) {
                // Now to work out how to encode the post data based on the headers
                var contentType = options.headers["Content-Type"];

                // If there is no type then default to form-encoded
                if (contentType === undefined) {
                    options.headers["Content-Type"] = ContentType.FORM_URLENCODED;
                    contentType = options.headers["Content-Type"];
                }
                switch (contentType) {
                    case ContentType.FORM_URLENCODED:
                        // Normal URL encoded form data
                        postdata = "";
                        var bFirstItem = true;

                        // Loop round each entry in the map and encode them into the post data
                        for (var key in options.postdata) {
                            if (options.postdata.hasOwnProperty(key)) {
                                if (bFirstItem) {
                                    bFirstItem = false;
                                } else {
                                    postdata += "&";
                                }
                                postdata += escape(key) + "=" + escape(options.postdata[key]);
                            }
                        }
                        break;
                    default:
                    case ContentType.JSON:
                        if (contentType == null) {
                            options.headers["Content-Type"] = ContentType.JSON;
                        }
                        postdata = JSON.stringify(options.postdata);
                        break;
                }
            } else {
                postdata = options.postdata;
            }
        }

        if (!xhr) {
            xhr = new window.XMLHttpRequest();
        }


        query = options.query || {};
        if (options.cache === false) {
            query.ts = Date.now();
        }

        url = urlUtils.generateQuery(url, query);

        xhr.open(method, url, options.async);
        xhr.withCredentials = options.withCredentials !== undefined ? options.withCredentials : false;
        xhr.responseType = options.responseType || this._guessResponseType(url);

        // Set the http headers
        var headerProps = Object.keys(options.headers);
        for (var i = 0; i < headerProps.length; ++i) {
            var headerProp = headerProps[i];
            var headerValue = options.headers[headerProp];
            xhr.setRequestHeader(headerProp, headerValue);
        }


        xhr.onreadystatechange = ()=> {
            this._onReadyStateChange(method, url, options, xhr);
        };

        xhr.onerror = ()=> {
            this._onError(method, url, options, xhr);
            errored = true;
        };

        try {
            xhr.send(postdata);
        } catch (e) {
            // DWE: Don't callback on exceptions as behaviour is inconsistent, e.g. cross-domain request errors don't throw an exception.
            // Error callback should be called by xhr.onerror() callback instead.
            if (!errored) {
                options.callback(xhr.status, null);
            }
        }

        // Return the request object as it can be handy for blocking calls
        return xhr;
    }

    private _guessResponseType (url:string) {
        var ext = path.getExtension(urlUtils.getPath(url));
        if (binaryExtensions.indexOf(ext) >= 0) {
            return ResponseType.ARRAY_BUFFER;
        }
        if (ext === ".xml") {
            return ResponseType.DOCUMENT;
        }
        return ResponseType.TEXT;
    }

    private _isBinaryContentType (contentType:ContentType) {
        var binTypes = [ContentType.MP4, ContentType.WAV, ContentType.OGG, ContentType.MP3, ContentType.BIN, ContentType.DDS];
        if (binTypes.indexOf(contentType) >= 0) {
            return true;
        }

        return false;
    }

    private _onReadyStateChange (method:string, url:string, options:HttpRequestOptions, xhr:XMLHttpRequest) {
        if (xhr.readyState === 4) {
            switch (xhr.status) {
                case 0: {
                    // If this is a local resource then continue (IOS) otherwise the request
                    // didn't complete, possibly an exception or attempt to do cross-domain request
                    if (url[0] != '/') {
                        this._onError(method, url, options, xhr);
                    }
                    break;
                }
                case 200:
                case 201:
                case 206:
                case 304: {
                    this._onSuccess(method, url, options, xhr);
                    break;
                }
                default: {
                    this._onError(method, url, options, xhr);
                    break;
                }
            }
        }
    }

    private _onSuccess (method:string, url:string, options:HttpRequestOptions, xhr:XMLHttpRequest) {
        var response;
        var header;
        var contentType:ContentType;
        var parts;
        header = xhr.getResponseHeader("Content-Type");
        if (header) {
            // Split up header into content type and parameter
            parts = header.split(";");
            contentType = parts[0].trim() as ContentType;
        }

        if(xhr.responseType === ResponseType.ARRAY_BUFFER || xhr.responseType === ResponseType.JSON){
            response = xhr.response;
        }else if(xhr.responseType === ResponseType.DOCUMENT || contentType === ContentType.XML){
            response = xhr.responseXML;
        }else if (contentType === ContentType.JSON || url.split('?')[0].endsWith(".json")) {
            response = JSON.parse(xhr.responseText);
        } else if (this._isBinaryContentType(contentType)) {
            response = xhr.response;
        } else {
            response = xhr.responseText;
        }
        options.callback(null, response);
    }

    private _onError (method:string, url:string, options:HttpRequestOptions, xhr:XMLHttpRequest) {
        options.callback(xhr.status, null);
    }
}



var binaryExtensions = [
    '.model',
    '.wav',
    '.ogg',
    '.mp3',
    '.mp4',
    '.m4a',
    '.aac',
    '.dds',
    '.wasm'
];


var http = new Http();

export {
    Http,
    http,
    HttpRequestOptions,
    ResponseType,
    ON_COMPLETE_CALL
};


