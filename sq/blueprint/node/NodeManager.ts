import { js } from "../../core/index";
import BPScript from "../BPScript";
import BaseNode from "./BaseNode";
import type { NodeData } from "./data/NodeData";

export default class NodeManager {
    private static _ins: NodeManager;
    private _nodePool: Map<Constructor<BaseNode>, Array<BaseNode>> = new Map();

    public static getInstance(): NodeManager {
        if (!NodeManager._ins) {
            NodeManager._ins = new NodeManager();
        }
        return NodeManager._ins;
    }

    getNode(data: NodeData, script: BPScript): BaseNode {
        let nodeCls: Constructor<BaseNode> = data.nodeCls;
        let pool: Array<BaseNode> = this._nodePool.get(nodeCls);
        if (!pool) {
            pool = [];
            this._nodePool.set(nodeCls, pool);
        }
        let node: BaseNode = pool.pop();
        if (!node) {
            node = new nodeCls();
        }
        //@ts-ignore
        node.script = script;
        //@ts-ignore
        node.data = data;
        return node;
    }

    recoveryNode(node: BaseNode): void {
        let nodeCls: Constructor<BaseNode> = js.getInstanceConstructor(node) as Constructor<BaseNode>;
        let pool: Array<BaseNode> = this._nodePool.get(nodeCls);
        if (!pool) {
            pool = [];
            this._nodePool.set(nodeCls, pool);
        }
        pool.push(node);
    }
}
