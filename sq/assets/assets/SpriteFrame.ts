import { Rect, Size, sqclass, Vec2 } from "../../core/index";
import Asset from "./Asset";
import { RenderTexutre } from "./RenderTexture";
import SpriteAtlas from "./SpriteAtlas";
import { Texture2d } from "./Texture2d";
import TextureBase from "./TextureBase";


/**
 * 图集中的精灵
 */
@sqclass("sq.SpriteFrame")
export default class SpriteFrame extends Asset {

    /**
     * 如果设置是一张完整的图片，则表示将整张图片当作一个SpriteFrame处理，以便与SpriteComponent对接
     */
    private _texture: TextureBase | SpriteAtlas;
    private _rect: Rect;
    private _rotated: boolean = false;

    //图片原始尺寸，即没有去掉图片周边空白像素的尺寸
    //_rect中存储的是去掉周边空白像素的尺寸
    public readonly originalSize: Size;
    public readonly uv: Readonly<Array<number>> = [0, 0, 0, 0, 0, 0, 0, 0];
    public readonly slicedUv: Readonly<Array<number>>;

    /**
     * 9宫格设置
     * 0:Left
     * 1:Right
     * 2:top
     * 3:bottom
     */
    private _grid9: Array<number>;

    public name: string;

    //unity中有的，设置每个Sprite的锚点
    public pivot?: Vec2;

    set grid9(grid: Array<number>) {
        this._grid9 = grid;
        this.calcuUv();
    }

    set rotated(b: boolean) {
        this._rotated = b;
    }

    set rect(rect: Rect) {
        if (!this._rect) this._rect = new Rect();
        this._rect.copyFrom(rect);
        if (this._texture instanceof SpriteAtlas) {
            this.calcuUv();
        }
    }

    get rect(): Readonly<Rect> {
        return this._rect;
    }

    get width(): number {
        return this._rect.width;
    }

    get height(): number {
        return this._rect.height;
    }

    set texture(texture: TextureBase | SpriteAtlas) {
        if (texture === this._texture) return;
        this._texture = texture;
        if (this._texture instanceof TextureBase) {
            if (!this._rect) this._rect = new Rect();
            this._rect.x = 0;
            this._rect.y = 0;
            this._rect.width = (texture as TextureBase).width;
            this._rect.height = (texture as TextureBase).height;

            if (!this.originalSize)
                //@ts-ignore
                this.originalSize = new Size();

            this.originalSize.width = this._rect.width;
            this.originalSize.height = this._rect.height;
            this.calcuUv();
        } else {
            this.calcuUv();
        }
    }

    get texture(): TextureBase {
        return this._texture instanceof SpriteAtlas ? this._texture.texture : this._texture;
    }

    get spriteAtlas(): SpriteAtlas {
        return this._texture as SpriteAtlas;
    }

    private calcuUv() {

        if (!this._texture || !this._rect) return;

        let uv: Array<number> = this.uv as Array<number>;
        if (this._texture instanceof RenderTexutre) {
            uv[0] = 0;
            uv[1] = 0;

            uv[2] = 1;
            uv[3] = 0;

            uv[4] = 0;
            uv[5] = 1;

            uv[6] = 1;
            uv[7] = 1;
        } else if (this._texture instanceof Texture2d) {
            uv[0] = 0;
            uv[1] = 1;

            uv[2] = 1;
            uv[3] = 1;

            uv[4] = 0;
            uv[5] = 0;

            uv[6] = 1;
            uv[7] = 0;
        } else if (this._texture instanceof SpriteAtlas) {

            //默认的图片是从上往下递减
            let w = this._texture.texture.width;
            let h = this._texture.texture.height;

            let x1, y1, x2, y2;
            if (this._rotated) {
                x1 = this.rect.x / w;
                x2 = (this.rect.x + this.rect.height) / w;

                y1 = this.rect.y / h;
                y2 = (this.rect.y + this.rect.width) / h;


                //uv顺序也需要翻转过来，否则宽高会错误
                uv[0] = x1;
                uv[1] = y1;

                uv[2] = x1;
                uv[3] = y2;

                // Web搞特殊，Web图片是翻转的，所以这里需要做翻转处理
                uv[4] = x2;
                uv[5] = y1;

                uv[6] = x2;
                uv[7] = y2;

            } else {


                x1 = this.rect.x / w;
                x2 = (this.rect.x + this.rect.width) / w;

                y1 = this.rect.y / h;
                y2 = (this.rect.y + this.rect.height) / h;

                //Unity中的(行业标准就是从下往上的)计算散图位置的Y轴是从下往上增加的
                // Web搞特殊，Web图片是翻转的，所以这里需要做翻转处理
                uv[0] = x1;
                uv[1] = y2;

                uv[2] = x2;
                uv[3] = y2;

                uv[4] = x1;
                uv[5] = y1;

                uv[6] = x2;
                uv[7] = y1;
            }
        }

        this.calculateSlicedUV();
    }

