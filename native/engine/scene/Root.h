#ifndef _ROOT_H_
#define _ROOT_H_
#include "../rendering/RenderPipeline.h"
#include "RenderWindow.h"
#include <unordered_map>

class Application;
class Root
{
private:
    pipeline::RenderPipeline *pipleLine;
    std::vector<Camera *> renderCameraList;
    std::vector<RenderWindow *> renderWindows;
public:
    /**
     * 默认的帧缓冲区。
     * OpenGL默认在屏幕上显示的就是这个缓冲区
     */
    RenderWindow *mainWindow;
    Root(Application *);
    void initialize();
    void start();
    void updateEnable();
    void updateDisable();
    void update(float dt);
    void setPipleline(pipeline::RenderPipeline *);
    RenderWindow *createRenderWindow(RenderWindowDefine &);
    void removeRenderWindow(RenderWindow *);
};

#endif