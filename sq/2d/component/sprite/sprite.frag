uniform sampler2D cc_spriteTexture[10];
in float textureIndex;
in vec2 uv;
in vec4 color;

vec4 getTextureColor() { 
     return textureIndex == 0.0 ? texture(cc_spriteTexture[0], uv) : 
          textureIndex == 1.0 ? texture(cc_spriteTexture[1], uv) : 
          textureIndex == 2.0 ? texture(cc_spriteTexture[2], uv) :
          textureIndex == 3.0 ? texture(cc_spriteTexture[3], uv) :
          textureIndex == 4.0 ? texture(cc_spriteTexture[4], uv) :
          textureIndex == 5.0 ? texture(cc_spriteTexture[5], uv) :
          textureIndex == 6.0 ? texture(cc_spriteTexture[6], uv) :
          textureIndex == 7.0 ? texture(cc_spriteTexture[7], uv) :
          textureIndex == 8.0 ? texture(cc_spriteTexture[8], uv) :
          textureIndex == 9.0 ? texture(cc_spriteTexture[9], uv) :
          vec4(1.);

}
vec4 frag() {
     return getTextureColor() * color;
}
