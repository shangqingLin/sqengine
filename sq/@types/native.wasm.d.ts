/**
 * JavaScript与原生桥接的接口声明
 */


declare var __ATINIT__: Array<Function>;

declare function _free(pointer: number): void;
declare function _malloc(size: number): number;

declare module Module {

   function destroy(obj: NativeBaseObj): void;
   function setValue(ptr: number, value: number, type: string): void;
   function getValue(ptr: number, type: string): number;
   function UTF8ToString(ptr: number): string;
   function ccall(name: string, returnvalue?: any, argsType?: Array<string>, args?: Array<any>): any;


   var HEAP8: Int8Array;
   var HEAP16: Int16Array;
   var HEAPU8: Uint8Array;
   var HEAPU16: Uint16Array;
   var HEAP32: Int32Array;
   var HEAPU32: Uint32Array;
   var HEAPF32: Float32Array;
   var HEAPF64: Float64Array;
   var calledRun: boolean;

   class NativeBaseObj {

   }

   class IUniform extends NativeBaseObj {
      setName(name: string): void;
      type: number;
      count: number;
   }

   class IBlockInfo extends NativeBaseObj {
      binding: number;
      count: number;
      buildin: boolean;
      pushMembers(uniform: IUniform): void;
      setBlockName(name: string): void;
   }

   class ISamplerTextureInfo extends NativeBaseObj {
      setName(name: string): void;
      binding: number;
      type: number;
      count: number;
      buildin: boolean;
   }

   class IAttributeInfo extends NativeBaseObj {
      format: number;
      setName(name: string): void;
   }

   class IShaderDefine extends NativeBaseObj {
      pushBlock(block: IBlockInfo): void;
      pushAttribute(attribute: IAttributeInfo): void;
      pushSamplerTexture(texture: ISamplerTextureInfo): void;
      pushMacro(name: string, flag: number): void;
   }

   class IPassInfo extends NativeBaseObj {
      shaderIndex: number;
      passStateBuffer: number;
   }

   class IEffectDefine extends NativeBaseObj {
      pushShaderDefine(info: IShaderDefine): void;
      pushPassInfo(info: IPassInfo): void;
   }


   class RasterizerStateInfo extends NativeBaseObj {
      cullMode: number;
      isFrontFaceCCW: boolean;
      depthBias: number;
      depthBiasSlop: number;
      lineWidth: number;
   }

   class DepthStateInfo extends NativeBaseObj {
      depthTest: boolean;
      depthWrite: boolean;
      depthFunc: any;
   }

   class StencilStateInfo extends NativeBaseObj {
      stencilTestFront: boolean;
      stencilTestBack: boolean;
      stencilRefBack: number;
      stencilFuncFront: any;
      stencilReadMaskFront: number;
      stencilWriteMaskFront: number;
      stencilFailOpFront: number;
      stencilZFailOpFront: number;
      stencilPassOpFront: number;
      stencilRefFront: number;

      stencilFuncBack: number;
      stencilReadMaskBack: number;
      stencilWriteMaskBack: number;
      stencilFailOpBack: number;
      stencilZFailOpBack: number;
      stencilPassOpBack: number;
   }


   class BlendStateInfo extends NativeBaseObj {
      isA2C: boolean;
      isIndepend: boolean;
      // Color blendColor;
      // BlendTargetInfoList targets;
   }

   class IPassStates extends NativeBaseObj {
      rasterizerState: RasterizerStateInfo;
      depthState: DepthStateInfo;
      stencilState: StencilStateInfo;
      blendState: BlendStateInfo;
   }

   class EffectAsset extends NativeBaseObj {
      constructor(id: number);
      initialize(shaderInfo: IEffectDefine): void;
   }


   class IMaterialInfo extends NativeBaseObj {
      pushState(states: IPassStates): void;
      pushMacro(name: string, value: string | number | boolean): void;
   }

   class Material extends NativeBaseObj {
      constructor(id: number);
      setEffectAsset(effect: EffectAsset): void;
      initialize(info: IMaterialInfo): void;
      setPropertyBridgeJsTexture(name: string, textureAssetId: number): void;
      setPropertyBridgeJsFloat(name: string, value: number): void;
      setPropertyBridgeJsMat4(name: string, value: number): void;
   }

