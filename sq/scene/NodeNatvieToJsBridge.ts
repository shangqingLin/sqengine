import { Byte } from "../core";
import { dispatch, NativeObjectType, NativeToJsObjectBridge, WasmByte } from "../native_binding";
import type Node from "./Node";
import NodeEventType from "./NodeEventType";

export default class NodeNativeToJsBridge extends NativeToJsObjectBridge {

    static initialize() {
        dispatch.registerNativeToJsObject(NativeObjectType.Node, new NodeNativeToJsBridge());
    }

    public override dispatch(byte: Byte): void {
        let op = byte.readUint8();
        switch (op) {
            case 1:
                {

                    let nodeId = byte.readInt32();
                    // console.info("NodeNativeToJsBridge", nodeId);
                    let node: Node = dispatch.getJsToNativeObject(nodeId).getCustomData() as Node;
                    node.fire(NodeEventType.TRASNFORM_CHANGE);
                    break;
                }
        }
    }
}