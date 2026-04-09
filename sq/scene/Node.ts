/* debug:start */
import GameStat from "../profiler/GameStat";
/* debug:end */

import { editableProp, EventDispatch, SQBoolean, serializable, sqclass, SQString, type, js, EditablePropConfig, findClassSlash } from "../core/index";

//不要直接导入，避免循环引用
import type Component from "../framework/component/Component";
import type { Transform2DComponent } from "../2d/index";
import { Event, EventType } from "../input/Events";
import { dispatch, JsToNativeObjectBridge, ObjectType } from "../native_binding/index";
import { Layers } from "./Layers";
import NodeEventType from "./NodeEventType";

//不能直接导入，避免循环错误
import { Deserialize, SerializedObjectReference, SerializedTypedObject, SerializedValue } from "../serialization/index";
import { bpClass } from "../blueprint/index";
import { getComponentKey, setComponentKey } from "./componentKey";
import NodeManager from "./NodeManager";


@sqclass("sq.Node")
@bpClass
export default class Node extends EventDispatch {


    @type(function () {
        return js.getClassByName("sq.Component");
    }, true)
    @serializable
    private components: Array<Component>;

    private componentMap: { [key: string]: Component };

    @type(SQBoolean)
    @serializable
    private _visible: boolean = true;

    /**
     * 使用哪个Camera
     */
    private _layer: number = Layers.INHERIT;

    protected _activeInHierarchy: boolean = false;

    protected nativeObject: JsToNativeObjectBridge;

    @type(Node, true)
    @serializable
    protected _children: Array<Node>;

    @type(Node)
    @serializable
    protected _parent: Node = null;

    /* editor:start */
    @type(SQString)
    @serializable
    @editableProp(0)
    public name: string;
    /* editor:end */


    // private initActive: boolean;

    public transform: Transform2DComponent;

    constructor(name?: string) {
        super();
        if (name) {
            this.name = name;
        }
        this.createNativeObj();

        //不用导入Transform2D，避免循环引用
        let transform2DComponentCls = js.getClassByName("sq.Transform2DComponent");
        //@ts-ignore
        this.transform = this.addComponent(transform2DComponentCls);

        /* debug:start */
        ++GameStat.nodeNum;
        /* debug:end */
    }

    protected createNativeObj(): void {
        this.nativeObject = dispatch.createJsToNativeObject(ObjectType.Node, this);
        this.nativeObject.beginOp(1);
        this.nativeObject.endOp();
    };

    get id(): number {
        return this.nativeObject.getId();
    }

    addChild(node: Node) {

        if (node === this) {
            /* debug:start */
            console.error("不能添加自己为子节点");
            /* debug:end */
            return;
        }

        if (node._parent) {
            node._parent.removeChild(node);
        }
        node._parent = this;
        if (!this._children) {
            this._children = [];
        }
        this._children.push(node);
        this.nativeObject.beginOp(2, 0, true);
        this.nativeObject.writeOpArg("i32", node.nativeObject.getId());
        this.nativeObject.endOp();

        // console.info("js addChild parent:",this.getId(),"child：",node.getId());

        node._onHierarchyChanged();
        node.fire(NodeEventType.NODE_ADD_PARENT);
    }

    addChildAt(node: Node, insertIndex: number): void {
        if (node._parent === this) {
            let index = this._children.indexOf(node);
            if (index === insertIndex) return;
        }

        if (node._parent) {
            let index = node._parent._children.indexOf(node);
            node._parent._children.splice(index, 1);
        }
        if (!this._children) {
            this._children = [];
        }
        node._parent = this;
        this._children.splice(insertIndex, 0, node);

        this.nativeObject.beginOp(14, 0, true);
        this.nativeObject.writeOpArg("i32", node.nativeObject.getId());
        this.nativeObject.writeOpArg("i32", insertIndex);
        this.nativeObject.endOp();

        node._onHierarchyChanged();
        node.fire(NodeEventType.NODE_ADD_PARENT);
    }

    /**
     * 设置当前节点在父节点中位置
     * @param index 
     */
    setSiblingIndex(index: number): void {
        if (this._parent) {
            this._parent.addChildAt(this, index);
        }
    }

    private addChild2(node: Node) {
        this.nativeObject.beginOp(2, 0, true);
        this.nativeObject.writeOpArg("i32", node.nativeObject.getId());
        this.nativeObject.endOp();
        node._parent = this;
        node._onHierarchyChanged();
    }

