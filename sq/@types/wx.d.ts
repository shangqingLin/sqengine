
/**
 * 微信小游戏接口定义
 */


declare module wx {


    interface DeviceInfo {

        //应用（微信APP）二进制接口类型（仅 Android 支持）	
        abi: string;

        //设备二进制接口类型（仅 Android 支持）	2.25.1
        deviceAbi: string;

        //	设备性能等级（仅 Android 支持）。取值为：-2 或 0（该设备无法运行小游戏），-1（性能未知），>=1（设备性能值，该值越高，设备性能越好，目前最高不到50）
        //  注意：从基础库3.4.5开始，本返回值停止维护，请使用wx.getDeviceBenchmarkInfo获取设备性能等级	
        benchmarkLevel: number;

        //设备品牌
        brand: string;

        //设备型号。新机型刚推出一段时间会显示unknown，微信会尽快进行适配。	
        model: string;

        //操作系统及版本
        system: string;

        /**
         * 客户端平台
         *  ios：iOS微信（包含 iPhone、iPad）
            android:Android微信
            ohos:HarmonyOS 手机端微信
            ohos_pc:HarmonyOS PC微信
            windows:Windows微信
            mac:macOS微信
            devtools:微信开发者工具
         */
        platform: string;

        //设备 CPU 型号（仅 Android 支持）（Tips: GPU 型号可通过 WebGLRenderingContext.getExtension('WEBGL_debug_renderer_info') 来获取）	2.29.0
        cpuType: string;

        //设备内存大小，单位为 MB	2.30.0
        memorySize: string;

    }

    function getDeviceInfo(): DeviceInfo;


    interface BenchmarkInfoSuccuss {
        benchmarkLevel: number;
        modelLevel: number;
    }

    interface BenchmarkInfoParam {
        success: (data: BenchmarkInfoSuccuss) => void;
        fail: () => void;
        complete: () => void;
    }

    function getDeviceBenchmarkInfo(confg: BenchmarkInfoParam): void;

    function setPreferredFramesPerSecond(fps:number): void;
    
    //触控事件接口
    function onTouchStart(listener: Function): void;
    function onTouchMove(listener: Function): void;
    function onTouchEnd(listener: Function): void;
    function onTouchCancel(listener: Function): void;

    function onKeyUp(listener: Function): void;
    function onKeyDown(listener: Function): void;
    function offKeyUp(listener: Function): void;
    function offKeyDown(listener: Function): void;


    class LoadSubpackageTask {
        onProgressUpdate(listener: Function): void;
    }

    /**
     * 加载分包
     * @param param 
     */
    function loadSubpackage(param: {
        name: string; //分包的名字
        success?: (res: DownLoadFileResp) => void;
        fail?: (res: DownLoadFileResp) => void;
        complete?: (res: DownLoadFileResp) => void;
    }): LoadSubpackageTask;

    enum env {
        //文件系统中的用户目录路径 (本地路径)
        USER_DATA_PATH
    }

    interface DownLoadFileResp {
        /**
         * 临时文件路径 (本地路径)。没传入 filePath 指定文件存储路径时会返回，下载后的文件会存储到一个临时文件
         */
        tempFilePath: string;

        /**
         * 用户文件路径 (本地路径)。传入 filePath 时会返回，跟传入的 filePath 一致
         */
        filePath?: string;

        /**
         * 开发者服务器返回的 HTTP 状态码	
         */
        statusCode: number;

        /**
         * 网络请求过程中一些调试信息
         * https://developers.weixin.qq.com/miniprogram/dev/framework/performance/network.html
         */
        profile: Object;

        errMsg?: string;
    }

    interface DownLoadFileParam {
        url: string;

        /**
         * 指定下载之后存储到本地目录。
         * 不指定则默认有WX决定存储在哪里
         */
        filePath?: string;

        /**
         * HTTP 请求的 Header，Header 中不能设置 Referer
         */
        header?: { [key: string]: any };

        /**
         * 超时时间，单位为毫秒，默认值为 60000 即一分钟。	
         * 默认为60000毫秒
         */
        timeout?: number;

