in vec2 aPos;
in float aParticleIndex;

uniform sampler2D posBuffer;
uniform sampler2D rowColumnTexture;
uniform vec2 posTexSize;
uniform float pointSize;
uniform vec2 cc_screenSize;
uniform mat4 cc_matViewProj;

const float physics_unit = 50.;

out float gridRow;
out float gridColumn;
out vec2 pos;

vec4 getParticlePosAndGrid() {
  vec2 invertPosTexSize = 1.0 / posTexSize;
   //一个像素存储两个粒子位置，aParticleIndex的偶数位于rg，基数位于ba
  float index = floor(aParticleIndex * 0.5);
  float row = floor(index * invertPosTexSize.x);
  float col = mod(index, posTexSize.x);
  vec2 uv = vec2((col + 0.5) * invertPosTexSize.x, (row + 0.5) * invertPosTexSize.y);
  vec4 color = texture(posBuffer, uv);
  vec4 gridColor = texture(rowColumnTexture, uv);

  float d = mod(aParticleIndex, 2.);
  vec4 result = vec4(0.);
  result.xy = mix(color.rg, color.ba, d);
  result.zw = mix(gridColor.rg, gridColor.ba, d);
  return result;
}


vec4 vert() {
  vec4 particlePosAndGrid = getParticlePosAndGrid();
  vec2 pixelPos = particlePosAndGrid.xy * physics_unit;
  float quatSize = pointSize * 1.5; //在此基础上再扩大1.5倍增加检查SDF的像素个数
  vec2 vertexPos = aPos * quatSize * 0.01 + pixelPos;
  vec4 position = cc_matViewProj * vec4(vertexPos, -10., 1.0);
  gridRow = particlePosAndGrid.w;
  gridColumn = particlePosAndGrid.z;
  pos = vertexPos / cc_screenSize;
  return position;
}