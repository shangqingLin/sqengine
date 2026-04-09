
/**
 * 蓝图相关的装饰器
 */

import { ClassStash, getClassSlash, js, LegacyPropertyDecorator } from "../../core/index"
import { NodeData, PinType } from "../node/data/NodeData";


// ------------------ 定义蓝图节点 --------------------------

/**
 * 定义节点的引脚
 */
export interface BPPinData {

    /**
     * 必须指定
     */
    id: number;

    /**
     * 数据类型
     * 只有输入和输出值的引脚才有
     */
    dataType?: Constructor | object;

    /**
     * 引脚处显示的标题
     */
    title?: string;

    type?: PinType;
}

export interface BPNodeMetaData {



    /**
     * 节点的标题
     */
    name?: string;

    /**
     * 在节点选择菜单中显示的分类的名字
     */
    category?: string;


    /**
     * 节点是否有输入执行引脚
     * 默认为true
     */
    entryIn?: boolean;

    /**
     * 节点是否有输出执行引擎
     * 默认为true
     */
    entryOut?: boolean;

    /**
     * 节点的输入引脚
     */
    inputs?: Array<BPPinData>;

    /**
     * 节点的输出引脚
     */
    output?: Array<BPPinData>;

    /**
   * 默认为true
   */
    innerNode?: boolean;

    /**
     * 默认为NodeData
     */
    dataCls?: Constructor<NodeData>;
}

export interface BPNodeClassStash extends ClassStash {
    meta: BPNodeMetaData;
}



let bpNodeClassStashs: Array<BPNodeClassStash> = [];


/* debug:start */
function checkPin(classStash: BPNodeClassStash,pins: Array<BPPinData>,type:number) {
    let checkMap = Object.create(null);
    for (let i = 0, n = pins.length; i < n; ++i) {
        let pinCfg = pins[i];
        if (pinCfg.id === undefined) {
            throw new Error(classStash.clsName + "Node Pin必须指定ID");
        }

        if (checkMap[pinCfg.id]) {
            throw new Error(classStash.clsName + "Node Pin ID重复");
        }

        if(type === 1){
            if(pinCfg.dataType === undefined){
                throw new Error(classStash.clsName + "Node Pin DataType必须指定");
            }
        }else if(type === 2) {
            if( pinCfg.type !== PinType.PARAMETER_ENTRY_OUT &&  pinCfg.dataType === undefined){
                throw new Error(classStash.clsName + "Node Pin Type必须指定");
            }
        }
        checkMap[pinCfg.id] = pinCfg.id;
    }
}
/* debug:end */


/**
 * 将一个类声明了一个蓝图节点
 * 在使用这个装饰器之前必须先使用sqclass装饰器注册类
 * @returns
 */
export const bpNode: (setting: BPNodeMetaData) => ClassDecorator = function (setting: BPNodeMetaData): ClassDecorator {
    return function (target: any) {
        let classStash: BPNodeClassStash = getClassSlash(target) as BPNodeClassStash;
        /* debug:start */
        //检查配置是否合法
        if (setting.inputs) {
            checkPin(classStash,setting.inputs,1);
        }
        if(setting.output){
            checkPin(classStash,setting.output,2);
        }
        /* debug:end */

        classStash.meta = setting;
        bpNodeClassStashs.push(classStash);
    }
}

export function getAllBPNode(): Array<BPNodeClassStash> {
    return bpNodeClassStashs;
}

//---------------------------- 定义蓝图类 ----------------------------------

/**
 * 蓝图类上的函数
 */
export interface BPClassFunction {
    name: string;

    /**
     * 函数入参
     */
    parameters?: Array<BPClassFunctionParameter>;

    /**
     * 函数返回值
     */
    returnParam?: BPClassFunctionParameter;

    /**
     * 当前的函数是否是getter/setter
     */
    isAccessor?: boolean;
}


/**
 * 蓝图类上的函数参数
 */
export interface BPClassFunctionParameter {

    /**
     * 参数名字
     */
    name?: string;

    /**
     * 属性的数据类型
     * 基础数据类型为字符串类型
     * Object类型为具体的类
     */
    type: Constructor | Object  | Array<Constructor | Object >;

    /**
     * type是否为数组类型，如果是数组类型，则type表示为数组的元素类型
     */
    isArrayType?: boolean

    /**
     * 该参数是否是必须的，默认为true
     */
    require?:boolean;
}


export interface BPClassEvent {
    name: string;
}

export interface BPClassStash extends ClassStash {
    blueprint: boolean;
    events?: Array<BPClassEvent>;
    functionMaps?: { [key: string]: BPClassFunction };
}


let bpClasses: Array<BPClassStash> = [];

export const bpClass: (target: any) => void = function (target: any) {
    let classStash: BPClassStash = getClassSlash(target) as BPClassStash;
    classStash.blueprint = true;
    bpClasses.push(classStash);
}

export function getBpClasses(): Array<BPClassStash> {
    return bpClasses;
}

/**
 * 
 * 将类中的方法暴露给蓝图编辑器
 * 使用这个装饰器之前需要在sqclass将类声明为蓝图类
 */
export const bpFunction: (param?: Array<BPClassFunctionParameter>, returnParam?: BPClassFunctionParameter) => MethodDecorator = function (param?: Array<BPClassFunctionParameter>, returnParam?: BPClassFunctionParameter): MethodDecorator {
    return function (target: Function, propertyKey: string, descriptor: TypedPropertyDescriptor<any>) {
        let classStash: BPClassStash = getClassSlash(target) as BPClassStash;
        if (!classStash.functionMaps) {
            classStash.functionMaps = js.createMap();
        }
        let func = js.createMap() as BPClassFunction;
        func.name = propertyKey;
        if (param) {
            func.parameters = param;
        }
        if (returnParam) {
            func.returnParam = returnParam;
        }
        classStash.functionMaps[propertyKey] = func;
        return null;
    }
}


/**
 * 在类上声明类中能够触发的事件。这些事件会在蓝图编辑器中显示
 */
export const bpEvents: (events: Array<BPClassEvent>) => ClassDecorator = function (events: Array<BPClassEvent>): ClassDecorator {
    return function (target: any) {
        let classStash: BPClassStash = getClassSlash(target) as BPClassStash;
        classStash.events = events;
    }
}


/**
 * 蓝图类上的getter和setter方法
 * @param type 
 * @param isArray 
 * @returns 
 */
export const bpAccessor: (type: Constructor | Array<Constructor> | Array<any> | Function | any, isArray?: boolean) => LegacyPropertyDecorator =
    function (type: Constructor, isArray?: boolean): LegacyPropertyDecorator {
        return (target: any, propertyKey: string, descriptor?: PropertyDescriptor): void => {
            let classStash: BPClassStash = getClassSlash(target) as BPClassStash;
            if (!classStash.functionMaps) {
                classStash.functionMaps = js.createMap();
            }
            let func = js.createMap() as BPClassFunction;
            func.name = propertyKey;
            func.isAccessor = true;
            func.parameters = [
                {
                    type: type,
                    isArrayType: isArray
                }
            ];
            classStash.functionMaps[propertyKey] = func;
        }
    }



