import { js, Size } from "../../../core/index";
import { EventMouse, EventType, createEvent } from "../../../input/Events";
import { screen } from "../Screen";

export default class Touch {

    private _handlerEvent: (event: EventMouse) => void;
    private renderSize: Size;
    private windowInfo: wx.WindowInfo;
    private touchIdMap: { [key: number]: EventMouse } = js.createMap();
    constructor() {
        this.attach();
        this.renderSize = new Size();
        this.windowInfo = wx.getWindowInfo();
    }

    set handlerEvent(callback: (event: EventMouse) => void) {
        this._handlerEvent = callback;
    }


    private canvasScreenPointProcess(clientX: number, clientY: number, e: EventMouse) {
        screen.getRenderSize(this.renderSize);
        let dx = clientX / this.windowInfo.windowWidth;
        let dy = 1 - clientY / this.windowInfo.windowHeight;
        e.x = dx * this.renderSize.width;
        e.y = dy * this.renderSize.height;
    }

    attach() {
        wx.onTouchStart(this._handlerDown.bind(this));
        wx.onTouchEnd(this._handlerUp.bind(this));
        wx.onTouchMove(this._handlerMove.bind(this));
        wx.onTouchCancel(this._handlerCancel.bind(this));
    }

    detach() {

    }

    private _handlerDown(e: wx.TouchEvent) {
        for (let i = 0, n = e.touches.length; i < n; ++i) {
            let touch = e.touches[i];
            let gt: EventMouse = createEvent(EventType.touchstart) as EventMouse;
            gt.tocuhId = touch.identifier;
            this.touchIdMap[touch.identifier] = gt;
            this.canvasScreenPointProcess(touch.clientX, touch.clientY, gt);
            this._handlerEvent(gt);
        }
    }

    private _handlerUp(e: wx.TouchEvent) {
        this._handlerCancel(e);
    }

    private _handlerMove(e: wx.TouchEvent) {
        for (let i = 0, n = e.changedTouches.length; i < n; ++i) {
            let touch = e.changedTouches[i];
            let gt: EventMouse = this.touchIdMap[touch.identifier] as EventMouse;
            gt.type = EventType.touchmove;
            this.canvasScreenPointProcess(touch.clientX, touch.clientY, gt);
            this._handlerEvent(gt);
        }
    }

    private _handlerCancel(e: wx.TouchEvent) {
        for (let i = 0, n = e.changedTouches.length; i < n; ++i) {
            let touch = e.changedTouches[i];
            let gt: EventMouse = this.touchIdMap[touch.identifier] as EventMouse;
            gt.type = EventType.tocuhcancel;
            //@ts-ignore
            gt.processRemove = true;
            this.canvasScreenPointProcess(touch.clientX, touch.clientY, gt);
            this._handlerEvent(gt);
        }
    }
}