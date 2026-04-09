import { sqclass, editorMenu, Size, serializable, type, editableProp } from "../../../core/index";
import { ComponentType } from "../../../native_binding/index";
import { SqHexColor } from "../define";
import Collision2DComponent from "./Collision2DComponent";


/* editor:start */
@editorMenu("physics 2D/BoxCollision2DComponent")
/* editor:end */
@sqclass("sq.BoxCollision2DComponent")
export default class BoxCollision2DComponent extends Collision2DComponent {

    private size: Size;

    constructor() {
        super(ComponentType.BoxCollision2DComponent);
    }

    /* editor:start */
    @editableProp(0)
    /* editor:end */
    @type(Size)
    @serializable
    set box(size: Size) {
        //size指定为半径
        
        if (!this.size) {
            this.size = new Size();
        }
        if (size.width <= 0 || size.height <= 0) {
            return;
        }

        if (this.size.width === size.width && this.size.height === size.height) return;
        
        this.size.width = size.width;
        this.size.height = size.height;
        if (this.physicsEnable()) {
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("f32", size.width);
            this.nativeWriteOpArg("f32", size.height);
            this.nativeEndOp();
        }
    }

    setSize(width: number, height: number): void {
        Size.TEMP_SIZE.set(width / 2, height / 2);
        this.box = Size.TEMP_SIZE;
    }

    get box(): Size {
        return this.size;
    }
}