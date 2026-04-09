#define PARTICLE_FLAG_ACTIVE int(1)
#define PARTICLE_FLAG_STARTED int(4)
#define PI 3.1415926

uniform float SEED;
uniform vec2 cc_screenSize;
uniform float cc_time;

//渲染相关
uniform sampler2D aTexture;
uniform sampler2D particleTexture;
uniform vec2 particleTextureSize;

//发射阶段：初始速度
uniform float spread_angle;
uniform vec2 spread_direction;
uniform vec2 initialLinearVelocityMinMax;
uniform float inherit_emitter_velocity_ratio;

//速度更新阶段
uniform vec2 orbitVelocitMinMax;
uniform vec2 radialVelocityMinMax;
uniform vec2 turbulenceInfluenceMinMax;
uniform vec2 velocity_pivot;
uniform vec2 angularVelocityMinMax;
uniform vec2 directionalVelocityMinMax;

//吸引器
uniform float attractorType;
uniform mat3 attractorTransform;

//uniform太多了，合并
uniform vec2 attractorShapeExtents;
uniform vec2 attractorAttenuationAndStrength;
vec2 attractor_force = vec2(0.);

//物理模拟
uniform vec2 gravity;
uniform vec2 dampingMinMax;
uniform vec2 linearAccelMinMax;
uniform vec2 radialAccelMinMax;
uniform vec2 tangentAccelMinMax;

//初始发射位置相关属性
#ifdef EMIT_SHAPE_POINTS
uniform sampler2D emission_texture_points;
uniform vec2 emission_texture_size;
uniform float emission_texture_point_count;
uniform float emission_texture_normal;
#else
uniform float emssion_shape;
uniform vec2 emission_box_extents;
uniform float emssion_shape_radius;
uniform float emission_ring_height;
uniform float emission_ring_cone_angle;
uniform vec2 emission_ring_axis;
uniform float emission_ring_inner_radius;
#endif

struct Particle {
    int flag;
    float lifetime_percent;
    vec2 position;
};

struct DynamicsParameters {
    float angular_velocity;
    float initial_velocity_multiplier;
    float directional_velocity;
    float radial_velocity;
    float orbit_velocity;
    float turb_influence;
};

struct PhysicalParameters {
    float linear_accel;
    float radial_accel;
    float tangent_accel;
    float damping;
};

PhysicalParameters physicsParam;
DynamicsParameters dynamicsParam;

vec2 safe_normalize(vec2 direction) {
    return mix(vec2(0.0),normalize(direction),step(0.00001,dot(direction,direction)));
}

#ifdef WEBGL2 
uint hash(uint x) {
    x = ((x >> uint(16)) ^ x) * uint(73244475);
    x = ((x >> uint(16)) ^ x) * uint(73244475);
    x = (x >> uint(16)) ^ x;
    return x;
}
#else
float hash(float x) {
    x = fract(x * 0.1031);
    x *= x + 33.33;
    x *= x + x;
    return fract(x);
}
#endif

float getParticleIndex() {
    // 当前 fragment 的线性索引（0, 1, 2, ...）
    return (gl_FragCoord.y - 0.5) * cc_screenSize.x + (gl_FragCoord.x - 0.5);
    // return gl_FragCoord.y * cc_screenSize.x + gl_FragCoord.x;
}

/**
* 这个是LCG随机算法，有笔记，查看《LCG伪随机算法.md》。
* LCG算法要求seed是一个inout,即每次调用就修改seed的值，这次的随机值
* 需要基于上次得到的结果（随机数）来计算的。
* 返回的结果为[0,1]范围内的值
*/
int randSeed = int(0); //全局保持累加
float rand_from_seed() {
    int k;
    int s = randSeed;
    if(s == 0) {
        s = 305420679;
    }
    k = s / 127773;
    s = 16807 * (s - k * 127773) - 2836 * k;
    if(s < 0) {
        s += 2147483647;
    }
    randSeed = s;

    //表示随机的数字65536次作为一次周期

    #ifdef WEBGL2
    return float(randSeed % 65536) / 65535.0;
    #else
      // 取模在数学上的定义本来就是： s mod 65536 = s - floor(s / 65536) * 65536
    // 你可以举例子使用这个公式计算与使用%计算，他们得出的结果是一样的
    float low16 = float(randSeed) - floor(float(randSeed) / 65536.0) * 65536.0;
    return low16 / 65535.0;
    #endif
}

