#ifndef _BINDINGS_WEBGL_COMMAND_H_
#define _BINDINGS_WEBGL_COMMAND_H_

#include <vector>

namespace bridge
{

    struct ActiveGPUUniform
    {
        unsigned short location;

        // 这个Uniform在哪个Block中
        unsigned short blockIndex;

        // 在block哪个位置上
        unsigned short memberIndex;
    };

    struct ActiveAttribute
    {
        unsigned short index;
        unsigned short location;
    };

    struct GPUShaderInfo
    {
        std::vector<ActiveAttribute> activeAttributes;
        std::vector<ActiveGPUUniform> activeUniform;
        std::vector<ActiveGPUUniform> activeSampler;
    };

    class WebGLCommandBridge
    {
    public:
        static WebGLCommandBridge *getInstance();
        void createShader(int program, int effectId, unsigned int macroFlags, int shaderIndex, GPUShaderInfo &);
    };
} // namespace bridge

#endif