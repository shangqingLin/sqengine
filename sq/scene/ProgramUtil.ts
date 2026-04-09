// import { Type } from "../gfx/index";

import { Effect, MacroRecord } from "../assets/assets/Effect";
import { js } from "../core";

// const typeMask    = 0xfc000000; // 表示将Uniform变量类型的值使用高位的6个字节存储
// const bindingMask = 0x03f00000; // 表示将Uniform属于哪个Block(binding)的值相对于typeMask的偏移之后的6个位存储
// const countMask   = 0x000ff000; // 表示将Uniform的数目的值相对于bindingMask的偏移之后的8个字节存储
// const offsetMask  = 0x00000fff; // Uniform在Block中的偏移量使用 12个位存储

// //返回的数字使用32位存储，虽然在JS中数字是64位的，但只使用了从低位开始的32个bit
// export const genHandle = (binding: number, type: Type, count: number, offset = 0): number => ((type << 26) & typeMask)
//     | ((binding << 20) & bindingMask) | (count << 12) & countMask | (offset & offsetMask);


export default class ProgramUtils {
    private static buildInMarcos: Array<{ name: string, value: string | boolean | number, shaderType: number }> = [];

    /**
     * 
     * @param name 
     * @param value 
     * @param shaderType 为哪种Shape注册的，有如下值：
     * 
     * 1：表示所有Shader类型都声明这些宏
     * 2：表示为顶点着色器声明
     * 3：表示为片元着色器声明
     *  
     */
    public static registerBuildInMacros(name: string, value: boolean | string | number, shaderType: number) {
        //对于着色器来说，宏声明的顺序同样重要

        this.buildInMarcos.push({
            name: name,
            value: value,
            shaderType: shaderType
        });
    }

    public static generateBuildInMarcors(shaderType: number): string {
        let code: string = "";
        for (let i = 0; i < this.buildInMarcos.length; ++i) {
            let info = this.buildInMarcos[i];
            if (info.shaderType === shaderType || info.shaderType == 1) {
                let name: string = info.name;
                if (name.startsWith("#")) {
                    code += name;
                } else {
                    code += "#define " + name;
                }
                code += " " + this.buildInMarcos[i].value + "\n";
            }
        }
        return code;
    }
}