    private calculateSlicedUV(): void {
        if (!this._grid9) return;

        let texture: TextureBase = this._texture instanceof SpriteAtlas ? this._texture.texture : this._texture;

        //@ts-ignore
        this.slicedUv = [];

        const leftWidth = this._grid9[0];
        const rightWidth = this._grid9[1];
        const centerWidth = this._rect.width - leftWidth - rightWidth;
        const topHeight = this._grid9[2];
        const bottomHeight = this._grid9[3];
        const centerHeight = this._rect.height - topHeight - bottomHeight;

        let rect: Rect = this._rect;
        let column: Array<number> = [];
        let row: Array<number> = [];

        if (this._rotated) {

        } else {

            column[0] = rect.x / texture.width;
            column[1] = (rect.x + leftWidth) / texture.width;
            column[2] = (rect.x + leftWidth + centerWidth) / texture.width;
            column[3] = (rect.x + rect.width) / texture.width;

            row[3] = rect.y / texture.height;
            row[2] = (rect.y + topHeight) / texture.height;
            row[1] = (rect.y + topHeight + centerHeight) / texture.height;
            row[0] = (rect.y + rect.height) / texture.height;
        }

        let uv: Array<number> = this.slicedUv as Array<number>;

        let index = 0;

        // 第一行第一列
        uv[index++] = column[0];
        uv[index++] = row[0];

        if (leftWidth !== 0) {

            // 第一行第二列
            uv[index++] = column[1];
            uv[index++] = row[0];
        }

        if (rightWidth !== 0) {
            // 第一行第三列
            uv[index++] = column[2];
            uv[index++] = row[0];
        }

        // 第一行第四列
        uv[index++] = column[3];
        uv[index++] = row[0];

        if (bottomHeight !== 0) {

            // 第二行第一列
            uv[index++] = column[0];
            uv[index++] = row[1];
            if (leftWidth !== 0) {
                // 第二行第二列
                uv[index++] = column[0];
                uv[index++] = row[1];
            }

            if (rightWidth !== 0) {
                // 第二行第二列
                uv[index++] = column[2];
                uv[index++] = row[1];
            }

            // 第二行第三列
            uv[index++] = column[3];
            uv[index++] = row[1];
        }

        if (topHeight !== 0) {
            // 第三行第一列
            uv[index++] = column[0];
            uv[index++] = row[2];

            if (leftWidth !== 0) {
                // 第三行第二列
                uv[index++] = column[1];
                uv[index++] = row[2];
            }

            if (rightWidth !== 0) {
                // 第三行第二列
                uv[index++] = column[2];
                uv[index++] = row[2];
            }

            // 第三行第三列
            uv[index++] = column[3];
            uv[index++] = row[2];
        }

        // 第四行第一列
        uv[index++] = column[0];
        uv[index++] = row[3];

        if (leftWidth !== 0) {
            // 第四行第二列
            uv[index++] = column[1];
            uv[index++] = row[3];
        }

        if (rightWidth !== 0) {
            // 第四行第三列
            uv[index++] = column[2];
            uv[index++] = row[3];
        }

        // 第四行第四列
        uv[index++] = column[3];
        uv[index++] = row[3];
    }
}