import { sqclass } from "../core/index";
import { dispatch, ObjectType } from "../native_binding/index";
import Node from "./Node";

@sqclass("sq.Scene")
export default class Scene extends Node {
    protected override createNativeObj() {
        this.nativeObject =  dispatch.createJsToNativeObject(ObjectType.Scene, this);
        this.nativeObject.beginOp(1);
        this.nativeObject.endOp();
    }


    protected override _onHierarchyChanged(): void {
        // this.setEnabled(this._activeInHierarchy);
        if (this._children) {
            for (let j = 0, n = this._children.length; j < n; ++j) {
                //@ts-ignore
                this._children[j]._onHierarchyChanged();
            }
        }
    }

    /**
     * 引擎内部调用
     * @param active 
     * @private
     */
    _active(active: boolean) {
        if (this._activeInHierarchy === active) return;
        this._activeInHierarchy = active;
        this._onHierarchyChanged();
        this.nativeObject.beginOp(10);
        this.nativeObject.writeOpArg("i8", active ? 1 : 0);
        this.nativeObject.endOp();
    }

    /* edtior:start  */
    _activeInEditor(active: boolean) {
        if (this._activeInHierarchy === active) return;
        this._activeInHierarchy = active;
        this._onHierarchyChanged();
        this.nativeObject.beginOp(13);
        this.nativeObject.writeOpArg("i8", active ? 1 : 0);
        this.nativeObject.endOp();
    }
    /* editor:end */
}