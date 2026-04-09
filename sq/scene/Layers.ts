
/**
 * Layers用于设置到Camera和Node上，表示Node使用哪个Camera进行渲染。
 * 只有Node的Layers和Camera设置的Layers匹配得上，表示这个Node使用这个Layers渲染的
 * 
 * Layers数值为32位数值，表示支持32个Layer
 * 引擎内部占用目前占用32~31的位，其他的你外部可以扩展
 */
export enum Layers {

    INHERIT = 0, //Default。继承父节点的Layer
    GROUND = 3, // 地图的地表
    UI = 5,
    TRUCK = 6, //车身
    MAP = 7,
    EFFECT = 8, //游戏中所有的特效，可以与喷水发生碰撞
    BACKGROUND_FRA = 9, // 远景
    BACKGROUND_MID = 10, // 中景
    BACKGROUND_NEAR = 11, // 近景
    BACKGROUND_FORE = 12, // 前景
    FLUID = 13,
    BLOCKER_POINTER = 14,
    EMITTER_WATER = 15, //喷水
    SCENE_OBJECT = 16, //场景中一些物体，可以与Water、Ground等碰撞
    FLMAE = 17,
};


export type LayerMask = number;

export function GetLayerMask(layers: Layers | Array<Layers>): LayerMask {
    if (!Array.isArray(layers)) {
        layers = [layers];
    }
    let mask: number = 0;
    for (let i = 0, n = layers.length; i < n; ++i) {
        mask |= 1 << layers[i];
    }
    return mask;
}

