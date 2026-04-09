import { ComponentType } from "../../../native_binding/index";
import { Component } from "../../../framework/index";
import { Vec2, js } from "../../../core/index";
import type Node from "../../../scene/Node";

export enum JointType {
    DISTANCE,
    MOTOR,
    MOUSE,
    WELD,
    WHEEL,
    PRISMATIC,
    REVOLUTE
};

export interface BaseJointDef {
    collideConnected?: boolean;
    localAnchorA?: Vec2;
    localAnchorB?: Vec2;
    localARotate?: number;
    localBRotate?: number;
};

/**
 * 旋转关节Define
 */
export interface RevoluteJointDef extends BaseJointDef {


    localAnchorAPivot?: Vec2;
    localAnchorBPivot?: Vec2;
    referenceAngle?: number;
    enableSpring?: boolean;
    hertz?: number;
    dampingRatio?: number;
    enableLimit?: boolean;
    lowerAngle?: number;
    upperAngle?: number;
    enableMotor?: boolean;
    maxMotorTorque?: number;
    motorSpeed?: number;

};

export interface PrismaticJointDef extends BaseJointDef {
    localAxisA?: Vec2;
    referenceAngle?: number;
    enableSpring?: boolean;
    hertz?: number;
    dampingRatio?: number;
    enableLimit?: boolean;
    lowerTranslation?: number;
    upperTranslation?: number;
    enableMotor?: boolean;
    maxMotorForce?: number;
    motorSpeed?: number;
};

export interface MouseJointDef extends BaseJointDef {
    target?: Vec2;
    hertz?: number;
    dampingRatio?: number;
    maxForce?: number;
};

export interface WeldJointDef extends BaseJointDef {
    referenceAngle?: number;
    linearHertz?: number;
    angularHertz?: number;
    linearDampingRatio?: number;
    angularDampingRatio?: number;
};

export interface MotorJointDef extends BaseJointDef {
    linearOffset?: Vec2;
    angularOffset?: number;
    maxForce?: number;
    maxTorque?: number;
    correctionFactor?: number;
};

export interface WheelJointDef extends BaseJointDef {
    localAxisA?: Vec2;
    enableSpring?: boolean;
    hertz?: number;
    dampingRatio?: number;
    enableLimit?: boolean;
    lowerTranslation?: number;
    upperTranslation?: number;
    enableMotor?: boolean;
    maxMotorTorque?: number;
    motorSpeed?: number;
};

export interface DistanceJointDef extends BaseJointDef {
    length?: number;
    enableSpring?: boolean;
    hertz?: number;
    dampingRatio?: number;
    enableLimit?: boolean;
    minLength?: number;
    maxLength?: number;
    enableMotor?: boolean;
    maxMotorForce?: number;
    motorSpeed?: number;
};


export class JointConnect2DComponent extends Component {

    private jointMap: { [key: string]: JointType } = js.createMap();
    constructor() {
        super();
        this.nativeType = ComponentType.JointConnect2DComponent;
    }


    private getKey(nodeA: Node, nodeB: Node): string {
        let aId = Math.min(nodeA.id, nodeB.id);
        let bId = Math.max(nodeA.id, nodeB.id);
        let key = aId + "_" + bId;
        return key;
    }

    private checkJoint(type: JointType, nodeA: Node, nodeB: Node): boolean {
        let key = this.getKey(nodeA, nodeB);
        if (this.jointMap[key] && this.jointMap[key] === type) {
            return true;
        }
        return false;
    }

    create(type: JointType, nodeA: Node, nodeB: Node, def?: BaseJointDef) {
        if (this.checkJoint(type, nodeA, nodeB)) {
            return;
        }
        this.writeDefine(type, nodeA, nodeB, 1, def);
        this.jointMap[this.getKey(nodeA, nodeB)] = type;
    }

    setProperty(type: JointType, nodeA: Node, nodeB: Node, def: BaseJointDef) {
        if (!this.checkJoint(type, nodeA, nodeB)) {
            return;
        }
        this.writeDefine(type, nodeA, nodeB, 2, def);
    }

    private writeDefine(type: JointType, nodeA: Node, nodeB: Node, op: number, def?: BaseJointDef) {
        def = def || {};
        switch (type) {
            case JointType.DISTANCE:
                this.writeDistanceJoint(def, op, nodeA, nodeB);
                break;
            case JointType.MOUSE:
                this.writeMouseJoint(def, op, nodeA, nodeB);
                break;
            case JointType.MOTOR:
                this.writeMotorJoint(def, op, nodeA, nodeB);
                break;
            case JointType.PRISMATIC:
                this.writePrismaticJoint(def, op, nodeA, nodeB);
                break;
            case JointType.REVOLUTE:
                this.writeRevoluteJoint(def, op, nodeA, nodeB);
                break;
            case JointType.WELD:
                this.writeWeldJoint(def, op, nodeA, nodeB);
                break;
            case JointType.WHEEL:
                this.writeWheelJoint(def, op, nodeA, nodeB);
                break;
        }
    }

