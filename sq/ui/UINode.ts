import { Transform2DComponent, UIContentComponent } from "../2d/index";
import Node from "../scene/Node";

export default class UINode extends Node {

    protected content: UIContentComponent;

    constructor() {
        super();
        this.createUIContentComponent();
    }

    /**
     * 子类实现，如果添加Sprite、Graphics等2D组件
     */
    protected createUIContentComponent() {
        this.content = this.addComponent(UIContentComponent);
    }

    set width(width: number) {
        this.content.width = width;
    }

    get width(): number {
        return this.content.width;
    }

    set height(height: number) {
        this.content.height = height;
    }

    get height(): number {
        return this.content.height;
    }

    set anchorX(anchorX: number) {
        this.content.anchorX = anchorX;
    }


    get anchorX() {
        return this.content.anchorX;
    }

    set anchorY(anchorY: number) {
        this.content.anchorY = anchorY;
    }

    get anchorY() {
        return this.content.anchorY;
    }

    set x(x: number) {
        this.transform.x = x;
    }

    get x(): number {
        return this.transform.x;
    }

    set y(y: number) {
        this.transform.y = y;
    }

    get y(): number {
        return this.transform.y;
    }

}