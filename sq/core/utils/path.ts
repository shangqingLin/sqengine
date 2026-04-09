import CommonUtils from "./CommonUtils";

var path = {
    delimiter: "/",
    join () {
        var index;
        var num = arguments.length;
        var result = arguments[0];

        for (index = 0; index < num - 1; ++index) {
            var one = arguments[index];
            var two = arguments[index + 1];
            if (!CommonUtils.isDefine(one) || !CommonUtils.isDefine(two)) {
                throw new Error("undefined argument to pc.path.join");
            }
            if (two[0] === this.delimiter) {
                result = two;
                continue;
            }

            if (one && two && one[one.length - 1] !== this.delimiter && two[0] !== this.delimiter) {
                result += (this.delimiter + two);
            } else {
                result += (two);
            }
        }

        return result;
    },


    normalize (path:string) {
        var lead = path.startsWith(this.delimiter);
        var trail = path.endsWith(this.delimiter);

        var parts = path.split('/');

        var result = '';

        var cleaned:Array<string> = [];

        for (var i = 0; i < parts.length; i++) {
            if (parts[i] === '') continue;
            if (parts[i] === '.') continue;
            if (parts[i] === '..' && cleaned.length > 0) {
                cleaned = cleaned.slice(0, cleaned.length - 2);
                continue;
            }

            if (i > 0) cleaned.push(this.delimiter);
            cleaned.push(parts[i]);
        }


        result = cleaned.join('');
        if (!lead && result[0] === this.delimiter) {
            result = result.slice(1);
        }

        if (trail && result[result.length - 1] !== this.delimiter) {
            result += this.delimiter;
        }

        return result;
    },


    split(path:string) {
        var parts = path.split(this.delimiter);
        var tail = parts.slice(parts.length - 1)[0];
        var head = parts.slice(0, parts.length - 1).join(this.delimiter);
        return [head, tail];
    },

    /**
     * 获取路径中的文件名字
     * @param path 
     * @returns 
     */
    getBasename (path:string) {
        return this.split(path)[1];
    },

    /**
     * 获取目录
     * @param path 
     * @returns 
     */
    getDirectory (path:string) {
        var parts = path.split(this.delimiter);
        return parts.slice(0, parts.length - 1).join(this.delimiter);
    },

    /**
     * 获取路径的后缀名
     * @param path 
     * @param needPoint 
     * @returns 
     */
    getExtension (path:string,needPoint:boolean = true) {
        var ext = path.split('?')[0].split('.').pop();
        if (ext !== path) {
            return needPoint ? "." + ext : ext;
        }
        return "";
    },

    /**
     * 是否是相对路径
     * @param s
     * @return {boolean}
     */
    isRelativePath (s:string) {
        return s.charAt(0) !== "/" && s.match(/:\/\//) === null;
    },

    extractPath (s:string) {
        var path = ".",
            parts = s.split("/"),
            i = 0;

        if (parts.length > 1) {
            if (this.isRelativePath(s) === false) {
                path = "";
            }
            for (i = 0; i < parts.length - 1; ++i) {
                path += "/" + parts[i];
            }
        }
        return path;
    }
};
export default path