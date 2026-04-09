
#include <emscripten.h>
#include <stdlib.h>

EM_JS_DEPS(webidl_binder, "$intArrayFromString,$UTF8ToString,$alignMemory");

extern "C" {

// Define custom allocator functions that we can force export using
// EMSCRIPTEN_KEEPALIVE.  This avoids all webidl users having to add
// malloc/free to -sEXPORTED_FUNCTIONS.
EMSCRIPTEN_KEEPALIVE void webidl_free(void* p) { free(p); }
EMSCRIPTEN_KEEPALIVE void* webidl_malloc(size_t len) { return malloc(len); }


// Interface: VoidPtr


void EMSCRIPTEN_KEEPALIVE emscripten_bind_VoidPtr___destroy___0(void** self) {
  delete self;
}

// Interface: IAttributeInfo


IAttributeInfo* EMSCRIPTEN_KEEPALIVE emscripten_bind_IAttributeInfo_IAttributeInfo_0() {
  return new IAttributeInfo();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IAttributeInfo_setName_1(IAttributeInfo* self, char* name) {
  self->setName(name);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_IAttributeInfo_get_format_0(IAttributeInfo* self) {
  return self->format;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IAttributeInfo_set_format_1(IAttributeInfo* self, int arg0) {
  self->format = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IAttributeInfo___destroy___0(IAttributeInfo* self) {
  delete self;
}

// Interface: ISamplerTextureInfo


ISamplerTextureInfo* EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_ISamplerTextureInfo_0() {
  return new ISamplerTextureInfo();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_setName_1(ISamplerTextureInfo* self, char* name) {
  self->setName(name);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_get_binding_0(ISamplerTextureInfo* self) {
  return self->binding;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_set_binding_1(ISamplerTextureInfo* self, int arg0) {
  self->binding = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_get_type_0(ISamplerTextureInfo* self) {
  return self->type;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_set_type_1(ISamplerTextureInfo* self, int arg0) {
  self->type = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_get_count_0(ISamplerTextureInfo* self) {
  return self->count;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_set_count_1(ISamplerTextureInfo* self, int arg0) {
  self->count = arg0;
}

bool EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_get_buildin_0(ISamplerTextureInfo* self) {
  return self->buildin;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo_set_buildin_1(ISamplerTextureInfo* self, bool arg0) {
  self->buildin = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ISamplerTextureInfo___destroy___0(ISamplerTextureInfo* self) {
  delete self;
}

// Interface: IUniform


IUniform* EMSCRIPTEN_KEEPALIVE emscripten_bind_IUniform_IUniform_0() {
  return new IUniform();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IUniform_setName_1(IUniform* self, char* name) {
  self->setName(name);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_IUniform_get_type_0(IUniform* self) {
  return self->type;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IUniform_set_type_1(IUniform* self, int arg0) {
  self->type = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_IUniform_get_count_0(IUniform* self) {
  return self->count;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IUniform_set_count_1(IUniform* self, int arg0) {
  self->count = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IUniform___destroy___0(IUniform* self) {
  delete self;
}

// Interface: IBlockInfo


IBlockInfo* EMSCRIPTEN_KEEPALIVE emscripten_bind_IBlockInfo_IBlockInfo_0() {
  return new IBlockInfo();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IBlockInfo_pushMembers_1(IBlockInfo* self, IUniform* uniform) {
  self->pushMembers(uniform);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IBlockInfo_setBlockName_1(IBlockInfo* self, char* name) {
  self->setBlockName(name);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_IBlockInfo_get_binding_0(IBlockInfo* self) {
  return self->binding;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IBlockInfo_set_binding_1(IBlockInfo* self, int arg0) {
  self->binding = arg0;
}

bool EMSCRIPTEN_KEEPALIVE emscripten_bind_IBlockInfo_get_buildin_0(IBlockInfo* self) {
  return self->buildin;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IBlockInfo_set_buildin_1(IBlockInfo* self, bool arg0) {
  self->buildin = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IBlockInfo___destroy___0(IBlockInfo* self) {
  delete self;
}

// Interface: IShaderDefine


IShaderDefine* EMSCRIPTEN_KEEPALIVE emscripten_bind_IShaderDefine_IShaderDefine_0() {
  return new IShaderDefine();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IShaderDefine_pushBlock_1(IShaderDefine* self, IBlockInfo* blockInfo) {
  self->pushBlock(blockInfo);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IShaderDefine_pushAttribute_1(IShaderDefine* self, IAttributeInfo* attributeInfo) {
  self->pushAttribute(attributeInfo);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IShaderDefine_pushSamplerTexture_1(IShaderDefine* self, ISamplerTextureInfo* sampler) {
  self->pushSamplerTexture(sampler);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IShaderDefine_pushMacro_2(IShaderDefine* self, char* name, int flag) {
  self->pushMacro(name, flag);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IShaderDefine___destroy___0(IShaderDefine* self) {
  delete self;
}

// Interface: IPassInfo


IPassInfo* EMSCRIPTEN_KEEPALIVE emscripten_bind_IPassInfo_IPassInfo_0() {
  return new IPassInfo();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_IPassInfo_get_shaderIndex_0(IPassInfo* self) {
  return self->shaderIndex;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IPassInfo_set_shaderIndex_1(IPassInfo* self, int arg0) {
  self->shaderIndex = arg0;
}

char* EMSCRIPTEN_KEEPALIVE emscripten_bind_IPassInfo_get_passStateBuffer_0(IPassInfo* self) {
  return self->passStateBuffer;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IPassInfo_set_passStateBuffer_1(IPassInfo* self, char* arg0) {
  self->passStateBuffer = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IPassInfo___destroy___0(IPassInfo* self) {
  delete self;
}

// Interface: IEffectDefine


IEffectDefine* EMSCRIPTEN_KEEPALIVE emscripten_bind_IEffectDefine_IEffectDefine_0() {
  return new IEffectDefine();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IEffectDefine_pushShaderDefine_1(IEffectDefine* self, IShaderDefine* info) {
  self->pushShaderDefine(info);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IEffectDefine_pushPassInfo_1(IEffectDefine* self, IPassInfo* info) {
  self->pushPassInfo(info);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IEffectDefine___destroy___0(IEffectDefine* self) {
  delete self;
}

// Interface: EffectAsset


EffectAsset* EMSCRIPTEN_KEEPALIVE emscripten_bind_EffectAsset_EffectAsset_1(int id) {
  return new EffectAsset(id);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_EffectAsset_setKeyUrl_1(EffectAsset* self, char* url) {
  self->setKeyUrl(url);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_EffectAsset_initialize_1(EffectAsset* self, IEffectDefine* info) {
  self->initialize(info);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_EffectAsset___destroy___0(EffectAsset* self) {
  delete self;
}

// Interface: IMaterialInfo


IMaterialInfo* EMSCRIPTEN_KEEPALIVE emscripten_bind_IMaterialInfo_IMaterialInfo_0() {
  return new IMaterialInfo();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IMaterialInfo_pushMacro_2(IMaterialInfo* self, char* name, int value) {
  self->pushMacro(name, value);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_IMaterialInfo___destroy___0(IMaterialInfo* self) {
  delete self;
}

// Interface: Material


Material* EMSCRIPTEN_KEEPALIVE emscripten_bind_Material_Material_1(int id) {
  return new Material(id);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Material_setKeyUrl_1(Material* self, char* url) {
  self->setKeyUrl(url);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Material_setEffectAsset_1(Material* self, EffectAsset* effect) {
  self->setEffectAsset(effect);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Material_initialize_1(Material* self, IMaterialInfo* info) {
  self->initialize(info);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Material_setPropertyBridgeJsTexture_2(Material* self, char* name, int textureAssetId) {
  self->setPropertyBridgeJsTexture(name, textureAssetId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Material_setPropertyBridgeJsFloat_2(Material* self, char* name, float value) {
  self->setPropertyBridgeJsFloat(name, value);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Material_setPropertyBridgeJsMat4_2(Material* self, char* name, void* value) {
  self->setPropertyBridgeJsMat4(name, value);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Material___destroy___0(Material* self) {
  delete self;
}

// Interface: ITexture2DCreateInfo


ITexture2DCreateInfo* EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_ITexture2DCreateInfo_0() {
  return new ITexture2DCreateInfo();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_get_width_0(ITexture2DCreateInfo* self) {
  return self->width;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_set_width_1(ITexture2DCreateInfo* self, int arg0) {
  self->width = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_get_height_0(ITexture2DCreateInfo* self) {
  return self->height;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_set_height_1(ITexture2DCreateInfo* self, int arg0) {
  self->height = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_get_format_0(ITexture2DCreateInfo* self) {
  return self->format;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_set_format_1(ITexture2DCreateInfo* self, int arg0) {
  self->format = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_get_mipmapLevel_0(ITexture2DCreateInfo* self) {
  return self->mipmapLevel;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_set_mipmapLevel_1(ITexture2DCreateInfo* self, int arg0) {
  self->mipmapLevel = arg0;
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_get_samplerHash_0(ITexture2DCreateInfo* self) {
  return self->samplerHash;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo_set_samplerHash_1(ITexture2DCreateInfo* self, int arg0) {
  self->samplerHash = arg0;
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_ITexture2DCreateInfo___destroy___0(ITexture2DCreateInfo* self) {
  delete self;
}

// Interface: Texture2d


Texture2d* EMSCRIPTEN_KEEPALIVE emscripten_bind_Texture2d_Texture2d_1(int id) {
  return new Texture2d(id);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Texture2d_setKeyUrl_1(Texture2d* self, char* url) {
  self->setKeyUrl(url);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Texture2d_create_1(Texture2d* self, ITexture2DCreateInfo* info) {
  self->create(info);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Texture2d_uploadData_0(Texture2d* self) {
  self->uploadData();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Texture2d_resize_2(Texture2d* self, int width, int height) {
  self->resize(width, height);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Texture2d_setSamplerFromJs_1(Texture2d* self, int hash) {
  self->setSamplerFromJs(hash);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Texture2d___destroy___0(Texture2d* self) {
  delete self;
}

// Interface: RenderTexture


RenderTexture* EMSCRIPTEN_KEEPALIVE emscripten_bind_RenderTexture_RenderTexture_1(int id) {
  return new RenderTexture(id);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_RenderTexture_initiliazeFormJS_1(RenderTexture* self, char* data) {
  self->initiliazeFormJS(data);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_RenderTexture___destroy___0(RenderTexture* self) {
  delete self;
}

// Interface: Spine


Spine* EMSCRIPTEN_KEEPALIVE emscripten_bind_Spine_Spine_1(int id) {
  return new Spine(id);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Spine_setKeyUrl_1(Spine* self, char* url) {
  self->setKeyUrl(url);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Spine_parseBuffer_2(Spine* self, char* buffer, int size) {
  self->parseBuffer(buffer, size);
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Spine_getAnimatoinCount_0(Spine* self) {
  return self->getAnimatoinCount();
}

void* EMSCRIPTEN_KEEPALIVE emscripten_bind_Spine_getAllAnimationNames_0(Spine* self) {
  return self->getAllAnimationNames();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Spine_setTextures_2(Spine* self, char* buffer, int count) {
  self->setTextures(buffer, count);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Spine___destroy___0(Spine* self) {
  delete self;
}

// Interface: FreeTypeFont


FreeTypeFont* EMSCRIPTEN_KEEPALIVE emscripten_bind_FreeTypeFont_FreeTypeFont_1(int id) {
  return new FreeTypeFont(id);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_FreeTypeFont_setKeyUrl_1(FreeTypeFont* self, char* url) {
  self->setKeyUrl(url);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_FreeTypeFont_loadFontFile_2(FreeTypeFont* self, char* data, int size) {
  self->loadFontFile(data, size);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_FreeTypeFont___destroy___0(FreeTypeFont* self) {
  delete self;
}

// Interface: WebGLDevice


WebGLDevice* EMSCRIPTEN_KEEPALIVE emscripten_bind_WebGLDevice_WebGLDevice_0() {
  return new WebGLDevice();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_WebGLDevice_initializeCapability_1(WebGLDevice* self, char* data) {
  self->initializeCapability(data);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_WebGLDevice___destroy___0(WebGLDevice* self) {
  delete self;
}

// Interface: WebGL2Device


WebGL2Device* EMSCRIPTEN_KEEPALIVE emscripten_bind_WebGL2Device_WebGL2Device_0() {
  return new WebGL2Device();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_WebGL2Device_initializeCapability_1(WebGL2Device* self, char* data) {
  self->initializeCapability(data);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_WebGL2Device___destroy___0(WebGL2Device* self) {
  delete self;
}

// Interface: System


void EMSCRIPTEN_KEEPALIVE emscripten_bind_System___destroy___0(System* self) {
  delete self;
}

// Interface: RenderPipeline


void EMSCRIPTEN_KEEPALIVE emscripten_bind_RenderPipeline___destroy___0(pipeline::RenderPipeline* self) {
  delete self;
}

// Interface: FireGameRenderPipeline


pipeline::FireGameRenderPipeline* EMSCRIPTEN_KEEPALIVE emscripten_bind_FireGameRenderPipeline_FireGameRenderPipeline_0() {
  return new pipeline::FireGameRenderPipeline();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_FireGameRenderPipeline_enableWashRender_2(pipeline::FireGameRenderPipeline* self, bool b, int waterMarkMaterialId) {
  self->enableWashRender(b, waterMarkMaterialId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_FireGameRenderPipeline___destroy___0(pipeline::FireGameRenderPipeline* self) {
  delete self;
}

// Interface: Application


Application* EMSCRIPTEN_KEEPALIVE emscripten_bind_Application_Application_0() {
  return new Application();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Application_initialize_0(Application* self) {
  self->initialize();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Application_update_0(Application* self) {
  self->update();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Application_postUpdate_0(Application* self) {
  self->postUpdate();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Application_registerSystem_1(Application* self, System* system) {
  self->registerSystem(system);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Application_setRenderPipeline_1(Application* self, pipeline::RenderPipeline* pipeline) {
  self->setRenderPipeline(pipeline);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Application___destroy___0(Application* self) {
  delete self;
}

// Interface: InputManager


InputManager* EMSCRIPTEN_KEEPALIVE emscripten_bind_InputManager_InputManager_0() {
  return new InputManager();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_InputManager_setEventDataPointer_2(InputManager* self, char* pointer, int size) {
  self->setEventDataPointer(pointer, size);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_InputManager_update_0(InputManager* self) {
  self->update();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_InputManager___destroy___0(InputManager* self) {
  delete self;
}

// Interface: Screen


Screen* EMSCRIPTEN_KEEPALIVE emscripten_bind_Screen_Screen_0() {
  return new Screen();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Screen_setRenderSize_2(Screen* self, int width, int height) {
  self->setRenderSize(width, height);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Screen_setDesignSize_2(Screen* self, int width, int height) {
  self->setDesignSize(width, height);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Screen___destroy___0(Screen* self) {
  delete self;
}

// Interface: PhysicsSystem


physics2d::PhysicsSystem* EMSCRIPTEN_KEEPALIVE emscripten_bind_PhysicsSystem_PhysicsSystem_0() {
  return new physics2d::PhysicsSystem();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_PhysicsSystem___destroy___0(physics2d::PhysicsSystem* self) {
  delete self;
}

// Interface: SkeletonSystem


SkeletonSystem* EMSCRIPTEN_KEEPALIVE emscripten_bind_SkeletonSystem_SkeletonSystem_0() {
  return new SkeletonSystem();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_SkeletonSystem___destroy___0(SkeletonSystem* self) {
  delete self;
}

// Interface: TweenSystem


TweenSystem* EMSCRIPTEN_KEEPALIVE emscripten_bind_TweenSystem_TweenSystem_0() {
  return new TweenSystem();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_TweenSystem___destroy___0(TweenSystem* self) {
  delete self;
}

// Interface: Truck


game::Truck* EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_Truck_0() {
  return new game::Truck();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_initFromJs_2(game::Truck* self, char* data, int partCount) {
  self->initFromJs(data, partCount);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setLift_1(game::Truck* self, int distance) {
  self->setLift(distance);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_moveForward_0(game::Truck* self) {
  self->moveForward();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_moveBack_0(game::Truck* self) {
  self->moveBack();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_stop_0(game::Truck* self) {
  self->stop();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setSpeed_1(game::Truck* self, int speed) {
  self->setSpeed(speed);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setSlop_2(game::Truck* self, int min, int max) {
  self->setSlop(min, max);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setCollisionFilter_2(game::Truck* self, int categoryBits, int maskBits) {
  self->setCollisionFilter(categoryBits, maskBits);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_sparge_0(game::Truck* self) {
  self->sparge();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_stopSpare_0(game::Truck* self) {
  self->stopSpare();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setFluidMaterial_2(game::Truck* self, int id, int renderType) {
  self->setFluidMaterial(id, renderType);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setCollisionFuildFilter_2(game::Truck* self, int group, int mask) {
  self->setCollisionFuildFilter(group, mask);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setWaterParticleAmout_1(game::Truck* self, int amout) {
  self->setWaterParticleAmout(amout);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setFuildViewportPos_2(game::Truck* self, float x, float y) {
  self->setFuildViewportPos(x, y);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setFuildViewportSize_2(game::Truck* self, float x, float y) {
  self->setFuildViewportSize(x, y);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_playExplosion_0(game::Truck* self) {
  self->playExplosion();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_recoveryFormExposion_0(game::Truck* self) {
  self->recoveryFormExposion();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_active_1(game::Truck* self, bool active) {
  self->active(active);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck_setSpareSpeed_1(game::Truck* self, int speed) {
  self->setSpareSpeed(speed);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Truck___destroy___0(game::Truck* self) {
  delete self;
}

// Interface: Level_1


game::Level_1* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_1_Level_1_0() {
  return new game::Level_1();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_1_init_0(game::Level_1* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_1_cut_4(game::Level_1* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_1_setParentNodeFromJs_1(game::Level_1* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_1___destroy___0(game::Level_1* self) {
  delete self;
}

// Interface: Level_2


game::Level_2* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_2_Level_2_0() {
  return new game::Level_2();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_2_init_0(game::Level_2* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_2_cut_4(game::Level_2* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_2_setParentNodeFromJs_1(game::Level_2* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_2___destroy___0(game::Level_2* self) {
  delete self;
}

// Interface: Level_3


game::Level_3* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_3_Level_3_0() {
  return new game::Level_3();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_3_init_0(game::Level_3* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_3_cut_4(game::Level_3* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_3_setParentNodeFromJs_1(game::Level_3* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_3___destroy___0(game::Level_3* self) {
  delete self;
}

// Interface: Level_4


game::Level_4* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_4_Level_4_0() {
  return new game::Level_4();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_4_init_0(game::Level_4* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_4_cut_4(game::Level_4* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_4_setParentNodeFromJs_1(game::Level_4* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_4___destroy___0(game::Level_4* self) {
  delete self;
}

// Interface: Level_5


game::Level_5* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_5_Level_5_0() {
  return new game::Level_5();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_5_init_0(game::Level_5* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_5_cut_4(game::Level_5* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_5_setParentNodeFromJs_1(game::Level_5* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_5___destroy___0(game::Level_5* self) {
  delete self;
}

// Interface: Level_6


game::Level_6* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_6_Level_6_0() {
  return new game::Level_6();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_6_init_0(game::Level_6* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_6_cut_4(game::Level_6* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_6_setParentNodeFromJs_1(game::Level_6* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_6___destroy___0(game::Level_6* self) {
  delete self;
}

// Interface: Level_7


game::Level_7* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_7_Level_7_0() {
  return new game::Level_7();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_7_init_0(game::Level_7* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_7_cut_4(game::Level_7* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_7_setParentNodeFromJs_1(game::Level_7* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_7___destroy___0(game::Level_7* self) {
  delete self;
}

// Interface: Level_8


game::Level_8* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_8_Level_8_0() {
  return new game::Level_8();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_8_init_0(game::Level_8* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_8_cut_4(game::Level_8* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_8_setParentNodeFromJs_1(game::Level_8* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_8___destroy___0(game::Level_8* self) {
  delete self;
}

// Interface: Level_9


game::Level_9* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_9_Level_9_0() {
  return new game::Level_9();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_9_init_0(game::Level_9* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_9_cut_4(game::Level_9* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_9_setParentNodeFromJs_1(game::Level_9* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_9___destroy___0(game::Level_9* self) {
  delete self;
}

// Interface: Level_C1_10


game::Level_C1_10* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_10_Level_C1_10_0() {
  return new game::Level_C1_10();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_10_init_0(game::Level_C1_10* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_10_cut_4(game::Level_C1_10* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_10_setParentNodeFromJs_1(game::Level_C1_10* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_10___destroy___0(game::Level_C1_10* self) {
  delete self;
}

// Interface: Level_C1_11


game::Level_C1_11* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_11_Level_C1_11_0() {
  return new game::Level_C1_11();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_11_init_0(game::Level_C1_11* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_11_cut_4(game::Level_C1_11* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_11_setParentNodeFromJs_1(game::Level_C1_11* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_11___destroy___0(game::Level_C1_11* self) {
  delete self;
}

// Interface: Level_C1_12


game::Level_C1_12* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_12_Level_C1_12_0() {
  return new game::Level_C1_12();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_12_init_0(game::Level_C1_12* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_12_cut_4(game::Level_C1_12* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_12_setParentNodeFromJs_1(game::Level_C1_12* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_12___destroy___0(game::Level_C1_12* self) {
  delete self;
}

// Interface: Level_C1_13


game::Level_C1_13* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_13_Level_C1_13_0() {
  return new game::Level_C1_13();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_13_init_0(game::Level_C1_13* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_13_cut_4(game::Level_C1_13* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_13_setParentNodeFromJs_1(game::Level_C1_13* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_13___destroy___0(game::Level_C1_13* self) {
  delete self;
}

// Interface: Level_C1_14


game::Level_C1_14* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_14_Level_C1_14_0() {
  return new game::Level_C1_14();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_14_init_0(game::Level_C1_14* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_14_cut_4(game::Level_C1_14* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_14_setParentNodeFromJs_1(game::Level_C1_14* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_14___destroy___0(game::Level_C1_14* self) {
  delete self;
}

// Interface: Level_C1_15


game::Level_C1_15* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_15_Level_C1_15_0() {
  return new game::Level_C1_15();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_15_init_0(game::Level_C1_15* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_15_cut_4(game::Level_C1_15* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_15_setParentNodeFromJs_1(game::Level_C1_15* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_15___destroy___0(game::Level_C1_15* self) {
  delete self;
}

// Interface: Level_C1_16


game::Level_C1_16* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_16_Level_C1_16_0() {
  return new game::Level_C1_16();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_16_init_0(game::Level_C1_16* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_16_cut_4(game::Level_C1_16* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_16_setParentNodeFromJs_1(game::Level_C1_16* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_16___destroy___0(game::Level_C1_16* self) {
  delete self;
}

// Interface: Level_C1_17


game::Level_C1_17* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_17_Level_C1_17_0() {
  return new game::Level_C1_17();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_17_init_0(game::Level_C1_17* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_17_cut_4(game::Level_C1_17* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_17_setParentNodeFromJs_1(game::Level_C1_17* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_17___destroy___0(game::Level_C1_17* self) {
  delete self;
}

// Interface: Level_C1_18


game::Level_C1_18* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_18_Level_C1_18_0() {
  return new game::Level_C1_18();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_18_init_0(game::Level_C1_18* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_18_cut_4(game::Level_C1_18* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_18_setParentNodeFromJs_1(game::Level_C1_18* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_18___destroy___0(game::Level_C1_18* self) {
  delete self;
}

// Interface: Level_C1_19


game::Level_C1_19* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_19_Level_C1_19_0() {
  return new game::Level_C1_19();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_19_init_0(game::Level_C1_19* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_19_cut_4(game::Level_C1_19* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_19_setParentNodeFromJs_1(game::Level_C1_19* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_19___destroy___0(game::Level_C1_19* self) {
  delete self;
}

// Interface: Level_C1_20


game::Level_C1_20* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_20_Level_C1_20_0() {
  return new game::Level_C1_20();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_20_init_0(game::Level_C1_20* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_20_cut_4(game::Level_C1_20* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_20_setParentNodeFromJs_1(game::Level_C1_20* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_20___destroy___0(game::Level_C1_20* self) {
  delete self;
}

// Interface: Level_C1_21


game::Level_C1_21* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_21_Level_C1_21_0() {
  return new game::Level_C1_21();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_21_init_0(game::Level_C1_21* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_21_cut_4(game::Level_C1_21* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_21_setParentNodeFromJs_1(game::Level_C1_21* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_21___destroy___0(game::Level_C1_21* self) {
  delete self;
}

// Interface: Level_C1_22


game::Level_C1_22* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_22_Level_C1_22_0() {
  return new game::Level_C1_22();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_22_init_0(game::Level_C1_22* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_22_cut_4(game::Level_C1_22* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_22_setParentNodeFromJs_1(game::Level_C1_22* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_22___destroy___0(game::Level_C1_22* self) {
  delete self;
}

// Interface: Level_C1_23


game::Level_C1_23* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_23_Level_C1_23_0() {
  return new game::Level_C1_23();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_23_init_0(game::Level_C1_23* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_23_cut_4(game::Level_C1_23* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_23_setParentNodeFromJs_1(game::Level_C1_23* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_23___destroy___0(game::Level_C1_23* self) {
  delete self;
}

// Interface: Level_C1_24


game::Level_C1_24* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_24_Level_C1_24_0() {
  return new game::Level_C1_24();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_24_init_0(game::Level_C1_24* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_24_cut_4(game::Level_C1_24* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_24_setParentNodeFromJs_1(game::Level_C1_24* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_24___destroy___0(game::Level_C1_24* self) {
  delete self;
}

// Interface: Level_C1_25


game::Level_C1_25* EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_25_Level_C1_25_0() {
  return new game::Level_C1_25();
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_25_init_0(game::Level_C1_25* self) {
  self->init();
}

int EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_25_cut_4(game::Level_C1_25* self, float x1, float y1, float x2, float y2) {
  return self->cut(x1, y1, x2, y2);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_25_setParentNodeFromJs_1(game::Level_C1_25* self, int nativeId) {
  self->setParentNodeFromJs(nativeId);
}

void EMSCRIPTEN_KEEPALIVE emscripten_bind_Level_C1_25___destroy___0(game::Level_C1_25* self) {
  delete self;
}

}

