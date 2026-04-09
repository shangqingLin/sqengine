#ifndef _GFX_BUFFER_OBJECT_H_
#define _GFX_BUFFER_OBJECT_H_
#include "./define.h"
#include "../../core/common/ArrayBuffer.h"
class BufferObject{
     public:
     
       BufferObject();
       
       virtual ~BufferObject(){};
       /**
        * 创建BufferObject的时候进行初始化
       */
       virtual void initialize(BufferInfo& info) = 0;

       /**
        * 更新缓冲区的数据
       */
       virtual void update (char* data, int size) = 0;

       /**
        * Buffer是JS端创建的，数据也存储在JS端没必要再传递到C++端了
        * 单独使用这个方法来更新JS端Buffer。
        */
       virtual void updateFromJs(int nativeId,int size) = 0;

       virtual void resize(int size) = 0;
};

#endif