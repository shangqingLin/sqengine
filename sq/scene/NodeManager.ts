import type Node from "./Node";

export default class NodeManager {

    private static waitDestroyNode: Array<Node> = [];
    static addNodeDestroy(node: Node) {
        this.waitDestroyNode.push(node);
    }

    static destroyNode() {
        for (let i = 0, n = this.waitDestroyNode.length; i < n; ++i) {
            //@ts-ignore
            this.waitDestroyNode[i].destroy2();
        }
        this.waitDestroyNode.length = 0;
    }
}