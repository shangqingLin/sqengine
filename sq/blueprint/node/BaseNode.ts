import BPScript from "../BPScript";
import { GetEventParameterData } from "./data/EventNodeData";
import { NodeData, NodePinData, PinType } from "./data/NodeData";
import ParameterNodeData from "./data/ParameterNodeData";
import NodeManager from "./NodeManager";

/**
 * 蓝图所有节点的基本节点
 */
export default abstract class BaseNode {

    protected readonly script: BPScript;
    protected readonly data: NodeData;

    //当事件触发时会从事件触发处传递一些数据进来，然后从事件节点开始一个节点接着一个节点往下传递这个数据
    protected eventTriggerData: any;

    abstract entryIn(): void;

    protected entryOut(): void {
        let pinData: NodePinData = this.data.findPinById(4);
        if (pinData) {
            let node: BaseNode = NodeManager.getInstance().getNode(pinData.linkNodeData, this.script);
            node.eventTriggerData = this.eventTriggerData;
            node.entryIn();
        }
    }

    protected runInput(pinId: number): any {
        let pinData = this.data.findPinById(1, pinId);
        if (pinData) {
            return this.runInputPin(pinData);
        }
    }

    protected runInputPin(pinData: NodePinData): any {
        if (pinData.linkNodeData) {
            if (pinData.linkNodeData instanceof ParameterNodeData) {
                let node: BaseNode = NodeManager.getInstance().getNode(pinData.linkNodeData, this.script);
                node.eventTriggerData = this.eventTriggerData;
                node.entryIn();
            } else if (pinData.linkNodeData instanceof GetEventParameterData) {
                return this.eventTriggerData;
            }
            return this.script.getTmpParameter(pinData.linkNodeData.id, pinData.linkPinData.id);
        } else if (pinData.linkParameterId !== undefined) {
            return this.script.getParameter(pinData.linkParameterId);
        }
    }

    protected runOutput(pinId: number, param?: any) {
        let pinData = this.data.findPinById(2, pinId);
        if (pinData) {
            this.runOutputPin(pinData, param);
        }
    }

    protected runOutputPin(pinData: NodePinData, param?: any): void {
        if (pinData.type === PinType.PARAMETER_OUT) {
            this.script.setTmpParameter(pinData.ownerNodeData.id, pinData.id, param);
        } else if (pinData.type === PinType.PARAMETER_ENTRY_OUT) {
            if (pinData.linkNodeData) {
                let node: BaseNode = NodeManager.getInstance().getNode(pinData.linkNodeData, this.script);
                node.eventTriggerData = this.eventTriggerData;
                return node.entryIn();
            }
        }
    }
}


/**
 * 所有具有异步操作的节点必须实现这个类
 */
export abstract class BPBaseAsyncNode extends BaseNode {

}