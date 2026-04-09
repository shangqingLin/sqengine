import { ComponentType } from "../../../native_binding";
import { AssetManager, ImageAsset, IMemoryImageSource, Material, SpriteFrame, Texture2d } from "../../../assets";
import { TextureFilter, Format, DeviceManager } from "../../../gfx/index";
import { Color, js, Mat3, Path, serializable, SQBoolean, sqclass, sqenum, SQFloat, SQInteger, SQString, TextureUtils, type, Vec2 } from "../../../core";
import { Application } from "../../../framework/Application";
import ParticleShader from "./ParticleShader";
import { MacroRecord } from "../../../assets";
import buidinResManager from "../../../assets/assetmanager/BuildinResManager";
import {RenderComponent} from "../../../framework/component/RenderComponent";
import { Deserialize } from "script/engine/sq/serialization";

export enum ParticleEmitterShape {
    //默认
    POINT,
    BOX,
    RING,
    SPHERE,
    POINTS //在一个图片存储顶点，粒子在这个图片取样，从而实现你想要的任何形状
}


export enum Particle2DColorGradientMixType {
    ADD = 1, //颜色相加
    LERP = 2 //颜色插值
}

export interface Particle2DColorGradientDefine {
    color: string | Color; //十六进制的颜色值
    p: number //多个进度加起来必须为1
    type?: Particle2DColorGradientMixType;
};

export type CurvePoint = {
    x: number;
    y: number;

    //通过调整切线方向来调整贝塞尔曲线
    //知道为角度，不支持小数
    tangentRightAngle?: number;
    tangentLeftAngle?: number;
} & Record<string, any>;

export interface Particle2DCurveDefine {
    p1: CurvePoint,
    p2: CurvePoint,
    p3?: CurvePoint,
    p4?: CurvePoint
};

export enum AttractorShapeType
{
    BOX = 1,
    SPHERE
};

@sqclass("sq.Particle2DComponent")
export class Particle2DComponent extends RenderComponent {

    private propertyTexture: Texture2d;
    private particleRenderTexture: Texture2d;
    private texturePoints: Texture2d;
    private propertyTextureData: Float32Array;
    private updateProcessMaterial: Material;
    private renderMaterial: Material;
    private renderSceneMarkMaterial: Material;
    private useCPURender: boolean;

    @type(ParticleEmitterShape)
    @serializable
    private emitterShape: ParticleEmitterShape;


    @type(SQInteger)
    @serializable
    private amount: number


    @type(SQFloat)
    @serializable
    private amountRatio: number

    @type(SQBoolean)
    @serializable
    private emit: number;

    @type(SQBoolean)
    @serializable
    private oneShot: boolean;

    @type(SQFloat)
    @serializable
    private lifeTime: number;

    @type(SQFloat)
    @serializable
    private seed: number;

    @type(SQFloat)
    @serializable
    private shapeRadius: number


    @type(SQFloat)
    @serializable
    private ringShapeHeight: number


    @type(SQFloat)
    @serializable
    private ringShapeConeAngle: number

    @type(Vec2)
    @serializable
    private emitRingShapeAxis: Vec2;

    @type(SQFloat)
    @serializable
    private ringShapeInnerRadius: number;

    @type(Vec2)
    @serializable
    private emitBoxExtents: Vec2;


    @type(SQFloat)
    @serializable
    private spreadAngle: number

    @type(Vec2)
    @serializable
    private emitSpreadDirection: Vec2;

    @type(SQFloat)
    @serializable
    private inheritEmitterVelocityRatio: number

    @type(Vec2)
    @serializable
    private initMinMaxVelocityMuti: Vec2;

    @type(Vec2)
    @serializable
    private minMaxLinearAccel: Vec2;

    @type(Vec2)
    @serializable
    private minMaxRadiaAccel: Vec2;

    @type(Vec2)
    @serializable
    private minMaxTangentAccel: Vec2;

    @type(Vec2)
    @serializable
    private minMaxDamping: Vec2;

    @type(Vec2)
    @serializable
    private minMaxAngularVelocity: Vec2;

    @type(Vec2)
    @serializable
    private minMaxDirectionalVelocity: Vec2;

    @type(Vec2)
    @serializable
    private minMaxRadialVelocity: Vec2;

