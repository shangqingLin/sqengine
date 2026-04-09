import { Type } from "../../gfx/index"
import { IBlockInfo, ISamplerTextureInfo } from "../../assets/assets/Effect";
export interface BuildInBlock {
    block: IBlockInfo;
}

export const UBOCamera: BuildInBlock = {
    block: {
        buildIn: true,
        name: "CCCamera",
        binding: 0,
        members: [
            {
                type: Type.MAT4,
                name: "cc_matViewProj",
                count: 1
            }
        ]
    }
}

export const UBOGlobal: BuildInBlock = {
    block: {
        buildIn: true,
        name: "CCGlobal",
        binding: 1,
        members: [
            //游戏帧时间
            {
                type:Type.FLOAT,
                count:1,
                name:"cc_time"
            },
            {
                type:Type.FLOAT,
                count:1,
                name:"sq_gameDuration"
            },

            //屏幕分辨率
            {
                type:Type.FLOAT2,
                count:1,
                name:"cc_screenSize"
            },
        ]
    }
}

export const UBOLocal: BuildInBlock = {
    block: {
        buildIn: true,
        name: "CCLocal",
        binding: 1,
        members: [
            {
                type: Type.MAT4,
                name: "cc_matWorld",
                count: 1
            }
        ]
    }
}

export const samplerTextures: ISamplerTextureInfo = {
    name: "cc_spriteTexture",
    count: 10,
    type: Type.SAMPLER2D,
    binding: 0,
    buildin: true
}


