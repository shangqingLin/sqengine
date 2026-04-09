import OS from "./OS";
import Platform from "./platform-define";

export interface ISystemInfo {
    readonly isNative: boolean;

    /**
     * 是否在浏览器中
     * 移动平台和PC平台都可以有浏览器的
     */
    readonly isBrowser: boolean;

    /**
     * 是否在移动平台上
     */
    readonly isMobile: boolean;

    readonly platform: Platform;
    readonly os: OS;
    readonly osVersion: string;
    readonly osMainVersion: number;
    readonly browserVersion: string;

    get supportWebGL2(): boolean;

    /**
     * 是否在小游戏平台
     * 微信、抖音、小红书等等小游戏平台都返回true
     */
    get mingame(): boolean;
}