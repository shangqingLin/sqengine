#include "Asset.h"
#include "AssetManager.h"

Asset::Asset(int id) : id(id),
                       keyUrl("")
{
    // 先保存id的map
    AssetManager::getInstance()->addAsset(this);
}

void Asset::setKeyUrl(char *url)
{
    keyUrl = url;
    // 保存有url的map
    AssetManager::getInstance()->setAssetUrl(url, this);
}

Asset::~Asset()
{
    AssetManager::getInstance()->removeAsset(this);
}