    @type(Vec2)
    @serializable
    private minMaxOrbitVelocity: Vec2;

    @type(Vec2)
    @serializable
    private minMaxTurbulenceInfluence: Vec2;


    @type(Vec2)
    @serializable
    private minMaxInitialAngle: Vec2;

    @type(Vec2)
    @serializable
    private gravity: Vec2;

    @type(Vec2)
    @serializable
    private velocityPivot: Vec2;

    @type(Vec2)
    @serializable
    private shapeOffset: Vec2;

    @type(Vec2)
    @serializable
    private minMaxScale: Vec2;

    @type(Vec2)
    @serializable
    private renderSize: Vec2;

    @type(Array)
    @serializable
    private pathPoints: Array<number>;

    @type(SQBoolean)
    @serializable
    private hasPointPathNormal: boolean;

    @type(SQInteger)
    @serializable
    private pointPathNormalType: number;

    @type(Array)
    @serializable
    private points: Array<number>;

    @type(SQString)
    @serializable
    private particleTexture: string;

    constructor() {
        super(ComponentType.Particle2DComponent);

        /**
       * 要使用GPU计算，必须支持RenderTexture可以设置为32位浮点数，有些平台不支持，那么就只能
       * 切换到CPU端计算
       */
        this.setUseCPURender(!DeviceManager.ins.device.caps.support32FloatRenderTexture);
    }

    protected override onInitialize(): void {
        this.InitPathPoint();
    }

    protected override onEnabled(): void {
        this.createPropertyTexture();
    }

    protected override onRemove(): void {
        if (this.propertyTexture) {
            this.propertyTexture.destroy();
        }

        if (this.updateProcessMaterial) {
            this.updateProcessMaterial.destroy();
        }

        if (this.renderMaterial) {
            this.renderMaterial.destroy();
        }

        if (this.renderSceneMarkMaterial) {
            this.renderSceneMarkMaterial.destroy();
        }

        if (this.texturePoints) {
            this.texturePoints.destroy();
        }
    }

    private deserialize_amount(sValue: any, deserialize: Deserialize) {
        this.setAmount(sValue);
    }

    setAmount(amount: number): void {
        this.nativeBeginOp(51);
        this.nativeWriteOpArg("i32", amount);
        this.nativeEndOp();
    }

    private deserialize_amountRatio(sValue: any, deserialize: Deserialize) {
        this.setAmountRatio(sValue);
    }

    setAmountRatio(ratio: number) {
        this.nativeBeginOp(86);
        this.nativeWriteOpArg("f32", ratio);
        this.nativeEndOp();
    }

    private deserialize_seed(sValue: any, deserialize: Deserialize) {
        this.setSeed(sValue);
    }

    setSeed(seed: number) {
        this.nativeBeginOp(90);
        this.nativeWriteOpArg("f32", seed);
        this.nativeEndOp();
    }

    //==========设置速度更新相关属性====================

    private deserialize_spreadAngle(sValue: any, deserialize: Deserialize) {
        this.setEmitSpreadAngle(sValue);
    }

    setEmitSpreadAngle(angle: number): void {
        this.nativeBeginOp(52);
        this.nativeWriteOpArg("f32", angle);
        this.nativeEndOp();
    }

    private deserialize_emitSpreadDirection(sValue: Vec2, deserialize: Deserialize) {
        this.setEmitSpreadDirection(sValue.x, sValue.y);
    }

