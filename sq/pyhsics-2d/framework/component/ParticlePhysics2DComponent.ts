import { ComponentType } from "../../../native_binding/index";
import { Color, sqclass, Vec2 } from "../../../core/index";
import {RenderComponent} from "../../../framework/component/RenderComponent";
import PhysicsSystem from "../PhysicsSystem";
import ParticleShader from "../../rendering/ParticleShader";
import { Material, RenderTexutre } from "../../../assets";
import { ContactManifold } from "../define";
import Node from "../../../scene/Node";

/**
 * 设置粒子的运动方式
 */
export enum ParticleFlag {
    /// Water particle.
    waterParticle = 0,
    /// Removed after next simulation step.
    zombieParticle = 1 << 1,
    /// Zero velocity.
    wallParticle = 1 << 2,
    /// With restitution from stretching.
    springParticle = 1 << 3,
    /// With restitution from deformation.
    elasticParticle = 1 << 4,

    ///粒子之间的是否需要粘度
    viscousParticle = 1 << 5,
    /// Without isotropic pressure.
    powderParticle = 1 << 6,
    /// With surface tension.
    tensileParticle = 1 << 7,
    /// Mix color between contacting particles.
    colorMixingParticle = 1 << 8,
    /// Call b2DestructionListener on destruction.
    destructionListenerParticle = 1 << 9,
    /// Prevents other particles from leaking.
    barrierParticle = 1 << 10,
    /// Less compressibility.
    staticPressureParticle = 1 << 11,
    /// Makes pairs or triads with other particles.
    reactiveParticle = 1 << 12,
    /// With high repulsive force.
    repulsiveParticle = 1 << 13,
    /// Call b2ContactListener when this particle is about to interact with
    /// a rigid body or stops interacting with a rigid body.
    /// This results in an expensive operation compared to using
    /// b2_fixtureContactFilterParticle to detect collisions between
    /// particles.
    fixtureContactListenerParticle = 1 << 14,
    /// Call b2ContactListener when this particle is about to interact with
    /// another particle or stops interacting with another particle.
    /// This results in an expensive operation compared to using
    /// b2_particleContactFilterParticle to detect collisions between
    /// particles.
    particleContactListenerParticle = 1 << 15,
    /// Call b2ContactFilter when this particle interacts with rigid bodies.
    fixtureContactFilterParticle = 1 << 16,
    /// Call b2ContactFilter when this particle interacts with other
    /// particles.
    particleContactFilterParticle = 1 << 17,
}

export enum ParticleGroupFlag {
    solidParticleGroup = 1 << 0,
    rigidParticleGroup = 1 << 1,
    particleGroupCanBeEmpty = 1 << 2,
    particleGroupWillBeDestroyed = 1 << 3,
    particleGroupNeedsUpdateDepth = 1 << 4,
    particleGroupInternalMask = particleGroupWillBeDestroyed | particleGroupNeedsUpdateDepth
}

export enum ParticleGroupShape {
    circle,
    polygon,
    edge
}

export interface ParticlePhysics2DGroupDef {
    flags?: ParticleFlag; //默认粒子的类型
    groupFlag?: ParticleGroupFlag;
    position?: Vec2;
    angle?: number;
    linearVelocity?: Vec2;
    angularVelocity?: number;
    color?: Color;
    strength?: number;
    shape: ParticleGroupShape;
    stride?: number;
    particleCount?: number;
    lifetime?: number;
    group?: number;
}

export interface ParticlePhysicsCreateDefine {
    count?: number; //创建粒子的数量，默认是1
    flags?: ParticleFlag,
    position?: Vec2;// 初始位置
    velocity?: Vec2; //初始速度
    lifetime?: number;
    color?: Color;
}

@sqclass("sq.ParticlePhysics2DComponent")
export class ParticlePhysics2DComponent extends RenderComponent {

    public waterRenderMaterial: Material;

    private enableBodyContactListener: boolean;
    private enableParticleContactListener: boolean;
    //在NativeToJsPhysicsObjBridge中调用
    private _onBeginContactBodyCallback: (contact: ContactManifold) => void;
    private _onEndContactBodyCallback: (contact: ContactManifold) => void;
    private _onBeginContactParticleCallback: () => void;
    private _onEndContactParticleCallback: (particleIdA: number, particleIdB: number) => void;
    constructor() {
        super();
        if (PhysicsSystem.enabled)
            this.nativeType = ComponentType.ParticlePhysics2DComponent;
    }

    // protected override onInitialize(): void {
    //     let material = ParticleShader.getMarkFluidAreaMaterial();
    //     this.setShaderMaterial(material);
    // }

