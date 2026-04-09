import { samplerTextures, Shader, UBOCamera } from "../../../rendering";
import { Effect, IMaterialInfo, Material } from "../../../assets";
import { BlendState, Format, Type } from "../../../gfx";
import spritefrag from "./sprite.frag"
import spritevert from "./sprite.vert"


export default {
    getDefaultSpriteMaterial(): Material {
        if (!this.buildInSpriteMaterial) {
            let shader = new Shader();
            shader.addAttribute(Format.RG32F, "aPos");
            shader.addAttribute(Format.RG32F, "aUv");
            shader.addAttribute(Format.RGBA32F, "aColor");
            shader.addAttribute(Format.R32F, "aTextureIndex");
            shader.addBuildInBlock(UBOCamera);
            shader.addSamplerTextureInfo(samplerTextures);
            shader.addVarying(Format.RGBA32F, "color");
            shader.setShaderCode({
                vert: spritevert,
                frag: spritefrag
            });
            
            shader.addPass({
                shaderIndex: 0,
                states: {
                    blendState: {
                        blendTarget: {
                            blend: true
                        }
                    } as BlendState
                }
            });

            let effect = new Effect();
            effect.initialize(shader.createEffectDefine());
            this.buildInSpriteMaterial = new Material();
            this.buildInSpriteMaterial.initialize({
                shaderAsset: effect
            } as IMaterialInfo);
        }
        return this.buildInSpriteMaterial;
    }
}