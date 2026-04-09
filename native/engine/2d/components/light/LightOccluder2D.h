#pragma once

/**
 * 我们需要为产生阴影的物体添加几何形状
 * 光照照射到此物体上时就是按照这个形状来投射形成投影的形状的
 * 
 * 在3D中，物体本来就已经可以建模成3维的几何形状了，但在2D中，例如Sprite，只是一张矩形图片
 * 所以我们并不能直接知道图片内容的形状，所以这里可以额外指定形状
 */

 class LightOccluder2D
 {
 private:
    /* data */
 public:
    LightOccluder2D(/* args */);
    ~LightOccluder2D();
 };
 
