import { BlueprintData } from "../assets";
import { js } from "../core";
import BPScript from "./BPScript";
import BaseNode from "./node/BaseNode";
import { BPEventNode } from "./node/CommonNode";
import { EventNodeData } from "./node/data/EventNodeData";
import { NodeData, NodePinData } from "./node/data/NodeData";

/**
 * 在不使用编辑器的情况下，可以直接通过这里的提供的工具来生成蓝图
 */
export default class BlueprintGenerator {

    private eventData: Array<EventNodeData>;
    private script: BPScript;
    constructor(script: BPScript) {
        this.script = script;
    }

    addEvent(eventName: string | number): EventNodeData {
        if (!this.eventData) this.eventData = [];
        let event: EventNodeData = new EventNodeData();
        event.eventName = eventName;
        event.nodeCls = BPEventNode;
        this.eventData.push(event);
        return event;
    }

    createNode(nodeCls: Constructor<BaseNode>): NodeData {
        let data = new NodeData();
        data.nodeCls = nodeCls;
        return data;
    }

    defineScriptParameter(value: any): number {
        let parameterId = this.script.defineParameter(value);
        this.script.setParemeter(parameterId, value);
        return parameterId;
    }

    addGetParameterPin(pinId: number, node: NodeData, parameterId: number) {
        if (!node.inputMap) {
            node.inputMap = js.createMap();
        }

        /* debug:start */
        if (node.inputMap[pinId]) throw new Error(pinId + "已经存在");
        /* debug:end */

        let pinData = new NodePinData();
        pinData.linkParameterId = parameterId;
        node.inputMap[pinId] = pinData;
    }

    addGetParameterFromGetNode(pinId: number, node: NodeData, getNode: NodeData) {
        if (!node.inputMap) {
            node.inputMap = js.createMap();
        }

        /* debug:start */
        if (node.inputMap[pinId]) throw new Error(pinId + "已经存在");
        /* debug:end */

        let pinData = new NodePinData();
        pinData.linkNodeData = getNode;
        node.inputMap[pinId] = pinData;
    }

    linkEntryOut(srcNode: NodeData, destNode: NodeData) {
        srcNode.entryOut = new NodePinData();
        srcNode.entryOut.linkNodeData = destNode;
    }

    generator(): BlueprintData {
        let data = js.createMap() as BlueprintData;
        data.eventMapData = this.eventData;
        return data;
    }
}