import { Effect, Material } from "../../assets";
import { BlendFactor, BlendMode, BlendOp, BlendState, ColorMask, Format, PrimitiveMode, Type } from "../../gfx";
import { samplerTextures, Shader, UBOCamera, UBOGlobal, UBOLocal } from "../../rendering";
import particleSdfFrag from "./particle-sdf.frag";
import particleSdfVert from "./particle-sdf.vert";
import particleTextureBlurFrag from "./particle-texture-blur.frag";
import particleTextureBlurVert from "./particle-texture-blur.vert";

// https://www.shadertoy.com/view/Ms2SD1
//https://www.shadertoy.com/view/lsXGzH
//https://www.shadertoy.com/view/lt3GWj



export default {
    /**
     * 使用普通的纹理渲染粒子
     * @returns 
     */
    getTextureBlurRenderMaterial(): Material {
        let shader = new Shader();
        shader.addAttribute(Format.RG32F, "aParticlePos");
        shader.addBuildInBlock(UBOCamera);
        shader.beginBlock("data");
        shader.addUniformToBlock(Type.FLOAT, "pointSize");
        shader.endBlock();
        shader.addSamplerTexture(Type.SAMPLER2D,"aBlurTexture");
        shader.addPass({
            shaderIndex: 0,
            states: {
                primitive: PrimitiveMode.POINT_LIST,
                blendState: {
                   blendMode:BlendMode.ADD
                } as BlendState
            }
        });

        shader.setShaderCode({
            vert: particleTextureBlurVert,
            frag: particleTextureBlurFrag
        });

        let effect = new Effect();
        effect.initialize(shader.createEffectDefine());
        let particleMaterial = new Material();
        particleMaterial.initialize({
            shaderAsset: effect
        });

        return particleMaterial;
    },


    /**
     * 使用SDF渲染水体粒子
     * @returns 
     */
    getSdfRenderMaterial(): Material {
        if (!this.buildInEffect) {
            let shader = new Shader();
            shader.addAttribute(Format.RG32F, "aPos");
            shader.addAttribute(Format.RG32F, "aUv");
            shader.addAttribute(Format.R32F, "aParticleIndex");
            shader.addBuildInBlock(UBOCamera);
            shader.addBuildInBlock(UBOGlobal);
            shader.beginBlock("data");
            shader.addUniformToBlock(Type.FLOAT, "pointSize");
            shader.addUniformToBlock(Type.FLOAT2, "sdfTexSize");
            shader.addUniformToBlock(Type.FLOAT2, "posTexSize");
            shader.endBlock();
            shader.addSamplerTexture(Type.SAMPLER2D,"posBuffer");
            shader.addSamplerTexture(Type.SAMPLER2D,"rowColumnTexture");
            shader.addSamplerTexture(Type.SAMPLER2D,"sdfBufferTexture");
            shader.setShaderCode({
                vert: particleSdfVert,
                frag: particleSdfFrag
            });
            let effect = new Effect();
            effect.initialize(shader.createEffectDefine());
            this.buildInEffect = effect;
        };

        let material = new Material();
        material.initialize({
            shaderAsset: this.buildInEffect
        });
        return material;
    }
} as {
    getTextureBlurRenderMaterial(): Material;
    getSdfRenderMaterial(): Material;
}