float rand_from_seed_m1_p1() {
    //将结果变换到[-1,1]范围内
    return rand_from_seed() * 2.0 - 1.0;
}

vec4 getPixlesFromArrayByIndex(float index, sampler2D tex, float texWidth, float texHeight) {
    float row = floor(index / texWidth);
    float col = mod(index, texWidth);
    //OpenGL底层采样是从像素中心开始，而不是像素矩形左下角开始的，所以需要加0.5来保证取到的是纹理的中心点。
    vec2 uv = vec2((col + 0.5) / texWidth, (row + 0.5) / texHeight);
    return texture(tex, uv);
}

/**
 * 根据指定的路径形状的位置来作为发射点
 * 这样粒子系统可以支持任何的发射形状了 
*/
#ifdef EMIT_SHAPE_POINTS
vec4 emitPath() {
    vec4 result = vec4(0.);
    float pointIndex = min(emission_texture_point_count - 1., floor(rand_from_seed() * emission_texture_point_count));
    pointIndex = mix( floor(pointIndex * 0.5 ) , pointIndex,step(1.0, emission_texture_normal));

    vec4 positionData = getPixlesFromArrayByIndex(pointIndex, emission_texture_points, emission_texture_size.x, emission_texture_size.y);

    // if(emission_texture_normal != 0.) {
    //     // 一个像素记录一个位置,一个法线的方式
    //     result.xy = positionData.xy;

    //     //朝normal的方向发射粒子，这样效果就能和形状更加吻合
    //     result.zw = normalize(positionData.zw);
    // } else {
    //     //一个像素4个数字，所以存储了两个position
    //     //偶数取xy，奇数取zw。避免使用if
    //     float which = mix(0., 1.0, mod(pointIndex, 2.));
    //     float index = pointIndex - which;
    //     index = ceil(index * 0.5);
    //     result.xy = (1.0 - which) * positionData.xy + which * positionData.zw;
    // }


    //避免使用if分支
    float which = mix(0., 1.0, mod(pointIndex, 2.));
    // emission_texture_normal 设置为1表示使用normal
    result.xy = mix((1.0 - which) * positionData.xy + which * positionData.zw, positionData.xy, step(1.0, emission_texture_normal));
    result.zw = mix(vec2(0.), normalize(positionData.zw), step(1.0, emission_texture_normal));
    return result;
}
#else

vec2 pointEmitShape() {
    return vec2(0., 0.);
}

/**
* 在一个矩形内随机位置
*/
vec2 boxEmitShape() {
    vec2 point;
    point.x = rand_from_seed() * 2.0 - 1.0;
    point.y = rand_from_seed() * 2.0 - 1.0;
    point *= emission_box_extents;
    return point;
}

/**
 * 我们的目的是在一个圆形内均匀分布随机点，这里重点就是要做到均匀
 * https://zhuanlan.zhihu.com/p/447898464
 */
vec2 sphereEmitShape() {
    //随机一个半径
    float radius = emssion_shape_radius * sqrt(rand_from_seed());

    // 在360度内随机一个角度
    float theta = rand_from_seed() * 2.0 * PI;

    float x1 = radius * cos(theta);
    float y1 = radius * sin(theta);

    //在此基础上再加一点插值，随之更加随机
    float p = rand_from_seed();
    vec2 point;
    point.x = mix(0., x1, p);
    point.y = mix(0., y1, p);
    return point;
}

