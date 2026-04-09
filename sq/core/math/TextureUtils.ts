

export default {
    calculateTextureSize(pixlesNum: number, powerOfTwo: boolean = false) {
        let length = 8;
        let buffer = window._malloc(length);
        window.Module.ccall("calculateTextureSize", null, ["number", "number", "number", "bool"], [pixlesNum, buffer, 0, powerOfTwo]);
        let output = Module.HEAP32.subarray(buffer / 4, buffer / 4 + 2);
        let result = {
            width: output[0],
            height: output[1]
        }
        window._free(buffer);
        return result;
    }
} as
    {
        calculateTextureSize(pixlesNum: number, powerOfTwo: boolean): { width: number, height: number }
    }
