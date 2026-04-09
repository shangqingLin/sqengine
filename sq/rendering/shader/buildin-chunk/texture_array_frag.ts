
export const texture_array_define = "uniform sampler2D cc_spriteTexture[10];"
export const texture_array_varying = "varying float textureIndex;\nvarying vec2 uv;";

export const texture_array_func = `
vec4 getTextureColor(){ 
     //GLSL中规定数组访问必须使用常量
    if(textureIndex == 0.){      
        return texture(cc_spriteTexture[0],uv);
    }else if(textureIndex == 1.0){
        return texture(cc_spriteTexture[1],uv);
    }else if(textureIndex == 2.0){
        return texture(cc_spriteTexture[2],uv);  
    }else if(textureIndex == 3.0){
        return texture(cc_spriteTexture[3],uv);    
    }else if(textureIndex == 4.0){
        return texture(cc_spriteTexture[4],uv);   
    }else if(textureIndex == 5.0){
        return texture(cc_spriteTexture[5],uv);  
    }else if(textureIndex == 6.0){   
        return texture(cc_spriteTexture[6],uv);   
    }else if(textureIndex == 7.0){
        return texture(cc_spriteTexture[7],uv);
    }else if(textureIndex == 8.0){
        return texture(cc_spriteTexture[8],uv);
    }else if(textureIndex == 9.0){
        return texture(cc_spriteTexture[9],uv);
    }
    return vec4(1.);
}
    
vec4 getTextureColorWithUv(vec2 uv){ 
    if(textureIndex == 0.){      
        return texture(cc_spriteTexture[0],uv);
    }else if(textureIndex == 1.0){
        return texture(cc_spriteTexture[1],uv);
    }else if(textureIndex == 2.0){
        return texture(cc_spriteTexture[2],uv);  
    }else if(textureIndex == 3.0){
        return texture(cc_spriteTexture[3],uv);    
    }else if(textureIndex == 4.0){
        return texture(cc_spriteTexture[4],uv);   
    }else if(textureIndex == 5.0){
        return texture(cc_spriteTexture[5],uv);  
    }else if(textureIndex == 6.0){   
        return texture(cc_spriteTexture[6],uv);   
    }else if(textureIndex == 7.0){
        return texture(cc_spriteTexture[7],uv);
    }else if(textureIndex == 8.0){
        return texture(cc_spriteTexture[8],uv);
    }else if(textureIndex == 9.0){
        return texture(cc_spriteTexture[9],uv);
    }
    return vec4(1.);
}  
`