   class WebGLDevice extends NativeBaseObj {
      initializeCapability(buffer: number): void;
   }

   class WebGL2Device extends NativeBaseObj {
      initializeCapability(buffer: number): void;
   }

   class Application extends NativeBaseObj {
      initialize(): void;
      update(): void;
      postUpdate(): void;
      registerSystem(system: any): void;
      setRenderPipeline(render: RenderPipline): void;
   }

   class Screen extends NativeBaseObj {
      setRenderSize(width: number, height: number): void;
      setDesignSize(width: number, height: number): void;
   }

   class ITexture2DCreateInfo extends NativeBaseObj {
      width: number;
      height: number;
      format: number;
      mipmapLevel: number;
      samplerHash: number;
   }

   class Texture2d extends NativeBaseObj {
      constructor(id: number);
      create(info: ITexture2DCreateInfo): void;
      uploadData(): void;
      resize(width: number, height: number): void;
      setSamplerFromJs(hash: number): void;
   }

   class PhysicsSystem extends NativeBaseObj {
   }

   class TweenSystem extends NativeBaseObj {

   }

   class InputManager extends NativeBaseObj {
      setEventDataPointer(pointer: number, size: number): void;
      update(): void;
   }

   class SkeletonSystem extends NativeBaseObj {

   }

   class Spine extends NativeBaseObj {
      constructor(id: number);
      parseBuffer(bufferAdress: number, size: number): void;
      getAnimatoinCount(): number;
      getAllAnimationNames(): number;
      setTextures(ids: number, count: number): void
   }

   class FreeTypeFont extends NativeBaseObj {
      constructor(id: number);
      loadFontFile(data: number, size: number): void
   }

   class RenderTexture extends NativeBaseObj {
      constructor(id: number);
      initiliazeFormJS(data: number): void;
   }

   class Truck extends NativeBaseObj {
      initFromJs(data: number, dataSize: number): void;
      setLift(distance: number): void;
      setFuildViewportPos(x: number, y: number): void;
      setFuildViewportSize(viewWidth: number, viewHeight: number): void;
      moveForward(): void;
      moveBack(): void;
      stop(): void;
      setSpeed(speed: number): void;
      setSlop(min: number, max: number): void;
      setCollisionFilter(categoryBits: number, maskBits: number): void;
      sparge(): void;
      stopSpare(): void;
      setSpareSpeed(speed: number): void;
      setFluidMaterial(id: number, render: number): void;
      setDataTexture(id: number, nodeId: number): void;
      setCollisionFuildFilter(group: number, mask: number): void;
      setWaterParticleAmout(amout: number): void;
      playExplosion(): void;
      recoveryFormExposion(): void;
      active(b: boolean): void;
   }

   class RenderPipline extends NativeBaseObj {

   }

   class FireGameRenderPipeline extends RenderPipline {
      enableWashRender(b: boolean, waterMarkMaterialId: number): void;
   }


   //=======================game====================
   class Rope extends NativeBaseObj {
      play(): void;
      paused(): void;
      setParentNodeFromJs(nativeId: number): void;
      setRopeTextureFromJs(textureAssetId: number): void;
      begin(x: number, y: number, mass: number): void;
      lineTo(x: number, y: number, mass: number): void;
      draw(): void;
   }

   class RopeCut extends NativeBaseObj {
      setParentNodeFromJs(nativeId: number): void;
      setRopeTextureFromJs(textureAssetId: number): void;
      setMinCutSegmentLength(length: number): void;
      begin(x: number, y: number, mass: number): void;
      lineTo(x: number, y: number, mass: number): void;
      draw(): void;
      cut(x1: number, y1: number, x2: number, y2: number): boolean;
   }

   class LevelBase extends NativeBaseObj {
      init(): void;
      cut(x1: number, y1: number, x2: number, y2: number): number;
      setParentNodeFromJs(nativeId: number): void;
   }

   class Level_1 extends LevelBase {

   }

   class Level_2 extends LevelBase {

   }

   class Level_3 extends LevelBase {

   }

   class Level_4 extends LevelBase { }
   class Level_5 extends LevelBase { }
   class Level_6 extends LevelBase { }
   class Level_7 extends LevelBase { }
   class Level_8 extends LevelBase { }
   class Level_9 extends LevelBase { }
}
