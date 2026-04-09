import { Shader, UBOCamera, UBOLocal } from "../../../rendering";
import { Effect, Material } from "../../../assets";
import { Format } from "../../../gfx";
import graphicsFrag from "./graphics.frag";
import graphicsVert from "./graphics.vert";

export default {
    getDefaultGraphicsMaterial(): Material {
        if (!this.buildInGraphicsMaterial) {
            let shader = new Shader();
            shader.addAttribute(Format.RG32F, "aPos");
            shader.addAttribute(Format.RGBA32F, "aColor");
            shader.addBuildInBlock(UBOCamera);
            shader.addBuildInBlock(UBOLocal);
            shader.addVarying(Format.RGBA32F, "color");
            shader.setShaderCode({
                vert: graphicsVert,
                frag: graphicsFrag
            });
            shader.addPass({
                shaderIndex: 0,
                states: {
                    blendState: {
                        blendTarget: {
                            blend: true
                        }
                    }
                }
            })
            let effect = new Effect();
            effect.initialize(shader.createEffectDefine());
            this.buildInGraphicsMaterial = new Material();
            this.buildInGraphicsMaterial.initialize({
                shaderAsset: effect
            });
        }
        return this.buildInGraphicsMaterial;
    }

}