in vec2 aPos;

vec4 vert() {
    vec4 pos = vec4(aPos, -1., 1.);
    return pos;
}