/**
 * GPU和CPU渲染都使用同一个着色器文件即可，大部分是一样的。
 因为GPU和CPU唯一的不同就是Update阶段，渲染阶段是一样的。
*/
#define PARTICLE_FLAG_ACTIVE int(1)
#define PARTICLE_FLAG_STARTED int(4)

//Math::PI / 180 = 0.017453292519943295
#define DEG_TO_RAD 0.0174532

in vec2 aPos;
in vec2 aUv;

#ifdef USE_GPU
in float particleIndex;
#else
//CPU
in vec2 particlePosition;
in float lifeTimePercent;
#endif

uniform vec2 renderSize;
uniform vec4 initColor;
uniform mat4 cc_matViewProj;
uniform mat4 cc_matWorld;

#ifdef USE_GPU
uniform vec2 aTextureSize;
uniform sampler2D aTexture;

uniform sampler2D particleTexture;
uniform vec2 particleTextureSize;
#endif

uniform sampler2D propertyTexture;
uniform vec2 propertyTextureSize;
vec2 inverPropertyTextureSize;

uniform vec2 initialAngleMinMax;
uniform vec2 initialScaleMinMax;

//色相变化
uniform float hueVariationMinMax;

out vec4 color;
out vec2 uv;

vec4 rotate_hue(vec4 current_color, float hue_rot_angle) {
    float hue_rot_c = cos(hue_rot_angle);
    float hue_rot_s = sin(hue_rot_angle);
    mat4 hue_rot_mat = mat4(vec4(0.299, 0.587, 0.114, 0.0), vec4(0.299, 0.587, 0.114, 0.0), vec4(0.299, 0.587, 0.114, 0.0), vec4(0.000, 0.000, 0.000, 1.0)) +
        mat4(vec4(0.701, -0.587, -0.114, 0.0), vec4(-0.299, 0.413, -0.114, 0.0), vec4(-0.300, -0.588, 0.886, 0.0), vec4(0.000, 0.000, 0.000, 0.0)) *
        hue_rot_c +
        mat4(vec4(0.168, 0.330, -0.497, 0.0), vec4(-0.328, 0.035, 0.292, 0.0), vec4(1.250, -1.050, -0.203, 0.0), vec4(0.000, 0.000, 0.000, 0.0)) * hue_rot_s;

    return hue_rot_mat * current_color;
}

// void calculate_initial_display_params(out DisplayParameters params,inout uint alt_seed) {
    // params.scale = vec3(mix(scale_min, scale_max, rand_from_seed(alt_seed)));
    // params.scale = sign(params.scale) * max(abs(params.scale), 0.001);
    // params.hue_rotation = PI * 2.0 * mix(hue_variation_min, hue_variation_max, rand_from_seed(alt_seed));
    // params.animation_speed = mix(anim_speed_min, anim_speed_max, rand_from_seed(alt_seed));
    // params.animation_offset = mix(anim_offset_min, anim_offset_max, rand_from_seed(alt_seed));
    // params.lifetime = (1.0 - lifetime_randomness * rand_from_seed(alt_seed));
    // params.color = color_value;
    // if(color_initial_ramp.is_valid()) {
    //     params.color *= texture(color_initial_ramp, vec2(rand_from_seed(alt_seed)));
    // }
    // if(emission_shape == EMISSION_SHAPE_POINTS || emission_shape == EMISSION_SHAPE_DIRECTED_POINTS) {
    //     params.emission_texture_position = rand_from_seed(alt_seed);

    //     if(emission_color_texture.is_valid()) {
    //         int point = min(emission_texture_point_count - 1, int(params.emission_texture_position * float(emission_texture_point_count)));
    //         ivec2 emission_tex_size = aTextureSize(emission_texture_points, 0);
    //         ivec2 emission_tex_ofs = ivec2(point % emission_tex_size.x, point / emission_tex_size.x);
    //         params.color *= texelFetch(emission_texture_color, emission_tex_ofs, 0);
    //     }
    // }
// };

