#include "NativeToJsObjectMangaer.h"

using namespace bridge;

static NativeToJsObjectMangaer *_inst = nullptr;
NativeToJsObjectMangaer *NativeToJsObjectMangaer::getInstance()
{
    if (!_inst)
    {
        _inst = new NativeToJsObjectMangaer();
    }
    return _inst;
}

void NativeToJsObjectMangaer::addNativeToJsObject(NativeToJsObject *obj)
{
    objects.push(obj);
}

void NativeToJsObjectMangaer::removeNativeToJsObject(NativeToJsObject *obj)
{
    for (int i = 0; i < objects.getCount(); ++i)
    {
        if (*objects.get(i) == obj)
        {
            objects.removeSwap(i);
            return;
        }
    }
}

void NativeToJsObjectMangaer::onFrameBegin()
{
    for (int i = 0; i < objects.getCount(); ++i)
    {
        (*objects.get(i))->onFrameBegin();
    }
}