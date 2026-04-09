import js from "../../utils/js";
import { getOrCreateProperty, LegacyPropertyDecorator, PropertyStash } from "./define";

export class PrimitiveType<T> {
    public name: string;
    constructor(name: string) {
        this.name = name;
    }
    public toString(): string {
        return this.name;
    }
}

export const SQInteger = new PrimitiveType('Integer');
export const SQFloat = new PrimitiveType('Float');
export const SQBoolean = new PrimitiveType('Boolean');
export const SQString = new PrimitiveType('String');

/**
 * 如果是纯的普通JSON数据，使用这个进行标记
 * 否则序列化或反序列化操作会序列化到JSON内部的值的
 */
export const SQJSON = new PrimitiveType("JSON");

/**
 * 如果指定的为数组，则表示该属性可以接受多个数据类型。支持联合数据类型
 * 不支持Vec2\Size\Rect\Color\Number等这些基础数据类型指定联合类型，一般只用于接受资源类型
 * @param type 
 * @returns 
 */
export const type: (type: Constructor | Array<Constructor> | Array<any> | Function | any, isArray?: boolean) => LegacyPropertyDecorator =
    function (type: Constructor, isArray?: boolean): LegacyPropertyDecorator {
        return (target: any, propertyKey: string, descriptor?: PropertyDescriptor): void => {
            let prop: PropertyStash = getOrCreateProperty(target, propertyKey);
            if (isArray) {
                prop.isArrayType = isArray;
            }
            /* debug:start */
            if (!type) console.error("@type指定的数据类型为undefine");
            /* debug:end */

            //装饰器是在代码初始化的是执行，但由于import的循环引用关系，有些类型符号还没有import就不能立刻使用
            //所以这时需要指定一个function，这个function返回类型。避免了在初始化代码的时候就获取类型
            if (js.isFunction(type)) {
                Object.defineProperty(prop, "type", {
                    get: function () {
                        return (type as Function)();
                    }
                });
            } else {
                prop.type = type;
            }
        };
    }
