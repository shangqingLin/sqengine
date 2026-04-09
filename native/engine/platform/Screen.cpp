#include "Screen.h"

/**
 * 为全局单例，由JS端创建
 */
Screen *screen = nullptr;

Screen::Screen()
{
    screen = this;
}

void Screen::setRenderSize(int width, int height)
{
    renderSize.set(width, height);
}

void Screen::setDesignSize(int width, int height)
{
    designSize.set(width, height);
}