#ifdef USE_GPU
vec2 getParticlePos(float particleIndex) {
    // float texWidth = aTextureSize.x;
    // float texHeight = aTextureSize.y;
    vec2 inverTextureSize = 1.0 / aTextureSize;
    float row = floor(particleIndex * inverTextureSize.x);
    // float col = particleIndex - row * texWidth; // 等价于 gl_InstanceID % aTextureSize.x
    float col = mod(particleIndex, aTextureSize.x);
    vec2 uv = vec2((col + 0.5) * inverTextureSize.x, (row + 0.5) * inverTextureSize.y);
    // vec2 uv = vec2(col / texWidth, row / texHeight);
    vec4 info = texture(aTexture, uv);
    return info.xy;
}

struct Particle {
    int flag;
    float lifetime_percent;
    vec2 position;
};

// /**
//   说明请看particle-update.frag
// */
Particle getParticleParam(float particleIndex) {
    Particle param;
    vec2 invertParticleTextureSize = 1.0 / particleTextureSize;
    float row = floor(particleIndex * invertParticleTextureSize.x);
    float col = mod(particleIndex, particleTextureSize.x);
    vec2 uv = vec2((col + 0.5) * invertParticleTextureSize.x, (row + 0.5) * invertParticleTextureSize.y);
    vec4 pixle1 = texture(particleTexture, uv);
    param.flag = int(pixle1.x);
    param.lifetime_percent = pixle1.y;
    param.position = pixle1.zw;
    return param;
}
#endif

float cubicBezier(float t, float p0, float p1, float p2, float p3) {
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    float t2 = t * t;

    return p0 * one_minus_t2 * one_minus_t +
        p1 * 3.0 * t * one_minus_t2 +
        p2 * 3.0 * t2 * one_minus_t +
        p3 * t2 * t;
}

//根据起点和终点来生成三阶贝塞尔曲线，并取样
float GenerateAndSmaplerCubicBezier(float percent, vec4 p0, vec4 p1) {

    if(percent <= p0.x)
        return p0.y;
    if(percent >= p1.x)
        return p1.y;

    float d = length(p0.x - p1.x); //整条线的长度
    float offset = percent - p0.x; //当前进度距离起点的长度
    percent = offset / d; //在p0和p1之间的采样进度

    //生成三次贝塞尔曲线的控制点

    //三阶贝塞尔曲线是有4个点，形成三个区间，控制点偏移去平均偏移就可以了，当然你可以取任何你想要的，但这个比较合理一点
    // float cDist = d / 3.0;
    float cDist = d * 0.333333;

    //角度转弧度
    //Math::PI / 180 = 0.017453292519943295

    //第一个控制距离p0向前一点
    float c1y = p0.y + cDist * tan(p0.z * DEG_TO_RAD); //tangent right
    //第二控制点距离p1向后一点
    float c2y = p1.y - cDist * tan(p1.w * DEG_TO_RAD); //tangent left

    // if(int(p1.z) >> 10 == 70) return 10.;

    return cubicBezier(percent, p0.y, c1y, c2y, p1.y);
}

//起点和终点，中间有两个点，然后使用两条三阶贝塞尔曲线拼接起来的
float process_curve_4point(float percent, vec4 p0, vec4 p1, vec4 p2, vec4 p3) {
    if(p0.x <= percent && percent <= p1.x) {
        return GenerateAndSmaplerCubicBezier(percent, p0, p1);
    } else if(p1.x < percent && percent <= p2.x) {
        return GenerateAndSmaplerCubicBezier(percent, p1, p2);
    } else if(p2.x < percent && percent <= p3.x) {
        return GenerateAndSmaplerCubicBezier(percent, p2, p3);
    }

    //支持x不满足1的情况，取最后一个点的值
    if(p3.x > 0.) {
        return p3.y;
    } else if(p2.x > 0.) {
        return p2.y;
    } else if(p1.x > 0.) {
        return p1.y;
    } else if(p0.x > 0.) {
        return p0.y;
    }
    return 1.0;
}

