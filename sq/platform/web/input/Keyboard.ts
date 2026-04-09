import {EventType,createEvent,EventKeyboard} from "../../../input/Events";
import { getCanvas } from "../env";

/**
 * 键盘的封装
 */
export default class Keyboard{
    private _element:HTMLCanvasElement;
    private _handlerEvent:(event:EventKeyboard)=>void;
    constructor() {
        this._element = getCanvas().canvas;
        this.attach();
    }

    set handlerEvent(callback:(event:EventKeyboard)=>void){
        this._handlerEvent = callback;
    }

    private attach() {
        this._element.addEventListener("keydown", this._handleKeyDown.bind(this), false);
        this._element.addEventListener("keypress", this._handleKeyPress.bind(this), false);
        this._element.addEventListener("keyup", this._handleKeyUp.bind(this), false);
    }

    detach() {
        this._element.removeEventListener("keydown", this._handleKeyDown);
        this._element.removeEventListener("keypress", this._handleKeyPress);
        this._element.removeEventListener("keyup", this._handleKeyUp);
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

    private _handleKeyPress(event:KeyboardEvent) {
        let ge:EventKeyboard = createEvent(EventType.keydown) as EventKeyboard;
        ge.originEvent = event;
        this._handlerEvent(ge);
    }
}

