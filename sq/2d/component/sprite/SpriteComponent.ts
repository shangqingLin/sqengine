import { ComponentType } from "../../../native_binding/index";
import { Texture2d } from "../../../assets/assets/Texture2d";
import SpriteFrame from "../../../assets/assets/SpriteFrame";
import { AssetManager } from "../../../assets/assetmanager/AssetManager";
import { editableProp, editorMenu, math, serializable, Size, sqclass, SQFloat, SQString, type, Vec2 } from "../../../core/index";
import { Asset, RenderTexutre } from "../../../assets";
import SpriteShader from "./SpriteShader";
import UIContentComponent from "../UIContentComponent";

export enum SpriteType {
    /**
     * 普通类型。
     */
    SIMPLE = 0,
    /**
     * 切片（九宫格）类型。
     */
    SLICED = 1,
    /**
     * @zh  平铺类型
     */
    TILED = 2,
    /**
     * 填充类型。
     */
    FILLED = 3,
}


export enum SpriteFillType {
    /**
     * 水平方向填充。
     */
    HORIZONTAL = 0,

    /**
     * 垂直方向填充。
     */
    VERTICAL = 1,

    /*
     * 径向填充
     */
    RADIAL = 2,
}

export enum SpriteSizeMode {

    /**
     * 使用外部自己设置的尺寸。
     */
    CUSTOM = 0,

    /**
     * 自动适配为精灵裁剪后的尺寸。
     */
    TRIMMED = 1,

    /**
     * 自动适配为精灵原图尺寸。
     */
    RAW = 2,
}

/**
 * 2D精灵类，所有2D需要渲染的可见的元素都必须添加该组件
 * @constructor
 */
/* editor:start */
@editorMenu("2D/Sprite")
/* editor:end */
@sqclass("sq.SpriteComponent")
export class SpriteComponent extends UIContentComponent {

    private sprite: SpriteFrame;
    private spriteUrl: string;
    private spriteUUID: string;
    private innerSprite: boolean;
    private _type: SpriteType = SpriteType.SIMPLE;
    private _sizeMode: SpriteSizeMode = SpriteSizeMode.TRIMMED;
    private _fillType: SpriteFillType;
    private _fillCenter: Vec2;
    private _fillStart: number;
    private _fillRange: number;
    private _useGray: boolean;

    //当通过URL设置SpriteFrame时，监听此事件判断是否加载完毕
    public static readonly ONLOADED = 1;

    constructor() {
        super(ComponentType.SpriteComponent);
    }

    private updateMaterial() {
        if (!this.material) {
            this.setShaderMaterial(SpriteShader.getDefaultSpriteMaterial());
        }
    }


    @type(SpriteFrame)
    @serializable
    /*editor:start*/
    @editableProp(0)
    /*editor:end*/
    public set spriteFrame(sprite: SpriteFrame) {
        if (this.sprite === sprite) {
            return;
        }
        if (this.innerSprite) {
            AssetManager.getInstance().destroyAsset(this.sprite);
        }
        this.sprite = sprite;
        this.innerSprite = false;
        this._applySpriteSize();
        this.writeSpriteFrame(sprite)

        if (sprite) {
            this.updateMaterial();
        }
    }

    public get spriteFrame(): SpriteFrame {
        return this.sprite;
    }

    private writeSpriteFrame(sprite: SpriteFrame) {
        this.nativeBeginOp(50,true);
        let state = 0;
        let statePos = this.nativeWriteOpArg("i8", state);
        if (sprite && sprite.texture) {
            state = 1;
            this.nativeWriteOpArg("i32", sprite.texture.getId());
            this.nativeWriteOpArg("i32", sprite.rect.x);
            this.nativeWriteOpArg("i32", sprite.rect.y);
            this.nativeWriteOpArg("i32", sprite.rect.width);
            this.nativeWriteOpArg("i32", sprite.rect.height);
            this.nativeWriteOpArg("i8", sprite.rotated ? 1 : 0);

            for (let i = 0; i < 8; ++i) {
                this.nativeWriteOpArg("f32", sprite.uv[i]);
            }

            if (sprite.grid9 && sprite.grid9.length > 0) {
                state |= 1 << 2;
                let grid9: Array<number> = sprite.grid9;
                for (let i = 0; i < 4; ++i) {
                    this.nativeWriteOpArg("f32", grid9[i]);
                }
                this.nativeWriteOpArg("i32", sprite.slicedUv.length);
                for (let i = 0; i < sprite.slicedUv.length; ++i) {
                    this.nativeWriteOpArg("f32", sprite.slicedUv[i]);
                }
            }
        } else {
            state = 0;
        }
        this.nativeWriteOpArgOverride("i8", state, statePos);
        this.nativeEndOp();
    }

    public set spriteFrameUUID(uuid: string) {
        if (this.spriteUUID === uuid) {
            return;
        }
        if (this.spriteUrl) this.spriteUrl = null;
        this.spriteUUID = uuid;
        if (this.spriteUUID) {
            AssetManager.getInstance().loadById(uuid, (error, texture: Texture2d | SpriteFrame) => {
                if (error) return;

                if (texture.uuid !== this.spriteUUID) {
                    return;
                }
                this.setAsset(texture);
                this.fire(SpriteComponent.ONLOADED);
            });
        } else {
            this.spriteFrame = null;
        }
    }


