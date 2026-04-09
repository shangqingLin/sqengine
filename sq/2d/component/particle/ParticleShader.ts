import { Effect, Material } from "../../../assets";
import { BlendFactor, BlendMode, BlendState, ColorMask, Format, Type } from "../../../gfx";
import { UBOCamera, UBOGlobal, UBOLocal } from "../../../rendering/shader/buildin-block";
import Shader from "../../../rendering/shader/Shader";
import particleGPUUpdateVert from "./glsl/particle-gpu-update.vert";
import particleGPUUpdateFrag from "./glsl/particle-gpu-update.frag";
import particleRenderVert from "./glsl/particle-render.vert";
import particleRenderFrag from "./glsl/particle-render.frag";
import particleRenderSecneMarkFrag from "./glsl/particle-scene-mark.frag";
import particleRenderSenceMarkVert from "./glsl/particle-scene-mark.vert";
import { IPassDefine, MacroRecord } from "../../../assets/assets/Effect";
import { IMaterialInfo } from "../../../assets/assets/Material";



let _gpuUpdateProcessShader: Effect;
let _renderGPUShader: Effect;
let _renderCPUShader: Effect;

let _renderSceneMark: Effect;
let _renderTestShader: Effect;

function createParticleRenderMaterial(gpu: boolean): Effect {
    let shader = new Shader();
    shader.addAttribute(Format.RG32F, "aPos");
    shader.addAttribute(Format.RG32F, "aUv");

    if (gpu) {
        shader.addAttribute(Format.R32F, "particleIndex");
    } else {
        shader.addAttribute(Format.RG32F, "particlePosition");
        shader.addAttribute(Format.R32F, "lifeTimePercent");
    }

    shader.addVarying(Format.RG32F, "uv");
    shader.addVarying(Format.RGBA32F, "color");

    if (gpu) {
        shader.addSamplerTexture(Type.SAMPLER2D, "aTexture");
        shader.addSamplerTexture(Type.SAMPLER2D, "particleTexture");
    }

    shader.addSamplerTexture(Type.SAMPLER2D, "renderTexture");
    shader.addSamplerTexture(Type.SAMPLER2D, "propertyTexture");

    shader.beginBlock("particle");
    shader.addUniformToBlock(Type.FLOAT2, "renderSize");
    shader.addUniformToBlock(Type.FLOAT4, "initColor");

    if (gpu) {
        shader.addUniformToBlock(Type.FLOAT2, "particleTextureSize");
        shader.addUniformToBlock(Type.FLOAT2, "aTextureSize");
    }

    shader.addUniformToBlock(Type.FLOAT2, "propertyTextureSize");
    shader.addUniformToBlock(Type.FLOAT2, "initialAngleMinMax");
    shader.addUniformToBlock(Type.FLOAT2, "initialScaleMinMax");
    shader.endBlock();
    shader.addBuildInBlock(UBOCamera);
    shader.addBuildInBlock(UBOLocal);
    shader.addDefine("USE_GPU");
    shader.addDefine("USE_CPU");


    shader.addPass({
        states: {
            blendState: {
                blendMode: BlendMode.ADD
            }
        }
    } as IPassDefine)
    shader.setShaderCode({
        vert: particleRenderVert,
        frag: particleRenderFrag
    });
    let effect = new Effect();
    effect.initialize(shader.createEffectDefine());
    return effect;
}

