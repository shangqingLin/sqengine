#include "NativeToJsObject.h"
#include "../../engine/core/core.h"
#include "NativeToJsObjectMangaer.h"

using namespace bridge;

NativeToJsObject::NativeToJsObject(NativeObjectType type) : type(type)
{
    NativeToJsObjectMangaer::getInstance()->addNativeToJsObject(this);
}

NativeToJsObject::NativeToJsObject() : type(NativeObjectType::NONE)
{
    NativeToJsObjectMangaer::getInstance()->addNativeToJsObject(this);
}

NativeToJsObject::~NativeToJsObject()
{
    NativeToJsObjectMangaer::getInstance()->removeNativeToJsObject(this);
}

void NativeToJsObject::enableWriteRepeat(bool b)
{
    _enableWriteRepeat = b;
}

void NativeToJsObject::beginOp(unsigned char op, bool repeat)
{
    SQ_ASSERT(this->type != NativeObjectType::NONE);
    unsigned char type = toNumber(this->type);
    ArrayBuffer &buffer = DispatchManager::getInstance()->nativeToJsBuffer;

    
    if (!_enableWriteRepeat && !repeat)
    {
        std::unordered_map<int, unsigned int>::iterator it = addressOpMap.find(op);
        if (it != addressOpMap.end())
        {
            inRewritePos = buffer.getDataSize();
            buffer.setWritePos(it->second + 2); // 加上下面的type和op的字节偏移
            return;
        }
    }
    
    if (!_enableWriteRepeat)
        addressOpMap[op] = buffer.getDataSize();
    
    // printf("begin type %d op %d \n",type,op);
    
    buffer.append<unsigned char>(type);
    buffer.append<unsigned char>(op);
}

void NativeToJsObject::endOp()
{
    if (inRewritePos != -1)
    {
        // printf("end repeat \n");
        ArrayBuffer &buffer = DispatchManager::getInstance()->nativeToJsBuffer;
        buffer.setWritePos(inRewritePos);
        inRewritePos = -1;
    }
}

void NativeToJsObject::beginOpync()
{
    DispatchManager::getInstance()->beginOpNativeToJsSync();
}

void NativeToJsObject::endOpSync()
{
    DispatchManager::getInstance()->endOpNativeToJsSync();
}

void NativeToJsObject::onFrameBegin()
{
    addressOpMap.clear();
}

char *NativeToJsObject::getCurrentWriteBuffer()
{
    return DispatchManager::getInstance()->nativeToJsBuffer.getCurrentWriteBuffer();
}

int NativeToJsObject::getDataSize()
{
    return DispatchManager::getInstance()->nativeToJsBuffer.getDataSize();
}
