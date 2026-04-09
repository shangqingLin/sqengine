in vec2 aPos;
in vec4 aColor;
uniform mat4 cc_matViewProj;
uniform mat4 cc_matWorld;

out vec4 color;

vec4 vert() {
    vec4 position = cc_matViewProj * cc_matWorld * vec4(aPos, -1., 1.0);
    // 优化性能不使用除法，这里相当于：1/255
    float f225 = 0.00392156862745098;
    color = aColor * f225;
    return position;
}
