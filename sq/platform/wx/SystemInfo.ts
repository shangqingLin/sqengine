import { ISystemInfo, OS, PlatformDefine } from "../common/index";
class SystemInfo implements ISystemInfo {
    readonly isNative: boolean;
    readonly isBrowser: boolean;
    readonly isMobile: boolean;
    readonly platform: PlatformDefine;
    readonly os: OS;
    readonly osVersion: string;
    readonly osMainVersion: number;
    readonly browserVersion: string;

    constructor() {
        this.isNative = false;
        this.isBrowser = false;

        let deviceInfo = wx.getDeviceInfo();
        console.info("deviceInfo", deviceInfo);

        {
            const minigamePlatform = deviceInfo.platform.toLocaleLowerCase();
            if (minigamePlatform === 'android') {
                this.os = OS.ANDROID;
            } else if (minigamePlatform === 'ios') {
                this.os = OS.IOS;
            } else if (minigamePlatform === 'windows') {
                this.os = OS.WINDOWS;
            } else {
                this.os = OS.UNKNOWN;
            }
        }

        {
            let minigameSystem = deviceInfo.system.toLowerCase();
            // Adaptation to Android P
            if (minigameSystem === 'android p') {
                minigameSystem = 'android p 9.0';
            }
            const version = /[\d.]+/.exec(minigameSystem);
            this.osVersion = version ? version[0] : minigameSystem;
            this.osMainVersion = parseInt(this.osVersion);
        }

        this.platform = PlatformDefine.WECHAT_GAME;
        this.isMobile = this.os !== OS.WINDOWS;
    }

    private enableWebGL2OnIOS(): boolean {
        /**
        * 高性能+模式 全IOS平台支持WebGL2.0
        */
        //@ts-ignore
        if (GameGlobal.isIOSHighPerformanceModePlus) {
            return true;
        }

        if (window.WebGL2RenderingContext || window.canvas.getContext("webgl2")) {
            /**
             * iOS 15开始支持了WebGL2，但iOS15低版本的WebGL2（如iOS15.0和iOS15.1）是基本不可用的，有可能出现渲染黑屏、花屏等问题，
             *  所以需要判断当前版本 >= iOS 15.5 开启WebGL2。
             */
            // return this.osMainVersion >= 15.5;
            return true;
        };
        return false;
    }

    get supportWebGL2(): boolean {
        if (this.os === OS.IOS) {
            return this.enableWebGL2OnIOS();
        } else {
            return !!window.canvas.getContext("webgl2");
        }
    }

    get mingame(): boolean {
        return true;
    }
}

export const systemInfo = new SystemInfo();
