import Component from "../../../framework/component/Component";
import { ComponentType, dispatch } from "../../../native_binding/index";
import { sqclass, editorMenu, Vec2, serializable, SQBoolean, type, SQInteger, SQFloat } from "../../../core/index";
import PhysicsSystem from "../PhysicsSystem";
import { Serialize, SerializedValue } from "../../../serialization";


export enum RigidBody2DType {
    /**
     * 零质量，零速度，可以手动移动。
     */
    Static = 0,

    /**
     * 零质量，可以被设置速度。
     */
    Kinematic = 1,

    /**
     * 有质量，可以设置速度，力等。
     */
    Dynamic = 2
};

enum RigidbodySleepMode2D {
    NeverSleep = 1,
    StartAsleep = 2,
    StartAwake = 3,
};

/**
 * 2D刚体组件
 */

/* editor:start */
@editorMenu("physics 2D/RigidBody2DComponent")
/* editor:end */
@sqclass("sq.RigidBody2DComponent")
export class RigidBody2DComponent extends Component {

    private mass: number;


    @serializable
    @type(RigidbodySleepMode2D)
    private sleepMode: RigidbodySleepMode2D;

    @serializable
    @type(SQInteger)
    private bodyType: number;


    @serializable
    @type(SQFloat)
    private gravityScale: number

    @serializable
    @type(SQInteger)
    private freeze: number;

    @serializable
    @type(SQBoolean)
    private active: boolean;

    constructor() {
        super();
        if (PhysicsSystem.enabled)
            this.nativeType = ComponentType.RigidBody2DComponent;
    }

    protected override onRemove(): void {
        //Body移除了，那么与这个Body关联的Joint、Collision Shape全部都需要移除


    }

    private deserialize_sleepMode(value: SerializedValue, serialize: Serialize) {
        this.setSleepMode(value);
    }

    public setSleepMode(mode: RigidbodySleepMode2D): void {
        this.nativeBeginOp(1);
        this.nativeWriteOpArg("i8", mode);
        this.nativeEndOp();
    }

    public setBullet(bullet: boolean): void {
        this.nativeBeginOp(2);
        this.nativeWriteOpArg("i8", bullet ? 1 : 0);
        this.nativeEndOp();
    }

    private deserialize_bodyType(value: SerializedValue, serialize: Serialize) {
        this.setType(value);
    }

    public setType(type: RigidBody2DType): void {
        this.nativeBeginOp(3);
        this.nativeWriteOpArg("i8", type);
        this.nativeEndOp();
    }

    public setLinearDamping(damping: number): void {
        this.nativeBeginOp(4);
        this.nativeWriteOpArg("f32", damping);
        this.nativeEndOp();
    }

    public setAngularDamping(damping: number): void {
        this.nativeBeginOp(5);
        this.nativeWriteOpArg("f32", damping);
        this.nativeEndOp();
    }

