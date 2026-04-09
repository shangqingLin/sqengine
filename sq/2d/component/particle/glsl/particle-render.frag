uniform sampler2D renderTexture;

in vec4 color;
in vec2 uv;

vec4 frag() {
    vec4 textureColor = texture(renderTexture, uv);
    return textureColor * color;
}
