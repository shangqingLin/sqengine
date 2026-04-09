import js from "./js";

class TimerHandler {

    public once: boolean = false;
    public delay: number = 0;
    public exeTime: number = 0;
    public useFrame: boolean = false;
    public scope: any = null;
    public fun: Function = null;
    public args: any;
    public key: string;
    public static _pool: Array<TimerHandler> = [];
    clear() {
        this.scope = null;
        this.fun = null;
        this.args = null;
        this.key = null;
    }

    static create(): TimerHandler {
        var handler = TimerHandler._pool.pop();
        if (!handler) {
            handler = new TimerHandler();
        }
        return handler;
    }
}

var scope_id: number = 0;
var method_id: number = 0;



/**
 * 基于帧循环的定时器管理工具
 */

class Timer {
    private _reigster: any = {};// js.createMap();
    private _handlers: Array<TimerHandler> = [];
    private _callLaterArray: Array<TimerHandler> = [];
    private _curretTime: number = 0;
    private _currFrame: number = 0;
    private _clearDelta: number = 0;

    public readonly dt: number = 0;
    private update(dt: number) {
        var self = this;
        //@ts-ignore
        this.dt = dt;

        ++self._currFrame;
        self._curretTime += dt;
        var t, handler;
        for (var i = 0, n = self._handlers.length; i < n; ++i) {
            handler = self._handlers[i];
            if (handler.fun !== null) {
                t = handler.useFrame ? self._currFrame : self._curretTime;
                if (t >= handler.exeTime) {
                    if (handler.once) {

                        //非常恐怖，外部可能在自己的回调函数又再次注册了自己
                        //所以如果你先apply自己，apply内部注册自己，但接下来又clear掉自己
                        //那么你就死定了，因为下面的clear会把你刚注册的也给清掉
                        // handler.fun.apply(handler.scope, handler.args);
                        // self.clear(handler.scope, handler.fun);

                        let fun = handler.fun;
                        let scope = handler.scope;
                        let args = handler.args;
                        this.clear(scope, fun);
                        fun.apply(scope, args);
                    } else {
                        handler.exeTime += handler.delay;
                        handler.fun.apply(handler.scope, handler.args);
                    }
                }
            }
        }

        self._clearDelta += dt;

        if (self._clearDelta >= 500) {
            let n = self._handlers.length - 1;
            for (let i = n; i >= 0; --i) {
                handler = self._handlers[i];
                if (handler.fun === null) {
                    self._handlers.splice(i, 1);
                    TimerHandler._pool.push(handler);
                }
            }
        }
    }

    /**
     * 延迟多久执行，基于帧时间
     * @param delay 毫秒
     * @param fun
     * @param scope
     * @param args
     */
    once(delay: number, scope: object, fun: Function, args?: any) {
        this._create(false, true, delay, scope, fun, args);
    }

    /**
     * 每隔多少毫秒执行一次
     * @param delay 毫秒
     * @param scope 
     * @param fun 
     * @param args 
     */
    loop(delay: number, scope: object, fun: Function, args?: any) {
        this._create(false, false, delay, scope, fun, args);
    }

    /**
     * 
     * @param delay 多少帧
     * @param scope 
     * @param fun 
     * @param args 
     */
    frameOnce(delay: number, scope: object, fun: Function, args?: any) {
        this._create(true, true, delay, scope, fun, args);
    }

    /**
     * 
     * @param delay 多少帧
     * @param scope 
     * @param fun 
     * @param args 
     */
    frameLoop(delay: number, scope: object, fun: Function, args?: any) {
        this._create(true, false, delay, scope, fun, args);
    }

    nextTick(scope: object, fun: Function, args?: any) {
        this._create(true, true, 1, scope, fun, args);
    }

    callLater(scope: object, fun: Function, args?: any) {
        var handler = this._getHandler(scope, fun);
        if (!handler) {
            handler = TimerHandler.create();
            this.register(handler, scope, fun);
            this._callLaterArray.push(handler);
        }
        handler.scope = scope;
        handler.fun = fun;
        handler.args = args;
    }

    private runCallLater() {
        for (var i = 0, n = this._callLaterArray.length; i < n; ++i) {
            var handler = this._callLaterArray[i];
            if (handler.fun !== null) {
                handler.fun.apply(handler.scope, handler.args);
                this.clear(handler.scope, handler.fun);
            }
            TimerHandler._pool.push(handler);
        }
        this._callLaterArray.length = 0;
    }

    clear(scope: object, fun: Function) {
        var handler = this._getHandler(scope, fun);
        if (handler) {
            this._reigster[handler.key] = null;
            handler.clear();
        }
    }

    clearAll(scope: object) {
        for (var i = 0, n = this._handlers.length; i < n; i++) {
            var handler = this._handlers[i];
            if (handler.scope === scope) {
                this.clear(scope, handler.fun);
            }
        }

        for (var i = 0, n = this._callLaterArray.length; i < n; ++i) {
            var handler = this._callLaterArray[i];
            if (handler.scope === scope) {
                this.clear(handler.scope, handler.fun);
            }
        }
    }

    private _create(useFrame: boolean, once: boolean, delay: number, scope: object, fun: Function, args: any) {
        if (!delay) {
            fun.apply(scope, args);
            return null;
        }
        var handler = this._getHandler(scope, fun);
        if (!handler) {
            handler = TimerHandler.create();
            this.register(handler, scope, fun);
            this._handlers.push(handler);
        }

        handler.once = once;
        handler.useFrame = useFrame;
        handler.delay = useFrame ? delay : delay;
        handler.scope = scope;
        handler.fun = fun;
        handler.args = args;
        handler.exeTime = handler.delay + (useFrame ? this._currFrame : this._curretTime);
        return handler;
    }

    private _getHandler(caller: object, method: Function): TimerHandler {
        // @ts-ignore
        var cid = caller ? caller.$_GID || (caller.$_GID = ++scope_id) : 0;
        // @ts-ignore
        var mid = method.$_TID || (method.$_TID = ++method_id);
        return this._reigster[cid + "_" + mid];
    }

    private register(handler: TimerHandler, caller: object, method: Function) {
        // @ts-ignore
        handler.key = caller.$_GID + "_" + method.$_TID;
        this._reigster[handler.key] = handler;
    }

}
export default Timer;
