import { Asset } from "../assets/index";
import { ClassStash, findClassSlash, findPropertySlash, js, PropertyStash, PropertyStashFlag, SQJSON } from "../core/index";

/**
 * 对传入的对象进行序列化
 */

export class Serialize {
    public needRecordType: boolean = true;

    //true表示使用记录index方式来序列化
    private useIndex: boolean = true;
    private recordObjectIndexs: Array<any> = [];
    private result: Array<any> = [];

    private serializeObj(obj: Object) {
        let classStash: ClassStash = findClassSlash(obj);
        if (!classStash) {
            return null;
        }

        let index: number = this.recordObjectIndexs.indexOf(obj);
        let seriliationData: Record<string, any> = Object.create(null);

        if (this.useIndex) {
            if (index === -1) {
                index = this.recordObjectIndexs.push(obj) - 1;
                this.result.push(seriliationData);
            } else {
                seriliationData.__id__ = index;
                return seriliationData;
            }
        }

        if (this.needRecordType)
            seriliationData.__type__ = classStash.clsName;

        //@ts-ignore
        if (obj.serializable) {
            //@ts-ignore
            obj.serializable(seriliationData, this);
        } else {
            this.serializeProperties(seriliationData, obj);
        }

        // //@ts-ignore
        // if(obj._onSeriliation){
        //     //@ts-ignore
        //     obj._onSeriliationCallback(seriliationData);
        // }

        if (this.useIndex) {
            //不直接记录对应的对象，而是记录index
            seriliationData = Object.create(null);
            seriliationData.__id__ = index;
        }
        return seriliationData;
    }

    private serializeArray(array: Array<any>) {
        let result = new Array(array.length);
        for (let i = 0, n = array.length; i < n; ++i) {
            result[i] = this.serializeValue(array[i]);
        }
        return result;
    }

    private serializeJson(obj: Record<string, any>) {
        let seriliationData: Record<string, any> = Object.create(null);

        let index: number = this.recordObjectIndexs.indexOf(obj);

        if (this.useIndex) {
            if (index === -1) {
                index = this.recordObjectIndexs.push(obj) - 1;
                this.result.push(seriliationData);
            } else {
                seriliationData.__id__ = index;
                return seriliationData;
            }
        }

        let names: Array<string> = Object.keys(obj);
        for (let i = 0, n = names.length; i < n; ++i) {
            let name: string = names[i];
            let sValue: any = obj[name];
            if (sValue !== undefined && sValue !== null) {
                seriliationData[name] = this.serializeValue(sValue);
            }
        }

        if (this.useIndex) {
            seriliationData = Object.create(null);
            seriliationData.__id__ = index;
        }
        return seriliationData;
    }

    private serializeAsset(obj: Asset) {
        let seriliationData: Record<string, any> = Object.create(null);
        seriliationData.__uuid__ = obj.uuid;
        return seriliationData;
    }

    serializeValue(v: any, type?: any) {

        if (type) {
            if (type === SQJSON) {
                return v;
            }
        }

        if (Array.isArray(v)) {
            return this.serializeArray(v);
        } else if (typeof v === "object") {
            if (v instanceof Asset) {
                return this.serializeAsset(v);
            } else if (!js.getPrototypeByInstance(v)) {
                //表示为Object.create(null)创建的JSON
                return this.serializeJson(v);
            } else if (js.getInstanceConstructor(v) === Object) {
                //表示普通的直接 {}、new Object的JSON对象
                return this.serializeJson(v);
            } else {
                //表示应用中的类的实例
                return this.serializeObj(v);
            }
        } else {
            return v;
        }
    }

    serializeProperties(seriliationData: Record<string, any>, obj: Object,
        filter?: (propertyName: string) => boolean | null) {

        let propertyMaps: { [key: string]: PropertyStash } = findPropertySlash(obj);
        let names = Object.keys(propertyMaps);
        names.sort();

        for (let i = 0, n = names.length; i < n; ++i) {
            let name: string = names[i];
            let property: PropertyStash = propertyMaps[name];
            if (property.flag & PropertyStashFlag.SERIALIZABLE) {

                if (filter && !filter(name)) {
                    continue;
                }

                //在类中可以自定义个函数，用来自定义这个属性的如何序列化
                let func = (obj as Record<string, any>)["serialize_" + name];
                if (func) {
                    seriliationData[name] = func.call(obj, this);
                } else {
                    let v: any = (obj as Record<string, any>)[name];
                    if (v === undefined || v === null) {
                        seriliationData[name] = null;
                    } else {
                        seriliationData[name] = this.serializeValue(v, property.type);
                    }
                }
            }
        }
    }

    serialize(obj: Object): Array<any> {
        this.serializeObj(obj);
        return this.result;
    }

    serializeNoIndex(obj: Object) {
        this.useIndex = false;
        return this.serializeObj(obj);
    }
}

export function serialize(obj: Object, needRecordType: boolean = true): Array<any> {
    let s: Serialize = new Serialize();
    s.needRecordType = needRecordType;
    return s.serialize(obj);
}

export function serializeNoIndex(obj: Object, needRecordType: boolean = true): Object {
    let s: Serialize = new Serialize();
    s.needRecordType = needRecordType;
    return s.serializeNoIndex(obj);
}