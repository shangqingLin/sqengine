import { ComponentType } from "../../../native_binding/index";
import Component from "../../../framework/component/Component";

export default class Character2DComponent extends Component {
    constructor() {
        super();
        this.nativeType = ComponentType.Character2DComponent;
    }

    moveLeft(move: boolean): void {
        this.nativeBeginOp(1);
        this.nativeWriteOpArg("i8", move ? 1 : 0);
        this.nativeEndOp();
    }

    jump(): void {
        this.nativeBeginOp(2);
        this.nativeEndOp();
    }

    moveRight(move: boolean): void {
        this.nativeBeginOp(3);
        this.nativeWriteOpArg("i8", move ? 1 : 0);
        this.nativeEndOp();
    }

    setFilter(categoryBits: number, maskBits: number) {
        this.nativeBeginOp(4);
        this.nativeWriteOpArg("ui32", categoryBits);
        this.nativeWriteOpArg("ui32", maskBits);
        this.nativeEndOp();
    }
}