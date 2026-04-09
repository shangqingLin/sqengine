uniform sampler2D sdfBufferTexture;
uniform vec2 sdfTexSize;

in float gridRow;
in float gridColumn;
uniform float pointSize;
uniform vec2 cc_screenSize;
in vec2 pos;

// circular
// float smin(float a, float b, float k) {
//    // k *= 1.0 / (1.0 - sqrt(0.5));
//    // 3.414213562373096 = 1.0 / (1.0 - sqrt(0.5))
//    k *= 3.414213562373096;
//    float h = max(k - abs(a - b), 0.0) / k;
//    return min(a, b) - k * 0.5 * (1.0 + h - 1. / inversesqrt(1.0 - h * (h - 2.0)));
// }

// quadratic polynomial
float smin(float a, float b, float k) {
   k *= 4.0;
   float h = max(k - abs(a - b), 0.0) / k;
   return min(a, b) - h * h * k * 0.25;// (1.0/4.0);
}

vec4 getGridParticlePos(float column, float row, vec2 invertSdfTexSize) {
   vec2 uv = vec2((column + 0.5) * invertSdfTexSize.x, (row + 0.5) * invertSdfTexSize.y);
   vec4 color = texture(sdfBufferTexture, uv);
   return color;
}

float sdfCircle(vec2 pos, vec2 center, float raduis) {
   vec2 v = pos - center;
   float dd = dot(v, v);
   return 1.0 / inversesqrt(dd) - raduis;
}

/**
 * 因为屏幕的长宽不一致，而我这里的SDF坐标空间使用uv的，uv不考虑屏幕宽高比。
 * 所以当屏幕长度和高度不一致的时候就无法绘制圆形，而是得到一个椭圆。
 * 所以这里将uv坐标转换到使用一个正方形表示的空间内，即宽度和高度一样的空间绘制SDF。
 *  
*/
vec2 translateToQuat(vec2 uvPos, float dd) {

   //计算uv在屏幕中各维度占用的像素个数
   vec2 pixlePos = uvPos * cc_screenSize;

    //在这个正方形内，长度和宽度应该占多少个像素，uv是多少
   return vec2(pixlePos.x * dd - 1.0, pixlePos.y * dd - 1.0);
}

vec4 frag() {
      //取最长的那个维度作为正方形的长度
   float maxAixs = max(cc_screenSize.x, cc_screenSize.y);
   float dd = 1.0 / maxAixs * 2.0;
   vec2 invertSdfTexSize = 1.0 / sdfTexSize;
   vec2 fragPos = translateToQuat(pos, dd);
   float d = 1.0;

   //不要调大此数字低端机会卡顿。还有必须使用Int，因为WebGL1.0对for循环的计数必须是使用int
   const int CHECK_SIZE = 4;
   float circleRaduis = pointSize / maxAixs;
   float sminRaduis = 0.4 * circleRaduis;
   for(int column = -CHECK_SIZE; column < CHECK_SIZE; ++column) {
      float c = clamp(float(column) + gridColumn, 0.0, sdfTexSize.x);

      for(int row = -CHECK_SIZE; row < CHECK_SIZE; ++row) {
         float r = clamp(float(row) + gridRow, 0., sdfTexSize.y);

         vec4 gridParticleInfo = getGridParticlePos(c, r, invertSdfTexSize);

         if(gridParticleInfo.b == 1.0) {
            float dd = sdfCircle(fragPos, translateToQuat(gridParticleInfo.rg, dd), circleRaduis);
            d = smin(d, dd, sminRaduis);
         }
      }
   }
   return vec4(0., 0.0, 0., mix(4.0, 0.0, step(0.0, d)));
}
