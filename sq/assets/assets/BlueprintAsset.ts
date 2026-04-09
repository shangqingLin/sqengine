import { NodeData, ParameterData } from "../../blueprint/index";
import { Asset, AssetManager } from "../index";
import type { BlueprintMapSaveData } from "../index";
import type Node from "../../scene/Node";
import { BlueprintComponent } from "../../blueprint/index";
import { deserialize, DeserializeDependInfo, SerializedValue } from "../../serialization/index";
import { sqclass } from "../../core/index";

export interface BlueprintData {
    node: SerializedValue;

    //事件定义。定义了蓝图执行的入口
    eventMapData: Array<NodeData>;
    
    funcDatas: Array<Array<NodeData>>;

    //宏定义
    macroDatas: Array<Array<NodeData>>;

    //变量的定义
    parameterDatas: Array<ParameterData>;
    saveData?: BlueprintMapSaveData;
}

@sqclass("sq.BlueprintAsset")
export class BlueprintAsset extends Asset {
    nodeData: BlueprintData;
    instantiate(): Node {
        let result = deserialize(this.nodeData.node);
        let node = result.objects as Node;
        let comp: BlueprintComponent = node.getComponent(BlueprintComponent);
        comp.asset = this;
        return node;
    }

    async instantiateAndLoadDeped(waitLoadedAll: boolean = true): Promise<Node> {
        let result = deserialize(this.nodeData.node);
        let node = result.objects as Node;
        let comp: BlueprintComponent = node.getComponent(BlueprintComponent);
        comp.asset = this;

        if (!result.depend) {
            return Promise.resolve(result.objects as Node);
        }

        return new Promise<Node>((resolve, reject) => {
            let uuid: Array<string> = [];
            for (let i = 0; i < result.depend.length; ++i) {
                if (uuid.indexOf(result.depend[i].uuid) === -1) {
                    uuid.push(result.depend[i].uuid);
                }
            }


            if (!waitLoadedAll) {
                resolve(result.objects as Node);
            }

            AssetManager.getInstance().loadById(uuid, (error: Error, assets: Array<Asset>) => {
                for (let i = 0; i < result.depend.length; ++i) {
                    let item: DeserializeDependInfo = result.depend[i];
                    item.obj[item.name] = AssetManager.getInstance().getAssetByUUId(item.uuid);
                }

                if (waitLoadedAll) {
                    resolve(result.objects as Node);
                }

                /* debug:start */
                if (error) throw error;
                /* debug:end */

            })
        })
        return node;

    }
}
