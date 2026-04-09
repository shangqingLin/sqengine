//分析URL路径
var urlRegExt = /^(([^:\/?#]+):)?(\/\/([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?/;
export default {
    getQuery (url:string) {
        var result = url.match(urlRegExt);
        return result[7];
    },
    getPath (url:string) {
        var result = url.match(urlRegExt);
        return result[5];
    },

    getDomainName (url:string) {
        var result = url.match(urlRegExt);
        return result[4];
    },

    getScheme (url:string) {
        var result = url.match(urlRegExt);
        return result[2];
    },

    generateQuery (url:string, query:any) {
        var props = Object.keys(query);
        if (props.length > 0) {
            var q = "";
            if (this.getQuery(url)) {
                q += "&";
            } else {
                url += "?";
            }
            for (var i = 0; i < props.length; ++i) {
                q += props[i] + "=" + query[props[i]];
                if (i < props.length - 1) {
                    q += "&";
                }
            }
            url += q;
        }
        return url;
    }
}