        /**
         * 是否使用高性能模式
         * 目前只有Android支持
         */
        useHighPerformanceMode?: boolean;

        /**
         * 是否开启 profile，默认开启。开启后可在接口回调的 res.profile 中查看性能调试信息。
         */
        enableProfile?: boolean;

        /**
         * 是否开启 http2
         * 默认为false
         */
        enableHttp2?: boolean;

        /**
         * 是否开启 Quic 协议（gQUIC Q43）
         * 默认为false
         */
        enableQuic?: boolean;

        success?: (res: DownLoadFileResp) => void;
        fail?: (res: DownLoadFileResp) => void;
        complete?: (res: DownLoadFileResp) => void;
    }

    class DownloadTask {
        abort(): void;
        onProgressUpdate(listener: Function): void;
        offProgressUpdate(listener: Function): void;
        onHeadersReceived(listener: Function): void;
        offHeadersReceived(listener: Function): void;
    }

    /**
     * 将远程的文件下载到本地临时目录
     */
    function downloadFile(param: DownLoadFileParam): DownloadTask;

    interface FileSystemManagerAccessParam {

        path: string;

        //调用成功的回调
        success?: (res: any) => void;

        //调用失败的回调
        fail?: (res: any) => void;

        //不管成功或失败都回调
        complete?: (res: any) => void;
    }

    interface CommonCallBack {
        //调用成功的回调
        success?: (res: any) => void;

        //调用失败的回调
        fail?: (res: any) => void;

        //不管成功或失败都回调
        complete?: (res: any) => void;
    }

    interface FileSystemManagerAppendFileParam extends CommonCallBack {
        filePath: string;
        data: string | ArrayBuffer;
        encoding?: string;
    }


    interface FileSystemManagerSaveFileParam extends CommonCallBack {

        //临时缓存区中文件的临时路径。源路径
        tempFilePath: string;

        //存储到本地用户缓存哪个路径上。目标路径
        filePath: string;
    }

    interface FileSystemManagerGetSavedFileListParam extends CommonCallBack {
    }

    interface FileSystemManagerRemoveSavedFileParam extends CommonCallBack {
        //本地用户缓存中的文件路径
        filePath: string;
    }

    interface FileSystemManagerCopyFileParam extends CommonCallBack {
        srcPath: string;
        destPath: string;
    }

    interface FileSystemManagerFstatParam extends CommonCallBack {
        /**
         * 文件描述符
         * fd 通过 FileSystemManager.open 或 FileSystemManager.openSync 接口获得
         */
        fd: string;
    }

    interface FileSystemManagerGetFileInfoParam extends CommonCallBack {
        filePath: string;
        digestAlgorithm?: string;
    }

    interface FileSystemManagerMkdirParam extends CommonCallBack {
        dirPath: string;
        recursive?: boolean;
    }

    interface FileSystemManagerOpenParam extends CommonCallBack {
        filePath: string;
        flag?: string;
    }

    interface FileSystemManagerReadParam extends CommonCallBack {
        /**
         * 文件描述符
         * 通过open获得
         */
        fd: string;

        arrayBuffer: ArrayBuffer;

        offset?: number;
        length?: number;
        position?: number;
    }

    interface FileSystemManagerReadCompressedFileParam extends CommonCallBack {
        filePath: string;
        compressionAlgorithm: string;
    }

    interface FileSystemManagerReadFileParam extends CommonCallBack {
        filePath: string;
        encoding?: string;
    }

    interface FileSystemManagerReadZipEntryParam extends CommonCallBack {
        filePath: string;
        encoding?: string;
    }

    interface FileSystemManagerReaddir extends CommonCallBack {
        dirPath: string;
    }

    interface FileSystemManagerRenameParam extends CommonCallBack {
        oldPath: string;
        newPath: string;
    }

    interface FileSystemManagerRmdirParam extends CommonCallBack {
        dirPath: string;
        recursive?: boolean;
    }

    interface FileSystemManagerStatParam extends CommonCallBack {
        path: string;
        recursive?: boolean
    }

    interface FileSystemManagerTruncateParam extends CommonCallBack {
        filePath: string;
        length?: number;
    }