/**
* 用于实现圆台的形状，并在圆台内均匀分布。
  其所有的逻辑按照圆台计算，根据对emission_ring_cone_angle的设置，可以实现如下特殊的图形
  1、圆锥 当emission_ring_cone_angle设置为0度是为圆锥
  2、圆柱 当emission_ring_cone_angle设置为90度是为圆柱
  3、圆台 当emission_ring_cone_angle设置为(0,90)范围是为圆台

 圆环：
   所谓的圆环就是将圆台中心挖空，挖空的部分不被忽略掉不产生位置，挖空的大小由emission_ring_inner_radius设置
   如果emission_ring_inner_radius设置为0，则表示为实圆台。

 1、emission_ring_radius 圆台的底面半径
 2、emission_ring_height 圆台的高度
 3、emission_ring_cone_angle  圆台斜边与底面之间的夹角
 4、emission_ring_axis 用来设置圆台的朝向，也就是圆台是竖着放、斜着放等各种摆放的朝向
    emission_ring_axis = vec3(0.0, 1.0, 0.0) 表示圆台是竖着放的，从底部大圆向上朝 Y 轴方向延伸。
    emission_ring_axis = vec3(1.0, 0.0, 0.0) 表示圆台横着放，从底部大圆向右沿 X 轴方向延伸

*/
vec2 ringEmitShape() {

    float emission_ring_radius = emssion_shape_radius;
    float radiusClamped = max(0.001, emission_ring_radius);

    /**
      如果topRadius = 0  则表示为圆锥，即顶部是尖的
      如果topRadius!= 0 则表示为圆台， 顶部是圆锥的横截面，这个就是截面的半径
      下面算法是按照圆台方式计算，圆锥只不过是  topRadius = 0 的一种特殊情况
      算法：
      圆台侧视图（剖面），h（高度，emission_ring_height）与r成90度,斜面与r角度为θ（emission_ring_cone_angle）, topRadius = a
          a
        /--|
       /   | h 
      /θ   |
      ------
        r

     作如下图，即将h往左挪，与斜边形成夹角β
            a  
        /|----|
       /β| ←  | h
      /  |    |
      --b------
          r

     所以有 a = r - b, 我们只需要求出b即可，因为 tan(β) = b/h 所以 b = tan(β) * h，因为β = 180 - 90 - θ = 90 - θ     
     所以：
     a = r - tan(90 - θ) * h。

     当 emission_ring_cone_angle = 0时 tan(90)数学上不存在，则为无限的的数值，由于max的限制，所以0度时，topRadius = 0，为椎体

    */

    float angle = 90. - emission_ring_cone_angle;
    float topRadius = angle != 90. && angle != 180. ? max(radiusClamped - tan(radians(angle)) * emission_ring_height, 0.) : 0.0;
    float y_pos = rand_from_seed();
    float skew = max(min(radiusClamped, topRadius) / max(radiusClamped, topRadius), 0.5);
    y_pos = radiusClamped < topRadius ? pow(y_pos, skew) : 1.0 - pow(y_pos, skew);

    //在与emission_ring_axis垂直的方向上也需要随机
    //相当于我们要在emission_ring_axis垂直方向和水平方向上都要随机
    float ringRandomRadius = sqrt(rand_from_seed() * (radiusClamped * radiusClamped - emission_ring_inner_radius * emission_ring_inner_radius) + emission_ring_inner_radius * emission_ring_inner_radius);

    ringRandomRadius = mix(ringRandomRadius, ringRandomRadius * (topRadius / radiusClamped), y_pos);

    vec2 axis = emission_ring_axis == vec2(0.0) ? vec2(0., 1.0) : normalize(emission_ring_axis);
    vec2 vertical_axis = vec2(-axis.y, axis.x);
    vertical_axis = normalize(vertical_axis);

    vec2 point = vertical_axis * ringRandomRadius + (y_pos * emission_ring_height - emission_ring_height * 0.5) * axis;
    return point;
}

vec2 calculate_initial_position() {
    if(emssion_shape == 0.) {
        return pointEmitShape();
    } else if(emssion_shape == 1.0) {
        return boxEmitShape();
    } else if(emssion_shape == 2.0) {
        return ringEmitShape();
    } else if(emssion_shape == 3.0) {
        return sphereEmitShape();
    }
    return vec2(0., 0.);
}
#endif

