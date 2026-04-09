#pragma once
#include "../core/math/Size.h"

class Screen
{
private:
    Size<int> renderSize;
    Size<int> designSize;

public:
    Screen();
    inline const Size<int> &getRenderSize() { return renderSize; };
    inline const Size<int> &getDesignSize() { return designSize; };
    void setRenderSize(int width, int height);
    void setDesignSize(int width, int height);
};

extern Screen *screen;