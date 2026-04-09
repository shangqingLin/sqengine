#include "../../../bridge_comm.h"
#include <emscripten.h>
#include <bindings/DispatchManager.h>
#include <stdio.h>
// #include <bindings/box2d/b2WorldObjectBridge.h>
#include <engine/engine.h>
#include <engine/core/common/ArrayBuffer.h>
#include <iostream>
#include <vector>
using namespace bridge;

void onNativeToJsBufferResize(char *pointer, int size)
{
	EM_ASM("nativeDispatchManager.onResizeNativeToJsBuffer($0,$1);", pointer, size);
}

extern "C"
{

	EMSCRIPTEN_KEEPALIVE void resizeDispatchMemeory(char *buffer, int size)
	{
		DispatchManager::getInstance()->resizeJsToNativeBuffer(buffer, size);
	}

	EMSCRIPTEN_KEEPALIVE void jsToNativeDispatchSync()
	{
		DispatchManager::getInstance()->dispatchJsToNativeSync();
	}

	EMSCRIPTEN_KEEPALIVE void jsToNativeDispatch()
	{
		// printf("===============jsToNativeDispatch flush\n");
		DispatchManager::getInstance()->dispatchJsToNative();
	}

	// EMSCRIPTEN_KEEPALIVE void dispatch() {
	// printf("__________________________update size %d\n",size);
	// DispatchManager::getInstance()->dispatchToNative(size);
	// b2World* world = b2WorldObjectBridge::getInstance()->world;
	// if (world) {
	// 	world->Step(dt, 0, 0);
	// }
	// Application::getInstance()->update();
	// }

	EMSCRIPTEN_KEEPALIVE void test()
	{
		Mat3 mat1;
		Mat3 mat2;
		int c = 0;
		for(int i = 0; i < 100000000; ++i){
			// c+=1;
			mat1.mul(mat2);
		}
		mat1.print();
	}

	/*
	EMSCRIPTEN_KEEPALIVE void testHash(char* test) {

		std::vector<int> vec1;
		vec1.resize(10);
		for(int i = 0; i < vec1.size() ; ++i){
			printf("vec1 %d %d\n",i,vec1[i]);
		}


		std::vector<ArrayBuffer*> vec2;
		vec2.resize(10);
		for(int i = 0; i < vec2.size() ; ++i){
			printf("vec2 %d %p\n",i,vec2[i]);
		}

		// sqstd::hash_t seed = 2;
		// sqstd::hash_combine(seed,100);
		// sqstd::hash_combine(seed,101);
		// sqstd::hash_combine(seed,100);
		// sqstd::hash_combine(seed,100);
		// sqstd::hash_combine(seed,100);
		// printf("==================================seed first %u hash\n",seed);

		// sqstd::hash_t seed2 = 2;
		// sqstd::hash_combine(seed2,1);
		// sqstd::hash_combine(seed2,2);
		// sqstd::hash_combine(seed2,3);
		// sqstd::hash_combine(seed2,4);
		// sqstd::hash_combine(seed2,5);


		// sqstd::hash_t seed3 = 2;
		// sqstd::hash_combine(seed3,5);
		// sqstd::hash_combine(seed3,4);
		// sqstd::hash_combine(seed3,3);
		// sqstd::hash_combine(seed3,2);
		// sqstd::hash_combine(seed3,1);

		// printf("==================================seed two %u hash\n",seed2);

		// printf("==================================seeddd %d %d\n",seed == seed2,seed2 == seed3);


		// ArrayBuffer buffer;
		// buffer.append(10);
		// buffer.append(118);
		// buffer.append(1212);

		// ArrayBuffer buffer2;

		// unsigned char* adress = reinterpret_cast<unsigned char*>(buffer.getBuffer());

		// buffer2.append(adress);

		// EM_ASM("window.test($0)",buffer2.getBuffer());
	}*/
};