/*
  计算新生的粒子的速度相关的量
*/
void calculate_initial_dynamics_params() {
    dynamicsParam.angular_velocity = mix(angularVelocityMinMax.x, angularVelocityMinMax.y, rand_from_seed());
    dynamicsParam.initial_velocity_multiplier = mix(initialLinearVelocityMinMax.x, initialLinearVelocityMinMax.y, rand_from_seed());
    dynamicsParam.directional_velocity = mix(directionalVelocityMinMax.x, directionalVelocityMinMax.y, rand_from_seed());
    dynamicsParam.radial_velocity = mix(radialVelocityMinMax.x, radialVelocityMinMax.y, rand_from_seed());
    dynamicsParam.orbit_velocity = mix(orbitVelocitMinMax.x, orbitVelocitMinMax.y, rand_from_seed());
    dynamicsParam.turb_influence = mix(turbulenceInfluenceMinMax.x, turbulenceInfluenceMinMax.y, rand_from_seed());
}

/**
 * 初始化物理相关参数
*/
void calculate_initial_physical_params() {
    physicsParam.linear_accel = mix(linearAccelMinMax.x, linearAccelMinMax.y, rand_from_seed());
    physicsParam.radial_accel = mix(radialAccelMinMax.y, radialAccelMinMax.y, rand_from_seed());
    physicsParam.tangent_accel = mix(tangentAccelMinMax.x, tangentAccelMinMax.y, rand_from_seed());
    physicsParam.damping = mix(dampingMinMax.x, dampingMinMax.y, rand_from_seed());
}

/**
 * 随机一个速度的方向
 * 1、在[0,spreadAngle]范围内随机一个角度作为速度方向
 * 2、spreadAngle 和 dynamicDirection 可以组成一个扇形
*/
vec2 get_random_direction_from_spread(float spreadAngle, vec2 dynamicDirection) {

    //1、先在一个范围随机一个角度
    float spread_rad = radians(spreadAngle);// 转换为弧度
    float angle1_rad = rand_from_seed() * spread_rad; //随机一个角度

    //根据指定的发射方向计算一个角度
    //相当于在spreadAngle基础再偏移spread_direction的角度，组成一个扇形的范围
    angle1_rad += spread_direction.x != 0.0 ? atan(spread_direction.y, spread_direction.x) : sign(spread_direction.y) * (PI * 0.5);

    //还可以继续添加一个方向
    angle1_rad += dot(dynamicDirection, dynamicDirection) > 0. ? atan(dynamicDirection.y, dynamicDirection.x) : 0.;

    //根据总的角度计算出一个速度方向
    vec2 velocity;
    velocity.x = cos(angle1_rad);
    velocity.y = sin(angle1_rad);
    velocity = normalize(velocity);
    return velocity;
}

/*
  计算环绕速度。即速度绕圆形方向运动 （类似于龙卷风）
  实现了一个粒子系统中的环绕运动效果，实现的是粒子围绕发射器中心做匀速圆周运动的效果
*/
vec2 process_orbit_displacement(vec2 position, vec2 emission_position, float delta) {
    if(abs(dynamicsParam.orbit_velocity) < 0.01 || delta < 0.001) {
        return vec2(0.0);
    }
    vec2 displacement = vec2(0.0);
    float orbit_amount = dynamicsParam.orbit_velocity;
    if(orbit_amount != 0.0) {

        //以发射点emission_position为圆心，圆心到粒子当前的位置(position)为半径(diff)的圆
        //计算发射点到粒子当前位置的向量
        vec2 diff = position - emission_position;

        //计算旋转的角度
        // PI * 2.0 = 360度
        float ang = orbit_amount * PI * 2.0 * delta;

        //根据上面的ang旋转角度构建旋转矩阵
        mat2 rot = mat2(vec2(cos(ang), -sin(ang)), vec2(sin(ang), cos(ang)));

        //对diff向量旋转ang的角度
        vec2 newOr = rot * diff;

        //旋转后的向量减去原来的向量得出需要在这个方向上位移的向量
        displacement = newOr - diff;
    }

    //位移除以时间等于速度
    return displacement / delta;
}