    removeChild(child: Node) {
        var index = this._children.indexOf(child);
        if (index !== -1) {
            this._children.splice(index, 1);
            child._parent = null;
            this.nativeObject.beginOp(3, 0, true);
            this.nativeObject.writeOpArg("i32", child.nativeObject.getId());
            this.nativeObject.endOp();
            child.fire(NodeEventType.NODE_REMOVE_PARENT);
        }
    }

    removeFromParent() {
        if (this.parent) {
            this.parent.removeChild(this);
        }
    }

    getChild(index: number) {
        return this._children[index];
    }

    containChild(child: Node) {
        return this._children.indexOf(child) !== -1;
    }

    getChildIndex(child: Node) {
        return this._children.indexOf(child);
    }

    protected _onHierarchyChanged() {

        // if (this.name === "flame_0") {
        //     console.info("++++");
        // }

        let shouldActiveNow: boolean = !!(this._parent && this._parent._activeInHierarchy);
        if (this._activeInHierarchy != shouldActiveNow) {
            this._activeInHierarchy = shouldActiveNow;
            this.setEnabled(shouldActiveNow);

            if (this._children) {
                for (let j = 0, n = this._children.length; j < n; ++j) {
                    this._children[j]._onHierarchyChanged();
                }
            }
        }
    }

    get activeInHierarchy(): boolean {
        return this._activeInHierarchy;
    }

    protected setEnabled(enabled: boolean): void {
        if (this.components) {
            for (let i = 0, n = this.components.length; i < n; ++i) {
                let component = this.components[i];
                if (enabled) {
                    //@ts-ignore
                    component.onEnabled();
                } else {
                    //@ts-ignore
                    component.onDisabled();
                }
            }
        }

        // if (!this.initActive && enabled) {
        //     this.initActive = true;
        //     //首次被激活的时候调用。
        //     this.fire("onInitActive");
        // }
    }


    findChildByName(name: string): Array<Node> {
        let result: Array<Node> = [];
        this.walk((node: Node) => {
            if (node.name === name) {
                result.push(node);
            }
            return true;
        });
        return result;
    }



    walk(preFun: (node: Node) => boolean) {
        let nodeStack: Array<Node> = [];
        nodeStack[0] = this;

        let i: number = 0;
        let cur: Node = null;
        let size: number = 0;

        //使用传统的递归方式会因为方法调用太深和栈内存占用太多造成性能问题，
        //所以使用while循环的方式实现递归
        while (i >= 0) {
            cur = nodeStack[i--];
            let next: boolean = preFun(cur);

            if (!next) {
                break;
            }

            if (cur.children) {
                let children: Readonly<Array<Node>> = cur.children;
                size = children.length;
                if (size > 0) {
                    for (let j = 0; j < size; j++) {
                        nodeStack[++i] = children[j];
                    }
                }
            }
        }
    }

    get children(): Readonly<Array<Node>> {
        return this._children;
    }

    get parent(): Node {
        return this._parent;
    }

    addComponent<T extends Component>(componentCls: Constructor<T>): T {
        if (!this.componentMap) {
            this.componentMap = js.createMap();
        }

        let key: number = setComponentKey(componentCls);
        if (this.componentMap[key]) return this.componentMap[key] as T;

        if (!this.components) {
            this.components = [];
        }

        var instance = new componentCls();
        this.componentMap[key] = instance;
        this.components.push(instance);

        //@ts-ignore
        if (instance.nativeType) {

            this.nativeObject.beginOp(4, 0, true);
            // this.nativeObject.beginOpSync(4,0);
            //@ts-ignore
            this.nativeObject.writeOpArg("i8", instance.nativeType);
            this.nativeObject.endOp();
            // dispatch.flushJsToNative();
        }

        //@ts-ignore
        instance.node = this;

        //@ts-ignore
        instance.onInitialize();

        if (this._activeInHierarchy) {
            //@ts-ignore
            instance.onEnabled();
        }

        this.fire(NodeEventType.ADD_COMPONENT, instance);
        return instance;
    }



    getCompoentInherit<T extends Component>(componentCls: Constructor<T>) {
        let names = Object.keys(this.componentMap);
        for (let i = 0, n = names.length; i < n; ++i) {
            let component = this.componentMap[names[i]];
            if (component instanceof componentCls) {
                return component;
            }
        }
        return null;
    }