export default {
    getParticleGPUUpdateMaterial(macros?: MacroRecord): Material {
        if (!_gpuUpdateProcessShader) {
            let shader = new Shader();
            shader.addAttribute(Format.RG32F, "aPos");

            shader.addSamplerTexture(Type.SAMPLER2D, "aTexture");
            shader.addSamplerTexture(Type.SAMPLER2D, "particleTexture");
            shader.addSamplerTexture(Type.SAMPLER2D, "emission_texture_points");

            shader.beginBlock("emitter");
            shader.addUniformToBlock(Type.FLOAT2, "particleTextureSize");
            shader.addUniformToBlock(Type.FLOAT, "SEED");
            shader.addUniformToBlock(Type.FLOAT, "spread_angle");
            shader.addUniformToBlock(Type.FLOAT2, "spread_direction");
            shader.addUniformToBlock(Type.FLOAT2, "initialLinearVelocityMinMax");
            shader.addUniformToBlock(Type.FLOAT, "inherit_emitter_velocity_ratio");
            shader.addUniformToBlock(Type.FLOAT2, "linearAccelMinMax");
            shader.addUniformToBlock(Type.FLOAT2, "radialAccelMinMax");
            shader.addUniformToBlock(Type.FLOAT2, "tangentAccelMinMax");
            shader.addUniformToBlock(Type.FLOAT2, "gravity");
            shader.addUniformToBlock(Type.FLOAT2, "dampingMinMax");
            shader.addUniformToBlock(Type.FLOAT2, "angularVelocityMinMax");
            shader.addUniformToBlock(Type.FLOAT2, "directionalVelocityMinMax");
            shader.addUniformToBlock(Type.FLOAT2, "radialVelocityMinMax");
            shader.addUniformToBlock(Type.FLOAT2, "orbitVelocitMinMax");
            shader.addUniformToBlock(Type.FLOAT2, "turbulenceInfluenceMinMax");
            shader.addUniformToBlock(Type.FLOAT2, "velocity_pivot");
            shader.addUniformToBlock(Type.FLOAT, "emssion_shape");
            shader.addUniformToBlock(Type.FLOAT, "emssion_shape_radius");
            shader.addUniformToBlock(Type.FLOAT, "emission_ring_height");
            shader.addUniformToBlock(Type.FLOAT, "emission_ring_cone_angle");
            shader.addUniformToBlock(Type.FLOAT2, "emission_ring_axis");
            shader.addUniformToBlock(Type.FLOAT, "emission_ring_inner_radius");
            shader.addUniformToBlock(Type.FLOAT2, "emission_texture_size");
            shader.addUniformToBlock(Type.FLOAT, "emission_texture_point_count");
            shader.addUniformToBlock(Type.FLOAT, "emission_texture_normal");
            shader.addUniformToBlock(Type.FLOAT2, "emission_box_extents");

            shader.addUniformToBlock(Type.FLOAT,"attractorType");
            shader.addUniformToBlock(Type.FLOAT2, "attractorShapeExtents");
            shader.addUniformToBlock(Type.FLOAT2,"attractorAttenuationAndStrength");
            shader.addUniformToBlock(Type.MAT3,"attractorTransform");

            shader.endBlock();
            shader.addBuildInBlock(UBOCamera);
            shader.addBuildInBlock(UBOGlobal);

            shader.addDefine("EMIT_SHAPE_NORMAL");
            shader.addDefine("EMIT_SHAPE_POINTS");

            shader.setShaderCode({
                vert: particleGPUUpdateVert,
                frag: particleGPUUpdateFrag
            });

            let effect = new Effect();
            effect.initialize(shader.createEffectDefine());
            _gpuUpdateProcessShader = effect;
        }
        let particleUpdateMaterial = new Material();
        particleUpdateMaterial.initialize({
            shaderAsset: _gpuUpdateProcessShader,
            defines: macros
        });
        return particleUpdateMaterial;
    },




    getParticleGPURenderMaterial(macros?: MacroRecord): Material {
        if (!_renderGPUShader) {
            _renderGPUShader = createParticleRenderMaterial(true);
        }

        macros = macros || {};
        macros["USE_GPU"] = 1;

        let particleRenderMaterial = new Material();
        particleRenderMaterial.initialize({
            shaderAsset: _renderGPUShader,
            defines: macros
        } as IMaterialInfo);
        return particleRenderMaterial;
    },




    getParticleCPURenderMateria(macros?: MacroRecord): Material {
        if (!_renderCPUShader) {
            _renderCPUShader = createParticleRenderMaterial(false);
        }

        macros = macros || {};
        macros["USE_CPU"] = 1;

        let particleRenderMaterial = new Material();
        particleRenderMaterial.initialize({
            shaderAsset: _renderCPUShader,
            defines: macros
        } as IMaterialInfo);
        return particleRenderMaterial;
    },


    getParticleRenderMarkSceneMaterial(): Material {
        if (!_renderSceneMark) {
            let shader = new Shader();
            shader.addAttribute(Format.RG32F, "aPos");
            shader.addAttribute(Format.R32F, "particleIndex");
            shader.addSamplerTexture(Type.SAMPLER2D, "aTexture");

            shader.beginBlock("particle");
            shader.addUniformToBlock(Type.FLOAT2, "aTextureSize");
            shader.endBlock();

            shader.addBuildInBlock(UBOCamera);
            shader.addBuildInBlock(UBOLocal);

            shader.addPass({
                shaderIndex: 0,
                states: {
                    blendState: {
                        blendTarget: {
                            blendColorMask: ColorMask.A //只写入a通道，颜色通道不写入因为会覆盖场景背景颜色
                        }

                    } as BlendState
                }
            });
            shader.setShaderCode({
                vert: particleRenderSenceMarkVert,
                frag: particleRenderSecneMarkFrag
            });
            let effect = new Effect();
            effect.initialize(shader.createEffectDefine());
            _renderSceneMark = effect;
        }

        let material = new Material();
        material.initialize({
            shaderAsset: _renderSceneMark
        } as IMaterialInfo);
        return material;
    },

    getParticleTestRenderMaterial(macros?: MacroRecord): Material {
        if (!_renderTestShader) {
            let shader = new Shader();
            shader.addAttribute(Format.RG32F, "aPos");
            shader.addAttribute(Format.RG32F, "aUv");
            shader.addVarying(Format.RG32F, "uv");
            shader.addSamplerTexture(Type.SAMPLER2D, "aTexture");
            shader.addBuildInBlock(UBOCamera);
            shader.addBuildInBlock(UBOLocal);
            shader.setShaderCode({
                vert: `
                in vec2 aPos; 
                in vec2 aUv;  
                uniform mat4 cc_matViewProj;
                uniform mat4 cc_matWorld; 
                out vec2 uv;  
                vec4 vert(){\n uv=aUv; \nvec4 position = cc_matViewProj  * cc_matWorld * vec4(aPos,-1.,1.0);\n return position;\n} 
                `,
                frag: "uniform sampler2D aTexture; in vec2 uv; vec4 frag(){\n  return texture(aTexture,uv); \n}"
            });

            let effect = new Effect();
            effect.initialize(shader.createEffectDefine());
            _renderTestShader = effect;
        }


        let material = new Material();
        material.initialize({
            shaderAsset: _renderTestShader,
            defines: macros
        } as IMaterialInfo);
        return material;
    }
}





