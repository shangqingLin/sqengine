#include "BuildinResManager.h"
#include "AssetManager.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static std::unordered_map<std::string, Material *> materialMap;
Material *BuildinResManager::getMaterial(std::string name)
{
   std::unordered_map<std::string, Material *>::iterator it = materialMap.find(name);
   if (it == materialMap.end())
   {

#ifdef __EMSCRIPTEN__
      EM_ASM("let material = window.buildinResManager.getMaterial($0);window.Module.ccall('onGetBuildInAsset',null,['string','number'],[ window.Module.UTF8ToString($0),1,material.getId()]);", name.c_str());
#endif
   }
   return materialMap[name];
}

void BuildinResManager::onGetAsset(std::string name, int type, int id)
{
   if (type == 1)
   {
      Material *material = (Material *)AssetManager::getInstance()->findById(id);
      materialMap[name] = material;
   }
}

#ifdef __EMSCRIPTEN__
extern "C"
{
   EMSCRIPTEN_KEEPALIVE void onGetBuildInAsset(char *name, int type, int id)
   {
      BuildinResManager::onGetAsset(name, type, id);
   };
}
#endif
