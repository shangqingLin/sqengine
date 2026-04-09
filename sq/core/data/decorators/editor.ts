
import {
    LegacyPropertyDecorator, PropertyStashFlag,
    getOrCreateProperty, PropertyStash
} from "./define"


export interface EditablePropEnumConfig
{
    /**
     * 枚举是否是多选
     */
    multiple?:boolean;
}

export interface EditablePropConfig{

    /**
     * 如果属性是一个枚举类型，可以对其进行配置
     */
    enumConfig?:EditablePropEnumConfig
}

/**
 * 设置属性在编辑器上显示
 */
export const editableProp: (displayOrder: number, displayNameOrCfg?: EditablePropConfig | string,cfg?:EditablePropConfig) => LegacyPropertyDecorator =  function (displayOrder: number, displayNameOrCfg?: EditablePropConfig | string,cfg?:EditablePropConfig): LegacyPropertyDecorator {
    return (target: any, propertyKey: string, descriptor: PropertyDescriptor): void => {

        /* editor:start */
        let prop: PropertyStash = getOrCreateProperty(target, propertyKey, descriptor);
        prop.displayOrder = displayOrder;

        if(cfg){
            prop.displayCfg = cfg;
        }

        if (displayNameOrCfg) {
            
            if(typeof displayNameOrCfg === "string"){
                prop.displayName = displayNameOrCfg;
            }

            if(!prop.displayCfg && typeof displayNameOrCfg === "object"){
                prop.displayCfg = displayNameOrCfg
            }
        }

        prop.flag |= PropertyStashFlag.EIDTOR;
        /* editor:end */
    };
};


/**
 * 设置在编辑器添加组件的处的菜单
 * @param menuPath 
 * @returns 
 */
export const editorMenu: (menuPath: string) => ClassDecorator = function (menuPath: string): ClassDecorator {
    return (target: any): void => {
        /* editor:start */
        if(window.EditorExtend) EditorExtend.ComponentMenu.register(menuPath, target);
        /* editor:end */
    }
}

/**
 * 设置属性显示的表单类型
 */
export enum EditorFormTypeEnum {
    point
}

export const editorFormType: (type: EditorFormTypeEnum) => LegacyPropertyDecorator = function (type: EditorFormTypeEnum) {
    return (target: any, propertyKey: string, descriptor?: PropertyDescriptor): void => {
         /* editor:start */
        let prop: PropertyStash = getOrCreateProperty(target, propertyKey);
        prop.editorFormType = type;
        /* editor:end */
    };
}