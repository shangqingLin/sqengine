#include "DispatchManager.h"
#include "bridge_comm.h"
#include "./nativetojs/NativeToJsObjectMangaer.h"

using namespace bridge;

static DispatchManager *instance = new DispatchManager;

void bridge::onDispatchNativeToJsBufferResize()
{
	onNativeToJsBufferResize(DispatchManager::getInstance()->nativeToJsBuffer.getBuffer(), DispatchManager::getInstance()->nativeToJsBuffer.getBuffSize());
}

DispatchManager::DispatchManager() {}

void DispatchManager::registerObjectBridge(ObjectType type, JsToNativeObjectBridge *obj)
{
	dispatchMap[type] = obj;
}

DispatchManager *DispatchManager::getInstance()
{
	return instance;
}

void DispatchManager::initialize()
{
	nativeToJsBuffer.onResizeCall = onDispatchNativeToJsBufferResize;
	// 1024 * 1024 = 1M
	nativeToJsBuffer.resize(1048576);
	resetNativeToJs();
}

void DispatchManager::resetNativeToJs()
{
	nativeToJsBuffer.clearData();
	// 第一个4字节用来记录同步操作数据记录所在的位置
	// 第二个4字节用来记录异步操作时，记录数据的大小，等帧处理完毕后再设置数据大小
	nativeToJsBuffer.appendEmpty(8);
}

void DispatchManager::setupNativeToJs()
{
	unsigned int size = nativeToJsBuffer.getDataSize();
	nativeToJsBuffer.setValue(4, size);
}

void DispatchManager::beginOpNativeToJsSync()
{
	/**
	 * 为了从JS端读取到C++端的数据，传统的做法是直接从JS端调用一个方法，然后返回数据
	 * 但这种读取方式效率比较低：1、数据返回的时候需要申请内存写入，如果数据比较大或多就很麻烦
	 *
	 * 这里是直接写到Native内存中，不用重新申请内存
	 *
	 * nativeToJsSyncPos 先存储一下当前数据的位置，同步的数据从这个位置开始写入，
	 * endOpNativeToJsSync之后会回到这个位置
	 */
	nativeToJsSyncPos = nativeToJsBuffer.getDataSize();
}

void DispatchManager::endOpNativeToJsSync()
{
	nativeToJsBuffer.setWritePos(0);
	// Js端记录一下从哪个位置开始读取同步数据
	nativeToJsBuffer.append(nativeToJsSyncPos);

	// 重置回到同步操作之前的数据的位置
	nativeToJsBuffer.setWritePos(nativeToJsSyncPos);
	nativeToJsSyncPos = 0;
}

//======================

void DispatchManager::resizeJsToNativeBuffer(char *pointer, int size)
{
	jsToNativebuffer.setExternalBuffer(pointer, size);
}

void DispatchManager::runDispatchJsToNative()
{
	int pos = jsToNativebuffer.getCurrentPos();

	// printf("run op begin pos %d  \n", pos);

	unsigned short &type = *jsToNativebuffer.popp<unsigned short>();
	bridge::ObjectType objType = bridge::ObjectType(type);

	// printf(" run objType %d  \n", objType);

	dispatchMap.find(objType)->second->dispatch(jsToNativebuffer, objType);

	// printf("run op end pos %d  \n",jsToNativebuffer.getCurrentPos());
}

void DispatchManager::dispatchJsToNativeSync()
{
	jsToNativebuffer.setReadPos(0);
	unsigned int &address = *jsToNativebuffer.popp<unsigned int>();
	unsigned int &size = *jsToNativebuffer.popp<unsigned int>();
	unsigned int endPos = size + address;
	// printf("=================dispatchJsToNativeSync %u %u \n",address,size);
	jsToNativebuffer.setReadPos(address);
	while (jsToNativebuffer.getCurrentPos() < endPos)
	{
		runDispatchJsToNative();
	}

	jsToNativebuffer.setReadPos(0);
}

void DispatchManager::dispatchJsToNative()
{
	jsToNativebuffer.setReadPos(12);
	unsigned int &size = *jsToNativebuffer.popp<unsigned int>();

	// printf("++++++++++++++dispatchToNative %u\n",size);

	if (size > 0)
	{
		while (jsToNativebuffer.getCurrentPos() < size)
		{
			runDispatchJsToNative();
		}
		// 将size清空
		jsToNativebuffer.setValue<unsigned int>(12, 0);
	}

	jsToNativebuffer.clearData();
}

void DispatchManager::frameBegin()
{
	NativeToJsObjectMangaer::getInstance()->onFrameBegin();
}

void DispatchManager::frameEnd()
{
	setupNativeToJs();

	// 必须放到这里执行，因为可能帧的最后业务层又往这里填充数据了
	// 如果放在帧开始前，那么就清理掉帧最后的时候填充的数据了
	resetNativeToJs();
}

float DispatchManager::readFrameTime()
{
	jsToNativebuffer.setReadPos(8);
	return *jsToNativebuffer.popp<float>();
}