    interface FileSystemManagerUnlinkParam extends CommonCallBack {
        filePath: string;
    }

    interface FileSystemManagerUnzipParam extends CommonCallBack {
        zipFilePath: string;
        targetPath: string;
    }

    interface FileSystemManagerWriteParam extends CommonCallBack {
        fd: string;
        data: string | ArrayBuffer;
        offset?: number;
        length?: number;
        encoding?: string;
        position?: number;
    }

    interface FileSystemManagerWriteFileParam extends CommonCallBack {
        filePath: string;
        data: string | ArrayBuffer;
        encoding?: string;

    }

    interface Stats {
        /**
         * 文件的类型和存取的权限，对应 POSIX stat.st_mode
         */
        mode: number;

        /**
         * 文件大小，单位：B，对应 POSIX stat.st_size
         */
        size: number;

        /**
         * 文件最近一次被存取或被执行的时间，UNIX 时间戳，对应 POSIX stat.st_atime
         */
        lastAccessedTime: number;

        /**
         *  文件最后一次被修改的时间，UNIX 时间戳，对应 POSIX stat.st_mtime
         */
        lastModifiedTime: number;

        /**
         *   判断当前文件是否一个目录
         */
        isDirectory(): boolean;

        /**
         *判断当前文件是否一个普通文件
         */
        isFile(): boolean;
    }

    interface FileStats {
        path: string;
        stats: Stats;
    }

    class FileSystemManager {

        /**
         * 判断文件/目录是否存在
         */
        access(param: FileSystemManagerAccessParam): void;
        accessSync(path: string): boolean;
        /**
         * 在文件结尾追加内容
         */
        appendFile(param: FileSystemManagerAppendFileParam): void;

        /**
         * 将临时缓存中的文件存储到本地用户缓存中
         * @param param 
         */
        saveFile(param: FileSystemManagerSaveFileParam): void

        /**
         * 获取该小程序下本地用户缓存中已经保存的文件列表
         * @param param 
         */
        getSavedFileList(param: FileSystemManagerGetSavedFileListParam): void

        /**
         * 删除该小程序下本地用户缓存中已经保存的指定的文件
         * @param param 
         */
        removeSavedFile(param: FileSystemManagerRemoveSavedFileParam): void;

        /**
         * 
         * @param param 
         */
        copyFile(param: FileSystemManagerCopyFileParam): void;

        /**
         * 获取存储在用户本地缓存中的文件状态
         * @param param 
         */
        fstat(param: FileSystemManagerFstatParam): void;
        fstatSync(param: FileSystemManagerFstatParam): void;

        statSync(string: string, recursive: boolean): Array<FileStats>;

        /**
         * 获取该小程序下的 本地临时文件 或 本地缓存文件 信息
         * @param param 
         */
        getFileInfo(param: FileSystemManagerGetFileInfoParam): void


        open(param: FileSystemManagerOpenParam): void
        openSync(param: FileSystemManagerOpenParam): any;

        read(param: FileSystemManagerReadParam): void
        readSync(param: FileSystemManagerReadParam): any;

        readCompressedFile(param: FileSystemManagerReadCompressedFileParam): void
        readCompressedFileSync(param: FileSystemManagerReadCompressedFileParam): void;

        readFile(param: FileSystemManagerReadFileParam): void;
        readFileSync(filePath: string, encoding: string): any;

        writeFile(param: FileSystemManagerWriteFileParam): void;
        writeFileSync(filePath: string, data: string | ArrayBuffer, encoding: string): void;

        /**
         * 读取压缩包内的文件
         * @param param 
         */
        readZipEntry(param: FileSystemManagerReadZipEntryParam): void;

        /**
         * 读取目录内文件列表
         */
        readdir(param: FileSystemManagerReaddir): void;
        readdirSync(dirPath: string): Array<string>;

        /**
         * 重命名文件，并且可以剪贴文件，将newPath指定不同的目录就行
         * @param param 
         */
        rename(param: FileSystemManagerRenameParam): void;

        rmdir(param: FileSystemManagerRmdirParam): void;
        rmdirSync(dirPath: string, recursive: boolean): void;

