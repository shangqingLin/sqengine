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
        const nav: Navigator = window.navigator;
        const ua = nav.userAgent.toLowerCase();
        this.isNative = false;
        this.isBrowser = true;
        this.isMobile = /mobile|android|iphone|ipad/.test(ua);
        this.platform = this.isMobile ? PlatformDefine.MOBILE_BROWSER : PlatformDefine.DESKTOP_BROWSER;

        // init os, osVersion and osMainVersion
        let isAndroid = false;
        let iOS = false;
        let osVersion = '';
        let osMajorVersion = 0;
        let uaResult = /android\s*(\d+(?:\.\d+)*)/i.exec(ua) || /android\s*(\d+(?:\.\d+)*)/i.exec(nav.platform);
        if (uaResult) {
            isAndroid = true;
            osVersion = uaResult[1] || '';
            osMajorVersion = parseInt(osVersion) || 0;
        }
        uaResult = /(iPad|iPhone|iPod).*OS ((\d+_?){2,3})/i.exec(ua);
        if (uaResult) {
            iOS = true;
            osVersion = uaResult[2] || '';
            osMajorVersion = parseInt(osVersion) || 0;
            // refer to https://github.com/cocos-creator/engine/pull/5542 , thanks for contribition from @krapnikkk
            // ipad OS 13 safari identifies itself as "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/605.1.15 (KHTML, like Gecko)"
            // so use maxTouchPoints to check whether it's desktop safari or not.
            // reference: https://stackoverflow.com/questions/58019463/how-to-detect-device-name-in-safari-on-ios-13-while-it-doesnt-show-the-correct
            // FIXME: should remove it when touch-enabled mac are available
            // TODO: due to compatibility issues, it is still determined to be ios, and a new operating system type ipados may be added later？
        } else if (/(iPhone|iPad|iPod)/.exec(nav.platform) || (nav.platform === 'MacIntel' && nav.maxTouchPoints && nav.maxTouchPoints > 1)) {
            iOS = true;
            osVersion = '';
            osMajorVersion = 0;
        }

        let osName = OS.UNKNOWN;
        if (nav.appVersion.indexOf('Win') !== -1) {
            osName = OS.WINDOWS;
        } else if (iOS) {
            osName = OS.IOS;
        } else if (nav.appVersion.indexOf('Mac') !== -1) {
            osName = OS.OSX;
        } else if (nav.appVersion.indexOf('X11') !== -1 && nav.appVersion.indexOf('Linux') === -1) {
            osName = OS.LINUX;
        } else if (isAndroid) {
            osName = OS.ANDROID;
        } else if (nav.appVersion.indexOf('Linux') !== -1 || ua.indexOf('ubuntu') !== -1) {
            osName = OS.LINUX;
        }

        this.os = osName;
        this.osVersion = osVersion;
        this.osMainVersion = osMajorVersion;

        // init browserVersion
        this.browserVersion = '';
        const versionReg1 = /(mqqbrowser|micromessenger|qqbrowser|sogou|qzone|liebao|maxthon|uc|ucbs|360 aphone|360|baiduboxapp|baidu|maxthon|mxbrowser|miui(?:.hybrid)?)(mobile)?(browser)?\/?([\d.]+)/i;
        const versionReg2 = /(qq|chrome|safari|firefox|trident|opera|opr\/|oupeng)(mobile)?(browser)?\/?([\d.]+)/i;
        let tmp = versionReg1.exec(ua);
        if (!tmp) {
            tmp = versionReg2.exec(ua);
        }
        this.browserVersion = tmp ? tmp[4] : '';
    }


    get supportWebGL2(): boolean {
        return !!window.WebGL2RenderingContext;
        // return false;
    }

    get mingame(): boolean {
        return false;
    }
}

export const systemInfo = new SystemInfo();
