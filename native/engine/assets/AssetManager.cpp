#include "AssetManager.h"
#ifdef WASM
#include <emscripten.h>
#endif

#include "../core/core.h"

static AssetManager *_ins = new AssetManager();

AssetManager *AssetManager::getInstance()
{
   return _ins;
}

void AssetManager::addAsset(Asset *asset)
{
   // 表示C++内部创建的Asset，这种设计有点不怎么好
   if (asset->getId() < 0)
   {
      return;
   }

   SQ_ASSERT(assetMap.find(asset->getId()) == assetMap.end());
   assetMap[asset->getId()] = asset;
}

void AssetManager::setAssetUrl(std::string url, Asset *asset)
{
   urlAssetMap[asset->getKeyUrl()] = asset;
}

void AssetManager::removeAsset(Asset *asset)
{
   std::map<int, Asset *>::iterator it = assetMap.find(asset->getId());
   if (it != assetMap.end())
   {
      assetMap.erase(it);
   }

   std::map<std::string, Asset *>::iterator it2 = urlAssetMap.find(asset->getKeyUrl());
   if (it2 != urlAssetMap.end())
   {
      urlAssetMap.erase(it2);
   }
}

Asset *AssetManager::findById(int id)
{
   std::map<int, Asset *>::iterator it = assetMap.find(id);
   return it != assetMap.end() ? it->second : NULL;
}

Asset *AssetManager::findByUrl(std::string url)
{
   std::map<std::string, Asset *>::iterator it = urlAssetMap.find(url);
   return it != urlAssetMap.end() ? it->second : NULL;
}

bool AssetManager::checkLoading(std::string &url, LOAD_COMPLETE_FUNCTION &completeCallback)
{

   Asset *asset = findByUrl(url);
   if (asset)
   {
      completeCallback(0, asset);
      return true;
   }

   std::map<std::string, std::vector<LOAD_COMPLETE_FUNCTION>>::iterator it = waitCallFunction.find(url);
   if (it == waitCallFunction.end())
   {
      std::pair<std::string, std::vector<LOAD_COMPLETE_FUNCTION>> item = {url, std::vector<LOAD_COMPLETE_FUNCTION>()};
      waitCallFunction.insert(item);
   }
   it = waitCallFunction.find(url);
   it->second.push_back(completeCallback);

   bool loading = false;
   for (int i = 0; i < loadingRecord.size(); ++i)
   {
      if (loadingRecord[i] == url)
      {
         loading = true;
         break;
      }
   }
   if (!loading)
   {
      loadingRecord.push_back(url);
   }
   return loading;
}

void AssetManager::_onLoaded(char *keyUrl, int error, int id)
{
   for (int i = 0; i < loadingRecord.size(); ++i)
   {
      if (loadingRecord[i] == keyUrl)
      {
         loadingRecord.erase(loadingRecord.begin() + i);
         break;
      }
   }
   std::map<std::string, std::vector<LOAD_COMPLETE_FUNCTION>>::iterator it = waitCallFunction.find(keyUrl);
   std::vector<LOAD_COMPLETE_FUNCTION> &callbackFunctions = it->second;
   Asset *asset = findById(id);
   for (int i = 0; i < callbackFunctions.size(); ++i)
   {
      callbackFunctions[i](error, asset);
   }
   waitCallFunction.erase(it);
}

#ifdef WASM
void AssetManager::loadOne(std::string url, LOAD_COMPLETE_FUNCTION completeCallback)
{
   if (checkLoading(url, completeCallback))
   {
      return;
   }
   EM_ASM("let url = window.UTF8ToString($0);window.AssetManager.getInstance().load(url,function(error,asset){ window.Module.ccall('onAssetLoaded',null,['string','number','number'],[url,error || 0,asset ? asset.getId() : 0]) });", url.c_str());
}

typedef void (AssetManager::*OnLoadedFunc)();

extern "C"
{
   EMSCRIPTEN_KEEPALIVE void onAssetLoaded(char *url, int error, int id)
   {
      AssetManager::getInstance()->_onLoaded(url, error, id);
   };
}
#endif