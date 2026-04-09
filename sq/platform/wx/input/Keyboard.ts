import { EventType, createEvent, EventKeyboard } from "../../../input/Events";

/**
 * 键盘的封装
 */
export default class Keyboard {

    private keyDownBind:Function;
    private keyUpBind:Function;
    private _handlerEvent:(event:EventKeyboard)=>void;
    constructor(){
        this.keyDownBind = this._handleKeyDown.bind(this)
        this.keyUpBind =  this._handleKeyUp.bind(this)
        this.attach();
    }

    set handlerEvent(callback: (event: EventKeyboard) => void) {
        this._handlerEvent = callback;
    }

    attach() {
       wx.onKeyDown(this.keyDownBind);
       wx.onKeyUp(this.keyUpBind );
    }

    detach() {
        wx.offKeyDown(this.keyDownBind);
        wx.offKeyUp(this.keyUpBind);
    }

    private _handleKeyDown(event:KeyboardEvent) {
        let ge:EventKeyboard = createEvent(EventType.keydown) as EventKeyboard;
        ge.originEvent = event;
        this._handlerEvent(ge);
    }

    private _handleKeyUp(event:KeyboardEvent) {
        let ge:EventKeyboard = createEvent(EventType.keyup) as EventKeyboard;
        ge.originEvent = event;
        this._handlerEvent(ge);
    }

}

