in vec2 aParticlePos;
uniform float pointSize;
uniform mat4 cc_matViewProj;
const float physics_unit = 50.;
vec4 vert() {
  vec2 p = vec2(aParticlePos.x * physics_unit, aParticlePos.y * physics_unit);
  vec4 position = cc_matViewProj * vec4(p, -10., 1.0);
  gl_PointSize = pointSize;
  return position;
}
