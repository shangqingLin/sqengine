import { FillStyle, GraphicsComponent, StrokeStyle } from "../2d/index";
import UINode from "./UINode";

export default class UIGraphics extends UINode {
    protected override createUIContentComponent(): void {
        this.content = this.addComponent(GraphicsComponent);
    }

    public moveTo(x: number, y: number) {
        (this.content as GraphicsComponent).moveTo(x, y);
    }

    public lineTo(x: number, y: number) {
        (this.content as GraphicsComponent).lineTo(x, y);
    }


    public fill(fillStyle?: FillStyle) {
        (this.content as GraphicsComponent).fill(fillStyle);
    }

    public stroke(stye?: StrokeStyle) {
        (this.content as GraphicsComponent).stroke(stye);
    }

    public drawCircle(centerX: number, centerY: number, r: number) {
        (this.content as GraphicsComponent).drawCircle(centerX, centerY, r);
    }

    public drawEllipse(cx: number, cy: number, rx: number, ry: number) {
        (this.content as GraphicsComponent).drawEllipse(cx, cy, rx, ry);
    }

    public drawRect(x: number, y: number, w: number, h: number) {
        (this.content as GraphicsComponent).drawRect(x, y, w, h);
    }

    public clear() {
        (this.content as GraphicsComponent).clear();
    }

    public close() {
        (this.content as GraphicsComponent).close();
    }


    drawCapsule(c1x: number, c1y: number, c2x: number, c2y: number, radius: number): void {
        (this.content as GraphicsComponent).drawCapsule(c1x, c1y, c2x, c2y, radius);
    }
}