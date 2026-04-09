#pragma once
#include "../components/graphics/Graphics.h"

class GraphicsAssembler
{
    public:
        friend class Graphics;
        void updateRenderData(Graphics *grahics) ;
        void fillBuffers(Graphics* graphics);
        static GraphicsAssembler* getInstance();
};