float process_scale(float lifetime_percent) {
    vec4 point1 = texture(propertyTexture, vec2(0.5, 0.) * inverPropertyTextureSize);
    vec4 point2 = texture(propertyTexture, vec2(1.5, 0.) * inverPropertyTextureSize);
    vec4 point3 = texture(propertyTexture, vec2(2.5, 0.) * inverPropertyTextureSize);
    vec4 point4 = texture(propertyTexture, vec2(3.5, 0.) * inverPropertyTextureSize);
    float t = process_curve_4point(lifetime_percent, point1, point2, point3, point4);
    return t;
}

float process_rotate(float lifetime_percent) {
    float offset = 4.;
    vec4 point1 = texture(propertyTexture, vec2(offset + 0.5, 0.) * inverPropertyTextureSize);
    vec4 point2 = texture(propertyTexture, vec2(offset + 1.5, 0.) * inverPropertyTextureSize);
    vec4 point3 = texture(propertyTexture, vec2(offset + 2.5, 0.) * inverPropertyTextureSize);
    vec4 point4 = texture(propertyTexture, vec2(offset + 3.5, 0.) * inverPropertyTextureSize);
    float angle = process_curve_4point(lifetime_percent, point1, point2, point3, point4);

    //角度转弧度
    return angle * DEG_TO_RAD;
}

vec4 process_color(float percent) {
    float offset = 8.;
    vec4 color1 = texture(propertyTexture, vec2(offset + 0.5, 0.) * inverPropertyTextureSize);
    vec4 color2 = texture(propertyTexture, vec2(offset + 1.5, 0.) * inverPropertyTextureSize);
    vec4 color3 = texture(propertyTexture, vec2(offset + 2.5, 0.) * inverPropertyTextureSize);
    vec4 color4 = texture(propertyTexture, vec2(offset + 3.5, 0.) * inverPropertyTextureSize);
    vec4 times = texture(propertyTexture, vec2(offset + 4.5, 0.) * inverPropertyTextureSize);

    vec4 color = initColor;
    if(percent <= times.x) {
        color = mix(initColor, color1, percent);
    } else if(times.x < percent && percent <= times.y) {
        color = mix(color1, color2, (percent - times.x) / (times.y - times.x));
    } else if(times.y < percent && percent <= times.z) {
        color = mix(color2, color3, (percent - times.y) / (times.z - times.y));
    } else if(times.z < percent && percent <= times.w) {
        color = mix(color3, color4, (percent - times.z) / (times.w - times.z));
    }
    return color;
}

bool checkFlag(int flag, int eFlag) {
    #ifdef WEBGL2
    return bool(flag & eFlag);    
    #else
    return eFlag == PARTICLE_FLAG_ACTIVE ? flag == 1 || flag == 5 : eFlag == PARTICLE_FLAG_STARTED ? flag == 4 || flag == 5 : false;
    #endif
}

vec4 vert() {
    inverPropertyTextureSize = 1.0 / propertyTextureSize;

    #ifdef USE_GPU
    Particle particle = getParticleParam(particleIndex);
    float lifeTimePercent = particle.lifetime_percent;
    vec2 vpos = getParticlePos(particleIndex);
    #else
    vec2 vpos = particlePosition;
    #endif

    float scale = process_scale(lifeTimePercent); 

    //默认渲染的是100x100的大小的粒子，然后根据renderSize来缩放粒子，从而达到设置粒子大小的目的
    vec2 scaleXY = renderSize * 0.01 * scale;
    float rotation = process_rotate(lifeTimePercent);
    float rs = sin(rotation);
    float rc = cos(rotation);
    mat4 transform = mat4(vec4(rc * scaleXY.x, rs * scaleXY.x, 0., 0.), vec4(-rs * scaleXY.y, rc * scaleXY.y, 0., 0.), vec4(0., 0., 1.0, 0.), vec4(vpos, 0., 1.0));
    vec4 position = cc_matViewProj * cc_matWorld * transform * vec4(aPos, -1., 1.0);
    
    // vec4 position = cc_matViewProj * vec4(aPos,-1.,1.0);

    #ifdef USE_GPU
    color = checkFlag(particle.flag, PARTICLE_FLAG_ACTIVE) ? process_color(lifeTimePercent) : vec4(0.);
    #else
    color = process_color(lifeTimePercent);
    #endif

    uv = aUv;
    return position;
}