import { Vec2 } from "../../../core/index";
import { Component } from "../../../framework/index";
import { ComponentType } from "../../../native_binding/index";


export enum PBDStretchingModel {
    pbdStretchingModel,
    xpbdStretchingModel
}

export enum PBDBendingModel {
    springAngleBendingModel = 0,
    pbdAngleBendingModel,
    xpbdAngleBendingModel,
    pbdDistanceBendingModel,
    pbdHeightBendingModel,
    pbdTriangleBendingModel
}

/**
 * 详细用法请查看C++端对此结构的说明
 */
export interface PBDSetting {
    stretchingModel?: PBDStretchingModel;
    bendingModel?: PBDBendingModel;
    damping?: number;
    stretchStiffness?: number;
    stretchHertz?: number;
    stretchDamping?: number;
    bendStiffness?: number;
    bendHertz?: number;
    bendDamping?: number;
    isometric?: boolean;
    fixedEffectiveMass?: boolean;
    warmStart?: boolean;
}

export class PBD2DComponent extends Component {
    constructor() {
        super();
        this.nativeType = ComponentType.PBD2DComponent;
    }

    /**
     * 
     * @param points [x,y,mass,x1,y1,mass1,……]
     */
    create(points: Array<number>): void {
        this.nativeBeginOp(1);
        this.nativeWriteOpArg("i32", points.length / 3);
        for (let i = 0, n = points.length; i < n; i += 3) {
            this.nativeWriteOpArg("f32", points[i]);
            this.nativeWriteOpArg("f32", points[i + 1]);
        }

        //Mass
        for (let i = 2, n = points.length; i < n; i += 3) {
            this.nativeWriteOpArg("f32", points[i]);
        }

        this.nativeEndOp();
    }


    private writeSettingItem(type: string, value: any, bit: number,state: number): number {
        if (value !== undefined) {
            state |= 1 << bit;
            console.info(state,value);
            this.nativeWriteOpArg(type, value);
        }
        return state;
    }

    private writeSettingBoolean(value: boolean, bit: number,state: number): number {
        if (value !== undefined) {
            state |= 1 << bit;
            this.nativeWriteOpArg("i8", value === true ? 1 : 0);
        }
        return state;
    }

    pbdSetting(def: PBDSetting) {
        let state: number = 0;
        this.nativeBeginOp(5);
        let statePos = this.nativeWriteOpArg("i32", state);
        state = this.writeSettingItem("i8",def.stretchingModel,1,state);
        state = this.writeSettingItem("i8", def.bendingModel,2,state);
        state = this.writeSettingItem("f32", def.damping,3,state);
        state = this.writeSettingItem("f32", def.stretchStiffness,4,state);
        state = this.writeSettingItem("f32", def.stretchHertz,5,state);
        state = this.writeSettingItem("f32", def.stretchDamping,6,state);
        state = this.writeSettingItem("f32", def.bendStiffness,7,state);
        state = this.writeSettingItem("f32", def.bendHertz,8,state);
        state = this.writeSettingItem("f32", def.bendDamping,9,state);
        state = this.writeSettingBoolean(def.isometric,10,state);
        state = this.writeSettingBoolean(def.fixedEffectiveMass,11,state);
        state = this.writeSettingBoolean(def.warmStart,12,state);
        this.nativeWriteOpArgOverride("i32", state,statePos);
        this.nativeEndOp();
    }

    setLinearVelocity(pointIndex: number, velocity: Vec2) {
        this.nativeBeginOp(2);
        this.nativeWriteOpArg("i32", pointIndex);
        this.nativeWriteOpArg("f32", velocity.x);
        this.nativeWriteOpArg("f32", velocity.y);
        this.nativeEndOp();;
    }

    move(pointIndex: number, offset: Vec2): void {
        this.nativeBeginOp(3);
        this.nativeWriteOpArg("i32", pointIndex);
        this.nativeWriteOpArg("f32", offset.x);
        this.nativeWriteOpArg("f32", offset.y);
        this.nativeEndOp();;
    }

    /**
     * 设置指定的点的质量
     * 如果质量设置为0，则点为静态的点，不会被模拟的
     * @param mass 
     */
    setMass(pointIndex: number, mass: number) {
        this.nativeBeginOp(6);
        this.nativeWriteOpArg("i32", pointIndex);
        this.nativeWriteOpArg("f32", mass);
        this.nativeEndOp();
    }

    reset(): void {
        this.nativeBeginOp(4);
        this.nativeEndOp();
    }
}