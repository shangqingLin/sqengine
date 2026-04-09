
import { findClassSlashLightweight, sqclass, SQFloat, SQInteger, SQString } from "../../core/index";
import { BPClassFunction, BPClassStash, bpNode, BPNodeMetaData, BPPinData } from "../decorators/blueprint";
import BaseNode from "./BaseNode";
import {  PinType } from "./data/NodeData";
import FunctionCallNodeData from "./data/FunctionCallNodeData";
import { EventNodeData } from "./data/EventNodeData";
import ParameterNodeData from "./data/ParameterNodeData";

@sqclass("sq.BPFunctionCallNode")
/* editor:start */
@bpNode({
    innerNode: false,
    dataCls: FunctionCallNodeData,
    inputs: [
        {
            id: 1,
            title: "target",
            dataType: Object
        }
    ]
})
/* editor:end */
export class BPFunctionCallNode extends BaseNode {
    override entryIn(): void {
        let data: FunctionCallNodeData = this.data as FunctionCallNodeData;
        let target = this.runInput(1);
        let classStash: BPClassStash = findClassSlashLightweight(target) as BPClassStash;
        let func: BPClassFunction = classStash.functionMaps[data.funcName];
        if (func.parameters) {
            let n: number = func.parameters.length;
            let parameters = new Array(n);
            for (let i = 0; i < n; ++i) {
                //动态生成的ID，从10000，编辑器中规定的，编辑器中修改这里记得也修改
                parameters[i] = this.runInput(10000 + i);
            }
            let returnValue = target[data.funcName](...parameters);
            if (func.returnParam) {
                this.runOutput(1, returnValue);
            }
        } else {
            let returnValue = target[data.funcName]();
            if (func.returnParam) {
                this.runOutput(1, returnValue);
            }
        }
        this.entryOut();
    }
}


/**
 * 事件节点
 */
@sqclass("sq.BPEventNode")
@bpNode({
    innerNode: false,
    dataCls: EventNodeData,
    entryIn: false
})
export class BPEventNode extends BaseNode {
    override entryIn(): void {
        this.entryOut();
    }
}


@sqclass("sq.BPParameterGetNode")
@bpNode({
    innerNode: false,
    dataCls: ParameterNodeData,
    entryIn: false,
    entryOut: false,
    output: [
        {
            id: 1,
            dataType: Object,
            type: PinType.PARAMETER_OUT
        }
    ]
})
export class BPParameterGetNode extends BaseNode {
    override entryIn() {
        let data: ParameterNodeData = this.data as ParameterNodeData;
        let value = this.script.getParameter(data.parameterId);
        this.runOutput(1, value);
    }
}

@sqclass("sq.BPParameterSetNode")
@bpNode({
    innerNode: false,
    dataCls: ParameterNodeData,
    entryIn: false,
    entryOut: false,
    inputs: [
        {
            id: 1,
            dataType: Object,
            type: PinType.PARAMETER_IN
        }
    ]
})
export class BPParameterSetNode extends BaseNode {
    override entryIn(): void {

    }
}
/**
 * 打印日志
 */
@sqclass("sq.PrintLogNode")
@bpNode({
    /* editor:start */
    name: "Print String",
    category: "通用",
    /* editor:end */
    inputs: [
        {
            id: 1,
            dataType: SQString
        }
    ] as Array<BPPinData>
} as BPNodeMetaData)
export class PrintLogNode extends BaseNode {
    override entryIn(): void {
        let msg: string = this.runInput(1);
        console.log(msg);
        this.entryOut();
    }
}

