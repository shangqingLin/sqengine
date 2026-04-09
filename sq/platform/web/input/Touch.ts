import { js } from "../../../core";
import { EventMouse, EventType, createEvent } from "../../../input/Events";
import { getCanvas } from "../env";

export default class Touch {
    private _element: HTMLCanvasElement;
    private _handlerEvent: (event: EventMouse) => void;
    private touchIdMap: { [key: number]: EventMouse } = js.createMap();
    constructor() {
        this._element = getCanvas().canvas;
        this.attach();
    }

    set handlerEvent(callback: (event: EventMouse) => void) {
        this._handlerEvent = callback;
    }

    attach() {
        this._element.addEventListener("touchstart", this._handlerDown.bind(this), false);
        this._element.addEventListener("touchend", this._handlerUp.bind(this), false);
        this._element.addEventListener("touchmove", this._handlerMove.bind(this), false);
        this._element.addEventListener("tocuhcancel", this._handlerCancel.bind(this), false);
    }

    detach() {
        this._element.removeEventListener("touchstart", this._handlerDown);
        this._element.removeEventListener("touchend", this._handlerUp);
        this._element.removeEventListener("touchmove", this._handlerMove);
        this._element.removeEventListener("tocuhcancel", this._handlerCancel);
    }

    private canvasScreenPointProcess(e: globalThis.Touch, input: EventMouse) {
        let bound = this._element.getBoundingClientRect();

        //鼠标点击的位置减去element左上角的位置，得出鼠标在元素内与元素左上角的距离
        let dx = e.clientX - bound.x;

        //openGL Y轴向下才是负轴，所以对于OpenGL的View来说canvas的原点是从左下角的开始
        let dy = bound.height - (e.clientY - bound.y);

        let rx = dx / bound.width;
        let ry = dy / bound.height;

        let cx = rx * this._element.width;
        let cy = ry * this._element.height;
        input.x = cx;
        input.y = cy;
    }

    private _handlerDown(e: TouchEvent) {
        for (let i = 0; i < e.changedTouches.length; ++i) {
            let touch: globalThis.Touch = e.changedTouches[i];
            let gt: EventMouse = createEvent(EventType.touchstart) as EventMouse;
            gt.tocuhId = touch.identifier;
            console.info("_handlerDown", gt.id, gt.tocuhId);
            this.canvasScreenPointProcess(touch, gt);
            this.touchIdMap[touch.identifier] = gt;
            this._handlerEvent(gt);
        }
    }

    private _handlerUp(e: TouchEvent) {
        this._handlerCancel(e);
    }

    private _handlerMove(e: TouchEvent) {
        for (let i = 0; i < e.changedTouches.length; ++i) {
            let touch: globalThis.Touch = e.changedTouches[i];
            let gt: EventMouse = this.touchIdMap[touch.identifier] as EventMouse;
            gt.type = EventType.touchmove;
            this.canvasScreenPointProcess(touch, gt);
            this._handlerEvent(gt);
        }
    }

    private _handlerCancel(e: TouchEvent) {
        for (let i = 0; i < e.changedTouches.length; ++i) {
            let touch: globalThis.Touch = e.changedTouches[i];
            let gt: EventMouse = this.touchIdMap[touch.identifier] as EventMouse;
            gt.type = EventType.tocuhcancel;
            //@ts-ignore
            gt.processRemove = true;
            this.canvasScreenPointProcess(touch, gt);
            this._handlerEvent(gt);
        }
    }
}