#pragma once
#include "../../../core/math/Vec2.h"
#include "../../../core/core.h"
#include "../../../core/math/Color.h"
#include "../../../assets/Texture2d.h"
#include "../../../assets/Material.h"
/**
 *
 * 粒子系统：
 * 一个粒子系统分为三个阶段
 *
 * 1、发射阶段
 *    激活粒子，并初始化粒子相关的数据，如初始位置、初始速度、初始角度等。之后不再管发射出去的粒子了
 *
 * 2、更新阶段
 *   发射出来的粒子就处于更新阶段，在粒子的整个生命周期期间，粒子一直处于更新阶段
 *   更新阶段会不断计算粒子的各种属性，如物理效果（速度、加速度等）、位置、颜色、旋转角度等
 *
 * 3、渲染阶段
 *   在屏幕上显示粒子
 *
 */

enum class ParticleEmitterShape
{
    POINT,
    BOX,
    RING,
    SPHERE,
    PATH,
    NONE
};
SQ_ENUM_CONVERSION_OPERATOR(ParticleEmitterShape);


enum class AttractorShapeType
{
    NONE = 0,
    BOX = 1,
    SPHERE = 2 
};

/**
 *
 * 例子：
 * amount = 800
 * lifetime = 2.0
 * amount_ratio = 0.25
 * 则有：
 * 真实发射粒子数 ≈ 800 × 0.25 = 200
 * 真实发射速率 ≈ 200 / 2 = 100 粒子每秒
 */

struct Particle2DEmitterDefine
{

    // 使用GPU渲染的材质
    Material *gpuUpdateProcessMaterial{nullptr};
    Material *gpuRenderMaterial{nullptr};

    // 在CPU端渲染使用的材质
    Material *cpuRenderMaterial{nullptr};

    // 在场景中标记一下哪个像素是粒子的（CPU和GPU方式都使用）
    Material *renderSceneMarkMaterial{nullptr};

    /**
     * 最多同时存在的活跃粒子数量（粒子池大小）
     * 比如：amount = 1000 表示整个粒子系统最多只能有 1000 个活跃粒子。
     * 如果达到这个数量则不会再继续发射，之后新粒子只能等到旧粒子死亡后再发射
     */
    int amount = 0;

    /**
     * 用于控制粒子发射进度
     * 真实发射粒子总数 = floor(amount * amountRatio);
     *
     * 因为amount设置了之后就不需要再改变，因为改变的大家太大了，它内部需要重新初始化整个粒子系统
     * 但在运行过程中我们可能需要显示小于amount的个数或实时调整，那么就可以通过这个属性来控制
     */
    float amountRatio = 1.0f;

    /**
     * 每个粒子的存活时长。毫秒为单位
     */
    int lifeTime = 1000;

    /**
     * 如果为0则粒子连续发射
     * 如果为1泽粒子同时发射
     */
    float explosiveness = 0.f;


    /**
     * 粒子从发射开始，就会把指定属性“插值到它在生命周期结束时应该有的值”。
     * 粒子从出生时开始，按照它的生命周期时间，线性插值到你设置的最终值
     * interp_to_end的范围是[0,1],表示插值得到的最大进度值
     *
     * 举个例子：
     * 假设你有一个粒子属性，比如 粒子的大小（scale），你想让它从 0 缓慢变成 1，然后消失。     *
     * 比如 interp_to_end = 1, lifetime = 2 秒，那 scale 会在 0～2 秒之间，从 0 插值到 1
     *
     * 在代码中，你就有如下代码了
     * float t = age / lifetime; // 当前生命周期进度 [0.0 ~ 1.0]
     * float scale = mix(0.0, interp_to_end , t); // 从 0 插值到 1
     *
     * 这个会影响如下属性的插值
     * 1、color
     * 2、速度 velocity
     * 3、scale
     * 4、alpha
     */
    float interp_to_end = 0.f;

    /**
     * 系统中所有的粒子物理属性的初始值都可以随机化。
     * 即在发射粒子的时候我们需要为此粒子生成各种初值，如速度、旋转角度、缩放等等，这些只都可以随机生成
     * 范围是[0,1]
     *
     * 公式：
     * initial_value = param_value + param_value * randomness * randf()
     * 即：
     * 参数初始值 = 设定值 + 随机偏移量；
     * 随机偏移量 = 设定值 × randomness × randf()；
     * randf() 返回范围是 [0.0, 1.0] 的随机浮点数）
     * 如果设置为randomness = 0，则表示没有随机
     */
    float randomSeed = 0.f;