        mkdir(param: FileSystemManagerMkdirParam): void
        mkdirSync(path: string, recursive: boolean): void;

        stat(param: FileSystemManagerStatParam): void;
        truncate(param: FileSystemManagerTruncateParam): void
        truncateSync(param: FileSystemManagerTruncateParam): any;

        /**
         * 删除文件
         * @param param 
         */
        unlink(param: FileSystemManagerUnlinkParam): void;

        unzip(param: FileSystemManagerUnzipParam): void;

        write(param: FileSystemManagerWriteParam): void;
        writeSync(param: FileSystemManagerWriteParam): any;


    }

    //文件系统
    function getFileSystemManager(): FileSystemManager;


    interface WindowInfo {
        pixelRatio: number;//	设备像素比
        screenWidth: number;//	屏幕宽度，单位px
        screenHeight: number;//	屏幕高度，单位px
        windowWidth: number;//可使用窗口宽度，单位px
        windowHeight: number;//	可使用窗口高度，单位px
        statusBarHeight: number;//	状态栏的高度，单位px
        safeArea: object;//	在竖屏正方向下的安全区域。部分机型没有安全区域概念，也不会返回 safeArea 字段，开发者需自行兼容。
        screenTop: number;//	窗口上边缘的y值
    }

    /**
     * 获取窗口信息
     */
    function getWindowInfo(): WindowInfo;

    interface DeviceInfo {
        abi: string;//	应用（微信APP）二进制接口类型（仅 Android 支持）	
        deviceAbi: string;//	设备二进制接口类型（仅 Android 支持）	2.25.1
        benchmarkLevel: number;//	设备性能等级（仅 Android 支持）。取值为：-2 或 0（该设备无法运行小游戏），-1（性能未知），>=1（设备性能值，该值越高，设备性能越好，目前最高不到50）
        brand: string;//	设备品牌	
        model: string;//	设备型号。新机型刚推出一段时间会显示unknown，微信会尽快进行适配。	
        system: string;//	操作系统及版本	
        platform: string;//	客户端平台	
        cpuType: string;//设备 CPU 型号（仅 Android 支持）（Tips: GPU 型号可通过 WebGLRenderingContext.getExtension('WEBGL_debug_renderer_info') 来获取）	2.29.0
        memorySize: string;//	设备内存大小，单位为 MB	2.30.0
    }

    /**
     * 获取设备信息
     */
    function getDeviceInfo(): DeviceInfo;

    interface AppBaseInfo {
        /**
         * 客户端基础库版本	
         */
        SDKVersion: string;

        /**
         * 是否已打开调试。可通过右上角菜单或 wx.setEnableDebug 打开调试。
         */
        enableDebug: boolean;

        host: {
            appId: string
        };

        /**
         * 微信设置的语言
         */
        language: string;

        /**
         * 微信版本号	
         */
        version: string;

        /**
         * 微信字体大小缩放比例	
         */
        fontSizeScaleFactor: number;

        /**
         * 微信字体大小，单位px
         */
        fontSizeSetting: number;
    }

    function getAppBaseInfo(): AppBaseInfo;


    interface Touch {
        //Touch 对象的唯一标识符，只读属性。一次触摸动作(我们值的是手指的触摸)在平面上移动的整个过程中, 该标识符不变。可以根据它来判断跟踪的是否是同一次触摸过程。
        identifier: number;

        //触点相对于页面左边沿的 X 坐标。
        pageX: number

        //触点相对于页面上边沿的 Y 坐标。
        pageY: number

        //触点相对于可见视区左边沿的 X 坐标。
        clientX: number

        //触点相对于可见视区上边沿的 Y 坐标。
        clientY: number

        //手指挤压触摸平面的压力大小, 从0.0(没有压力)到1.0(最大压力)的浮点数（仅在支持 force touch 的设备返回）
        force: number
    }

    interface TouchEvent {
        /**
         * 新出行的触控点
         */
        touches: Array<wx.Touch>;

        /**
         * 发生修改的触控点
         */
        changedTouches: Array<wx.Touch>;

        timeStamp: number;
    }
}