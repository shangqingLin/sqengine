import { value } from "../utils/js-typed";


const enumRegister = Object.create(null);

/**
 * 需要获取枚举信息的，都需要使用这个进行声明一下
 * 一搬在编辑器中出现的枚举都需要这样这个方法声明一下
 * @param obj 
 * @returns 
 */
export function sqenum<EnumT extends {}>(enumType: EnumT): void {
    if (!('__enums__' in enumType)) {

        //使用keys作为枚举唯一的名字，应该不会有所有枚举名字都相同的两个枚举吧
        let keys = Object.keys(enumType);
        let key = keys.join("_");
        
        /* debug:start */
        if(enumRegister[key]) throw new Error("相同的枚举" + key);
        /* debug:end */

        enumRegister[key] = enumType;

        value(enumType, '__enums__', key, true);
    }
}

export function getEnum<EnumT extends {}>(enumName:string): EnumT
{
    return enumRegister[enumName];
}

export function getEnumName<EnumT extends {}>(enumType: EnumT):string
{
    //@ts-ignore
    return enumType["__enums__"];
}

/**
 * 判断是否为枚举类型
 * @param enumType 
 * @returns 
 */
export function isEnum<EnumT extends {}>(enumType: EnumT): boolean { return enumType && enumType.hasOwnProperty('__enums__')};
