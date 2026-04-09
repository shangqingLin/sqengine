/**
 * 引擎内部通用的内置Shader
 * 
 */


import stencilClearVert from "./buildinshader/stencil-clear.vert"
import stencilClearFrag from "./buildinshader/stencil-clear.frag"
import Shader from "./shader/Shader";
import { Effect, Material } from "../assets";
import { Format } from "../gfx";


let stencilClearMaterial: Material;

export default {
    getStencilClearMaterial(): Material {
        if (!stencilClearMaterial) {
            let shader = new Shader();
            shader.addAttribute(Format.RG32F, "aPos");
            shader.setShaderCode({
                vert: stencilClearVert,
                frag: stencilClearFrag
            });
            let effect = new Effect();
            effect.initialize(shader.createEffectDefine());
            stencilClearMaterial = new Material();
            stencilClearMaterial.initialize({
                shaderAsset: effect
            });
        }
        return stencilClearMaterial;
    }
} as
    {
        getStencilClearMaterial(): Material;
    }