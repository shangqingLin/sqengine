import { IDGen } from "../core/index";
import Node from "../scene/Node";

let idGen = new IDGen();

enum EventType {
    keydown,
    keyup,
    mousedown,
    mouseup,
    mouserightdown,
    mouserightup,
    mouseout,
    mousemove,
    mouseover,
    mouseclick,
    mouserightclick,
    mousewheel,
    touchstart,
    touchend,
    touchmove,
    tocuhcancel
}

class Event {

    //这个事件处理完毕之后是否移除
    //事件与触控点绑定的
    private processRemove: boolean = false;

    public type: EventType;
    public id: number;
    public target: Node;

    /**
     * 世界坐标系下鼠标的坐标点
     */
    public worldX: number;
    public worldY: number;

    /**
     * 鼠标在节点的本地坐标
     */
    public localX: number;
    public localY: number;

    private _stoped: boolean = false;
    private _recovery: boolean;
    constructor() {
        this.id = idGen.get();
    }

    /**
     *阻止对事件流中当前节点的后续节点中的所有事件侦听器进行处理。此方法不会影响当前节点 (currentTarget)中的任何事件侦听器。
     */
    stopPropagation() {
        this._stoped = true;
    }

    reset() {
        this._stoped = false;
        this.target = null;
        this.processRemove = false;
    }
}


class EventKeyboard extends Event {
    public originEvent: KeyboardEvent;
    override reset(): void {
        super.reset();
        this.originEvent = null;
    }
}


class EventMouse extends Event {

    public static readonly BUTTON_LEFT: number = 1;
    public static readonly BUTTON_RIGHT: number = 2;

    /**
     * Canvas空间下的位置
     */
    public x: number;
    public y: number;

    public originEvent: any;

    /**
     * 左键还是右键
     * 如果是touch事件，则固定为左键
     */
    public button: number = EventMouse.BUTTON_LEFT;

    //只有在Touch事件中才有值
    //如果是鼠标，则固定为0
    public tocuhId: number = 0;

    override reset(): void {
        super.reset();
        this.originEvent = null;
        this.tocuhId = 0;
        this.button = EventMouse.BUTTON_LEFT;
    }
}

let EventPoolMap: { [key: string]: Array<Event> } = Object.create(null);

function recoveryEvent(event: Event) {

    //@ts-ignore
    if (event._recovery) {
        return;
    }

    let key: string = event.type === EventType.keydown || event.type === EventType.keyup ? "keyboard" : "mouse";
    let pool: Array<Event> = EventPoolMap[key];
    if (!pool) {
        pool = [];
        EventPoolMap[key] = pool;
    }
    //@ts-ignore
    event._recovery = true;
    event.reset();
    pool.push(event);
}

function createEvent(type: EventType): Event {
    let key: string;
    let cls: any;
    if (type === EventType.keydown || type === EventType.keyup) {
        key = "keyboard";
        cls = EventKeyboard;
    } else {
        key = "mouse";
        cls = EventMouse;
    }

    let pool: Array<Event> = EventPoolMap[key];
    let event: Event;
    if (pool) {
        event = pool.pop();
        if (event) {
            //@ts-ignore
            event._recovery = false;
        }
    }
    if (!event) {
        event = new cls();
    }
    event.type = type;
    return event;
}

export {
    recoveryEvent,
    createEvent,
    EventType,
    Event,
    EventMouse,
    EventKeyboard
}


