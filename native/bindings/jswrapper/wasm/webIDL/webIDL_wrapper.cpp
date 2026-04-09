#include <engine/assets/EffectAsset.h>
#include <engine/assets/Material.h>
#include <engine/assets/Texture2d.h>
#include <spine/assets/Spine.h>
#include <engine/gfx/webgl/WebGLDevice.h>
#include <engine/gfx/webgl2/WebGL2Device.h>
#include <engine/framework/Application.h>
#include <engine/input/InputManager.h>
// #include <physics-2d/index.h>
#include <physics2d/index.h>
#include <spine/framework/SkeletonSystem.h>
#include <engine/assets/FreeTypeFont.h>
#include <engine/assets/RenderTexture.h>
#include <engine/tween/TweenSystem.h>
#include <engine/scene/graphics/Mesh.h>
#include <engine/platform/Screen.h>

#include <engine/rendering/FireGameRenderPipeline.h>


#include <game/index.h>
#include <firegame/index.h>
#include <hitgame/index.h>

//必须放到最后
#include "idl_output/webIDL.cpp"