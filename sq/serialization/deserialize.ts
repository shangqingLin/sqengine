import { findClassSlash, js, PrimitiveType, PropertyStash, PropertyStashFlag } from "../core/index";
import { DeserializeDependInfo, DeserializeResult, SerializedObjectReference, SerializedTypedObject, SerializedValue } from "./define";
import { Node } from "../scene";
/**
 * 类的反序列化
 */

/**
 * {
 *    obj_fileId :Array<{ propertyName: string, value: SerializedValue}>
 * }
 */
export type OverrideValues = { [key: string]: Array<{ propertyName: string, value: SerializedValue; }> };

export interface DeserializeContext {
    override?: OverrideValues;
    checkExportProperty?: (obj: any, propertName: string) => boolean;
}

export class Deserialize {

    serializedData: Array<SerializedValue>;
    private result: DeserializeResult = Object.create(null);
    context: DeserializeContext;
    private parseObjectMap = Object.create(null);

    private _processReferenceAsset(serialized: SerializedValue, obj: any, prop: string | number) {

        /**
         * 如果__uuid__有值，表示引用的是一个Asset资源对象
         * 如果是资源对象，则交个外部处理了，因为资源还需要加载过程
         */

        if ((serialized as SerializedObjectReference).__uuid__) {
            if (!this.result.depend) {
                this.result.depend = [];
            }
            let g: DeserializeDependInfo = Object.create(null);
            this.result.depend.push(g);
            g.name = prop;
            g.obj = obj;
            g.uuid = serialized.__uuid__;
            return true;
        }
        return false;
    }

    private _deserializeTypeObject(index: number, serialized: SerializedTypedObject, src_obj?: unknown) {
        let obj: any = src_obj;
        if (!obj) {
            let contructor: Constructor = js.getClassByName(serialized.__type__);
            obj = new contructor();
        }
        if(index !== -1) this.parseObjectMap[index] = obj;

        let properties: Array<PropertyStash> = findClassSlash(obj).propertiesArray;
        for (let i = 0, n = properties.length; i < n; ++i) {
            let prop: PropertyStash = properties[i];
            if (!prop || !prop.type || !(prop.flag & PropertyStashFlag.SERIALIZABLE)) continue;
            if (this.context && this.context.checkExportProperty) {
                if (!this.context.checkExportProperty(obj, prop.propertyName)) {
                    continue;
                }
            }

            /**
             * 实现A.prefab中内嵌了B.prefab，然后在A中修改了B的属性值但不存储到B中的
             * 单纯是A中修改的,然后将A中修改的数值覆盖到B中，不使用B原来的数值。
             */
            let sValue: any = serialized[prop.propertyName];
            if (serialized.fileId && this.context && this.context.override) {
                let overrideValue = this.context.override[serialized.fileId];
                if (overrideValue) {
                    // console.info("check " ,prop.propertyName,serialized.fileId);
                    for (let n = 0, nn = overrideValue.length; n < nn; ++n) {
                        if (overrideValue[n].propertyName === prop.propertyName) {
                            sValue = overrideValue[n].value;
                            break;
                        }
                    }
                }
            }

            if (sValue === undefined || sValue === null) continue;

            // console.info("dd", prop.propertyName);

            //在类中可以自定义一个反序列化函数，用来自定义这个属性的如何反序列化
            let func = obj["deserialize_" + prop.propertyName];
            if (func) {
                func.call(obj, sValue, this);
            } else {
                if (prop.type instanceof PrimitiveType) {
                    //表示为基本数据类型、普通JSON数据
                    obj[prop.propertyName] = sValue;
                } else if (prop.isArrayType || js.isClass(prop.type) || Array.isArray(prop.type)) {

                    //普通的类类型
                    if (prop.isArrayType) {
                        obj[prop.propertyName] = this.deserializeArray(sValue, obj[prop.propertyName]);
                    } else {
                        if (!this._processReferenceAsset(sValue, obj, prop.propertyName)) {
                            obj[prop.propertyName] = this.deserializeObject(sValue, obj[prop.propertyName]);
                        }
                    }
                } else {
                    //为枚举类型
                    obj[prop.propertyName] = sValue;
                }
            }

            //当反序列化属性之后，可以调用一个方法通知类这个属性序列化完啦，类中从而可以做一些自己的初始化逻辑
            // let afterFunc = obj["afterDeserialize_" + prop.propertyName];
            // if (afterFunc) {
            //     afterFunc.call();
            // }
        }

        if (obj.afterDeserialize) {
            obj.afterDeserialize();
        }
        return obj;
    }

