#ifndef _SCENE_RENDER_WINDOW_H_
#define _SCENE_RENDER_WINDOW_H_

#include <vector>
#include "../gfx/base/RenderPass.h"
#include "../gfx/base/Texture.h"
#include "../gfx/base/RenderPass.h"

class Camera;
struct RenderWindowDefine
{
    int width;
    int height;
    std::vector<gfx::ColorAttachment> colorAttachemts;
    std::optional<gfx::DepthStencilAttachment> depthStencilAttachment;

    /**
     * 是否为默认的帧缓冲区
     */
    bool defaultRenderPass = false;
};

/**
 * 代表渲染目标。实现主渲染和离屏渲染
 * 将渲染结果输出到这里代表的目标上。
 */
class RenderWindow
{
private:
    std::vector<Camera *> cameras;

    // 存储颜色缓冲区的纹理
    std::vector<gfx::Texture *> colorTextures;

    // 存储深度模板缓冲区的纹理
    gfx::Texture *depthStencilTexture;

    gfx::RenderPass *renderPass;
    
public:
    friend class Root;
    int width;
    int height;
    RenderWindow();
    ~RenderWindow();
    void initialize(RenderWindowDefine &);
    void attachCamera(Camera *);
    void detachCamera(Camera *);
    void extractRenderCameras(std::vector<Camera *> &);
    inline gfx::Texture *getColorAttachmentTexture(int index) { return colorTextures[index]; };
    inline gfx::RenderPass* getRenderPass(){ return renderPass; };
};

#endif