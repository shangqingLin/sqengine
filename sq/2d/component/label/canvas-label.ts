
/**
 * 使用Canvas方式来绘制文本
 */

import { LabelComponent } from "./LabelCompoinent";


export class CanvasPool {
    private static _canvasPool: CanvasPool;
    private pool: Array<{ canvas: HTMLCanvasElement, context: RenderingContext }> = [];
    static getInstance(): CanvasPool {
        if (!CanvasPool._canvasPool) {
            CanvasPool._canvasPool = new CanvasPool();
        }
        return CanvasPool._canvasPool;
    }
    public get(): { canvas: HTMLCanvasElement, context: RenderingContext } {
        let data = this.pool.pop();
        if (!data) {
            const canvas = window.document.createElement('canvas');
            const context = canvas.getContext('2d');
            data = {
                canvas,
                context,
            };
        }
        return data;
    }

    public put(canvas: { canvas: HTMLCanvasElement, context: RenderingContext }): void {
        this.pool.push(canvas);
    }
}




export default
    {

        generator: function (label: LabelComponent) {
            let canvas = CanvasPool.getInstance().get();

        }
    }


