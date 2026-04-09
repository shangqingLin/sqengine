import { NodeSaveData } from "../../../assets/index";
import { IDGen } from "../../../core/index";
import BaseNode from "../BaseNode";

var idGen = new IDGen();

export enum PinType {

    //输入输出脚引
    ENTRY_IN,
    ENTRY_OUT,

    //数据脚引
    PARAMETER_IN,
    PARAMETER_OUT,

    //动态输出引脚
    PARAMETER_ENTRY_OUT
}


export class NodePinData {
    id:Readonly<number>;
    type: PinType;
    ownerNodeData: NodeData;
    linkNodeData?: NodeData;
    linkPinData?: NodePinData;
    linkParameterId? : number;
}

export class NodeData {
    readonly id: number;
    entryIn?: NodePinData;
    entryOut?: NodePinData;
    inputMap?:{[key:number]:NodePinData};
    outputMap?: {[key:number]:NodePinData};
    nodeCls: Constructor<BaseNode>;

    /* editor:start */
    saveData: NodeSaveData;
    /* editor:end */

    constructor() {
        this.id = idGen.get();
    }

    /**
     * 
     * @param type 1:input 2:outputs 3:entryIn 4:entryOut
     * @param id bpNodeClassMeta设置的pin id
     * @returns 
     */
    findPinById(type: number, id?: number): NodePinData | null {
        if (type === 1) {
            return this.inputMap ? this.inputMap[id] : null;
        } else if (type === 2) {
            return this.outputMap ? this.outputMap[id] : null;
        } else if (type == 3) {
            return this.entryIn;
        } else if (type === 4) {
            return this.entryOut;
        }
    }

    destroy() {
        idGen.recovery(this.id);
    }
}