in vec2 aPos;
in vec2 aUv;
in vec4 aColor;
in float aTextureIndex;
uniform mat4 cc_matViewProj;

out vec4 color;
out vec2 uv;
out float textureIndex;
vec4 vert() {
    vec4 position = cc_matViewProj * vec4(aPos, -1., 1.0);
    textureIndex = aTextureIndex;
    uv = aUv;
    color = aColor / 255.0;
    return position;
}
