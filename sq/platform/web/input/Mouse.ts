import { js } from "script/engine/sq/core";
import { EventMouse, EventType, createEvent } from "../../../input/Events";
import { getCanvas } from "../env";

/**
 * 包括鼠標事件
 * @constructor
 */
export default class Mouse {
    private _element: HTMLCanvasElement;
    private _handlerEvent: (event: EventMouse) => void;

    private moveMouseEvent: EventMouse;
    private mouseLeftDownEvent: EventMouse;
    private mouseRightDownEvent: EventMouse;
    constructor() {
        this._element = getCanvas().canvas;
        this.attach();
    }

    set handlerEvent(callback: (event: EventMouse) => void) {
        this._handlerEvent = callback;
    }

    attach() {
        this._element.addEventListener("mousedown", this._handlerDown.bind(this), false);
        this._element.addEventListener("mouseup", this._handlerUp.bind(this), false);
        this._element.addEventListener("mousemove", this._handlerMove.bind(this), false);
        this._element.addEventListener("mousewheel", this._handlerWheel.bind(this), false);
        this._element.addEventListener("DOMMouseScroll", this._handlerWheel.bind(this), false);
        this._element.addEventListener("mouseout", this._handlerOut.bind(this), false);
    }

    detach() {
        this._element.removeEventListener("mousedown", this._handlerDown);
        this._element.removeEventListener("mouseup", this._handlerUp);
        this._element.removeEventListener("mousemove", this._handlerMove);
        this._element.removeEventListener("mousewheel", this._handlerWheel);
        this._element.removeEventListener("DOMMouseScroll", this._handlerWheel);
    }


    private canvasScreenPointProcess(e: MouseEvent, input: EventMouse) {
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

    private _handlerDown(e: MouseEvent) {

        //底层是使用mouse move来检测节点是否与鼠标碰撞，所以没有mouse move事件则虚构一个
        if (!this.moveMouseEvent) {
            this._handlerMove(e);
        }

        let eventObj;
        if (e.button === 0) {
            //左键
            this.mouseLeftDownEvent = this.moveMouseEvent;
            this.mouseLeftDownEvent.type = EventType.mousedown;
            this.mouseLeftDownEvent.button = EventMouse.BUTTON_LEFT;
            eventObj = this.mouseLeftDownEvent;
        } else {
            this.mouseRightDownEvent = this.moveMouseEvent;
            this.mouseRightDownEvent.type = EventType.mouserightdown;
            this.mouseRightDownEvent.button = EventMouse.BUTTON_RIGHT;
            eventObj = this.mouseRightDownEvent;
        }
        this.moveMouseEvent = null;
        eventObj.originEvent = e;
        this.canvasScreenPointProcess(e, eventObj);
        this._handlerEvent(eventObj);
    }

    private _handlerUp(e: MouseEvent) {
        let eventObj;
        if (e.button === 0) {
            //左键
            this.mouseLeftDownEvent.type = EventType.mouseup;
            this.mouseLeftDownEvent.button = EventMouse.BUTTON_LEFT;
            eventObj = this.mouseLeftDownEvent;
            this.mouseLeftDownEvent = null;
        } else {
            this.mouseRightDownEvent.type = EventType.mouserightup;
            this.mouseRightDownEvent.button = EventMouse.BUTTON_RIGHT;
            eventObj = this.mouseRightDownEvent;
            this.mouseRightDownEvent = null;
        }
        //@ts-ignore
        eventObj.processRemove = true;
        eventObj.originEvent = e;
        this.canvasScreenPointProcess(e, eventObj);
        this._handlerEvent(eventObj);
    }

    private _handlerMove(e: MouseEvent) {
        if (!this.moveMouseEvent) {
            this.moveMouseEvent = createEvent(EventType.mousemove) as EventMouse;
        }
        this.moveMouseEvent.type = EventType.mousemove;
        this.moveMouseEvent.originEvent = e;
        this.canvasScreenPointProcess(e, this.moveMouseEvent);
        this._handlerEvent(this.moveMouseEvent);
        // console.info("_handlerMove", this.moveMouseEvent.id, this.moveMouseEvent.x, this.moveMouseEvent.y);
    }

    private _handlerWheel(e: MouseEvent) {
        // this.mouseEvent.type = EventType.mousewheel;
        // this.mouseEvent.originEvent = e;
        // this.canvasScreenPointProcess(e, this.mouseEvent);
        // this._handlerEvent(this.mouseEvent);
    }

    private _handlerOut(e: MouseEvent) {
        this.moveMouseEvent.type = EventType.mouseout;
        this.moveMouseEvent.originEvent = e;
        this._handlerEvent(this.moveMouseEvent);
    }
}