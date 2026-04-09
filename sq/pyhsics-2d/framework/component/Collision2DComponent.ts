import { LayerMask, Layers } from "../../../scene/Layers";
import { serializable, sqclass, Vec2, type, editableProp, SQFloat, SQBoolean, SQInteger, js } from "../../../core/index";
import { ContactManifold, SqHexColor } from "../define";
import PhysicsSystem from "../PhysicsSystem";
import { Component } from "../../../framework";
import type Node from "../../../scene/Node";
import { getComponentKey } from "../../../scene/componentKey";

@sqclass("sq.Collision2DComponent")
export default class Collision2DComponent extends Component {
    private _offset: Vec2;
    private _density: number;
    private _friction: number;
    private _restitution: number;
    private _isSensor: boolean;
    private _tangentSpeed: number;

    private contanctListenerEnable: boolean;

    @serializable
    @type(SQInteger)
    private mask: number;

    @serializable
    @type(SQInteger)
    private category: number;

    private beginContactHandler: (contact: ContactManifold) => void;
    private endContactHandler: (collisionNode:Node,contactId:number) => void;
    private updateContactHandler: (contact: ContactManifold) => void;
    private sensorBeginHandler: (node: Node) => void;
    private sensorEndHandler: (node: Node) => void;
    private particleBeginContactHandler: (contactNode: Node, particleId: number) => void;
    private particleEndContactHandler: (contactNode: Node, particleId: number) => void;

    constructor(nativeType: number) {
        super();
        if (this.physicsEnable())
            this.nativeType = nativeType;
    }


    /**
     * 判断物理系统是否开启了
     */
    protected physicsEnable(): boolean {
        return PhysicsSystem.enabled;
    }

    /**
       *  在PhysicsBridge中调用
       * @param contact 
       */
    private onTriggerBeginContact(contact: ContactManifold) {
        if (this.beginContactHandler) this.beginContactHandler(contact);
    }

    /**
    * 在PhysicsBridge中调用
    * @param contact 
    */
    private onTriggerUpdateContact(contact: ContactManifold) {
        if (this.updateContactHandler) {
            this.updateContactHandler(contact);
        }
    }

    /**
     * 在PhysicsBridge中调用
     * @param contact 
     */
    private onTriggerEndContact(collisionNode:Node,contactId:number) {
        if (this.endContactHandler) this.endContactHandler(collisionNode,contactId);
    }

    /**
    * 在PhysicsBridge中调用
    * @param contact 
    */
    private onTriggerSensorBegin(node: Node) {
        if (this.sensorBeginHandler) this.sensorBeginHandler(node);
    }

    /**
    * 在PhysicsBridge中调用
    * @param contact 
    */
    private onTriggerSensorEnd(node: Node) {
        if (this.sensorEndHandler) this.sensorEndHandler(node);
    }

    set onBeginContactHandler(callback: (contact: ContactManifold) => void) {
        this.contantEnable(!!callback || !!this.endContactHandler);
        this.beginContactHandler = callback;
    }

    set onEndContactHandler(callback: (collisionNode:Node,contactId:number) => void) {
        this.contantEnable(!!callback || !!this.beginContactHandler);
        this.endContactHandler = callback;
    }

    set onParticleBeginContactHandler(callback: (contactNode: Node, particleId: number) => void) {
        if (this.particleBeginContactHandler === callback) return;
        this.particleBeginContactHandler = callback;
        this.onWriteHandler(!!callback, 4);
    }

    set onParticleEndContactHandler(callback: (node: Node, particleId: number) => void) {
        if (this.particleBeginContactHandler === callback) return;
        this.particleBeginContactHandler = callback;
        this.onWriteHandler(!!callback, 5);
    }

    private contantEnable(b: boolean) {
        if (b) {
            if (!this.contanctListenerEnable) {
                this.contanctListenerEnable = true;
                this.onWriteHandler(true, 1);
            }
        } else {
            if (this.contanctListenerEnable) {
                this.contanctListenerEnable = false;
                this.onWriteHandler(false, 1);
            }
        }
    }

    set onUpdateContactHandler(callback: (contact: ContactManifold) => void) {
        this.updateContactHandler = callback;
    }

    set onSensorBeginHandler(callback: (node: Node) => void) {
        if (callback) {
            if (!this.sensorBeginHandler) this.onWriteHandler(true, 2);
        } else {
            if (this.sensorBeginHandler) this.onWriteHandler(false, 2);
        }
        this.sensorBeginHandler = callback;
    }