    public createGroup(def: ParticlePhysics2DGroupDef) {

        /* debug:start */
        if (def.shape === undefined || def.shape === null) {
            throw new Error("必须指定shaper");
        }
        /* debug:end */


        let state: number = 0;

        this.nativeBeginOp(1);

        let statePos = this.nativeWriteOpArg("i32", state);
        this.nativeWriteOpArg("i8", def.shape);

        if (def.flags) {
            state |= 1;
            this.nativeWriteOpArg("ui32", def.flags);
        }

        if (def.groupFlag) {
            state |= 2;
            this.nativeWriteOpArg("ui32", def.groupFlag);
        }

        if (def.position) {
            state |= 4;
            this.nativeWriteOpArg("f32", def.position.x);
            this.nativeWriteOpArg("f32", def.position.y);
        }

        if (def.angle) {
            state |= 8;
            this.nativeWriteOpArg("f32", def.angle);
        }

        if (def.linearVelocity) {
            state |= 16;
            this.nativeWriteOpArg("f32", def.linearVelocity.x);
            this.nativeWriteOpArg("f32", def.linearVelocity.y);
        }

        if (def.angularVelocity) {
            state |= 32;
            this.nativeWriteOpArg("f32", def.angularVelocity);
        }

        if (def.color) {
            state |= 64;
            this.nativeWriteOpArg("ui8", def.color.r);
            this.nativeWriteOpArg("ui8", def.color.g);
            this.nativeWriteOpArg("ui8", def.color.b);
            this.nativeWriteOpArg("ui8", def.color.a);
        }

        if (def.strength) {
            state |= 128;
            this.nativeWriteOpArg("f32", def.strength);
        }

        if (def.stride) {
            state |= 256;
            this.nativeWriteOpArg("f32", def.stride);
        }

        if (def.particleCount) {
            state |= 512;
            this.nativeWriteOpArg("i32", def.particleCount);
        }

        if (def.lifetime) {
            state |= 1024;
            this.nativeWriteOpArg("f32", def.lifetime);
        }

        if (def.group) {
            state |= 2048;
            this.nativeWriteOpArg("i8", def.group);
        }

        let nativeObj = this.getNativeObject();
        nativeObj.rewriteArg(statePos, "i32", state);
        this.nativeEndOp();
    }

    // protected override _onMaterialModified() {
    //     this.nativeBeginOp(2);
    //     this.nativeWriteOpArg("i32", this.sharedMaterial.getId());
    //     this.nativeEndOp();
    // }

    // public setWaterDataMaterial(material: Material) {
    //     if (this.waterRenderMaterial === material) {
    //         return;
    //     }
    //     this.waterRenderMaterial = material;
    //     this.nativeBeginOp(4);
    //     this.nativeWriteOpArg("i32", material.getId());
    //     this.nativeEndOp();
    // }

    public setGravityScale(scale: number) {
        this.nativeBeginOp(5);
        this.nativeWriteOpArg("f32", scale);
        this.nativeEndOp();
    }



    public setParticleFlags(particleIndex: number, newMaskFlags: number) {
        this.nativeBeginOp(6);
        this.nativeWriteOpArg("i32", particleIndex);
        this.nativeWriteOpArg("ui32", newMaskFlags);
        this.nativeEndOp();
    }

    ApplyLinearImpulseInRang(firstParticleIndex: number, lastParticleIndex: number, force: Vec2): void {
        this.nativeBeginOp(7);
        this.nativeWriteOpArg("i32", firstParticleIndex);
        this.nativeWriteOpArg("i32", lastParticleIndex);
        this.nativeWriteOpArg("f32", force.x);
        this.nativeWriteOpArg("f32", force.y);
        this.nativeEndOp();;
    }

    applyForceInRang(firstParticleIndex: number, lastParticleIndex: number, force: Vec2): void {
        this.nativeBeginOp(8);
        this.nativeWriteOpArg("i32", firstParticleIndex);
        this.nativeWriteOpArg("i32", lastParticleIndex);
        this.nativeWriteOpArg("f32", force.x);
        this.nativeWriteOpArg("f32", force.y);
        this.nativeEndOp();
    }

    /**
     * 设置粘性系数。需要事先开启粒子粘性模拟
     * @param v 
     */
    setViscousStrength(v: number): void {
        this.nativeBeginOp(9);
        this.nativeWriteOpArg("f32", v);
        this.nativeEndOp();
    }

    setSurfaceTensionPressureStrength(v: number) {
        this.nativeBeginOp(10);
        this.nativeWriteOpArg("f32", v);
        this.nativeEndOp();
    }

    setSurfaceTensionNormalStrength(v: number) {
        this.nativeBeginOp(11);
        this.nativeWriteOpArg("f32", v);
        this.nativeEndOp();
    }