/*
 这段代码实现的是粒子系统中的 径向位移效果（Radial Displacement）
 简单来说，这个函数的作用就是让粒子沿着从 velocity_pivot（中心点）指向粒子当前位置的方向移动（径向运动）。
 其中velocity_pivot就是设置的中心点

 应用场景：
 1、粒子系统的径向发射效果
 2、爆炸效果的碎片扩散
 3、吸引效果（当速度为负时）
*/
vec2 process_radial_displacement(vec2 transform, float delta) {
    vec2 radial_displacement = vec2(0.0);
    if(delta < 0.001) {
        return radial_displacement;
    }
    float radial_displacement_multiplier = 1.0;

    //如果当前位置与velocity_pivot点距离大于0.01,则就按照这个方向移动

    vec2 direction = transform - velocity_pivot;
    if(dot(direction, direction) > 0.0001) { //length(direction)，只不过为了节省性能求长度平方
        radial_displacement = normalize(direction) * radial_displacement_multiplier * dynamicsParam.radial_velocity;
    } else {
        //如果velocity_pivot点到当前位置距离太小，则随机一个方向移动
        radial_displacement = get_random_direction_from_spread(360.0, vec2(0.)) * dynamicsParam.radial_velocity;
    }

    //如果是小于0，则表示与radial_displacement相反的运动
    //确保位移长度不超过当前位置到中心点的距离除以时间增量
    //防止物体越过中心点时出现闪烁现象
    if(radial_displacement_multiplier * dynamicsParam.radial_velocity < 0.0) {
        radial_displacement = normalize(radial_displacement) *
            min(abs(radial_displacement_multiplier * dynamicsParam.radial_velocity), length(direction / delta));
    }

    return radial_displacement;
}

void process_physical_parameters(float lifetime_percent) {
    // if(tex_parameters[PARAM_LINEAR_ACCEL].is_valid()) {
    //     physicsParam.linear_accel *= texture(linear_accel_texture, vec2(lifetime_percent)).r;
    // }
    // if(tex_parameters[PARAM_RADIAL_ACCEL].is_valid()) {
    //     physicsParam.radial_accel *= texture(radial_accel_texture, vec2(lifetime_percent)).r;
    // }
    // if(tex_parameters[PARAM_TANGENTIAL_ACCEL].is_valid()) {
    //     physicsParam.tangent_accel *= texture(tangent_accel_texture, vec2(lifetime_percent)).r;
    // }
    // if(tex_parameters[PARAM_DAMPING].is_valid()) {
    //     physicsParam.damping *= texture(damping_texture, vec2(lifetime_percent)).r;
    // }
}

vec3 process_directional_displacement(DynamicsParameters param, float lifetime_percent, mat4 transform, mat4 emission_transform) {
    // vec3 displacement = texture(directional_velocity_curve, vec2(lifetime_percent)).xyz * param.directional_velocity;
    // if(directional_velocity_global) {
    //     displacement = (emission_transform * vec4(displacement, 0.0)).xyz;
    // }
    // return displacement;
    return vec3(0.);
}

Particle getParticleParam(float startIndex) {
    Particle param;
    vec4 pixle1 = getPixlesFromArrayByIndex(startIndex, particleTexture, particleTextureSize.x, particleTextureSize.y);
    param.flag = int(pixle1.x);
    param.lifetime_percent = pixle1.y;
    param.position = pixle1.zw;
    return param;
}

void attractorForce(vec2 particlePosition) {
    attractor_force = vec2(0.);
    if(attractorType == 0.) return;

    float directionality = 0.17;

    vec2 rel_vec = particlePosition - attractorTransform[2].xy;
    vec2 abs_pos = abs(rel_vec / attractorShapeExtents);
    float d = max(abs_pos.x, abs_pos.y);

    // attractorTransform.point(pos) - vec2(0);

    if (d > 1.0) {
        return; //表示超出了边界的粒子，不受吸引器控制
    }

    float amount = max(0.0, 1.0 - d);
    amount = pow(amount,attractorAttenuationAndStrength.x);
    vec2 dir = safe_normalize(rel_vec);
    // dir = safe_normalize(mix(dir, FRAME.attractors[i].transform[2].xyz,directionality);
    attractor_force -=  amount * dir * attractorAttenuationAndStrength.y;
}

