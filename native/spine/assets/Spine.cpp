#include "Spine.h"
#include <string.h>
#include <engine/core/sqstd/Byte.h>
#include <spine/AtlasAttachmentLoader.h>
#include <spine/extension.h>
#include <spine/SkeletonBinary.h>
#include <engine/assets/AssetManager.h>
static char* readStr(sqstd::Byte &buffer)
{
    int strLength = buffer.poppUTFStringLength();
    char *string = CALLOC(char, strLength);
    return buffer.popUTFString(string, strLength);
}

static  spAtlas* readAtlas(sqstd::Byte &buffer)
{
    int pageCount = *buffer.popp<unsigned char>();
    spAtlas* atlas = NEW(spAtlas);

    spAtlasPage *prevPage = nullptr; 
    spAtlasPage *rootRootPage = nullptr;
    int strLength = 0;
    spAtlasPage *pages[pageCount];
    for (int i = 0; i < pageCount; ++i)
    {
        char* name = readStr(buffer);
        spAtlasPage *page = spAtlasPage_create(atlas,name);
        page->width = *buffer.popp<int>();
        page->height = *buffer.popp<int>();
        
        if(prevPage) prevPage->next = page;
        if(!rootRootPage) rootRootPage = page;
        prevPage = page;
        pages[i] = page;
    }

    spAtlasRegion *prevRegion = nullptr; 
    spAtlasRegion *rootRegion = nullptr;
    for (int i = 0; i < pageCount; ++i)
    {
        spAtlasPage *page = pages[i];
        int regionCount = *buffer.popp<unsigned short>();
        for (int n = 0; n < regionCount; ++n)
        {
            spAtlasRegion *region = spAtlasRegion_create();
            region->name = readStr(buffer);
            region->super.degrees = *buffer.popp<int>();
            region->x = *buffer.popp<int>();
            region->y = *buffer.popp<int>();
            region->super.width = *buffer.popp<int>();
            region->super.height = *buffer.popp<int>();
            region->super.originalWidth = *buffer.popp<int>();
            region->super.originalHeight = *buffer.popp<int>();
            region->super.offsetX = *buffer.popp<float>();
            region->super.offsetY = *buffer.popp<float>();
            region->index = *buffer.popp<int>();
            region->page = page;

            // printf("readAtlas %s size(%d %d), osize(%d %d),offset (%f %f) \n", region->name, 
            //     region->super.width,region->super.height,
            //     region->super.originalWidth, region->super.originalHeight,
            //     region->super.offsetX, region->super.offsetY );


            region->super.u = (float) region->x / page->width;
			region->super.v = (float) region->y / page->height;
			if (region->super.degrees == 90) {
				region->super.u2 = (float) (region->x + region->super.height) / page->width;
				region->super.v2 = (float) (region->y + region->super.width) / page->height;
			} else {
				region->super.u2 = (float) (region->x + region->super.width) / page->width;
				region->super.v2 = (float) (region->y + region->super.height) / page->height;
			}

            if(prevRegion)
            {
                prevRegion->next = region;
            }
            if(!rootRegion){
                rootRegion = region;
            }
            prevRegion = region;
        }
    }

    atlas->regions = rootRegion;
    atlas->pages = rootRootPage;
    return atlas;
}

//=========================================================
Spine::Spine(int id) : Asset(id)
{
}


void Spine::parseSkeletonData(char *data, int size)
{
    sqstd::Byte buffer;
    buffer.setExternalBuffer(data,size);
    atlas = readAtlas(buffer);
    spAtlasAttachmentLoader *loader = spAtlasAttachmentLoader_create(atlas);
    spSkeletonBinary* binary = spSkeletonBinary_createWithLoader((spAttachmentLoader*)loader);
    skData = spSkeletonBinary_readSkeletonData(binary, (unsigned char*)(data+buffer.getReadPos()),size - buffer.getReadPos());
    spSkeletonBinary_dispose(binary);
    spAttachmentLoader_dispose((spAttachmentLoader*)loader);
}

void Spine::parseBuffer(char *buffer, int size)
{
    parseSkeletonData(buffer, size);
    initTextures();
}

const spAnimation *Spine::getAnimation(int index)
{
    if (index >= skData->animationsCount)
    {
        return nullptr;
    }
    return getAnimation(skData->animationNames[index]);
}

int Spine::getAnimatoinCount()
{
    return skData->animationsCount;
}

char** Spine::getAllAnimationNames()
{
    return skData->animationNames;
}

const spAnimation *Spine::getAnimation(const char *name)
{
    return spSkeletonData_findAnimation(skData, name);
}

int Spine::getAnimationIndex(const char *name)
{
    for (int i = 0; i < skData->animationsCount; ++i)
        if (strcmp(skData->animationNames[i], name) == 0)
        {
            return i;
        }
    return -1;    
}

void Spine::setTextures(char* idDatas,int count)
{
    if(count == 0 ) return;
    textureNum = count;
    textureIds = new int[count];
    sqstd::Byte buffer;
    buffer.setExternalBuffer(idDatas,count * 4);
    for(int i = 0; i < count; ++i){
        textureIds[i] = *buffer.popp<int>();
    }
    initTextures();
}

void Spine::initTextures()
{
    if(textureIds && atlas){
        
        
        for(int i = 0; i  < textureNum ; ++i){
            spAtlasPage* page = atlas->pages;
            while (page)
            {
                page->rendererObject = AssetManager::getInstance()->findById(textureIds[i]);
                page = page->next;
            }
        }
    }
}

Spine::~Spine()
{
    if (skData)
    {
        spSkeletonData_dispose(skData);
        skData = nullptr;
    }

    if(atlas){
        spAtlas_dispose(atlas);
        atlas = nullptr;
    }

    if(textureIds){
        delete[] textureIds;
        textureIds = nullptr;
    }
}
