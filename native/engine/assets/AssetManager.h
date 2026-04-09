#pragma once
#include <map>
#include "Asset.h"
#include <functional>

typedef std::function<void(int, Asset *)> LOAD_COMPLETE_FUNCTION;

class AssetManager
{
private:
    std::map<int, Asset *> assetMap;
    std::map<std::string, Asset *> urlAssetMap;
    std::vector<std::string> loadingRecord;
    std::map<std::string, std::vector<LOAD_COMPLETE_FUNCTION>> waitCallFunction;
    bool checkLoading(std::string &url, LOAD_COMPLETE_FUNCTION &completeCallback);
    void setAssetUrl(std::string url,Asset* asset);
public:
    friend class Asset;
    static AssetManager *getInstance();
    void addAsset(Asset *asset);
    void removeAsset(Asset *asset);
    Asset *findById(int id);
    Asset *findByUrl(std::string url);
    void loadOne(std::string url, LOAD_COMPLETE_FUNCTION completeCallback);
    void _onLoaded(char *keyUrl, int error, int id);
};
