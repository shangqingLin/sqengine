/**
 *  微信小游戏文件系统操作接口封装
 */

const fs: wx.FileSystemManager = wx.getFileSystemManager();

export default {

    /**
     * 判断用户目录容量是否满了
     * 默认是200M，可以继续向微信申请到1G
     */
    userLocalFull: false,

    getUserDataPath(): string {
        //本地用户目录的根目录，我们只能在这个目录下存储本地文件
        return wx.env.USER_DATA_PATH.toString();
    },

    /**
     * 客户端直接发起一个 HTTPS GET 请求，返回文件的本地临时路径 (本地路径)，单次下载允许的最大文件为 200MB
     * 即将文件下载中本地临时目录中存。本地临时目录不限制大小，但会在小程序退出的时候被微信清理
     * @param remoteUrl 
     * @param onComplete 
     * @param onProgress 
     * @param filePath 
     * @param header 
     */
    downloadFile(remoteUrl: string, onComplete: (error: Error, saveFilePath: string) => void, onProgress?: () => void, filePath?: string, header?: { [key: string]: any }) {
        const options: wx.DownLoadFileParam = {
            url: remoteUrl,
            success(res: wx.DownLoadFileResp) {
                if (res.statusCode === 200) {
                    onComplete && onComplete(null, res.tempFilePath || res.filePath);
                } else {
                    if (res.filePath) {
                        this.deleteFile(res.filePath);
                    }
                    console.warn(`Download file failed: path: ${remoteUrl} message: ${res.statusCode}`);
                    onComplete && onComplete(new Error(res.statusCode.toString()), null);
                }
            },
            fail(res: wx.DownLoadFileResp) {
                console.warn(`Download file failed: path: ${remoteUrl} message: ${res.errMsg}`);
                onComplete && onComplete(new Error(res.errMsg), null);
            },
        };
        if (filePath) options.filePath = filePath;
        if (header) options.header = header;
        const task = wx.downloadFile(options);
        onProgress && task.onProgressUpdate(onProgress);
    },


    /**
     * 统计用户目录下文件占用总量
     */
    getLocalUserCaptiy() {

    },

    /**
     * 从本地用户缓存目录删除一个文件
     * @param filePath 
     * @param onComplete 
     */
    deleteFile(filePath: string, onComplete?: (error: Error | null) => void) {
        fs.unlink(
            {
                filePath: filePath,
                success() {
                    console.log(`Delete file success: path: ${filePath}`);
                    onComplete && onComplete(null);
                },
                fail(res) {
                    console.warn(`Delete file failed: path: ${filePath} message: ${res.errMsg}`);
                    onComplete && onComplete(new Error(res.errMsg));
                },
            } as wx.FileSystemManagerUnlinkParam
        )
    },

    /**
     * 将本地临时目录中的文件存储到本地用户目录中
     * 本地缓存文件和本地用户文件的清理时机跟代码包一样，只有在代码包被清理的时会被清理。
     * @param srcPath 临时目录文件的路径
     * @param destPath 你需要存储在本地用户目录中的路径
     * @param onComplete 
     */
    saveFile(srcPath: string, destPath: string, onComplete?: (error: Error | null) => void) {
        fs.saveFile({
            tempFilePath: srcPath,
            filePath: destPath,
            success(res) {
                onComplete && onComplete(null);
            },
            fail(res) {
                this._checkLocalUserCapacity(res);
                console.warn(`Save file failed: path: ${destPath} message: ${res.errMsg}`);
                onComplete && onComplete(new Error(res.errMsg));
            },
        } as wx.FileSystemManagerSaveFileParam);
    },

    /**
     * 将本地临时目录或本地用户目录中的文件写入到本地用户目录中
     * @param srcPath 
     * @param destPath 
     * @param onComplete 
     */
    copyFile(srcPath: string, destPath: string, onComplete?: (error: Error | null) => void) {
        fs.copyFile({
            srcPath,
            destPath,
            success() {
                onComplete && onComplete(null);
            },
            fail(res) {
                this._checkLocalUserCapacity(res);
                console.warn(`Copy file failed: path: ${srcPath} message: ${res.errMsg}`);
                onComplete && onComplete(new Error(res.errMsg));
            },
        } as wx.FileSystemManagerCopyFileParam);
    },

    /**
     * 可以将本地临时目录或本地用户目录中的文件写入到本地用户目录中
     * @param path 
     * @param data 
     * @param onComplete 
     * @param encoding 
     */
    writeFile(path: string, data: string | ArrayBuffer, onComplete?: (error: Error | null) => void, encoding?: string) {
        fs.writeFile({
            filePath: path,
            encoding,
            data,
            success() {
                onComplete && onComplete(null);
            },
            fail(res) {
                this._checkLocalUserCapacity(res);
                console.warn(`Write file failed: path: ${path} message: ${res.errMsg}`);
                onComplete && onComplete(new Error(res.errMsg));
            },
        } as wx.FileSystemManagerWriteFileParam);
    },

    writeFileSync(path: string, data: string | ArrayBuffer, encoding?: string) {
        try {
            fs.writeFileSync(path, data, encoding);
            return null;
        } catch (e) {
            this._checkLocalUserCapacity(e);
            console.warn(`Write file failed: path: ${path} message: ${e.errMsg}`);
            return new Error(e.errMsg);
        }
    },

    _checkLocalUserCapacity(errorRes: any) {
        if (errorRes.errno === 1300202) {
            this.userLocalFull = true;
        }
    },

    /**
     * 从本地临时目录或本地用户目录中读取一个文件的内容
     * 是临时还是用户的，取决于你传递进来的路径
     * @param filePath 
     * @param onComplete 
     * @param encoding 
     */
    readFile(filePath: string, onComplete?: (error: Error | null, data: any) => void, encoding?: string) {

        let cfg = {
            filePath,
            success(res) {
                onComplete && onComplete(null, res.data);
            },
            fail(res) {
                console.warn(`Read file failed: path: ${filePath} message: ${res.errMsg}`);
                onComplete && onComplete(new Error(res.errMsg), null);
            },
        } as wx.FileSystemManagerReadFileParam

        if (encoding !== "binary") {
            //看不懂微信什么乱七八糟的操作，如果想返回二进制内容就不能指定encoding，返回其他的数据格式就必须指定
            cfg.encoding = encoding;
        }

        fs.readFile(cfg);
    },

    /**
     * 获取当前目录下所有的文件路径
     * @param filePath 
     * @param onComplete 
     */
    readDir(filePath: string, onComplete?: (error: Error | null, filePathList: Array<string>) => void) {
        fs.readdir({
            dirPath: filePath,
            success(res) {
                onComplete && onComplete(null, res.files);
            },
            fail(res) {
                console.warn(`Read directory failed: path: ${filePath} message: ${res.errMsg}`);
                onComplete && onComplete(new Error(res.errMsg), null);
            },
        } as wx.FileSystemManagerReaddir);
    },

    readdirSync(dirPath: string): Array<string> {
        return fs.readdirSync(dirPath);
    },

    /**
     * 获取dirPath目录下所有文件的路径，不包括目录路径
     * @param dirPath 
     * @param recursive 当为true是，则获取这个目录下包括子孙目录下所有的文件的相对路径
     * @returns 
     */
    getFileListSync(dirPath: string, recursive: boolean = true): Array<string> {
        if (!this.existsSync(dirPath)) {
            return [];
        }

        let statsInfo: Array<wx.FileStats> = fs.statSync(dirPath, recursive);
        let result: Array<string> = [];
        for (let i = 0, n = statsInfo.length; i < n; ++i) {
            let stats: wx.Stats = statsInfo[i].stats;
            if (stats.isFile()) {
                result.push(statsInfo[i].path);
            }
        }
        return result;
    },

    readText(filePath: string, onComplete?: (error: Error | null, content: string) => void) {
        this.readFile(filePath, onComplete, 'utf8');
    },

    readArrayBuffer(filePath: string, onComplete?: (error: Error | null, data: ArrayBuffer) => void) {
        this.readFile(filePath, onComplete, 'binary');
    },

    readJson(filePath: string, onComplete?: (error: Error | null, data: { [key: string]: any }) => void) {
        this.readFile(filePath, (err: Error, text: any) => {
            let out = null;
            if (!err) {
                try {
                    out = JSON.parse(text);
                } catch (e) {
                    console.warn(`Read json failed: path: ${filePath} message: ${e.message}`);
                    err = new Error(e.message);
                }
            }
            onComplete && onComplete(err, out);
        }, 'utf8');
    },

    readJsonSync(path: string) {
        try {
            const str = fs.readFileSync(path, 'utf8');
            return JSON.parse(str);
        } catch (e) {
            console.warn(`Read json failed: path: ${path} message: ${e.message}`);
            return new Error(e.message);
        }
    },

    makeDirSync(path: string, recursive: boolean = true) {
        try {
            fs.mkdirSync(path, recursive);
            return null;
        } catch (e) {
            console.warn(`Make directory failed: path: ${path} message: ${e.message}`);
            return new Error(e.message);
        }
    },

    rmdir(dirPath: string, onComplete?: (error: Error | null) => void, recursive: boolean = true) {
        fs.rmdir(
            {
                dirPath: dirPath,
                recursive: recursive,
                success(res) {
                    onComplete && onComplete(null);
                },
                fail(res) {
                    console.warn(`rmdir failed: path: ${dirPath} message: ${res.errMsg}`);
                    onComplete && onComplete(new Error(res.errMsg));
                }
            } as wx.FileSystemManagerRmdirParam
        )
    },

    rmdirSync(dirPath: string, recursive: boolean = true) {
        try {
            fs.rmdirSync(dirPath, recursive);
        } catch (e) {
            console.warn(`rm directory failed: path: ${dirPath} message: ${e.message}`);
            return new Error(e.message);
        }
    },

    /**
     * 判断文件是否存在
     * @param filePath 
     * @param onComplete 
     */
    exists(filePath: string, onComplete?: (b: boolean) => void) {
        fs.access({
            path: filePath,
            success() {
                onComplete && onComplete(true);
            },
            fail() {
                onComplete && onComplete(false);
            },
        } as wx.FileSystemManagerAccessParam);
    },

    existsSync(path: string): boolean {
        try {
            fs.accessSync(path);
        } catch (e) {
            return false;
        }
        return true;
    },


    loadSubpackage(name: string, onProgress?: () => void, onComplete?: (error: Error | null) => void) {
        const task = wx.loadSubpackage({
            name,
            success() {
                onComplete && onComplete(null);
            },
            fail(res) {
                console.warn(`Load Subpackage failed: path: ${name} message: ${res.errMsg}`);
                onComplete && onComplete(new Error(`Failed to load subpackage ${name}: ${res.errMsg}`));
            },
        });
        onProgress && task.onProgressUpdate(onProgress);
        return task;
    },

    unzip(zipFilePath: string, targetPath: string, onComplete?: (error: Error | null) => void) {
        fs.unzip({
            zipFilePath,
            targetPath,
            success() {
                onComplete && onComplete(null);
            },
            fail(res) {
                console.warn(`unzip failed: path: ${zipFilePath} message: ${res.errMsg}`);
                onComplete && onComplete(new Error(`unzip failed: ${res.errMsg}`));
            },
        } as wx.FileSystemManagerUnzipParam);
    },
}
