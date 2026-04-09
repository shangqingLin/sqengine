

var canvas: HTMLCanvasElement;
export function getCanvas(customCanvas?: HTMLCanvasElement): { canvas: HTMLCanvasElement } {
    if (customCanvas) {
        canvas = customCanvas;
    }

    if (canvas) {
        return {
            canvas,
        };
    }
    canvas = document.createElement("canvas");
    canvas.setAttribute("tabindex", "1000");
    canvas.focus();
    document.body.appendChild(canvas);
    return {
        canvas
    }
}