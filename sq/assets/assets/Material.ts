import Asset from "./Asset";
import { IPassStates, Effect, MacroRecord } from "./Effect";
import { Byte, CommonUtils, Vec2 } from "../../core/index";
import { js } from "../../core/index";
import Mat4 from "../../core/math/Mat4";

export type MaterialProperty = number | Vec2;

export interface IMaterialInfo {
    shaderAsset: Effect,

    /**
     * 为材质定义的宏
     */
    defines?: MacroRecord;

    /**
     * 渲染管线状态设置
     */
    state?: IPassStates;
}

/**
 * 材质资源类，包含模型绘制方式的全部细节描述。
 */
export class Material extends Asset {

    /**
     * 着色器中声明的Uniform属性值的设置
     * 在材质中，我们主要的任务就是设置Uniform属性值，都保存在这里了
     * key:属性名字
     * MaterialProperty:属性值
     * @protected
     */
    // protected props:Record<string, MaterialProperty>[] = []

    /**
     * 关联的着色器
     * @protected
     */
    protected _effect: Effect;

    private macros: MacroRecord;

    constructor() {
        super();
        this.native = new window.Module.Material(this.id);
    }

    get effect(): Effect {
        return this._effect;
    }

    initialize(materialInfo: IMaterialInfo) {
        this._effect = materialInfo.shaderAsset;
        this.macros = materialInfo.defines;

        //@ts-ignore
        this.native.setEffectAsset(this._effect.native);
        let nativeMaterialInfo = new window.Module.IMaterialInfo();

        if (this.macros) {
            let names = Object.keys(this.macros);
            for (let i = 0, n = names.length; i < n; ++i) {
                nativeMaterialInfo.pushMacro(names[i], this.macros[names[i]]);
            }
        }
        (this.native as Module.Material).initialize(nativeMaterialInfo);
        window.Module.destroy(nativeMaterialInfo);
    }


    public hasDefine(name: string): boolean {
        if (Array.isArray(this.macros)) {
            for (let i = 0, n = this.macros.length; i < n; +i) {
                if (CommonUtils.isDefine(this.macros[i][name])) {
                    return !!this.macros[i][name];
                }
            }
        } else {
            return this.macros && !!this.macros[name];
        }
        return false;
    }

    public setProperty(name: string, value: number | Asset | Mat4) {

        if (value instanceof Asset) {
            (this.native as Module.Material).setPropertyBridgeJsTexture(name, value.getId());
        } else if (value instanceof Mat4) {
            let buffer = window._malloc(64);
            let byte = new Byte();
            byte.setData(window.Module.HEAPU8.subarray(buffer, buffer + 64));
            for (let i = 0; i < 16; ++i) {
                byte.writeFloat32(value.data[i]);
            }
            (this.native as Module.Material).setPropertyBridgeJsMat4(name, buffer);
            window._free(buffer);
        } else {
            (this.native as Module.Material).setPropertyBridgeJsFloat(name, value);
        }
    }
}