    @serializable
    @type(SQString)
    public set spriteFrameUrl(url: string) {
        if (this.spriteUrl === url) {
            return;
        }
        this.spriteUrl = url;
        if (this.spriteUUID) this.spriteUUID = null;
        if (this.spriteUrl) {
            AssetManager.getInstance().load(url, (error, texture: Texture2d | SpriteFrame) => {
                if (error) return;

                // if (url === "map/level/level_2/ziyan_bg_jj") {
                //     console.info("spriteFrameUrl", url, texture.getId())
                // }

                if (texture.keyUrl !== this.spriteUrl) {
                    return;
                }
                this.setAsset(texture);
                this.fire(SpriteComponent.ONLOADED);
            });
        } else {
            this.spriteFrame = null;
        }
    }

    public set texture(texture: Texture2d | RenderTexutre) {
        let spriteFrame = new SpriteFrame();
        spriteFrame.texture = texture;
        this.spriteFrame = spriteFrame;
        this.innerSprite = true
    }

    private setAsset(texture: Texture2d | SpriteFrame) {
        if (texture instanceof Texture2d) {
            let spriteFrame = new SpriteFrame();
            spriteFrame.texture = texture;
            this.spriteFrame = spriteFrame;
            this.innerSprite = true
        } else {
            this.spriteFrame = texture;
            this.innerSprite = false;
        }
    }

    protected override onRemove(): void {
        super.onRemove();
        if (this.innerSprite) {
            AssetManager.getInstance().destroyAsset(this.sprite);
        }
    }

    @type(SpriteType)
    @serializable
    get type(): SpriteType {
        return this._type;
    }
    set type(value: SpriteType) {
        if (this._type !== value) {
            this._type = value;
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("i8", value);
            this.nativeEndOp();
        }
    }

    @type(SpriteFillType)
    @serializable
    get fillType(): SpriteFillType {
        return this._fillType;
    }
    set fillType(value: SpriteFillType) {
        if (this._fillType !== value) {
            this._fillType = value;
            this.nativeBeginOp(52);
            this.nativeWriteOpArg("i8", value);
            this.nativeEndOp();
        }
    }

    /**
     * fillType 设置为 SpriteType.FILLED 才有效，用来控制填充中心，即从哪个点开始填充
     */
    @type(Vec2)
    @serializable
    get fillCenter(): Vec2 {
        return this._fillCenter;
    }
    set fillCenter(value) {
        if (!this._fillCenter) this._fillCenter = new Vec2();
        if (this._fillCenter.x !== value.x || this.fillCenter.y !== value.y) {
            this._fillCenter.x = value.x;
            this._fillCenter.y = value.y;
            if (this._type === SpriteType.FILLED) {
                this.nativeBeginOp(56);
                this.nativeWriteOpArg("f32", value.x);
                this.nativeWriteOpArg("f32", value.y);
                this.nativeEndOp();
            }
        }
    }

    @type(SQFloat)
    @serializable
    get fillStart(): number {
        return this._fillStart;
    }

    set fillStart(value) {
        value = math.clamp(value, 0, 1);
        if (this._fillStart != value) {
            this._fillStart = value;
            if (this._type === SpriteType.FILLED) {
                this.nativeBeginOp(57);
                this.nativeWriteOpArg("f32", value);
                this.nativeEndOp();
            }
        }
    }

    @type(SQFloat)
    @serializable
    get fillRange(): number {
        return this._fillRange;
    }
    set fillRange(value) {
        value = math.clamp(value, -1, 1);
        if (this._fillRange !== value) {
            this._fillRange = value;
            if (this._type === SpriteType.FILLED) {
                this.nativeBeginOp(58);
                this.nativeWriteOpArg("f32", value);
                this.nativeEndOp();
            }
        }
    }

    @type(SpriteSizeMode)
    @serializable
    get sizeMode(): SpriteSizeMode {
        return this._sizeMode;
    }
    set sizeMode(value) {
        if (this._sizeMode === value) {
            return;
        }

        this._sizeMode = value;
        if (value !== SpriteSizeMode.CUSTOM) {
            this._applySpriteSize();
        }
    }
    private _applySpriteSize(): void {
        if (this.sprite) {
            if (SpriteSizeMode.RAW === this._sizeMode) {
                const size = this.sprite.originalSize;
                this.width = size.width;
                this.height = size.height;
            } else if (SpriteSizeMode.TRIMMED === this._sizeMode) {
                const rect = this.sprite.rect;
                this.width = rect.width;
                this.height = rect.height;
            }
        }
    }

    /**
     *  将精灵显示为灰色
     */
    get gray(): boolean {
        return this._useGray;
    }
    set gray(value: boolean) {
        if (this._useGray == value) {
            return;
        }
        this._useGray = value;
    }
}
