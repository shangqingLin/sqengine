import { GraphicsComponent } from "../../2d";
import System from "../../framework/System";
import { dispatch, JsToNativeObjectBridge, NativeObjectType, ObjectType } from "../../native_binding/index";
import Node from "../../scene/Node";
import NativeToJsPhysicsObjBridge from "./NativeToJsPhysicsObjBridge";

export default class PhysicsSystem extends System {
    private native: Module.PhysicsSystem;
    private nativeDispatch: JsToNativeObjectBridge;
    public static enabled: boolean = false;
    private static _instance: PhysicsSystem;
    private debugDrawNode: Node;
    private unitsPerMeter: number;
    constructor() {
        super();
        PhysicsSystem._instance = this;
        this.native = new window.Module.PhysicsSystem();
        this.nativeDispatch = dispatch.createJsToNativeObject(ObjectType.PyhsicsSystem);
        dispatch.registerNativeToJsObject(NativeObjectType.physics, NativeToJsPhysicsObjBridge.getInstance());
        PhysicsSystem.enabled = true;
    }

    static get ins(): PhysicsSystem {
        return PhysicsSystem._instance;
    }

    setDebugDraw(enable: boolean): void {
        if (enable) {
            if (!this.debugDrawNode) {
                this.debugDrawNode = new Node();
                this.debugDrawNode.addComponent(GraphicsComponent);
            }
        } else {
            if (this.debugDrawNode) {
                this.debugDrawNode.destroy();
                this.debugDrawNode = null;
            }
        }
        this.nativeDispatch.beginOp(1);
        this.nativeDispatch.writeOpArg("i8", enable ? 1 : 0);
        if (enable) {
            this.nativeDispatch.writeOpArg("i32", this.debugDrawNode.id);
        }
        this.nativeDispatch.endOp();
    }

    getDebugDrawNode(): Node {
        return this.debugDrawNode;
    }

    setUnitsPerMeter(pixleNum: number) {
        if (this.unitsPerMeter !== pixleNum) {
            this.unitsPerMeter = pixleNum;
            this.nativeDispatch.beginOp(2);
            this.nativeDispatch.writeOpArg("i32", pixleNum);
            this.nativeDispatch.endOp();
        }
    }
    getUnitPerMeter(): number {
        return this.unitsPerMeter || 1;
    }

    enableNextStep(b: boolean) {
        this.nativeDispatch.beginOp(3);
        this.nativeDispatch.writeOpArg("i8", b ? 1 : 0);
        this.nativeDispatch.endOp();
    }
    nextStep() {
        this.nativeDispatch.beginOp(4);
        this.nativeDispatch.endOp();
    }

    setDrawDebugFlag(flag: number) {
        this.nativeDispatch.beginOp(5);
        this.nativeDispatch.writeOpArg("ui32", flag);
        this.nativeDispatch.endOp();
    }
}