/**
 * 流程控制语句
 */

import { SQBoolean, sqclass, SQFloat, SQInteger } from "../../core/index";
import { Application } from "../../framework/Application";
import { bpNode, BPNodeMetaData, BPPinData } from "../decorators/blueprint";
import BaseNode from "./BaseNode";
import { NodeData, NodePinData, PinType } from "./data/NodeData";


//if
@sqclass("sq.BPBranch")
@bpNode({
    /* editor:start */
    name: "Branch",
    category: "流程控制",
    entryOut: false,
    /* editor:end */
    inputs: [
        {
            id:1,
            /* editor:start */
            title: "条件",
            /* editor:end */

            dataType: SQBoolean
        }
    ] as Array<BPPinData>,
    output: [
        {
            id:1,
            /* editor:start */
            title: "TRUE",
            /* editor:end */

            type: PinType.PARAMETER_ENTRY_OUT
        },
        {
            id:2,
            /* editor:start */
            title: "FALSE",
            /* editor:end */

            type: PinType.PARAMETER_ENTRY_OUT
        }
    ]
} as BPNodeMetaData)
export class BPBranch extends BaseNode {
    override entryIn(): void {
        let b:boolean = this.runInput(1);
        this.runOutput(b ? 1 : 2);
    }
}

/**
 * 执行N次
 */
@sqclass("sq.BPDoNNode")
@bpNode({
    /* editor:start */
    name: "DoN",
    category: "流程控制",
    entryOut: false,
    /* editor:end */
    inputs: [
        {
            id:1,
            /* editor:start */
            title: "N",
            /* editor:end */
            dataType: SQInteger
        }
    ],
    output: [
        {
            id:1,
            /* editor:start */
            title: "完成",
            /* editor:end */

            type: PinType.PARAMETER_ENTRY_OUT
        },
        {
            id:2,
            /* editor:start */
            title: "Counter",
            /* editor:end */

            type: PinType.PARAMETER_ENTRY_OUT
        }
    ]
} as BPNodeMetaData)
export class BPDoNNode extends BaseNode {
    override entryIn(): void {
        let n:number = this.runInput(1);
        for(let i = 0; i < n ; ++i){
            this.runOutput(2);
        }
        this.runOutput(1);
    }
}

/**
 * For循环
 */
@sqclass("sq.BPForNode")

/* editor:start */
@bpNode({
    name: "For",
    category: "流程控制",
    entryOut: false,
    inputs:[
        {
            id:1,
            title:"first index",
            dataType:SQInteger
        },
        {
            id:2,
            title:"last index",
            dataType:SQInteger
        }
    ],
    output:[
        {
            id:1,
            title:"Loop Body",
            type:PinType.PARAMETER_ENTRY_OUT
        },
        {
            id:2,
            title:"Index",
            dataType:SQInteger
        },
        {
            id:3,
            title:"Complete",
            type:PinType.PARAMETER_ENTRY_OUT
        }
    ]
} as BPNodeMetaData)
/* editor:end */
export class BPForNode extends BaseNode {
    override entryIn(): void {
        let firstIndex:number = this.runInput(1);
        let lastIndex:number = this.runInput(2);
        for(let i = firstIndex ; i < lastIndex ; ++i){
            this.runOutput(2,i);
            this.runOutput(1);
        }
        this.runOutput(3);
    }
}

/**
 * while循环
 */
@sqclass("sq.BPWhileNode")

/* editor:start */
@bpNode({
    name: "While",
    category: "流程控制",
    entryOut: false,
    inputs:[
        {
            id:1,
            title:"Condition",
            dataType:SQBoolean
        }
    ],
    output:[
        {
            id:1,
            title:"Loop Body",
            type:PinType.PARAMETER_ENTRY_OUT
        },
        {
            id:2,
            title:"Complete",
            type:PinType.PARAMETER_ENTRY_OUT
        }
    ]
} as BPNodeMetaData)
/* editor:end */
export class BPWhileNode extends BaseNode {
    override entryIn(): void {
       while(true){
          if(!this.runInput(1)){
            break;
          }
          this.runOutput(1);
       } 
       this.runOutput(2);
    }
}


/**
 * 延迟多少时间执行
 */
@sqclass("sq.BPTimeDelay")
/* editor:start */
@bpNode({
    name:"Delay Time",
    category: "流程控制",
    inputs:[
        {
            id:1,
            title:"Timer",
            dataType:SQFloat
        }
    ],
    output:[
        {
            id:1,
            title:"Complete",
            type:PinType.PARAMETER_ENTRY_OUT
        }
    ]
})
/* editor:end */
export class BPTimeDelay extends BaseNode
{
    override entryIn(): void {
        let delayTime:number = this.runInput(1);
        Application.ins.timer.once(delayTime,this,this.onComplete);
        this.entryOut();
    }
    private onComplete(){
        this.runOutput(1);
    }
}

/**
 * 延迟多少帧执行
 */
@sqclass("sq.BPTimeFrame")
/* editor:start */
@bpNode({
    name:"Delay Frame",
    category: "流程控制",
    inputs:[
        {
            id:1,
            title:"Counter",
            dataType:SQInteger
        }
    ],
    output:[
        {
            id:1,
            title:"Complete",
            type:PinType.PARAMETER_ENTRY_OUT
        }
    ]
})
/* editor:end */
export class BPTimeFrame extends BaseNode
{
    override entryIn(): void {
        let delayTime:number = this.runInput(1);
        Application.ins.timer.frameOnce(delayTime,this,this.onComplete);
        this.entryOut();
    }
    private onComplete(){
        this.runOutput(1);
    }
}