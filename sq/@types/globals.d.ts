declare const gfx: any;
declare const xr: any;

interface Window {

    [x: string]: any;

    WebGL2RenderingContext: any;

    sharedCanvas: any;
    __canvas: any;
    canvas: any;

    XMLHttpRequest: any;
    mozRequestAnimationFrame(callback: any, element?: any): any;
    oRequestAnimationFrame(callback: any, element?: any): any;
    msRequestAnimationFrame(callback: any, element?: any): any;
    cancelRequestAnimationFrame(callback: any, element?: any): any;
    msCancelRequestAnimationFrame(callback: any, element?: any): any;
    mozCancelRequestAnimationFrame(callback: any, element?: any): any;
    oCancelRequestAnimationFrame(callback: any, element?: any): any;
    webkitCancelRequestAnimationFrame(callback: any, element?: any): any;
    msCancelAnimationFrame(callback: any, element?: any): any;
    mozCancelAnimationFrame(callback: any, element?: any): any;
    ocancelAnimationFrame(callback: any, element?: any): any;
}

type SimplePoint = {
    x: number
    y: number;
} & Record<string, any>;

interface Document {
    mozHidden: any;
    msHidden: any;
    webkitHidden: any;
}

interface HTMLElement {
    content: any;
    name: any;
}

declare type CompareFunction<T> = (a: T, b: T) => number;


declare type TypedArray = Uint8Array | Uint8ClampedArray | Int8Array | Uint16Array |
    Int16Array | Uint32Array | Int32Array | Float32Array | Float64Array;

declare type TypedArrayConstructor = Uint8ArrayConstructor | Uint8ClampedArrayConstructor |
    Int8ArrayConstructor | Uint16ArrayConstructor | Int16ArrayConstructor | Uint32ArrayConstructor |
    Int32ArrayConstructor | Float32ArrayConstructor | Float64ArrayConstructor;

declare type Constructor<T = unknown> = new (...args: any[]) => T;

declare type AbstractedConstructor<T = unknown> = abstract new (...args: any[]) => T;



//==================== 定义预编译宏 ========================

//是否是开发过程
declare const DEV: boolean;

//是否在编辑器中运行
declare const EDITOR: boolean;