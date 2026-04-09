import { SpriteFrame } from "../../assets";
import { serializable, SQBoolean, sqclass, SQFloat, type } from "../../core";
import { Component } from "../../framework";
import { StencilStage } from "../../rendering";
import { NodeEventType } from "../../scene";
import { GraphicsComponent } from "./graphics/GraphicsComponent";
import { SpriteComponent } from "./sprite/SpriteComponent";
import UIContentComponent from "./UIContentComponent";

/**
 * Mask组件是用来实现遮罩效果的
 */


export enum MaskType {
    /**
     * 使用矩形作为遮罩。
     */
    GRAPHICS_RECT = 0,

    /**
     * 使用椭圆作为遮罩。
     */
    GRAPHICS_ELLIPSE = 1,

    /**
     * 使用图像模版作为遮罩。
     */
    GRAPHICS_STENCIL = 2,

    /**
     * 使用图片模版作为遮罩。
     */
    SPRITE_STENCIL = 3,
}

@sqclass("sq.MaskComponent")
export class MaskComponent extends Component {

    private _type = MaskType.GRAPHICS_RECT;
    private _sprite: SpriteComponent;
    private _graphics: GraphicsComponent;
    private _inverted: boolean;
    private _alphaThreshold: number;

    public static MASK_TYPE_CHANGE = "MASK_TYPE_CHANGE";

    protected override onEnabled(): void {
        this._changeRenderType();
        this._updateGraphics();
        this.node.on(NodeEventType.RESIZE, this, this._onNodeSizeChanged);
    }


    private _onNodeSizeChanged(): void {
        this._updateGraphics();
    }

    protected override onDisabled(): void {
        this.node.off(NodeEventType.RESIZE, this, this._onNodeSizeChanged);
    }

    @type(MaskType)
    @serializable
    get type(): MaskType {
        return this._type;
    }

    set type(value: MaskType) {
        if (this._type === value) {
            if (!this.subComp) {
                this._changeRenderType();
            }
            return;
        }

        this._type = value;

        if (this._type !== MaskType.SPRITE_STENCIL) {
            if (this._sprite) {
                this.node.removeComponent(SpriteComponent);
            }
            this._changeRenderType();
        } else {
            if (this._graphics) {
                this._graphics.clear();
                this.node.removeComponent(GraphicsComponent);
            }
            this._changeRenderType();
        }
    }

    private _changeRenderType(): void {
        const isGraphics = (this._type !== MaskType.SPRITE_STENCIL);
        if (isGraphics) {
            this._createGraphics();
        } else {
            this._createSprite();
        }
    }

    protected _createSprite(): void {

        if (this._graphics) {
            this.node.removeComponent(GraphicsComponent);
            this._graphics = null;
        }

        if (!this._sprite) {
            let sprite = this._sprite = this.node.getComponent(SpriteComponent);
            if (!sprite) {
                const node = this.node;
                sprite = this._sprite = node.addComponent(SpriteComponent);
            }
            this.node.fire(MaskComponent.MASK_TYPE_CHANGE);
        }
        this._sprite.stencilStage = this._inverted ? StencilStage.ENTER_LEVEL_INVERTED : StencilStage.ENTER_LEVEL;
    }

    protected _createGraphics(): void {

        if (this._sprite) {
            this.node.removeComponent(SpriteComponent);
            this._sprite = null;
        }

        if (!this._graphics) {
            let graphics = this._graphics = this.node.getComponent(GraphicsComponent);
            if (!graphics) {
                const node = this.node;
                graphics = this._graphics = node.addComponent(GraphicsComponent);
            }
            this.node.fire(MaskComponent.MASK_TYPE_CHANGE);
        }
        this._graphics.stencilStage = this._inverted ? StencilStage.ENTER_LEVEL_INVERTED : StencilStage.ENTER_LEVEL;
    }

    protected _updateGraphics(): void {
        if (!this._graphics || (this._type !== MaskType.GRAPHICS_RECT && this._type !== MaskType.GRAPHICS_ELLIPSE)) {
            return;
        }
        const graphics = this._graphics;
        graphics.clear();
        const width = graphics.width;
        const height = graphics.height;

        if (width <= 0 || height <= 0) {
            return;
        }

        const x = - graphics.getAnchorXInPix();
        const y = - graphics.getAnchorYInPix();
        if (this._type === MaskType.GRAPHICS_RECT) {
            graphics.drawRect(x, y, width, height);
        } else if (this._type === MaskType.GRAPHICS_ELLIPSE) {
            graphics.drawCircle(x + width / 2, y + height / 2, Math.min(width, height) / 2);
        }
        graphics.fill();
    }

    get subComp(): UIContentComponent {
        return this._sprite || this._graphics;
    }

    get graphics(): GraphicsComponent {
        return this._graphics;
    }

    get sprite(): SpriteComponent {
        return this._sprite;
    }

    @type(SQBoolean)
    @serializable
    get inverted(): boolean {
        return this._inverted;
    }

    set inverted(value) {
        this._inverted = value;
        if (this.subComp)
            this.subComp.stencilStage = this._inverted ? StencilStage.ENTER_LEVEL_INVERTED : StencilStage.ENTER_LEVEL;
    }


    @type(SpriteFrame)
    @serializable
    get spriteFrame(): SpriteFrame | null {
        if (this._sprite) {
            return this._sprite.spriteFrame;
        }
        return null;
    }

    set spriteFrame(value) {
        if (this._sprite) {
            this._sprite.spriteFrame = value;
        }
    }
}