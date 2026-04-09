#include "Mesh.h"
#include "MeshInstance.h"

DrawPrimitiveMesh::~DrawPrimitiveMesh()
{
}

Mesh::Mesh()
    : indexBuffer(NULL)
{
  inputAssembler = DeviceManager::getInstance()->device->createInputAssembler();
}

void Mesh::addVertextBuffer(VertexBuffer *buffer, const std::vector<Attribute> &attributes)
{
  addVertextBuffer(buffer);
  setVertexAttributeDscriptor(attributes, vertexBuffers.size() - 1);
}

/**
 * 添加一个顶点Buffer，并为这个Buffer设置Attribute（即GPU如何分配这个Buffer）
 */
void Mesh::addVertextBuffer(VertexBuffer *buffer, const Attribute &attribute)
{
  addVertextBuffer(buffer);
  setVertexAttributeDscriptor(attribute, vertexBuffers.size() - 1);
}

void Mesh::addVertextBuffer(VertexBuffer *v)
{

  bool newAdd = true;
  for (int i = 0; i < vertexBuffers.size(); ++i)
  {
    if (vertexBuffers[i] == v)
    {
      newAdd = false;
      break;
    }
  }
  if (newAdd)
  {

    inputAssembler->vertexBuffers.push_back(v->getBufferObject());
    vertexBuffers.push_back(v);
    inputAssembler->dirty = true;
  }
}

void Mesh::setIndexBuffer(IndexBuffer *i)
{
  if (indexBuffer != i)
  {
    indexBuffer = i;
    inputAssembler->indexBuffer = i->getBufferObject();
    inputAssembler->dirty = true;
  }
}

void Mesh::setVertexAttributeDscriptor(const Attribute &attribute, int index)
{
  std::vector<Attribute> attributes;
  attributes.push_back((Attribute &)attribute);
  setVertexAttributeDscriptor(attributes, index);
}

void Mesh::setVertexAttributeDscriptor(const std::vector<Attribute> &attributes, int index)
{
  int stride = getAttributeStride(attributes);

  // size()为无符号，如果size为0，则0-1=-1，-1不能存储到无符号中，所以需要现将size()
  // 转为有符号int才行
  int size = (int)inputAssembler->attrs.size() - 1;
  if (size - 1 < index)
  {
    inputAssembler->attrs.push_back(attributes);
    strides.push_back(stride);
  }
  else
  {
    inputAssembler->attrs[index] = attributes;
    strides[index] = stride;
  }
  inputAssembler->dirty = true;
}

void Mesh::clear()
{
  for (int i = 0; i < vertexBuffers.size(); ++i)
  {
    vertexBuffers[i]->clearData();
  }

  if (indexBuffer)
    indexBuffer->clearData();
}

void Mesh::upload() const
{
  SQ_ASSERT(indexBuffer);
  indexBuffer->upload();
  SQ_ASSERT(vertexBuffers.size() > 0);
  for (int c = 0; c < vertexBuffers.size(); ++c)
  {
    VertexBuffer *vb = vertexBuffers[c];
    vb->upload();
  }
}

void Mesh::reset()
{
  indexBuffer = nullptr;
  vertexBuffers.clear();
  inputAssembler->attrs.clear();
}

Mesh::~Mesh()
{

  // printf("autoDeleteState %p %d \n", this, autoDeleteBuffer);

  if (autoDeleteBuffer && indexBuffer)
  {
    delete indexBuffer;
  }
  indexBuffer = nullptr;

  if (autoDeleteBuffer)
  {
    for (int i = 0; i < vertexBuffers.size(); ++i)
    {
      delete vertexBuffers[i];
    }
  }

  vertexBuffers.clear();

  delete inputAssembler;
  inputAssembler = NULL;
}