/**
  计算应用到粒子上的力，模拟物理运动效果
  这里没有计算粒子质量，所以你记得要考虑上
*/
vec2 process_physical(float delta, vec2 velocity, vec2 pos, vec2 emitter_pos) {
    vec2 force = gravity;

    //下面代码记得要判断dot() > 0.，否则等于0的话证明x和y是0，normalize中有除法会造成数据错误的

    //应用线加速度
    force += dot(velocity, velocity) > 0. ? normalize(velocity) * physicsParam.linear_accel : vec2(0.);

    //应用径向加速度
    vec2 diff = pos - emitter_pos;
    force += dot(diff, diff) > 0. ? normalize(diff) * physicsParam.radial_accel : vec2(0.);

    //应用切线加速度。切线就是与当前位置垂直的向量
    // normalize(diff.yx * vec2(-1.0, 1.0)) 计算 diff的切线
    force += dot(diff.yx, diff.yx) > 0. ? normalize(diff.yx * vec2(-1.0, 1.0)) * physicsParam.tangent_accel : vec2(0.);

    force += attractor_force;

    // force += vec2(1.0) * length(diff);
    velocity += force * delta;

    // if(attractor_interaction_enabled) {
    //     force += ATTRACTOR_FORCE;
    // }

        //应用阻尼
    if(physicsParam.damping > 0.0) {
        float v = length(velocity);
            // if(!particle_flags[PARTICLE_FLAG_DAMPING_AS_FRICTION]) {
            //     v -= physicsParam.damping * delta;
            // } else {
        if(v > 0.001) {
                    // Realistic friction formula. We assume the mass of a particle to be 0.05 kg.
            float damp = v * v * physicsParam.damping * 0.05 * delta;
            v -= damp;
        }
            // }

        // if(v < 0.0) {
        //     velocity = vec2(0.0); 
        // } else {
        //     velocity = normalize(velocity) * v;
        // }
        // 替代上面if语句
        velocity = mix(vec2(0.0), normalize(velocity) * v, step(0.0, v));
    }
    return velocity;
}

bool checkFlag(int flag, int eFlag) {
    #ifdef WEBGL2
    return bool(flag & eFlag);    
    #else
    return eFlag == PARTICLE_FLAG_ACTIVE ? flag == 1 || flag == 5 : eFlag == PARTICLE_FLAG_STARTED ? flag == 4 || flag == 5 : false;
    #endif
}

vec4 frag() {
    float particleIndex = getParticleIndex();
    Particle particle = getParticleParam(particleIndex);
    if(!checkFlag(particle.flag, PARTICLE_FLAG_ACTIVE)) {
        discard;
    }

    #ifdef WEBGL2
    randSeed = int(hash(uint(particleIndex) + 1u + uint(SEED)));
    #else
    randSeed = int(hash(particleIndex + 1.0 + SEED));
    #endif

    float delta = cc_time * 0.001;
    vec2 v_uv = gl_FragCoord.xy / cc_screenSize;
    vec4 texInfo = texture(aTexture, v_uv);
    vec2 pos = texInfo.xy;
    vec2 velocity = texInfo.zw;
    calculate_initial_dynamics_params();
    calculate_initial_physical_params();

    if(checkFlag(particle.flag, PARTICLE_FLAG_STARTED)) {
        vec2 shapeEmitDirection = vec2(0.);

        #ifdef EMIT_SHAPE_POINTS
        vec4 pathInfo = emitPath();
        pos = pathInfo.xy;
        shapeEmitDirection = pathInfo.zw;
        #else
        pos = calculate_initial_position();
        #endif

        vec2 velocityDirection = get_random_direction_from_spread(spread_angle, shapeEmitDirection);
        vec2 newVelocity = velocityDirection * dynamicsParam.initial_velocity_multiplier;
        newVelocity += velocity * inherit_emitter_velocity_ratio;
        velocity = newVelocity;
    }

    attractorForce(pos);
    velocity = process_physical(delta, velocity, pos, vec2(0.));

    vec2 controlled_displacement = vec2(0.0);
    controlled_displacement += process_orbit_displacement(pos, particle.position, delta);
    controlled_displacement += process_radial_displacement(particle.position, delta);
            // controlled_displacement += process_directional_displacement();

    vec2 final_velocity = controlled_displacement + velocity;
    pos.x += final_velocity.x * delta;
    pos.y += final_velocity.y * delta;
    return vec4(pos, velocity);
}