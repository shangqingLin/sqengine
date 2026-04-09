#ifndef _INPUT_ASSEMBLER_H_
#define _INPUT_ASSEMBLER_H_

#include "define.h"
#include "BufferObject.h"

/**
 * 定义绘制时需的顶点属性数据、分配方式、索引绘制信息
 */
class InputAssembler
{
public:
  virtual ~InputAssembler() = default;
  std::vector<BufferObject *> vertexBuffers;

  /**
   * vertexAttribPointer 顶点Buffer的内存布局
   */
  std::vector<std::vector<Attribute>> attrs;

  BufferObject *indexBuffer = NULL;

  /**
   * 标记顶点的分配是否发生了改变
   * 为了配合VAO，发生了改变的话，需要重新设置一下VAO
   */
  bool dirty = true;
};

#endif