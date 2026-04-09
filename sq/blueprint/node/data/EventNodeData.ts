import { NodeData } from "./NodeData";

export class EventNodeData extends NodeData {
    public eventName: string | number;
}

/**
 * 获取事件节点传递进来的数据
 */
export class GetEventParameterData extends NodeData { }