    set onSensorEndHandler(callback: (node: Node) => void) {
        if (callback) {
            if (!this.sensorEndHandler) this.onWriteHandler(true, 3);
        } else {
            if (this.sensorEndHandler)
                this.onWriteHandler(false, 3);
        }
        this.sensorEndHandler = callback;
    }

    private onWriteHandler(b: boolean, type: number): void {
        this.nativeBeginOp(3, true);
        this.nativeWriteOpArg("i8", b ? 1 : 0);
        this.nativeWriteOpArg("i8", type);
        if (b) {
            let cls = js.getInstanceConstructor(this);
            this.nativeWriteOpArg("i32", getComponentKey(cls));
        }
        this.nativeEndOp();
    }


    /**
     *相对于当前节点的位置的偏移
     * offset必须在创建Shape之前设置
     * 也就是为Shape设置坐标点之前设置，否则offset不会生效
     */

    /* editor:start */
    @editableProp(0)
    /* editor:end */
    @type(Vec2)
    @serializable
    set offset(vec: Vec2) {

        /* editor:start */
        if (!this._offset) {
            this._offset = new Vec2();
        }
        this._offset.copy(vec);
        /* editor:end */

        if (this.physicsEnable()) {
            this.nativeBeginOp(4);
            this.nativeWriteOpArg("f32", vec.x);
            this.nativeWriteOpArg("f32", vec.y);
            this.nativeEndOp();
        }
    }

    get offset() {
        return this._offset;
    }


    /**
     * 默认为1
     */
    @type(SQFloat)
    @serializable
    /* editor:start */
    @editableProp(1)
    /* editor:end */
    set density(density: number) {
        if (this._density !== density) {
            this._density = density;
            this.nativeBeginOp(5);
            this.nativeWriteOpArg("f32", density);
            this.nativeEndOp();
        }
    }

    get density(): number {
        return this._density || 1;
    }

    @type(SQFloat)
    @serializable
    /* editor:start */
    @editableProp(1)
    /* editor:end */
    set friction(friction: number) {
        if (this._friction !== friction) {
            this._friction = friction;
            this.nativeBeginOp(6);
            this.nativeWriteOpArg("f32", friction);
            this.nativeEndOp();
        }
    }

    get friction() {
        return this._friction || 0;
    }

    @type(SQFloat)
    @serializable
    /* editor:start */
    @editableProp(1)
    /* editor:end */
    set restitution(restitution: number) {
        if (this._restitution !== restitution) {
            this._restitution = restitution;
            this.nativeBeginOp(7);
            this.nativeWriteOpArg("f32", restitution);
            this.nativeEndOp();
        }
    }

    get restitution() {
        return this._restitution || 0;
    }

    @type(SQBoolean)
    @serializable
    /* editor:start */
    @editableProp(1)
    /* editor:end */
    set isSensor(sensor: boolean) {
        if ((this._isSensor === undefined && this._isSensor !== false) && (this._isSensor !== sensor)) {
            this._isSensor = sensor;
            this.nativeBeginOp(8);
            this.nativeWriteOpArg("i8", sensor ? 1 : 0);
            this.nativeEndOp();
        }
    }

    get isSensor(): boolean {
        return this._isSensor || false;
    }

    set sensorAABB(b: boolean) {
        this.nativeBeginOp(11);
        this.nativeWriteOpArg("i8", b ? 1 : 0);
        this.nativeEndOp();
    }

    set tangentSpeed(speed: number) {
        if (this._tangentSpeed !== speed) {
            this._tangentSpeed = speed;
            this.nativeBeginOp(12);
            this.nativeWriteOpArg("f32", speed);
            this.nativeEndOp();
        }
    }

    get tangentSpeed(): number {
        return this._tangentSpeed || 0;
    }

    protected afterDeserialize() {
        if (this.category !== undefined && this.mask !== undefined) {
            this.setFilter(this.category, this.mask);
        }
    }

    setFilter(categoryBits: LayerMask, maskBits: LayerMask) {
        // console.info("??????????????",this,categoryBits,maskBits);
        this.mask = maskBits;
        this.nativeBeginOp(9);
        this.nativeWriteOpArg("ui32", categoryBits);
        this.nativeWriteOpArg("ui32", maskBits);
        this.nativeEndOp();
    }
    
    get maskBits() {
        return this.mask;
    }

    set customColor(color: SqHexColor) {
        this.nativeBeginOp(10);
        this.nativeWriteOpArg("ui32", color);
        this.nativeEndOp();
    }
}