    /**
     * 不同于randomSeed，这个只是用来随机每个粒子的生命长度lifeTime
     * 所以生命和其他属性随机是两个独立的种子
     */
    float randomness = 0.f;

    // 发射阶段相关设置

    //---这段设置都是为了计算最终的发射速度
    bool oneShot = false;
    bool emit = true;

    // 发射粒子：即粒子的初始位置
    ParticleEmitterShape shape = ParticleEmitterShape::POINT;
    Vec2 shapeOffet;
    float emssion_shape_radius{20.f};
    float emission_ring_height{20.f};
    float emission_ring_cone_angle{0.f};
    Vec2 emission_ring_axis;
    float emission_ring_inner_radius{0.f};

    // 如果是Box Shape，则设置Box的宽高的半径
    Vec2 emission_box_extents;

    // Path Shape
    Texture2d *emitTexturePoints{nullptr};
    float emissionTexturePointCount{0};
    // 如果当前的粒子是根据Points指定的形状发射的
    // 并且还提供了这个形状的法线数据，则发射的时候就按照这个法线的方向发射
    // 使得看上去粒子更加贴合形状
    bool emitEnableTexturePointNomral{false};

    // 发射粒子：粒子的初始速度和速度方向

    /**
     * 随机一个速度的方向
     * 1、在[0,spreadAngle]范围内随机一个角度作为速度方向
     * 2、如果还指定了 spreadDirection ，则spreadDirection组合 spreadAngle 可以组成一个扇形
     */
    float spreadAngle = 0.f;
    Vec2 spreadDirection;

    // 上面得到了速度的方向，我们还需要随机速度的大小。速度大小在[initEmitVelocityMinMuti,initEmitVelocityMaxMuti]区间随机
    float initEmitVelocityMinMuti = 0.f;
    float initEmitVelocityMaxMuti = 0.f;

    // 是否继承粒子死亡那一刻的速度。如果继承，则继承多少，inheritEmitterVelocityRatio就控制继承的比率
    // 即速度大小： [initEmitVelocityMinMuti,initEmitVelocityMaxMuti] + 死亡时的速度 * inheritEmitterVelocityRatio
    float inheritEmitterVelocityRatio = 0.f;

    //----end----

    // 速度更新阶段
    float angular_velocity_min = 0.f;
    float angular_velocity_max = 0.f;
    float directional_velocity_min = 0.f;
    float directional_velocity_max = 0.f;
    float radial_velocity_min = 0.f;
    float radial_velocity_max = 0.f;
    float orbit_velocity_min = 0.f;
    float orbit_velocity_max = 0.f;
    float turbulence_influence_min = 0.f;
    float turbulence_influence_max = 0.f;
    Vec2 velocityPivot;

    /**
     * 吸引器的主要作用就是控制速度总是朝一定的方向改变。使得粒子集中向某个方向移动.
     * 1、我们通过定义个Shape区域，在这个区域内的粒子将受到速度方向改变的作用
     */
    AttractorShapeType attractorShapeType = AttractorShapeType::NONE;
    
    //Box或 Sphere的半径，定义Shape区域的大小。超出这个区域之外的粒子不受吸引器的影响
    Vec2 attractorShapeExtents;

    //定义这个吸引器attractorShape所在的位置。相对于粒子系统本地坐标进行设置。
    Mat3 attractorShapeTransform;
    float attractor_attenuation;
    float attractor_strength;

    // display相关的
    /**
     * 粒子的渲染大小
     * 如果设置了这个就使用这里的大小，如果不设置就自动获取texture上的大小渲染
     */
    float width = 0.f;
    float height = 0.f;

    Texture2d *particleTexture{nullptr};
    Texture2d *propertyTexture{nullptr};
    float initial_angle_min = 0.f;
    float initial_angle_max = 0.f;
    float initial_scale_min = 1.f;
    float initial_scale_max = 1.f;
    Color initColor{1., 1., 1., 1.}; // 白色

    // 物理相关的设置
    Vec2 gravity{0.f, -9.8f};
    // 线加速度
    float linear_accel_min = 0.f;
    float linear_accel_max = 0.f;
    float radial_accel_min = 0.f;
    float radial_accel_max = 0.f;
    float tangent_accel_min = 0.f;
    float tangent_accel_max = 0.f;
    float damping_min = 0.f;
    float damping_max = 0.f;
};
