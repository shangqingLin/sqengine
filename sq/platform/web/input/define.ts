import { EventMouse } from "script/engine/sq/input/Events";

export function canvasScreenPointProcess(e:MouseEvent,input:EventMouse, element:HTMLCanvasElement){
    let bound = element.getBoundingClientRect();
    let dx = e.clientX - bound.x;

    //openGL Y轴向下才是负轴，所以这里的canvas的原点是从左下角的开始
    let dy = bound.y + bound.height - e.clientY;
            
    let rx = dx / bound.width;
    let ry = dy / bound.height;

    let cx = rx * element.width;
    let cy = ry * element.height;
    input.x = cx;
    input.y = cy;
}