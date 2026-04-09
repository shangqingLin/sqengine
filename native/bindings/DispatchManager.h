#ifndef DispatchManager_H__
#define DispatchManager_H__

#include "../engine/core/common/ArrayBuffer.h"
#include <unordered_map>
#include "define.h"
#include "./jstonative/JsToNativeObjectBridge.h"

namespace bridge
{

	class DispatchManager
	{

	private:
		ArrayBuffer nativeToJsBuffer;
		std::unordered_map<bridge::ObjectType, JsToNativeObjectBridge *> dispatchMap;
		ArrayBuffer jsToNativebuffer;
		unsigned int nativeToJsSyncPos = 0;

		void resetNativeToJs();
		void setupNativeToJs();
		void runDispatchJsToNative();
		void beginOpNativeToJsSync();
		void endOpNativeToJsSync();

	public:
		friend class NativeToJsObject;
		friend void onDispatchNativeToJsBufferResize();
		static DispatchManager *getInstance();
		DispatchManager();
		float readFrameTime();
		void initialize();
		void resizeJsToNativeBuffer(char *pointer, int size);
		void dispatchJsToNative();
		void dispatchJsToNativeSync();
		void frameBegin();
		void frameEnd();
		void registerObjectBridge(ObjectType, JsToNativeObjectBridge *);
	};
}

#endif