import { Event, EventKeyboard, EventMouse, EventType, recoveryEvent } from "./Events"
import { EventDispatch } from "../core/index";
import { Touch, Mouse, Keyboard } from "../platform/index";
import { WasmByte, dispatch } from "../native_binding/index";
import type Node from "../scene/Node";

class InputManager extends EventDispatch {

    private keyborad: Keyboard;
    private mouse: Mouse;
    private tocuh: Touch;
    private _keyboardEvents: Array<any> = [];
    private _nativeEvent: { [key: number]: Event } = Object.create(null);
    private _datas: WasmByte;
    private native: Module.InputManager;
    private eventCount: number = 0;
    private keyBoardDownCodeMap: { [key: string]: boolean } = Object.create(null);
    initialize() {
        var _this = this;

        _this.native = new window.Module.InputManager();
        _this._datas = new WasmByte();
        _this._datas.onResizeHandler = _this.onResizeData.bind(_this);

        // 50 x 28 : 一帧处理50个事件（应该不会超过这么多事件了吧），每个事件占用的内存为28个字节
        //如果真的超过了，那么说明游戏卡帧了，这种情况下其实可以忽略一些输入事件不处理，反正游戏都卡了
        _this._datas.resize(1204);
        _this._datas.pos = 4;

        _this.tocuh = new Touch();
        _this.tocuh.handlerEvent = _this._handlerTouch.bind(_this);

        _this.mouse = new Mouse();
        _this.mouse.handlerEvent = _this._handlerMouse.bind(_this);

        _this.keyborad = new Keyboard();
        _this.keyborad.handlerEvent = _this._handlerKeyborad.bind(_this);
    }

    private onResizeData(pointer: number, size: number) {
        this.native.setEventDataPointer(pointer, size);
    }

    private _handlerMouse(event: EventMouse) {
        var _this = this;
        if (_this.eventCount > 50) return recoveryEvent(event);


        ++_this.eventCount;
        _this._datas.writeInt32(event.id);
        _this._datas.writeUint8(event.type);
        _this._datas.writeFloat32(event.x);
        _this._datas.writeFloat32(event.y); //13
        _this._nativeEvent[event.id] = event;

        //鼠标都跑出舞台了,直接触发舞台的mouseout事件
        if (event.type === EventType.mouseout) {
            this.fire(EventType.mouseout, event);
        }
        // console.info("======add ",event.type,event.id);
    }

    private _handlerTouch(event: EventMouse) {
        var _this = this;
        if (_this.eventCount > 50) return recoveryEvent(event);
        ++_this.eventCount;

        _this._datas.writeInt32(event.id);
        _this._datas.writeUint8(event.type);
        _this._datas.writeFloat32(event.x);
        _this._datas.writeFloat32(event.y);
        _this._nativeEvent[event.id] = event;
    }

    private _handlerKeyborad(event: EventKeyboard) {
        this._keyboardEvents.push(event);
    }

    /**
     * 实现节点冒泡
     * @param node 
     * @param event 
     */
    private dispatchBubbleEvent(node: Node, event: Event): void {
        let checkNode: Node = node;
        let breakEvent: boolean = false;
        while (true) {
            if (!checkNode) break;

            checkNode.dispatchEvent(event);
            //@ts-ignore
            if (event._stoped) {
                breakEvent = true;
                break;
            }

            checkNode = checkNode.parent;
        }

        /**
         * InputManager也可以触发事件，这些事件是全局的
         * 相当于舞台事件
         */
        if (!breakEvent && event.type !== EventType.mouseout) {
            this.fire(event.type, event);
        }
    }

    update() {

        /**
         * 说明：
         *  为了支持多点触控， 对于触控\鼠标事件，同一个Touch ID（鼠标其实只有一个Touch ID）可以出发不同的类型的事件，他们共用一个Event实例
         */

        let size = this._datas.getDataSize() - 4;
        if (size > 0) {

            // js to native
            this._datas.pos = 0;
            this._datas.writeInt32(size);

            //C++端处理事件
            this.native.update();

            // C++ 端处理完毕，到JS端处理了
            //native to js native和js端共用一个Buffer来进行数据交互
            this._datas.pos = 0;
            size = this._datas.readInt32();
            if (size > 0) {
                while (this._datas.pos < size) {
                    let id = this._datas.readInt32();
                    let eventType = this._datas.readUint8();
                    let nodeNativeId = this._datas.readInt32();
                    let event: Event = this._nativeEvent[id];

                    // console.info("process",eventType,id);

                    event.worldX = this._datas.readFloat32();
                    event.worldY = this._datas.readFloat32();
                    event.localX = this._datas.readFloat32();
                    event.localY = this._datas.readFloat32();
                    event.type = eventType;

                    let node: Node = dispatch.getJsToNativeObject(nodeNativeId).getCustomData();
                    this.dispatchBubbleEvent(node, event);

                    //@ts-ignore
                    event._stoped = false;
                }
            }

            let keys = Object.keys(this._nativeEvent);
            for (let i = 0, n = keys.length; i < n; ++i) {
                //@ts-ignore
                let event = this._nativeEvent[keys[i]];
                //@ts-ignore
                if (event && event.processRemove) {
                    // if (!event.process) this.fire(event.type, event);
                    recoveryEvent(event);
                    //@ts-ignore
                    this._nativeEvent[keys[i]] = null;
                }
            }

            this._datas.clearData();
            this._datas.writeInt32(0);
            this.eventCount = 0;
        }


        //处理键盘事件
        let n = this._keyboardEvents.length;
        if (n > 0) {
            for (let i = 0; i < n; ++i) {
                let event: Event = this._keyboardEvents[i];
                switch (event.type) {
                    case EventType.keydown:
                        {
                            let keyEvent = event as EventKeyboard;
                            //硬件设备默认一直按下就会一直触发事件的
                            //这里引擎keydown只触发一次，直到keyup取消keydown。
                            if (!this.keyBoardDownCodeMap[keyEvent.originEvent.code]) {
                                this.keyBoardDownCodeMap[keyEvent.originEvent.code] = true;
                                this.fire(EventType.keydown, event);
                            }
                            break;
                        }
                    case EventType.keyup:
                        {
                            let keyEvent = event as EventKeyboard;
                            this.keyBoardDownCodeMap[keyEvent.originEvent.code] = false;
                            this.fire(EventType.keyup, event);
                            break;
                        }
                }
                recoveryEvent(event);
            }
            this._keyboardEvents.length = 0;
        }
    }
}

export default new InputManager();

