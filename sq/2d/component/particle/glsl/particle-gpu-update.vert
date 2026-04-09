in vec2 aPos;

uniform mat4 cc_matViewProj;

vec4 vert()
{
    vec4 position = cc_matViewProj * vec4(aPos, -1., 1.0);
    return position;
}