    getComponent<T extends Component>(componentCls: Constructor<T>): T {
        let key: number = getComponentKey(componentCls);
        return this.componentMap[key] as T;
    }

    getComponentByKey<T extends Component>(key: number): T {
        return this.componentMap[key] as T;
    }

    getAllComponent(): Array<Component> {
        return this.components;
    }

    hasComponent<T extends Component>(componentCls: Constructor<T>): boolean {
        let key: number = getComponentKey(componentCls);
        return !!this.componentMap[key];
    }

    hasComponentInherit<T extends Component>(componentCls: Constructor<T>): boolean {
        let names = Object.keys(this.componentMap);
        for (let i = 0, n = names.length; i < n; ++i) {
            let component = this.componentMap[names[i]];
            if (component instanceof componentCls) {
                return true;
            }
        }
        return false;
    }

    removeComponent<T extends Component>(componentCls: Constructor<T>) {
        let key: number = getComponentKey(componentCls);
        var component = this.componentMap[key];
        if (component) {
            delete this.componentMap[key];
            this.removeComponent2(component);
        }
    }

    removeComponentByInstance(component: Component) {
        let keys = Object.keys(this.componentMap);
        for (let i = 0, n = keys.length; i < n; ++i) {
            let cc = this.componentMap[keys[i]];
            if (cc === component) {
                this.removeComponent2(cc);
                delete this.componentMap[keys[i]];
                break;
            }
        }
    }

    private removeComponent2(component: Component) {
        let index = this.components.indexOf(component);
        this.components.splice(index, 1);

        //@ts-ignore
        if (component.nativeType) {
            this.nativeObject.beginOp(5, 0, true);
            //@ts-ignore
            this.nativeObject.writeOpArg("i8", component.nativeType);
            this.nativeObject.endOp();
        }
        //@ts-ignore
        component.onRemove();
        this.fire(NodeEventType.REMOVE_COMPONENT, component);
    }

    getChildHasComponent<T extends Component>(componentCls: Constructor<T>, first: boolean = true): Array<Node> | Node | null {
        let result: any = null;
        this.walk((node: Node) => {
            let c = (node as Node).getComponent(componentCls);
            if (c) {
                if (first) {
                    result = node;
                    return false;
                } else {
                    if (!result) result = [];
                    result.push(node);
                }
            }
            return true;
        });
        return result;
    }

    set visible(visible) {
        if (visible !== this._visible) {
            this._visible = visible;
            this.nativeObject.beginOp(6);
            this.nativeObject.writeOpArg("i8", visible ? 1 : 0);
            this.nativeObject.endOp();
        }
    }

    get visible() {
        return this._visible;
    }

    @type(Layers)
    @serializable
    /* editor:start */
    @editableProp(1, {
        enumConfig: {
            multiple: true
        }
    } as EditablePropConfig)
    /* editor:end */
    set layer(layer: Layers) {
        if (this._layer !== layer) {
            this._layer = layer;
            this.nativeObject.beginOp(7);
            this.nativeObject.writeOpArg("i32", layer);
            this.nativeObject.endOp();

            this.fire(NodeEventType.LAYER_CHANGE);
        }
    }

    get layer(): Layers {
        if (this._layer === Layers.INHERIT) {
            return this.parent ? this.parent.layer : this._layer;
        }
        return this._layer;
    }

    override on(name: string | number, scope: object, callback: Function, params?: any): EventDispatch {
        if (!this.hasEvent(name)) {
            this.onProcess(name as NodeEventType);
        }
        return super.on(name, scope, callback, params);
    }

    override once(name: string | number, scope: object, callback: Function, params?: any): EventDispatch {
        if (!this.hasEvent(name)) {
            this.onProcess(name as NodeEventType);
        }
        return super.once(name, scope, callback, params);
    }


    override off(name: string | number, scope: object, callback: Function): EventDispatch {
        super.off(name, scope, callback);

        //可能在fire中的回调函数中off自己，当是fire函数已经写了处理
        //offsetProcess的逻辑，为了避免重复处理，这里加了lock判断
        if (this.locakFireEvent !== name)
            if (!this.hasEvent(name)) {
                this.offProcess(name as NodeEventType);
            }
        return this;
    }