    setEmitSpreadDirection(x: number, y: number): void {
        this.nativeBeginOp(55);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    private deserialize_initMinMaxVelocityMuti(sValue: Vec2, deserialize: Deserialize) {
        this.setEmitInitMinMaxVelocityMuti(sValue.x, sValue.y);
    }

    setEmitInitMinMaxVelocityMuti(min: number, max: number): void {
        this.nativeBeginOp(53);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_inheritEmitterVelocityRatio(sValue: number, deserialize: Deserialize) {
        this.setInheritEmitterVelocityRatio(sValue);
    }
    setInheritEmitterVelocityRatio(ratio: number) {
        this.nativeBeginOp(89);
        this.nativeWriteOpArg("f32", ratio);
        this.nativeEndOp();
    }

    private deserialize_minMaxLinearAccell(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxLinearAccel(sValue.x, sValue.y);
    }

    setMinMaxLinearAccel(min: number, max: number): void {
        this.nativeBeginOp(54);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_minMaxRadiaAccel(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxRadialAccel(sValue.x, sValue.y);
    }

    setMinMaxRadialAccel(min: number, max: number): void {
        this.nativeBeginOp(56);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_minMaxTangentAccel(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxTangentAccel(sValue.x, sValue.y);
    }
    setMinMaxTangentAccel(min: number, max: number): void {
        this.nativeBeginOp(57);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_minMaxDamping(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxDamping(sValue.x, sValue.y);
    }

    setMinMaxDamping(min: number, max: number): void {
        this.nativeBeginOp(58);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_minMaxAngularVelocity(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxAngularVelocity(sValue.x, sValue.y);
    }

    setMinMaxAngularVelocity(min: number, max: number): void {
        this.nativeBeginOp(59);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_minMaxDirectionalVelocity(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxDirectionalVelocity(sValue.x, sValue.y);
    }

    setMinMaxDirectionalVelocity(min: number, max: number): void {
        this.nativeBeginOp(60);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_minMaxRadialVelocity(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxRadialVelocity(sValue.x, sValue.y);
    }
    setMinMaxRadialVelocity(min: number, max: number): void {
        this.nativeBeginOp(62);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_minMaxOrbitVelocity(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxOrbitVelocity(sValue.x, sValue.y);
    }
    setMinMaxOrbitVelocity(min: number, max: number): void {
        this.nativeBeginOp(61);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_minMaxTurbulenceInfluence(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxOrbitVelocity(sValue.x, sValue.y);
    }
    setMinMaxTurbulenceInfluence(min: number, max: number): void {
        this.nativeBeginOp(63);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_minMaxInitialAngle(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxInitialAngle(sValue.x, sValue.y);
    }
    setMinMaxInitialAngle(min: number, max: number): void {
        this.nativeBeginOp(64);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private deserialize_gravity(sValue: Vec2, deserialize: Deserialize) {
        this.setGravity(sValue.x, sValue.y);
    }
    setGravity(x: number, y: number): void {
        this.nativeBeginOp(65);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    private deserialize_velocityPivot(sValue: Vec2, deserialize: Deserialize) {
        this.setVelocityPivot(sValue.x, sValue.y);
    }
    setVelocityPivot(x: number, y: number): void {
        this.nativeBeginOp(66);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    //============设置发射阶段相关的属性====================
    private deserialize_emitterShape(sValue: any, deserialize: Deserialize) {
        this.setEmitShape(sValue);
    }

    setEmitShape(shape: ParticleEmitterShape): void {
        if (shape === this.emitterShape) {
            return;
        }

        if (!this.useCPURender) {
            if (shape === ParticleEmitterShape.POINTS) {
                this.processPointsShape();
            } else {
                this.processOtherEmitShape();
            }
        }

        this.emitterShape = shape;
        this.nativeBeginOp(67);
        this.nativeWriteOpArg("i8", shape);
        this.nativeEndOp();
    }

    private processPointsShape() {
        let emitShapeName: string = "EMIT_SHAPE_POINTS";
        if (this.updateProcessMaterial && this.updateProcessMaterial.hasDefine(emitShapeName)) {
            return;
        }
        let macros = js.createMap() as MacroRecord;
        macros[emitShapeName] = 1;
        this.setUpdateProcessMaterial(ParticleShader.getParticleGPUUpdateMaterial(macros));
        if (!this.renderMaterial) this.setRenderMaterial(ParticleShader.getParticleGPURenderMaterial());
    }

    private processOtherEmitShape() {
        let emitShapeName: string = "EMIT_SHAPE_NORMAL";
        if (this.updateProcessMaterial && this.updateProcessMaterial.hasDefine(emitShapeName)) {
            return;
        }
        let macros = js.createMap() as MacroRecord;
        macros[emitShapeName] = 1;
        this.setUpdateProcessMaterial(ParticleShader.getParticleGPUUpdateMaterial(macros));
        if (!this.renderMaterial) this.setRenderMaterial(ParticleShader.getParticleGPURenderMaterial());
    }

    setUseCPURender(b: boolean) {
        if (this.useCPURender == b) return;
        this.useCPURender = b;
        if (this.useCPURender) {
            if (this.updateProcessMaterial) {
                this.updateProcessMaterial.destroy();
                this.renderMaterial.destroy();
                this.updateProcessMaterial = null;
                this.renderMaterial = null
            }
            this.setRenderMaterial(ParticleShader.getParticleCPURenderMateria());
        } else {
            if (this.renderMaterial) {
                this.renderMaterial.destroy();
                this.renderMaterial = null;
            }
        }
    }


    private deserialize_shapeOffset(sValue: Vec2, deserialize: Deserialize) {
        this.setEmitShapeOffset(sValue.x, sValue.y);
    }
    setEmitShapeOffset(x: number, y: number): void {
        this.nativeBeginOp(68);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    private deserialize_shapeRadius(sValue: number, deserialize: Deserialize) {
        this.setEmitShapeRadius(sValue);
    }
    setEmitShapeRadius(radius: number): void {
        this.nativeBeginOp(69);
        this.nativeWriteOpArg("f32", radius);
        this.nativeEndOp();
    }
    private deserialize_ringShapeHeight(sValue: number, deserialize: Deserialize) {
        this.setEmitRingShapeHeight(sValue);
    }
    setEmitRingShapeHeight(height: number): void {
        this.nativeBeginOp(70);
        this.nativeWriteOpArg("f32", height);
        this.nativeEndOp();
    }

    private deserialize_ringShapeConeAngle(sValue: number, deserialize: Deserialize) {
        this.setEmitRingShapeConeAngle(sValue);
    }
    setEmitRingShapeConeAngle(angle: number): void {
        this.nativeBeginOp(71);
        this.nativeWriteOpArg("f32", angle);
        this.nativeEndOp();
    }

    private deserialize_ringShapeInnerRadius(sValue: number, deserialize: Deserialize) {
        this.setEmitRingShapeInnerRadius(sValue);
    }
    setEmitRingShapeInnerRadius(radius: number): void {
        this.nativeBeginOp(72);
        this.nativeWriteOpArg("f32", radius);
        this.nativeEndOp();
    }

    private deserialize_emitRingShapeAxis(sValue: Vec2, deserialize: Deserialize) {
        this.setEmitRingShapeAxis(sValue.x, sValue.y);
    }
    setEmitRingShapeAxis(x: number, y: number): void {
        this.nativeBeginOp(73);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    private deserialize_emitBoxExtents(sValue: Vec2, deserialize: Deserialize) {
        this.setEmitBoxExtents(sValue.x, sValue.y);
    }
    setEmitBoxExtents(widthRadius: number, heightRadius: number) {
        this.nativeBeginOp(91);
        this.nativeWriteOpArg("f32", widthRadius);
        this.nativeWriteOpArg("f32", heightRadius);
        this.nativeEndOp();
    }

    private deserialize_lifeTime(sValue: number, deserialize: Deserialize) {
        this.setLifeTime(sValue);
    }
    setLifeTime(time: number): void {
        this.nativeBeginOp(74);
        this.nativeWriteOpArg("f32", time);
        this.nativeEndOp();
    }

    private deserialize_particleTexture(sValue: string, deserialize: Deserialize) {
        this.setParticleTextureUrl(sValue);
    }

    setParticleTextureUrl(url: string) {
        AssetManager.getInstance().load(url, (error, texture: Texture2d) => {
            if (error) return;
            this.setParticleTexture(texture);
        });
    }

    setParticleTexture(texture: Texture2d): void {
        if (this.particleRenderTexture == texture) return;
        this.particleRenderTexture = texture;
        this.nativeBeginOp(75);
        this.nativeWriteOpArg("i32", texture.getId());
        this.nativeEndOp();
    }

    private deserialize_emit(sValue: boolean, deserialize: Deserialize) {
        this.setEmit(sValue);
    }

    setEmit(b: boolean): void {
        this.nativeBeginOp(76);
        this.nativeWriteOpArg("i8", b);
        this.nativeEndOp();
    }

    private deserialize_oneShot(sValue: boolean, deserialize: Deserialize) {
        this.setOneShot(sValue);
    }
    setOneShot(b: boolean): void {
        this.nativeBeginOp(77);
        this.nativeWriteOpArg("i8", b ? 1 : 0);
        this.nativeEndOp();
    }

    //===========渲染相关=====================

    private deserialize_minMaxScale(sValue: Vec2, deserialize: Deserialize) {
        this.setMinMaxScale(sValue.x, sValue.y);
    }
    setMinMaxScale(min: number, max: number): void {
        this.nativeBeginOp(79);
        this.nativeWriteOpArg("f32", min);
        this.nativeWriteOpArg("f32", max);
        this.nativeEndOp();
    }

    private setUpdateProcessMaterial(materia: Material): void {
        if (this.updateProcessMaterial === materia) return;
        if (this.updateProcessMaterial) this.updateProcessMaterial.destroy();
        this.updateProcessMaterial = materia;
        this.nativeBeginOp(80);
        this.nativeWriteOpArg("i32", materia.getId());
        this.nativeEndOp();
    }

    private setRenderMaterial(materia: Material): void {
        if (this.renderMaterial === materia) return;
        if (this.renderMaterial) this.renderMaterial.destroy();
        this.renderMaterial = materia;
        // console.info("js setRenderMaterial", materia.getId());
        this.nativeBeginOp(81);
        this.nativeWriteOpArg("i8", this.useCPURender ? 1 : 0);
        this.nativeWriteOpArg("i32", materia.getId());
        this.nativeEndOp();

        if (!this.particleRenderTexture) {
            //使用默认的Texture渲染
            this.setParticleTexture(buidinResManager.getWhiteColortexture());
            this.particleRenderTexture = null;
        }
    }

    public setEnableBlendNotBg() {
        if (!this.renderSceneMarkMaterial) this.setRenderSceneMarkMaterial(ParticleShader.getParticleRenderMarkSceneMaterial());
    }

    private setRenderSceneMarkMaterial(materia: Material) {
        if (this.renderSceneMarkMaterial === materia) return;
        if (this.renderSceneMarkMaterial) this.renderSceneMarkMaterial.destroy();
        this.renderSceneMarkMaterial = materia;
        this.nativeBeginOp(88);
        this.nativeWriteOpArg("i32", materia.getId());
        this.nativeEndOp();
    }

    private InitPathPoint() {
        if (this.pathPoints) {
            this.setPathPoints(this.pathPoints);
            delete this.pathPoints;
        } else if (this.points) {
            this.setPoints(this.points);
            delete this.points;
        }
    }

    setPathPoints(pathPoints:Array<number> | Float32Array,hasPointPathNormal:boolean = false,pointPathNormalType?:number)
    {

        if(pathPoints.length < 4) throw new Error();

        this.hasPointPathNormal = hasPointPathNormal;
        this.pointPathNormalType = pointPathNormalType;
        let path = new Path();
        path.moveTo(pathPoints[0], pathPoints[1]);
        for (let i = 2; i < pathPoints.length; i += 2) {
            path.lineTo(pathPoints[i], pathPoints[i + 1]);
        }
        let points = path.sampleStrokePoints(2, this.hasPointPathNormal, this.pointPathNormalType);
        this.setEmitTexturePointEnableNormal(this.hasPointPathNormal);
        this.setPoints(points);
    }

    setPoints(points: Float32Array | Array<number>) {


        let pointsCount = this.hasPointPathNormal ? points.length * 0.25 : points.length * 0.5;
        this.setEmitTexturePointCount(pointsCount);

        if(this.useCPURender){
            if(this.texturePoints){
                this.texturePoints.destroy();
                this.texturePoints = null;
            }
            return;
        }

        
        let pixlesNum: number = this.hasPointPathNormal ? pointsCount : Math.ceil(pointsCount * 0.5);
        let size = TextureUtils.calculateTextureSize(pixlesNum, false);
        if (!this.texturePoints) {
            this.texturePoints = new Texture2d();
            this.texturePoints.setFilters(TextureFilter.NEAREST, TextureFilter.NEAREST);
            this.texturePoints.create(size.width, size.height, Format.RGBA32F);
            this.setEmitTexturePoints(this.texturePoints);
        }

        // console.info("point count",pointsCount);
        // console.info(points);
        // console.info(size.width,size.height);

        //纹理数据上传的数据不能小于实际像素需要的数据
        let num = size.width * size.height * 4;
        if (points.length < num) {
            let points1 = new Float32Array(num);
            points1.fill(0);
            points1.set(points);
            points = points1;
        }
        let imgSource = new ImageAsset();
        imgSource.source = {
            _data: points,
            width: size.width,
            height: size.height
        } as IMemoryImageSource;
        this.texturePoints.setBufferData(imgSource);
    }

    setEmitTexturePoints(texture: Texture2d): void {
        if(this.useCPURender) throw Error("CPU模式不支持");
        if (this.emitterShape !== ParticleEmitterShape.POINTS) {
            throw new Error("请先设置发射类型设置为：ParticleEmitterShape.POINTS");
        }
        this.nativeBeginOp(82);
        this.nativeWriteOpArg("i32", texture.getId());
        this.nativeEndOp();
    }

    setEmitPointsBuffer(points:Array<number> | Float32Array)
    {
        if(!this.useCPURender) throw Error("GPU模式不支持");

    }

    private deserializ_renderSize(sValue: Vec2, deserialize: Deserialize) {
        this.setPartilceRenderSize(sValue.x, sValue.y);
    }
    setPartilceRenderSize(width: number, height: number) {
        this.nativeBeginOp(83);
        this.nativeWriteOpArg("f32", width);
        this.nativeWriteOpArg("f32", height);
        this.nativeEndOp();
    }

    setEmitTexturePointCount(count: number) {
        this.nativeBeginOp(84);
        this.nativeWriteOpArg("f32", count);
        this.nativeEndOp();
    }

    setEmitTexturePointEnableNormal(b: boolean) {
        this.nativeBeginOp(85);
        this.nativeWriteOpArg("i8", b ? 1 : 0);
        this.nativeEndOp();
    }


    /**
  * 设置颜色。这个颜色和当前节点的纹理颜色相乘
  * 设置的为[0,1]区间的浮点值
  * @param color 
  */
    public setColor(color: Color | string) {
        this.nativeBeginOp(87);

        let color1: Color = color as Color;
        if (typeof color === "string") {
            color1 = Color.strHexToColor(color);
            color1.r /= 255;
            color1.g /= 255;
            color1.b /= 255;
            color1.a /= 255;
        }
        this.nativeWriteOpArg("f32", color1.r);
        this.nativeWriteOpArg("f32", color1.g);
        this.nativeWriteOpArg("f32", color1.b);
        this.nativeWriteOpArg("f32", color1.a);
        this.nativeEndOp();
    }

    /**
   * 设置缩放的三阶贝塞尔插值数据
   * 所有的Point中的数值要求的范围是[0,1]
  * @param beginPoint 起点
  * @param endPoint 终点
  * @param c1 第一个过曲线上的点
  * @param c2 第二个过曲线上的点
  */
    setScaleCurve(define: Particle2DCurveDefine) {
        this.createPropertyTexture();
        this.writeCurveToData(0, define);
        Application.ins.timer.callLater(this, this.onUpdatePropertyData);
    }

    /**
     * 设置旋转的三阶贝塞尔插值数据
     * 所有的Point中的数值要求的范围是[0,1]
     * @param beginPoint 
     * @param endPoint 
     * @param c1 
     * @param c2 
     */
    setRotateCurve(define: Particle2DCurveDefine) {
        this.createPropertyTexture();
        this.writeCurveToData(16, define);
        Application.ins.timer.callLater(this, this.onUpdatePropertyData);
    }

    /**
     * 添加需要插值的颜色，目前支持4最多四个颜色值。
     * @param hexColor 
     * @param time time加起来必须为1
     */
    setColorGradient(gradients: Array<Particle2DColorGradientDefine>) {
        this.createPropertyTexture();
        gradients.sort((a1: Particle2DColorGradientDefine, a2: Particle2DColorGradientDefine) => {
            return a1.p - a2.p;
        });

        let numColor: number = 4;
        let begin = 32;
        let timeOffset = begin + numColor * 4;
        let data: Float32Array = this.propertyTextureData;
        for (let index = 0; index < numColor; ++index) {
            let offset = begin + index * 4;
            let config = gradients[index];
            if (config) {
                let color: Color = typeof config.color === "string" ? Color.strHexToColor(config.color) : config.color;
                data[offset] = color.r / 255;
                data[offset + 1] = color.g / 255;
                data[offset + 2] = color.b / 255;
                data[offset + 3] = color.a / 255;

                //记录进度时间
                offset = timeOffset + index;
                data[offset] = config.p;
            } else {
                data[offset] =
                    data[offset + 1] =
                    data[offset + 2] =
                    data[offset + 3] = 0;

                offset = timeOffset + index;
                data[offset] = 0;
            }
        }

        // let a = this.propertyTextureData.slice(24);
        Application.ins.timer.callLater(this, this.onUpdatePropertyData);
    }


    private writeCurveToData(offset: number, define: Particle2DCurveDefine) {
        //一个顶点使用一个像素，所以一条曲线使用4个像素
        this.writeCurvePoint(offset, define.p1);
        this.writeCurvePoint(offset + 4, define.p2);
        this.writeCurvePoint(offset + 8, define.p3);
        this.writeCurvePoint(offset + 12, define.p4);
    }

    private writeCurvePoint(start: number, point: CurvePoint | null) {
        let data: Float32Array = this.propertyTextureData;

        if (point) {
            data[start] = point.x;
            data[start + 1] = point.y;
            data[start + 2] = point.tangentLeftAngle;
            data[start + 3] = point.tangentRightAngle;
        } else {
            data[start] =
                data[start + 1] =
                data[start + 2] =
                data[start + 3] = 0;
        }
    }

    private onUpdatePropertyData() {
        let source: IMemoryImageSource = {
            _data: this.propertyTextureData,
            width: this.propertyTexture.width,
            height: 1
        }
        let imageAsset = new ImageAsset();
        imageAsset.source = source;
        this.propertyTexture.setBufferData(imageAsset, false);

        // console.info(this.propertyTexture.width +" ==== "+this.propertyTextureData.join(","));
    }


    private createPropertyTexture() {
        if (this.propertyTexture) return;

        let propertyTexture = new Texture2d();
        //数组的访问方式，不插值
        propertyTexture.setFilters(TextureFilter.NEAREST, TextureFilter.NEAREST);

        /**
         * scale: 4 point x 4 = 16 个数字
         * rotation:4 point x 4 = 16 个数字
         * colorNUm : 4 + 4 + 4 + 4 4个像素
         * time: 4 1个像素
         * colorFlag: 4个数字 1个像素
         * 
         * pixlesNum = 40 / 4
         */
        let pixlesNum = 14;
        this.propertyTextureData = new Float32Array(pixlesNum * 4);
        this.propertyTextureData.fill(0);
        propertyTexture.create(pixlesNum, 1, Format.RGBA32F);
        this.propertyTexture = propertyTexture;
        this.nativeBeginOp(78);
        this.nativeWriteOpArg("i32", propertyTexture.getId());
        this.nativeEndOp();
        this.onUpdatePropertyData();
    }

    setAttractorShape(type:AttractorShapeType)
    {
        this.nativeBeginOp(93);
        this.nativeWriteOpArg("i8", type);
        this.nativeEndOp();
    }


    setAttractorShapeTransform(mat:Mat3)
    {
        this.nativeBeginOp(94);
        this.nativeWriteOpArg("f32", mat.data[0]);
        this.nativeWriteOpArg("f32", mat.data[1]);
        this.nativeWriteOpArg("f32", mat.data[2]);
        this.nativeWriteOpArg("f32", mat.data[3]);
        this.nativeWriteOpArg("f32", mat.data[4]);
        this.nativeWriteOpArg("f32", mat.data[5]);
        this.nativeEndOp();
    }

    setAttractorAttenuation(attenuation:number)
    {
        this.nativeBeginOp(95);
        this.nativeWriteOpArg("f32", attenuation);
        this.nativeEndOp();
    }

    setAttractorStrength(strength:number)
    {
        this.nativeBeginOp(96);
        this.nativeWriteOpArg("f32", strength);
        this.nativeEndOp();
    }

    setAttractorDirectionality(directionality:number)
    {

    }

}