    public setLinearVelocity(x: number, y: number) {
        this.nativeBeginOp(6);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    public setAngularVelocity(v: number): void {
        this.nativeBeginOp(7);
        this.nativeWriteOpArg("f32", v);
        this.nativeEndOp();
    }

    private deserialize_freeze(value: SerializedValue, serialize: Serialize) {
        if (value !== 0) {
            this.setFixedPosition(!!(value & 1), !!(value & 2));
            if (value & 4) this.setFixedRotation(true);
        }
    }

    public setFixedRotation(b: boolean) {
        this.nativeBeginOp(8);
        this.nativeWriteOpArg("i8", b ? 1 : 0);
        this.nativeEndOp();
    }

    public setFixedPosition(x: boolean, y: boolean) {
        this.nativeBeginOp(22);
        this.nativeWriteOpArg("i8", x ? 1 : 0);
        this.nativeWriteOpArg("i8", y ? 1 : 0);
        this.nativeEndOp();
    }

    public ApplyForce(force: Vec2, point: Vec2, wake: boolean = true): void {
        this.nativeBeginOp(9);
        this.nativeWriteOpArg("f32", force.x);
        this.nativeWriteOpArg("f32", force.y);
        this.nativeWriteOpArg("f32", point.x);
        this.nativeWriteOpArg("f32", point.y);
        this.nativeWriteOpArg("i8", wake ? 1 : 0);
        this.nativeEndOp();
    }

    public ApplyForceToCenter(force: Vec2, wake: boolean = true): void {
        this.nativeBeginOp(10);
        this.nativeWriteOpArg("f32", force.x);
        this.nativeWriteOpArg("f32", force.y);
        this.nativeWriteOpArg("i8", wake ? 1 : 0);
        this.nativeEndOp();
    }

    public ApplyTorque(torque: number, wake: boolean = true): void {
        this.nativeBeginOp(11);
        this.nativeWriteOpArg("f32", torque);
        this.nativeWriteOpArg("i8", wake ? 1 : 0);
        this.nativeEndOp();
    }

    public ApplyLinearImpulse(impulse: Vec2, point: Vec2, wake: boolean = true) {
        this.nativeBeginOp(12);
        this.nativeWriteOpArg("f32", impulse.x);
        this.nativeWriteOpArg("f32", impulse.y);
        this.nativeWriteOpArg("f32", point.x);
        this.nativeWriteOpArg("f32", point.y);
        this.nativeWriteOpArg("i8", wake ? 1 : 0);
        this.nativeEndOp();
    }

    public ApplyLinearImpulseToCenter(impulse: Vec2, wake: boolean = true) {
        this.nativeBeginOp(13);
        this.nativeWriteOpArg("f32", impulse.x);
        this.nativeWriteOpArg("f32", impulse.y);
        this.nativeWriteOpArg("i8", wake ? 1 : 0);
        this.nativeEndOp();
    }

    public ApplyAngularImpulse(impulse: number, wake: boolean = true) {
        this.nativeBeginOp(14);
        this.nativeWriteOpArg("f32", impulse);
        this.nativeWriteOpArg("i8", wake ? 1 : 0);
        this.nativeEndOp();
    }


    private deserialize_gravityScale(value: SerializedValue, serialize: Serialize) {
        if (value !== 1)
            this.SetGravityScale(value);
    }

    public SetGravityScale(scale: number): void {
        this.nativeBeginOp(15);
        this.nativeWriteOpArg("f32", scale);
        this.nativeEndOp();
    }

    public SetActive(active: boolean) {
        if (this.active === active) {
            return;
        }
        this.active = active
        this.nativeBeginOp(16);
        this.nativeWriteOpArg("i8", active ? 1 : 0);
        this.nativeEndOp()
    }

    private deserialize_mass(value: SerializedValue, serialize: Serialize) {
        if (value !== 1) {
            this.setMass(value);
        }
    }

    setMass(mass: number): void {
        if (this.mass !== mass) {
            this.mass = mass;
            this.nativeBeginOp(17);
            this.nativeWriteOpArg("f32", mass);
            this.nativeEndOp()
        }
    }


    setRotationalInertia(v: number): void {
        this.nativeBeginOp(18);
        this.nativeWriteOpArg("f32", v);
        this.nativeEndOp()
    }

    getMass(): number {
        //if(this.mass === undefined){
        this.nativeBeginOpSysc(19);
        this.nativeEndOp();
        let nativeBuffer = dispatch.getNativeToJsByte();
        this.mass = nativeBuffer.nativeByte.readFloat32();
        // }
        return this.mass;
    }

    setEnableContinuous(enable: boolean): void {
        this.nativeBeginOp(20);
        this.nativeWriteOpArg("i8", enable);
        this.nativeEndOp()
    }

    setAwake(b: boolean): void {
        this.nativeBeginOp(21);
        this.nativeWriteOpArg("i8", b);
        this.nativeEndOp()
    }

    /**
    * 当粒子碰撞到Body时，是否应用粒子的压力到Body上
    * 默认为true
    */
    setEnableParticlePressure(b: boolean) {
        this.nativeBeginOp(23);
        this.nativeWriteOpArg("i8", b);
        this.nativeEndOp()
    }

    /**
     * 当粒子碰撞到Body时，是否应用阻力到Body上
     * 默认为true
     */
    setEnablePartcileDamping(b: boolean): void {
        this.nativeBeginOp(24);
        this.nativeWriteOpArg("i8", b);
        this.nativeEndOp()
    }
}