    override fire(name: string | number, ...arg: any): EventDispatch {
        let b = this.hasEvent(name);
        super.fire(name, ...arg);

        //可能在fire的事件函数中destroy当前的Node
        //造成this.nativeObject为NULL    
        if (this.nativeObject && b && !this.hasEvent(name)) {
            this.offProcess(name as NodeEventType);
        }
        return this;
    }

    override offAll(scope?: object): EventDispatch {
        super.offAll(scope);
        if (this._callbacks) {
            let names = Object.keys(this._callbacks);
            for (let i = 0, n = names.length; i < n; ++i) {
                let funcs = this._callbacks[names[i]];
                if (!funcs || funcs.length === 0) {
                    if (!this.hasEvent(names[i])) {
                        this.offProcess(Number(names[i]));
                    }
                }
            }
        }
        return this;
    }

    private onProcess(name: NodeEventType): void {
        if (name === NodeEventType.TRASNFORM_CHANGE || isMouseEvent(name)) {
            this.nativeObject.beginOp(11, 0, true);
            this.nativeObject.writeOpArg("i32", name);
            this.nativeObject.endOp();
        }
    }

    private offProcess(name: NodeEventType): void {
        //还有事件，那么就不要通知C++端移除了
        if (this.hasEvent(name)) return;
        if (name === NodeEventType.TRASNFORM_CHANGE || isMouseEvent(name)) {
            //this.nativeObject 没有表示当前正在执行Destroy操作，就不需要在告诉C++端了，C++端在Destroy自动off所有
            if (this.nativeObject) {
                this.nativeObject.beginOp(12, 0, true);
                this.nativeObject.writeOpArg("i32", name);
                this.nativeObject.endOp();
            }
        }
    }


    dispatchEvent(event: Event): void {
        event.target = this;
        switch (event.type) {
            case EventType.mouseclick:
                this.fire(NodeEventType.MOUSE_CLICK, event);
                break;
            case EventType.mousedown:
                this.fire(NodeEventType.MOUSE_DOWN, event);
                break;
            case EventType.mousemove:
                this.fire(NodeEventType.MOUSE_MOVE, event);
                break;
            case EventType.mouseup:
                this.fire(NodeEventType.MOUSE_UP, event);
                break;
            case EventType.mouserightdown:
                this.fire(NodeEventType.MOUSE_RIGHT_DOWN, event);
                break;
            case EventType.mouserightup:
                this.fire(NodeEventType.MOUSE_RIGHT_UP, event);
                break;
            case EventType.mouseout:
                this.fire(NodeEventType.MOUSE_OUT, event);
                break;
            case EventType.mouseover:
                this.fire(NodeEventType.MOUSE_OVER, event);
                break;
        }
    }


    /**
     * C++底层已经从当前的destroy节点开始往下destroy了
     * 子孙节点不需要再beginOp等操作了
     */
    private _destroyInner() {

        {
            let i = 0, component;
            let keys = Object.keys(this.componentMap);
            if (keys.length > 0) {
                for (i = 0; i < keys.length; ++i) {
                    component = this.componentMap[keys[i]];

                    //有些组件中的component.onRemove()可能移除了其他的组件
                    //所以这里取到可能为空
                    if (component) {
                        //@ts-ignore
                        component.onRemove();
                    }
                }
            }
            this.componentMap = null;
        }

        if (this.children) {
            let ccc = this.children;
            let length = ccc.length, node: Node;
            for (let i = 0; i < length; ++i) {
                node = ccc[i];

                //避免触发事件，以为Component remove会remove
                node.nativeObject = null;
                node._destroyInner();
            }
            this._children = null;
        }
    }

    /**
     * NodeMangaer中调用
     * @returns 
     */
    private destroy2(): void {

        //表示已经destroy了
        if (!this.nativeObject) return;

        /* debug:start */
        ++GameStat.nodeNum;
        /* debug:end */

        this.fire(NodeEventType.DESTROY);
        this.nativeObject.beginOp(100);
        this.nativeObject.endOp();
        this.nativeObject.destroy();
        this.nativeObject = null;

        if (this._parent) {
            let index = this._parent.children.indexOf(this);
            (this._parent.children as Array<Node>).splice(index, 1);
            this._parent = null;
        }

        this._destroyInner();
    }

    public destroy() {
        NodeManager.addNodeDestroy(this);
    }