    /**
     * 删除Joint
     * @param nodeA 
     * @param nodeB 
     */
    remove(type: JointType, nodeA: Node, nodeB: Node) {
        this.nativeBeginOp(1);
        this.nativeWriteOpArg("i8", type);
        this.nativeWriteOpArg("i32", nodeA.id);
        this.nativeWriteOpArg("i32", nodeB.id);
        this.nativeEndOp();
    }

    removeAll() {
        this.nativeBeginOp(2);
        this.nativeEndOp();
    }

    private writeFloat(value: number | undefined, state: number, bit: number): number {
        if (value !== undefined) {
            state |= 1 << bit;
            this.nativeWriteOpArg("f32", value);
        }
        return state;
    }

    private writeBoolean(value: boolean | undefined, state: number, bit: number): number {
        if (value !== undefined) {
            state |= 1 << bit;
            this.nativeWriteOpArg("i8", value ? 1 : 0);
        }
        return state;
    }

    private writeVec2(value: Vec2 | undefined, state: number, bit: number): number {
        if (value !== undefined) {
            state |= 1 << bit;
            this.nativeWriteOpArg("f32", value.x);
            this.nativeWriteOpArg("f32", value.y);
        }
        return state;
    }

    private writeI8(value: any | undefined, state: number, bit: number): number {
        if (value !== undefined) {
            state |= 1 << bit;
            this.nativeWriteOpArg("i8", value);
        }
        return state;
    }

    private writeRevoluteJoint(def: RevoluteJointDef, op: number, nodeA: Node, nodeB: Node) {
        let state: number = 0;
        this.nativeBeginOp(3, true);
        this.nativeWriteOpArg("i8", op);
        let statePos = this.nativeWriteOpArg("i32", state);
        this.nativeWriteOpArg("i32", nodeA.id);
        this.nativeWriteOpArg("i32", nodeB.id);
        state = this.writeVec2(def.localAnchorA, state, 1);
        state = this.writeVec2(def.localAnchorB, state, 2);
        state = this.writeFloat(def.referenceAngle, state, 3);
        state = this.writeBoolean(def.enableSpring, state, 4);
        state = this.writeFloat(def.hertz, state, 5);
        state = this.writeFloat(def.dampingRatio, state, 6);
        state = this.writeBoolean(def.enableLimit, state, 7);
        state = this.writeFloat(def.lowerAngle, state, 8);
        state = this.writeFloat(def.upperAngle, state, 9);
        state = this.writeBoolean(def.enableMotor, state, 10);
        state = this.writeFloat(def.maxMotorTorque, state, 11);
        state = this.writeFloat(def.motorSpeed, state, 12);
        state = this.writeVec2(def.localAnchorAPivot, state, 13);
        state = this.writeVec2(def.localAnchorBPivot, state, 14);
        state = this.writeBoolean(def.collideConnected, state, 15);
        state = this.writeFloat(def.localARotate, state, 16);
        state = this.writeFloat(def.localBRotate, state, 17);

        // state = this.writeI8(def.ragbollBoneA,state,16);
        // state = this.writeI8(def.ragbollBoneB,state,17);
        this.nativeWriteOpArgOverride("i32", state, statePos);
        this.nativeEndOp();
    }

    private writePrismaticJoint(def: PrismaticJointDef, op: number, nodeA: Node, nodeB: Node) {
        let state: number = 0;
        this.nativeBeginOp(4, true);
        this.nativeWriteOpArg("i8", op);
        let statePos = this.nativeWriteOpArg("i32", state);
        this.nativeWriteOpArg("i32", nodeA.id);
        this.nativeWriteOpArg("i32", nodeB.id);
        state = this.writeVec2(def.localAnchorA, state, 1);
        state = this.writeVec2(def.localAnchorB, state, 2);
        state = this.writeVec2(def.localAxisA, state, 3);
        state = this.writeFloat(def.referenceAngle, state, 4);
        state = this.writeBoolean(def.enableSpring, state, 5);
        state = this.writeFloat(def.hertz, state, 6);
        state = this.writeFloat(def.dampingRatio, state, 7);
        state = this.writeBoolean(def.enableLimit, state, 8);
        state = this.writeFloat(def.lowerTranslation, state, 9);
        state = this.writeFloat(def.upperTranslation, state, 10);
        state = this.writeBoolean(def.enableMotor, state, 11);
        state = this.writeFloat(def.maxMotorForce, state, 12);
        state = this.writeFloat(def.motorSpeed, state, 13);
        state = this.writeBoolean(def.collideConnected, state, 14);
        state = this.writeFloat(def.localARotate, state, 15);
        state = this.writeFloat(def.localBRotate, state, 16);
        this.nativeWriteOpArgOverride("i32", state, statePos);
        this.nativeEndOp();
    }

