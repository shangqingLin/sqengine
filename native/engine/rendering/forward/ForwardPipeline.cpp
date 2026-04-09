#include "ForwardPipeline.h"
#include "ForwardRenderFlow.h"

using namespace pipeline;

ForwardPipeline::ForwardPipeline():RenderPipeline(){
}

void ForwardPipeline::initialize(){
    RenderPipeline::initialize();
    ForwardRenderFlow *forward = new ForwardRenderFlow();
    flows.push_back(forward);
}

ForwardPipeline::~ForwardPipeline(){
    
}