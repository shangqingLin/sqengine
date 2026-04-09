import { isEnum, sqenum } from "../../../core/index";
import { ParameterSaveData } from "../../../assets/index";

export enum ParameterDataType {

    /**
     * 用户在编辑器中创建的变量
     */
    NORMAL,

    /**
     * 引擎内置变量
     */
    BUILDIN,

    /**
     * 临时变量
     */
    TEMP

}
sqenum(ParameterDataType);

export class ParameterData {

    id: number;

    /**
     * 变量名字
     */
    value: any;

    parameterType: ParameterDataType;

    /*
    * paremterType = BUILDIN
    * value = BUILDIN.COMPONENT
    * clsName =  sq.CameraComponent
    * 则表示要查找蓝图类上的 CameraComponent 组件实例
    */
    clsName?: string;

    /**
     * 存储资源中当前这个变量的数据
     * 发布环境没有
     */
    saveData?: ParameterSaveData
}