    private writeMouseJoint(def: MouseJointDef, op: number, nodeA: Node, nodeB: Node) {
        let state: number = 0;
        this.nativeBeginOp(5, true);
        this.nativeWriteOpArg("i8", op);
        let statePos = this.nativeWriteOpArg("i32", state);
        this.nativeWriteOpArg("i32", nodeA.id);
        this.nativeWriteOpArg("i32", nodeB.id);
        state = this.writeVec2(def.target, state, 1);
        state = this.writeFloat(def.hertz, state, 2);
        state = this.writeFloat(def.dampingRatio, state, 3);
        state = this.writeFloat(def.maxForce, state, 4);
        this.nativeWriteOpArgOverride("i32", state, statePos);
        this.nativeEndOp();
    }

    private writeWeldJoint(def: WeldJointDef, op: number, nodeA: Node, nodeB: Node) {

        let state: number = 0;
        this.nativeBeginOp(6, true);
        this.nativeWriteOpArg("i8", op);
        let statePos = this.nativeWriteOpArg("i32", state);
        this.nativeWriteOpArg("i32", nodeA.id);
        this.nativeWriteOpArg("i32", nodeB.id);
        state = this.writeVec2(def.localAnchorA, state, 1);
        state = this.writeVec2(def.localAnchorB, state, 2);
        state = this.writeFloat(def.referenceAngle, state, 3);
        state = this.writeFloat(def.linearHertz, state, 4);
        state = this.writeFloat(def.angularHertz, state, 5);
        state = this.writeFloat(def.linearDampingRatio, state, 6);
        state = this.writeFloat(def.angularDampingRatio, state, 7);
        state = this.writeBoolean(def.collideConnected, state, 8);
        this.nativeWriteOpArgOverride("i32", state, statePos);
        this.nativeEndOp();
    }

    private writeWheelJoint(def: WheelJointDef, op: number, nodeA: Node, nodeB: Node) {
        let state: number = 0;
        this.nativeBeginOp(7, true);
        this.nativeWriteOpArg("i8", op);
        let statePos = this.nativeWriteOpArg("i32", state);
        this.nativeWriteOpArg("i32", nodeA.id);
        this.nativeWriteOpArg("i32", nodeB.id);
        state = this.writeVec2(def.localAnchorA, state, 1);
        state = this.writeVec2(def.localAnchorB, state, 2);
        state = this.writeVec2(def.localAxisA, state, 3);
        state = this.writeBoolean(def.enableSpring, state, 4);
        state = this.writeFloat(def.hertz, state, 5);
        state = this.writeFloat(def.dampingRatio, state, 6);
        state = this.writeBoolean(def.enableLimit, state, 7);
        state = this.writeFloat(def.lowerTranslation, state, 8);
        state = this.writeFloat(def.upperTranslation, state, 9);
        state = this.writeBoolean(def.enableMotor, state, 10);
        state = this.writeFloat(def.maxMotorTorque, state, 11);
        state = this.writeFloat(def.motorSpeed, state, 12);
        state = this.writeBoolean(def.collideConnected, state, 14);
        this.nativeWriteOpArgOverride("i32", state, statePos);
        this.nativeEndOp();
    }

    private writeMotorJoint(def: MotorJointDef, op: number, nodeA: Node, nodeB: Node) {
        let state: number = 0;
        this.nativeBeginOp(8, true);
        this.nativeWriteOpArg("i8", op);
        let statePos = this.nativeWriteOpArg("i32", state);
        this.nativeWriteOpArg("i32", nodeA.id);
        this.nativeWriteOpArg("i32", nodeB.id);

        state = this.writeVec2(def.linearOffset, state, 1);
        state = this.writeFloat(def.angularOffset, state, 2);
        state = this.writeFloat(def.maxForce, state, 3);
        state = this.writeFloat(def.maxTorque, state, 4);
        state = this.writeFloat(def.correctionFactor, state, 5);
        state = this.writeBoolean(def.collideConnected, state, 6);
        this.nativeWriteOpArgOverride("i32", state, statePos);
        this.nativeEndOp();
    }

    private writeDistanceJoint(def: DistanceJointDef, op: number, nodeA: Node, nodeB: Node) {
        let state: number = 0;
        this.nativeBeginOp(9, true);
        this.nativeWriteOpArg("i8", op);
        let statePos = this.nativeWriteOpArg("i32", state);
        this.nativeWriteOpArg("i32", nodeA.id);
        this.nativeWriteOpArg("i32", nodeB.id);
        state = this.writeVec2(def.localAnchorA, state, 1);
        state = this.writeVec2(def.localAnchorB, state, 2);
        state = this.writeFloat(def.length, state, 3);
        state = this.writeBoolean(def.enableSpring, state, 4);
        state = this.writeFloat(def.hertz, state, 5);
        state = this.writeFloat(def.dampingRatio, state, 6);
        state = this.writeBoolean(def.enableLimit, state, 7);
        state = this.writeFloat(def.minLength, state, 8);
        state = this.writeFloat(def.maxLength, state, 9);
        state = this.writeBoolean(def.enableMotor, state, 10);
        state = this.writeFloat(def.maxMotorForce, state, 11);
        state = this.writeFloat(def.motorSpeed, state, 12);
        state = this.writeBoolean(def.collideConnected, state, 13);
        this.nativeWriteOpArgOverride("i32", state, statePos);
        this.nativeEndOp();
    }
}