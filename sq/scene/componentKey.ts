/**
 * 在Node中的Component不以组件的Class name作为key
 * 因为经过编译压缩之后，Class Name可能更改了或为undefine了
 */
let componentKey = 1;
function generateComponentKey(): number {
    return ++componentKey;
}

export function setComponentKey<T>(componentCls: Constructor<T>): number {

    //注意：子类也会继承父类的静态变量，所以父类有了____node_key，子类也会有
    //所以这些必须使用hasOwnProperty来判断，不查找原型链
    //@ts-ignore
    if (!componentCls.hasOwnProperty("____node_key")) {
        //@ts-ignore
        componentCls.____node_key = generateComponentKey();
    }

    //@ts-ignore
    return componentCls.____node_key;
}

export function getComponentKey<T>(componentCls: Constructor<T>): number {
    //@ts-ignore
    return componentCls.hasOwnProperty("____node_key") ? componentCls.____node_key : undefined;
}
