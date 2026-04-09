import { ComponentType } from "../../../native_binding/index";
import { Component } from "../../../framework/index";

export default class WallSlide2DComponent extends Component {

    private _onSlopStopHandler: () => void;
    constructor() {
        super();
        this.nativeType = ComponentType.WallSlide2DComponent;
    }
    moveForward(): void {
        this.nativeBeginOp(1);
        this.nativeEndOp();
    }

    moveBack(): void {
        this.nativeBeginOp(2);
        this.nativeEndOp();
    }

    stop() {
        this.nativeBeginOp(3);
        this.nativeEndOp();
    }

    setSpeed(speed: number) {
        this.nativeBeginOp(4);
        this.nativeWriteOpArg("f32", speed);
        this.nativeEndOp();
    }

    setSlop(min: number, max: number) {
        this.nativeBeginOp(5);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    set onSlopStopHandler(callback: () => void | null) {
        if (callback) {
            this._onSlopStopHandler = callback;
            this.nativeBeginOp(6);
            this.nativeWriteOpArg("i8", 1);
            this.nativeEndOp();
        } else {
            if (this._onSlopStopHandler) {
                this._onSlopStopHandler = null;
                this.nativeBeginOp(6);
                this.nativeWriteOpArg("i8", 0);
                this.nativeEndOp();
            }
        }
    }
}