    createParticle(def: ParticlePhysicsCreateDefine) {
        this.nativeBeginOp(12, true);

        let state = 0;
        let pos = this.nativeWriteOpArg("ui32", state);

        if (def.count) {
            state |= 1 << 1;
            this.nativeWriteOpArg("i32", def.count);
        }

        if (def.flags) {
            state |= 1 << 2;
            this.nativeWriteOpArg("ui32", def.flags);
        }

        if (def.lifetime) {
            state |= 1 << 3;
            this.nativeWriteOpArg("i32", def.lifetime);
        }

        if (def.position) {
            state |= 1 << 4;
            this.nativeWriteOpArg("f32", def.position.x);
            this.nativeWriteOpArg("f32", def.position.y);
        }

        if (def.velocity) {
            state |= 1 << 5;
            this.nativeWriteOpArg("f32", def.velocity.x);
            this.nativeWriteOpArg("f32", def.velocity.y);
        }

        if (def.color) {
            state |= 1 << 6;
            this.nativeWriteOpArg("ui8", def.color.r * 255 | 0);
            this.nativeWriteOpArg("ui8", def.color.g * 255 | 0);
            this.nativeWriteOpArg("ui8", def.color.b * 255 | 0);
            this.nativeWriteOpArg("ui8", def.color.a * 255 | 0);
        }
        let nativeObj = this.getNativeObject();
        nativeObj.rewriteArg(pos, "ui32", state);
        this.nativeEndOp();
    }

    setParticleRadius(radius: number): void {
        this.nativeBeginOp(13);
        this.nativeWriteOpArg("f32", radius);
        this.nativeEndOp();
    }

    setParticleRenderRadius(radius: number): void {
        this.nativeBeginOp(14);
        this.nativeWriteOpArg("f32", radius);
        this.nativeEndOp();
    }

    allocate(count: number) {
        this.nativeBeginOp(15);
        this.nativeWriteOpArg("i32", count);
        this.nativeEndOp();
    }

    setPressureStrength(value: number) {
        this.nativeBeginOp(16);
        this.nativeWriteOpArg("f32", value);
        this.nativeEndOp();
    }

    setDestructionByAge(b: boolean) {
        this.nativeBeginOp(17);
        this.nativeWriteOpArg("i8", b);
        this.nativeEndOp();
    }

    destroyParticle(particleIndex: number | Array<number>) {
        this.nativeBeginOp(18);
        if (typeof particleIndex === "number") {
            particleIndex = [particleIndex];
        }
        this.nativeWriteOpArg("i32", particleIndex.length);
        for (let i = 0, n = particleIndex.length; i < n; ++i) {
            this.nativeWriteOpArg("i32", particleIndex[i]);
        }
        this.nativeEndOp();
    }

    /**
     * 当有粒子碰撞到刚体是触发这个函数
     * @param callback null表示清除监听
     */
    set onBeginCantactBodyHandler(callback: (contact: ContactManifold) => void | null) {
        this.enableBodyContact(!!callback || !!this._onEndContactBodyCallback);
        this._onBeginContactBodyCallback = callback;
    }

    set onEndCantactBodyHandler(callback: (contact: ContactManifold) => void | null) {
        this.enableBodyContact(!!callback || !!this._onBeginContactBodyCallback);
        this._onEndContactBodyCallback = callback;
    }

    private enableBodyContact(b: boolean) {
        if (b) {
            if (this.enableBodyContactListener) return;
            this.enableBodyContactListener = true;
            this.writeEnableContactListener(true, 1);
        } else {
            if (!this.enableBodyContactListener) return;
            this.enableBodyContactListener = false;
            this.writeEnableContactListener(false, 1);
        }
    }

    set onBeginContactParticleHandler(callback: () => void) {
        this.enableParticleContact(!!callback || !!this._onEndContactParticleCallback);
        this._onBeginContactParticleCallback = callback;
    }

    set onEndContactParticleHandler(callback: (particleIdA: number, particleIdB: number) => void) {
        this.enableParticleContact(!!callback || !!this._onBeginContactParticleCallback);
        this._onEndContactParticleCallback = callback;
    }

    private enableParticleContact(b: boolean) {
        if (b) {
            if (this.enableParticleContactListener) return;
            this.enableParticleContactListener = true;
            this.writeEnableContactListener(true, 2);
        } else {
            if (!this.enableParticleContactListener) return;
            this.enableParticleContactListener = false;
            this.writeEnableContactListener(false, 2);
        }
    }
    private writeEnableContactListener(b: boolean, type: number) {
        this.nativeBeginOp(19);
        this.nativeWriteOpArg("i8", b ? 1 : 0);
        this.nativeWriteOpArg("i8", type);
        this.nativeEndOp();
    }

    setBodyContactFilter(categoryBits: number, maskBits: number): void {
        this.nativeBeginOp(20);
        this.nativeWriteOpArg("ui32", categoryBits);
        this.nativeWriteOpArg("ui32", maskBits);
        this.nativeEndOp();
    }
}