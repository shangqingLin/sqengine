import { sqclass, type, serializable, SQFloat } from "../data/index";


@sqclass("sq.Color")
export default class Color {


    public static WHITE = Object.freeze(new Color(255, 255, 255, 255));
    public static GRAY = Object.freeze(new Color(127, 127, 127, 255));
    public static BLACK = Object.freeze(new Color(0, 0, 0, 255));
    public static TRANSPARENT = Object.freeze(new Color(0, 0, 0, 0));
    public static RED = Object.freeze(new Color(255, 0, 0, 255));
    public static GREEN = Object.freeze(new Color(0, 255, 0, 255));
    public static BLUE = Object.freeze(new Color(0, 0, 255, 255));
    public static CYAN = Object.freeze(new Color(0, 255, 255, 255));
    public static MAGENTA = Object.freeze(new Color(255, 0, 255, 255));
    public static YELLOW = Object.freeze(new Color(255, 255, 0, 255));

    @type(SQFloat)
    @serializable
    public r: number = 0;

    @type(SQFloat)
    @serializable
    public g: number = 0;

    @type(SQFloat)
    @serializable
    public b: number = 0;

    @type(SQFloat)
    @serializable
    public a: number = 1;

    constructor(r?: number, g?: number, b?: number, a?: number) {
        this.r = r || 0;
        this.g = g || 0;
        this.b = b || 0;
        this.a = a || 0;
    }

    public set(r?: number, g?: number, b?: number, a?: number): Color {
        this.r = r || 0;
        this.g = g || 0;
        this.b = b || 0;
        this.a = a || 1;
        return this;
    }

    public setFormColor(color: Color): Color {
        this.r = color.r || 0;
        this.g = color.g || 0;
        this.b = color.b || 0;
        this.a = color.a || 1;
        return this;
    }

    /**
     * 将RGBA合并为一个数字来存储
     */
    public toNumber() {
        let colorNum: number = 0;
        colorNum |= this.r << 24;
        colorNum |= this.g << 16;
        colorNum |= this.b << 8;
        colorNum |= this.a;
        return colorNum;
    }

    toString() {
        return [ this.r, this.g, this.b,this.a].join(",");
    }

    public clone(): Color {
        let c = new Color();
        c.setFormColor(this);
        return c;
    }

    static hexToColor(hex: number): Color {
        return new Color(
            (hex >> 24) & 255,
            (hex >> 16) & 255,
            (hex >> 8) & 255,
            hex & 255,
        );
    }

    /**
     * 示例
     * console.log(hexToRgba("#ff00ff"));     // rgba(255, 0, 255, 1)
     * console.log(hexToRgba("#f0f"));        // rgba(255, 0, 255, 1)
     *console.log(hexToRgba("#ff00ff80"));   // rgba(255, 0, 255, 0.502)
     * @param hex 
     * @returns 
     */
    static strHexToColor(hex: string): Color {
        hex = hex.replace(/^#/, "");
        if (hex.length === 3)
            hex = hex.split("").map(x => x + x).join("");
        if (hex.length === 6)
            hex += "ff";
        const num = parseInt(hex, 16);
        return Color.hexToColor(num);
    }
}