    public deserializeArray(value: Array<SerializedValue>, src_obj?: Array<unknown>) {
        let arrObj = src_obj || new Array<unknown>(value.length);
        let sValue: unknown;
        for (let i = 0; i < value.length; i++) {
            sValue = value[i];
            if (sValue && typeof sValue === 'object') {
                if (!this._processReferenceAsset(sValue, arrObj, i)) {
                    arrObj[i] = this.deserializeObject(sValue as SerializedTypedObject, arrObj[i]);
                }
            } else {
                arrObj[i] = sValue;
            }
        }
        return arrObj;
    }

    private _deserializeJson(value: { [key: string]: any }, src_obj?: Array<unknown>) {
        let json = src_obj || Object.create(null);
        let sValue: any;
        let keys: Array<string> = Object.keys(value);
        for (let i = 0, n = keys.length; i < n; ++i) {
            let name: string = keys[i];
            sValue = value[name];
            if (sValue && typeof sValue === 'object') {
                if (!this._processReferenceAsset(sValue, sValue, i)) {
                    json[name] = this.deserializeObject(sValue as SerializedTypedObject);
                }
            } else {
                json[name] = sValue;
            }
        }
        return json;
    }

    private _deserializeIdObject(index: number, src_obj?: unknown) {
        if (this.parseObjectMap[index]) {
            return this.parseObjectMap[index];
        }
        let serialized = (this.serializedData as Array<SerializedValue>)[index];
        this._deserializeTypeObject(index, serialized, src_obj);
        return this.parseObjectMap[index];
    }

    getSerializeDataByIndex(index: number): SerializedValue {
        return this.serializedData[index];
    }

    deserializeObject(serialized: SerializedValue, src_obj?: any): any {
        if ((serialized as SerializedTypedObject).__type__) {
            let index = this.serializedData.indexOf(serialized);
            if (index !== -1 && this.parseObjectMap[index]) {
                return this.parseObjectMap[index];
            }
            return this._deserializeTypeObject(index, serialized as SerializedTypedObject, src_obj);
        } else if ((serialized as SerializedObjectReference).__id__ !== undefined && (serialized as SerializedObjectReference).__id__ !== null) {
            //表示它引用的是当前这个文件中的对象，记录这个对象在数组中的索引
            return this._deserializeIdObject((serialized as SerializedObjectReference).__id__, src_obj);
        } else if (Array.isArray(serialized)) {
            return this.deserializeArray(serialized, src_obj);
        } else {
            //那么它就是一个普通的JSON对象,序列化这个JSON内部数据
            return this._deserializeJson(serialized, src_obj);
        }
    }

    deserialize(serializedData: Array<SerializedValue>): DeserializeResult {
        this.serializedData = serializedData;
        let n: number = serializedData.length;
        let objects: Array<Node> = [];
        for (let i = 0; i < n; ++i) {
            let node: any = this.deserializeObject(serializedData[i]);

            //只存储根节点
            if (node instanceof Node && node.parent === null) {
                objects.push(node);
            }
        }
        this.result.objects = objects.length == 1 ? objects[0] : objects;
        return this.result;
    }
}

export function deserialize(serializedData: SerializedValue | Array<SerializedValue>, context?: DeserializeContext): DeserializeResult {
    let d = new Deserialize();
    //@ts-ignore
    d.context = context;

    // let isArray = true;
    if (!Array.isArray(serializedData)) {
        serializedData = [serializedData];
        // isArray = false;
    }
    let result: DeserializeResult = d.deserialize(serializedData);
    // if (!isArray) {
    //     //如果外部传进来是单个，则返回也是单个，外部传进来来是数组，则返回数组
    //     result.objects = (result.objects as Array<any>)[0];
    // }
    return result;
}

