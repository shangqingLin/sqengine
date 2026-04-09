import js from "../../utils/js";
import { ClassStash, getClassSlash } from "./define";

export interface sqClassConfig
{
    //如果这个类是一个组件，则设置这个组件是否需要在编辑器的Inspector视图中显示
    //默认为显示的
    componentInspector?:boolean
}

/**
 * 
 * @param clsName 
 * @param blueprint 是否是蓝图类
 * @returns 
 */
export const sqclass: (clsName:string,cfg?:sqClassConfig) => ClassDecorator = function(clsName:string,cfg?:sqClassConfig) : ClassDecorator{
    return function(target: any){
        let classStash:ClassStash = getClassSlash(target);
        classStash.clsName = clsName;
        classStash.cls = target;
        if(cfg) classStash.cfg = cfg;
        js.setClassByName(clsName,target);
    }
};