    /* editor:start */
    // protected serialize__children(serialize: Serialize) {
    //     let result: Array<any> = null;
    //     if (this._children) {
    //         result = new Array(this._children.length);
    //         for (let i = 0, n = this._children.length; i < n; ++i) {
    //             if (this._children[i].prefab) {
    //                 //表示为预制体的根节点，这个节点不会序列化，使用PrefInfo替代记录序列化信息
    //                 result[i] = serialize.serializeValue(this._children[i].prefab);
    //             } else {
    //                 result[i] = serialize.serializeValue(this._children[i]);
    //             }
    //         }
    //     }
    //     return result;
    // }
    /* editor:end */

    private deserialize__children(sValue: any, deserialize: Deserialize) {
        this._children = [];
        let prefabInfoCls: any = js.getClassByName("sq.PrefadInstnace");
        for (let i = 0, n = sValue.length; i < n; ++i) {
            let obj = deserialize.deserializeObject(sValue[i]);
            if (obj instanceof prefabInfoCls) {
                obj.setRoot(this, i);
            } else {
                this._children.push(obj);
                this.addChild2(obj);
            }
        }
    }


    private deserialize_components(sValue: Array<SerializedValue>, deserialize: Deserialize) {
        /**
         * 必须保证在初始化Component属性之前,node属性已经设置到Component上
         * 因为有很多组件的属性初始化过程会用到node
         */
        let serializeComponents = [];
        if (this.components) {
            //可能组件在序列化之前就创建了，比如说TransformComponent

            for (let i = 0; i < sValue.length; ++i) {
                serializeComponents[i] = null;
                let d = deserialize.getSerializeDataByIndex((sValue[i] as SerializedObjectReference).__id__) as SerializedTypedObject;
                for (let n = 0; n < this.components.length; ++n) {
                    let classSlash = findClassSlash(this.components[n]);
                    if (classSlash && classSlash.clsName == d.__type__) {
                        serializeComponents[i] = this.components[n];
                        break;
                    }
                }
            }
        }


        let sComponent = [];
        for (let i = 0; i < sValue.length; ++i) {
            let d = deserialize.getSerializeDataByIndex((sValue[i] as SerializedObjectReference).__id__) as SerializedTypedObject;
            let contructor: Constructor = js.getClassByName(d.__type__);

            if (!serializeComponents[i]) {
                // if (!contructor) {
                //     console.info("????????????")
                // }
                let component = new contructor() as Component;
                serializeComponents[i] = component;
                sComponent.push(component);
                //@ts-ignore
                component.node = this;
                this.addComponent2(component);
            }
        }

        deserialize.deserializeArray(sValue, serializeComponents) as Array<Component>;

        for (let i = 0; i < sComponent.length; ++i) {
            //@ts-ignore
            sComponent[i].onInitialize();

            if (this._activeInHierarchy) {
                //@ts-ignore
                sComponent[i].onEnabled();
            }
        }
    }

    private addComponent2(component: Component) {
        let componentCls: Constructor = js.getInstanceConstructor(component);
        let key: number = setComponentKey(componentCls);
        if (this.componentMap[key]) return;

        this.componentMap[key] = component;
        if (!this.components) {
            this.components = [];
        }
        this.components.push(component);

        //@ts-ignore
        component.node = this;
        //@ts-ignore
        if (component.nativeType) {
            this.nativeObject.beginOp(4, 0, true);
            // this.nativeObject.beginOpSync(4,0);
            //@ts-ignore
            this.nativeObject.writeOpArg("i8", component.nativeType);
            this.nativeObject.endOp();
            // dispatch.flushJsToNative();
        }
    }

    // setTestNum(t:number) {
    //     this.nativeObject.beginOp(101);
    //     this.nativeObject.writeOpArg("i32",t);
    //     this.nativeObject.endOp();
    // }
}

function isMouseEvent(name: NodeEventType): boolean {
    return name === NodeEventType.MOUSE_CLICK ||
        name === NodeEventType.MOUSE_DOWN ||
        name === NodeEventType.MOUSE_UP ||
        name === NodeEventType.MOUSE_MOVE ||
        name === NodeEventType.MOUSE_OUT ||
        name === NodeEventType.MOUSE_OVER ||
        name === NodeEventType.MOUSE_RIGHT_DOWN ||
        name === NodeEventType.MOUSE_RIGHT_UP
}
