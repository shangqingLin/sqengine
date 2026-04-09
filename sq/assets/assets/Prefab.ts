import Asset from "./Asset";
import Node from "../../scene/Node";
import { sqclass, serializable, type } from "../../core/index";
import { deserialize, DeserializeDependInfo, DeserializeResult, OverrideValues, SerializedValue } from "../../serialization/index";
import { AssetManager } from "../assetmanager/AssetManager";
/**
 * 代表预制体资源
 */
@sqclass("sq.Prefab")
export default class Prefab extends Asset {

    /**
     * 文件内容
     * 在游戏中先不进行序列化（为了性能没有必要加载就创建一堆节点），所以data为文件内容
     * 在编辑器中创建Prefab就序列化，所以data为Node
     */
    @type(Node)
    @serializable
    public data: Array<SerializedValue>;

    public override clear(): void {
        this.data = null;
    }

    /**
     * 基于Prefab的数据实例化一个出来使用
     */
    public instantiate(overrides?: OverrideValues): DeserializeResult {
        return deserialize(this.data, {
            override: overrides
        });
    }

    /**
     * 实例化，并且如果内部依赖了资源，则加载资源
     */
    public async instantiateAndLoadDeped(waitLoadedAll: boolean = true, overrides?: OverrideValues): Promise<Node> {

        let result: DeserializeResult = deserialize(this.data, {
            override: overrides
        });

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
    }

    override destroy(): void {
        super.destroy();
        if (this.data instanceof Node) {
            this.data.destroy();
            this.data = null;
        }
    }
}