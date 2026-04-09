import { js } from "../../utils/index";
import { sqClassConfig } from "./class";
import { EditablePropConfig } from "./editor";

export type LegacyPropertyDecorator = (target: Record<string, any>, propertyKey: string | symbol, descriptor?:PropertyDescriptor) => void;
export enum PropertyStashFlag {
    
    /**
     * 表示该属性是否在编辑器中显示
     */
    EIDTOR = 1 << 1,

    /**
     * 表示该属性是否为Serializable属性
     */
    SERIALIZABLE = 1 << 2,
}


export interface PropertyStash{

    flag:number;

    /**
     * 属性的数据类型
     * 基础数据类型为字符串类型
     * Object类型为具体的类
     */
    type:Constructor | Object;

    /**
     * type是否为数组类型，如果是数组类型，则type表示为数组的元素类型
     */
    isArrayType?:boolean
    
    /**
     * 该属性在编辑器中的显示名称。
     */
    displayName?: string;

    /**
     * 该属性在编辑器中显示的顺序
     */
    displayOrder?: number;

    displayCfg?:EditablePropConfig;

    /**
     * 代码中属性的名字
     */
    propertyName:string;

    /** 这个属性使用什么类型的表单进行编辑 */
    editorFormType?:number;
}



export interface ClassStash {
    clsName:string;
    cls:Constructor;
    properties?: Record<PropertyKey, PropertyStash>;

    /**
     * 搞这个数组主要是为了对属性进行排序
     * 因为在反序列化的时候，可能有些逻辑需要依赖初始化顺序的初始化顺序
     */
    propertiesArray:Array<PropertyStash>;

    cfg?:sqClassConfig;
};


export function getClassSlash(cls:Function):ClassStash{
    let proto = js.getPrototypeByCls(cls);
    let key:string = "decorator";

    /**
     * 这里的hasOwnProperty非常关键
     * 
     * class Class1{}
     * class Class2 extends Class1{}
     * 
     * 由于Class2继承了Class1，则Class2的原型对象变为:
     * Class2.prototype = {
     *    __proto__:Class1.prototype
     * }
     * 
     * 然后Class1先注册了decorator：
     * Class1.prototype.decorator = {};
     * 
     * 然后再轮到Class2注册ClassStash。如果我们Class2.prototype.decorator判断是否有这个属性时
     * 则由于Class2.prototype对象上没有decorator这个属性，那么就转而去找他的原型，亦即__proto__指向的原型对象，
     * 由于他的原型对象为Class1.prototype，所以Class2.prototype.decorator其实拿到的是Class1上的decorator，造成冲突。
     * 真是操蛋。所以我们必须使用hasOwnProperty方法，从而绕过检查原型链上的属性
     */
    if(proto.hasOwnProperty(key)){
        return proto[key];
    }
    proto[key] = Object.create(null);
    return  proto[key];
}

export function getOrCreateDict(target:any,key:string):any{
    return target[key] = target[key] || Object.create(null);
}


/**
 * 保存装饰器传递过来的关于类的反射信息
 */
export function getOrCreateProperty(target:any, propertyKey : string, descriptor? : PropertyDescriptor) : PropertyStash{
   let classStash  : ClassStash = getClassSlash(target);
   if(!classStash.propertiesArray){
     classStash.propertiesArray = [];
   }
   let properties: Record<PropertyKey, PropertyStash> = getOrCreateDict(classStash,"properties");
   let propertyStash:PropertyStash = properties[propertyKey];
   if(!propertyStash){
        propertyStash = Object.create(null);
        properties[propertyKey] = propertyStash;
        classStash.propertiesArray.push(propertyStash);
        propertyStash.propertyName = propertyKey;
   }
   return propertyStash;
}

export function findClassSlashLightweight(obj:object) : ClassStash | null
{
    return js.getInstanceConstructor(obj).prototype.decorator || null;
}

export function findClassSlash(obj:object):ClassStash | null{
    findPropertySlash(obj);
    return js.getInstanceConstructor(obj).prototype.decorator || null;
}

export function findClassSlashByClass(cls:Constructor):ClassStash | null{
    findPropertySlashByClass(cls);
    return js.getPrototypeByCls(cls).decorator || null;
}

function findClassSlashByClass2(cls:Constructor):ClassStash | null
{
    return js.getPrototypeByCls(cls).decorator || null;
}


export function findPropertySlashByClass(cls:Constructor):Record<PropertyKey, PropertyStash>{
    let classStash:ClassStash = js.getPrototypeByCls(cls).decorator;
    if(!classStash) return null;

    if(!(classStash as any).extends && js.getSuper(cls) !== Object){        
        let result = Object.create(null);
        let propertiesArray : Array<PropertyStash> = [];
        let classStash2;
        let properties = classStash.properties;
        let propertiesArray2 = classStash.propertiesArray;
        while(true){
            if(properties){
                let names = Object.keys(properties);
                for(let i = 0, n = names.length ; i < n; ++i){
                    let name:string = names[i];
                    result[name] = properties[name];
                }
            }

            if(propertiesArray2){
                propertiesArray = propertiesArray.concat(propertiesArray2);
            }

            cls = js.getSuper(cls);
            if(cls === Object){
                break;
            }
            classStash2 = findClassSlashByClass2(cls);
            if(classStash2){
                properties = classStash2.properties;
                propertiesArray2 = classStash2.propertiesArray;
            }else{
                properties = null;
                propertiesArray2 = null;
            }
        }
        //在反序列化的时候，确保父类的属性先于子类的属性进行初始化
        propertiesArray.reverse();

        //缓存起来，减少上面代码的运行，提升效率
        (classStash as any).extends = true;
        classStash.properties = result;
        classStash.propertiesArray = propertiesArray;
    }
    return classStash.properties;
}

export function findPropertySlash(obj:object):Record<PropertyKey, PropertyStash>{
    let cls = js.getInstanceConstructor(obj); 
    return findPropertySlashByClass(cls);
}