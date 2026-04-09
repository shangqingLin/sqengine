in vec2 aPos;
in float particleIndex;

uniform vec2 aTextureSize;
uniform sampler2D aTexture;

uniform mat4 cc_matViewProj;
uniform mat4 cc_matWorld;

vec2 getParticlePos(float particleIndex) {
    vec2 inverTextureSize = 1.0 / aTextureSize;
    float row = floor(particleIndex * inverTextureSize.x);
    float col = mod(particleIndex, aTextureSize.x);
    vec2 uv = vec2((col + 0.5) * inverTextureSize.x, (row + 0.5) * inverTextureSize.y);
    vec4 info = texture(aTexture, uv);
    return info.xy;
}

vec4 vert() {
    vec2 vpos = getParticlePos(particleIndex);
    vec4 position = cc_matViewProj * cc_matWorld * vec4(aPos + vpos, -1., 1.0);
    return position;
}