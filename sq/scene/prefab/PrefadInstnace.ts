/**
 * 专门用于编辑器中的预制体
 */

import { Transform2DComponent } from "../../2d";
import { AssetManager, Prefab } from "../../assets";
import { serializable, sqclass, SQJSON, SQString, type } from "../../core/index";
import { Application } from "../../framework/Application";
import { Deserialize, OverrideValues, SerializedValue } from "../../serialization";
import Node from "../Node";

@sqclass("sq.PrefadInstnace")
export default class PrefadInstnace {

    private _root: Node;
    private childIndex: number;
    private prefab: Prefab;

    @serializable
    @type(SQString)
    private prefabUrl: string;


    /**
    * 实现A.prefab中内嵌了B.prefab，然后在A中修改了B的属性值但不存储到B中的
    * 单纯是A中修改的,然后将A中修改的数值覆盖到B中，不使用B原来的数值。
    */
    @serializable
    @type(SQJSON)
    private overrides: OverrideValues;


    private prefabInstance: Node;

    deserialize_prefabUrl(value: SerializedValue, deserialize: Deserialize): void {
        AssetManager.getInstance().load(value, (error: Error, asset: Prefab) => {
            this.prefab = asset;
            this.create();
        });
    }

    setRoot(root: Node, childIndex: number) {
        this._root = root;
        this.childIndex = childIndex;
        this.create();
    }

    destroy() {
        if (this.prefabInstance) {
            this.prefabInstance.destroy();
            this.prefabInstance = null;
        }
    }

    private create(): void {
        if (this._root && this.prefab) {
            if (this.prefabInstance) throw new Error();
            this.prefabInstance = this.prefab.instantiate(this.overrides).objects as Node;
            // console.info("PrefadInstnace create prefab:", this.childIndex,this.prefabInstance);

            // console.info(this.prefabInstance.getChild(0).getComponent(Transform2DComponent)?.getWorldTransform().toString());

            this._root.addChildAt(this.prefabInstance, this.childIndex);

            // console.info("PrefadInstnace create prefab:", this.prefabInstance.getChild(1).getId());

            // this._root.addChild(this.prefabInstance);
        }
    }

}



