#include "RenderTexture.h"
#include "../core/common/ArrayBuffer.h"
#include "../framework/Application.h"
#include "../core/base/config.h"

RenderTexture::RenderTexture()
    : Texture2d(-1), window(nullptr)
{
   // printf("create render texture \n");
}

RenderTexture::RenderTexture(int id)
    : Texture2d(id), window(nullptr)
{
   // printf("create render texture \n");
}

void RenderTexture::create(ITexture2DCreateInfo *)
{
   SQ_ASSERT(false);
}

void RenderTexture::initiliazeFormJS(char *data)
{
   ArrayBuffer buffer;
   buffer.setExternalBuffer(data, 13);
   RenderTextureDefine def;
   def.width = *buffer.popp<int>();
   def.height = *buffer.popp<int>();
   def.format = Format(*buffer.popp<unsigned int>());
   def.needDepthStencil = *buffer.popp<char>();
   def.op = gfx::AttachmentOp(*buffer.popp<char>());
   this->initiliaze(def);
}

// #include "../gfx/webgl2/WebGL2Texture.h"
void RenderTexture::initiliaze(RenderTextureDefine &define)
{
   if (window)
   {
      Application::getInstance()->getRoot().removeRenderWindow(window);
   }

   SQ_ASSERT(define.width > 0);
   SQ_ASSERT(define.height > 0);
   SQ_ASSERT(define.format != Format::UNKNOWN);

   width = define.width;
   height = define.height;

   if (define.sampler.has_value())
   {
      setFilters(define.sampler.value().minFilter, define.sampler.value().magFilter);
      setWrapMode(define.sampler.value().wrapS, define.sampler.value().wrapR, define.sampler.value().wrapT);
   }

   RenderWindowDefine windowDefine;
   windowDefine.width = define.width;
   windowDefine.height = define.height;
   windowDefine.colorAttachemts.assign(1, gfx::ColorAttachment());
   windowDefine.colorAttachemts[0].format = define.format;
   windowDefine.colorAttachemts[0].op = define.op;

   if (define.needDepthStencil)
   {
      // printf(">>>>>>>>>>>>>>>>>>>>>>>>> %d \n",define.needDepthStencil);
      windowDefine.depthStencilAttachment = gfx::DepthStencilAttachment();
      windowDefine.depthStencilAttachment.value().format = Format::DEPTH_STENCIL;
   }

   window = Application::getInstance()->getRoot().createRenderWindow(windowDefine);
   this->_gfxTexture = window->getColorAttachmentTexture(0);

   // printf("create renderTexture glTexture %d \n",dynamic_cast<WebGL2Texture *>(_gfxTexture)->getGPUTexture().glTexture);
}

RenderTexture::~RenderTexture()
{
   // window内部自动销毁所有的Texture
   _gfxTexture = nullptr;
   if (window)
   {
      Application::getInstance()->getRoot().removeRenderWindow(window);
      window = nullptr;
   }
}