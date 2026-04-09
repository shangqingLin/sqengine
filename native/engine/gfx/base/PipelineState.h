#ifndef _PILIE_STATE_H_
#define _PILIE_STATE_H_
#include "define.h"
#include "Shader.h"
#include "./InputAssembler.h"

struct IPipelineStateInfo{
    Shader *shader = nullptr;
    RasterizerState* rasterizerState = nullptr;
    DepthState *depthState = nullptr;
    StencilState *stencilState = nullptr;
    BlendState *blendState = nullptr;
    InputAssembler *assembler = nullptr;
    PrimitiveMode primitive;
};

class PipelineState {
    public:
        Shader *shader = nullptr;
        RasterizerState* rasterizerState = nullptr;
        DepthState *depthState = nullptr;
        StencilState *stencilState = nullptr;
        BlendState *blendState = nullptr;
        PrimitiveMode primitive = PrimitiveMode::TRIANGLE_LIST;
        InputAssembler *assembler = nullptr;
};

#endif