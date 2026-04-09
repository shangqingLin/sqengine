import CommonUtils from "./CommonUtils";

interface EventDispatchRegisterData {
    callback: Function;
    scope: any;
    params: any;
    once?: boolean;
}

export default class EventDispatch {

    protected _callbacks: { [key: string | number]: Array<EventDispatchRegisterData> };
    protected readonly lock: boolean = false;
    protected readonly locakFireEvent: string | number = null;
    private hasDestroyInLock: boolean = false;

    //专门用于触发蓝图事件的回调，在BlueprintComponent组件中注册这个
    private onBlueprintEventHandler: (eventName: string | number, ...arg: any) => void

    /**
     * 注册事件
     * @param name
     * @param scope
     * @param callback
     * @param params 事件触发的时候，需要传递的参数，位于fire的最前面传递进来
     */
    on(name: string | number, scope: any, callback: Function, params?: any): EventDispatch {
        if (!CommonUtils.isDefine(name) || !callback)
            throw new Error();

        if (!this._callbacks) {
            this._callbacks = Object.create(null);
        }

        if (!this._callbacks[name])
            this._callbacks[name] = [];


        var callbacks: Array<EventDispatchRegisterData> = this._callbacks[name];
        var callbackData;
        var scope = scope || this;

        //不能重复添加scope和callback一样的事件
        if (callbacks) {
            for (var i = 0; i < callbacks.length; ++i) {
                var data = callbacks[i];
                if (callback === data.callback && scope === data.scope) {
                    callbackData = data;
                    break;
                }
            }
        }

        if (params !== undefined) {
            if (!(params instanceof Array)) {
                params = [params];
            }
        }

        if (callbackData) {
            callbackData.params = params;
        } else {
            this._callbacks[name].push({
                callback: callback,
                scope: scope,
                params: params
            });
        }
        return this;
    }

    /**
     * 没有指定scope表示移除所有的事件
     * @param scope 
     * @returns 
     */
    offAll(scope?: object): EventDispatch {
        if (this._callbacks) {
            let names: Array<string> = Object.keys(this._callbacks);
            let callbacks: Array<EventDispatchRegisterData>;
            for (let i = 0, mm = names.length; i < mm; ++i) {
                callbacks = this._callbacks[names[i]];
                if (scope) {
                    for (let n = callbacks.length - 1; n >= 0; --n) {
                        if (callbacks[n].scope === scope) {
                            if (this.lock) {
                                callbacks[n].callback = null;
                                callbacks[n].scope = null;
                                this.hasDestroyInLock = true;
                            } else {
                                callbacks.splice(n, 1);
                            }
                        }
                    }
                } else {
                    this._callbacks[names[i]] = null;
                }
            }
        }
        return this;
    }

    off(name: string | number, scope: object, callback: Function): EventDispatch {

        if (!this._callbacks)
            return this;

        if (!callback)
            return this;

        var events = this._callbacks[name];
        if (!events)
            return this;

        for (let i = events.length - 1; i >= 0; --i) {
            if (events[i].callback === callback && events[i].scope === scope) {
                if (this.lock) {
                    events[i].callback = null;
                    events[i].scope = null;
                    this.hasDestroyInLock = true;
                } else {
                    events.splice(i, 1);
                }
                break;
            }
        }
        return this;
    }

    fire(name: string | number, ...arg: any): any {

        if (this.onBlueprintEventHandler) {
            this.onBlueprintEventHandler(name, ...arg);
        }

        if (!CommonUtils.isDefine(name) || !this._callbacks || !this._callbacks[name])
            return this;

        var callbacks: Array<EventDispatchRegisterData> = this._callbacks[name];
        var params = [];
        let returnParam;

        //这里调用了外面的逻辑，外面的逻辑可能对当前的事件对象进行off\on操作
        //所以callbacks数组会发生改变的，我们要处理这种情况
        //@ts-ignore
        this.lock = true;
        //@ts-ignore
        this.locakFireEvent = name;

        for (let i = 0, n = callbacks.length; i < n; ++i) {
            let evt = callbacks[i];

            if (!evt.callback) continue;

            if (arg.length > 0 && evt.params !== undefined) {
                params = arg.concat(evt.params);
            } else if (arg.length > 0) {
                params = arg;
            } else if (evt.params !== undefined) {
                params = evt.params;
            }
            returnParam = evt.callback.apply(evt.scope, params);
            if (evt.once) {
                evt.callback = null;
                evt.scope = null;
                this.hasDestroyInLock = true;
            }
        }

        //@ts-ignore
        this.lock = false;
        //@ts-ignore
        this.locakFireEvent = null;
        this.clearInLock();
        return returnParam;
    }

    private clearInLock() {
        if (this.hasDestroyInLock) {
            this.hasDestroyInLock = false;
            if (this._callbacks) {
                let names: Array<string> = Object.keys(this._callbacks);
                let callbacks: Array<EventDispatchRegisterData>;
                for (let i = 0, mm = names.length; i < mm; ++i) {
                    callbacks = this._callbacks[names[i]];

                    if (callbacks)
                        for (let n = callbacks.length - 1; n >= 0; --n) {
                            if (!callbacks[n].callback) {
                                callbacks.splice(n, 1);
                            }
                        }
                }
            }
        }
    }

    once(name: string | number, scope: object, callback: Function, params?: any): EventDispatch {
        this.on(name, scope, callback, params);
        var callbacks = this._callbacks[name];
        var lastOne = callbacks[callbacks.length - 1];
        lastOne.once = true;
        return this;
    }

    hasEvent(name: string | number, scope?: any, callback?: Function): boolean {
        let callbacks = this._callbacks && this._callbacks[name];
        if (callbacks && callbacks.length > 0) {
            if (scope && callback) {
                for (let i = 0; i < callbacks.length; ++i) {
                    let data = callbacks[i];
                    if (callback === data.callback && scope === data.scope) {
                        return true;
                    }
                }
            } else {
                //上面的off不会立刻从_callbacks中移除的，而是清空callback和scaop而已
                //所以要判断callback和scope为空

                let callbacks = this._callbacks[name];
                for (let i = 0; i < callbacks.length; ++i) {
                    let data = callbacks[i];
                    if (data.callback && data.scope) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
