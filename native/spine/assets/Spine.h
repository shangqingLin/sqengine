#ifndef _ASSET_SPINE_H_
#define _ASSET_SPINE_H_
#include <engine/assets/Asset.h>
#include <spine/SkeletonData.h>
#include <spine/Atlas.h>
#include <optional>
#include "SpineCache.h"

class Spine : public Asset
{
    private:
        spSkeletonData* skData = nullptr;
        spAtlas* atlas = nullptr;
        int* textureIds = nullptr;
        int textureNum = 0;
        void parseSkeletonData(char *data, int size);
        void initTextures();
     public:
        std::optional<SpineCache> spineCache;
        Spine(int id);
        virtual ~Spine();
        inline spSkeletonData* getSkeletonData(){return skData;};
        void parseBuffer(char* buffer,int size);
        const spAnimation* getAnimation(int index);
        const spAnimation* getAnimation(const char* name);
        int getAnimationIndex(const char* name);
        int getAnimatoinCount();
        char** getAllAnimationNames();
        void setTextures(char*,int count);
};
#endif