#ifndef _BRIDGE_JS_OBJECT_H_
#define _BRIDGE_JS_OBJECT_H_

#include "../define.h"
#include "../DispatchManager.h"
#include <unordered_map>

namespace bridge
{
    class NativeToJsObject
    {
    private:
        NativeObjectType type;
        unsigned int inRewritePos = -1;
        bool _enableWriteRepeat = false;
        std::unordered_map<int, unsigned int> addressOpMap;
        void onFrameBegin();

    public:
        friend class NativeToJsObjectMangaer;
        NativeToJsObject();
        NativeToJsObject(NativeObjectType type);

        virtual ~NativeToJsObject();

        void enableWriteRepeat(bool);

        /**
         * @param repeat 是否运行重复记录
         */
        void beginOp(unsigned char op, bool repeat = false);

        void endOp();
        void beginOpync();
        void endOpSync();

        template <class T>
        void writeOpArg(T v)
        {
            DispatchManager::getInstance()->nativeToJsBuffer.append<T>(v);
        }

        char *writeBuffer(const void *buffer, int byteSyze)
        {
            return DispatchManager::getInstance()->nativeToJsBuffer.append(buffer, byteSyze);
        }

        char *getCurrentWriteBuffer();
        int getDataSize();
    };
}

#endif