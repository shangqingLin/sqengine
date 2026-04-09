#pragma once
#include "NativeToJsObject.h"
#include "../../engine/core/sqstd/sqstd.h"

namespace bridge
{
    class NativeToJsObjectMangaer
    {
    private:
        sqstd::Array<NativeToJsObject *> objects;

    public:
        static NativeToJsObjectMangaer *getInstance();
        void addNativeToJsObject(NativeToJsObject*);
        void removeNativeToJsObject(NativeToJsObject*);
        void onFrameBegin();
    };
}