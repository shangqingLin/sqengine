#include "Delaunator.h" // 头文件
#include <cmath>        // 数学函数
#include <limits>       // 数值极值
#include <algorithm>    // std::max 等

using namespace geometry2d; // 使用命名空间

Delaunator Delaunator::FromPoints(const std::vector<Vec2> &points) // 从点集构建
{                                                                  
    const size_t n = points.size();                                // 点数量
    std::vector<float> coords;                                    // 坐标数组
    coords.resize(n * 2);                                          // 分配坐标长度
    for (size_t i = 0; i < n; ++i)                                 // 遍历点
    {                                                              
        coords[2 * i] = points[i].x;                               // x 坐标
        coords[2 * i + 1] = points[i].y;                           // y 坐标
    } 
    return Delaunator(coords);
} 

Delaunator::Delaunator(const std::vector<float> &input)
    : coords(input)                                                                                     // 拷贝坐标
{                                                                                                       
    const uint32_t n = static_cast<uint32_t>(coords.size() >> 1);                                       // 点数
    const uint32_t maxTriangles = static_cast<uint32_t>(std::max<int>(static_cast<int>(2 * n) - 5, 0)); // 最大三角形数
    _triangles.resize(maxTriangles * 3);                                                                // 分配三角形索引
    _halfedges.resize(maxTriangles * 3);                                                                // 分配半边

    _hashSize = static_cast<uint32_t>(std::ceil(std::sqrt(static_cast<float>(n)))); // 哈希大小
    _hullPrev.resize(n);                                                             // hull prev
    _hullNext.resize(n);                                                             // hull next
    _hullTri.resize(n);                                                              // hull tri
    _hullHash.resize(_hashSize);                                                     // hull hash

    _ids.resize(n);   // 索引数组
    _dists.resize(n); // 距离数组

    trianglesLen = 0; // 当前三角形长度
    _cx = 0.0;        // 圆心 x
    _cy = 0.0;        // 圆心 y
    _hullStart = 0;   // hull start

    update(); // 执行三角剖分
} 

void Delaunator::update() 
{                                                                 
    const uint32_t n = static_cast<uint32_t>(coords.size() >> 1); 
    if (n == 0)
    {                                                             
        triangles.clear();                                        // 清空三角形
        halfedges.clear();                                        // 清空半边
        hull.clear();                                             // 清空 hull
        return;                                                   // 返回
    } 

    float minX = std::numeric_limits<float>::infinity();  // 最小 x
    float minY = std::numeric_limits<float>::infinity();  // 最小 y
    float maxX = -std::numeric_limits<float>::infinity(); // 最大 x
    float maxY = -std::numeric_limits<float>::infinity(); // 最大 y

    for (uint32_t i = 0; i < n; ++i)        // 遍历点
    {                                       
        const float x = coords[2 * i];     // 取 x
        const float y = coords[2 * i + 1]; // 取 y
        if (x < minX)
            minX = x; // 更新最小 x
        if (y < minY)
            minY = y; // 更新最小 y
        if (x > maxX)
            maxX = x; // 更新最大 x
        if (y > maxY)
            maxY = y; // 更新最大 y
        _ids[i] = i;  // 写入索引
    } 

    const float cx = (minX + maxX) * 0.5; // bbox 中心 x
    const float cy = (minY + maxY) * 0.5; // bbox 中心 y

    uint32_t i0 = 0; // seed 0
    uint32_t i1 = 0; // seed 1
    uint32_t i2 = 0; // seed 2

    float minDist = std::numeric_limits<float>::infinity();            // 最小距离
    for (uint32_t i = 0; i < n; ++i)                                     // 找中心最近点
    {                                                                    
        const float d = dist(cx, cy, coords[2 * i], coords[2 * i + 1]); // 距离平方
        if (d < minDist)                                                 // 更近
        {                                                                
            i0 = i;                                                      // 更新 i0
            minDist = d;                                                 // 更新最小距离
        } 
    } 

    const float i0x = coords[2 * i0];     // i0 x
    const float i0y = coords[2 * i0 + 1]; // i0 y

    minDist = std::numeric_limits<float>::infinity(); // 重置最小距离
    for (uint32_t i = 0; i < n; ++i)                   // 找 i0 最近点
    {                                                  
        if (i == i0)
            continue;                                                      // 跳过自身
        const float d = dist(i0x, i0y, coords[2 * i], coords[2 * i + 1]); // 距离平方
        if (d < minDist && d > 0.0)                                        // 更新最近且非重复
        {                                                                  
            i1 = i;                                                        // 更新 i1
            minDist = d;                                                   // 更新最小距离
        } 
    } 

    float i1x = coords[2 * i1];     // i1 x
    float i1y = coords[2 * i1 + 1]; // i1 y

    float minRadius = std::numeric_limits<float>::infinity(); // 最小外接圆半径
    for (uint32_t i = 0; i < n; ++i)                            // 找最小外接圆
    {                                                           
        if (i == i0 || i == i1)
            continue;                                                                        // 跳过
        const float r = circumradius(i0x, i0y, i1x, i1y, coords[2 * i], coords[2 * i + 1]); // 外接圆半径平方
        if (r < minRadius)                                                                   // 更小
        {                                                                                    
            i2 = i;                                                                          // 更新 i2
            minRadius = r;                                                                   // 更新半径
        } 
    } 

    float i2x = coords[2 * i2];     // i2 x
    float i2y = coords[2 * i2 + 1]; // i2 y

    if (!std::isfinite(minRadius)) // 共线情况
    {
        for (uint32_t i = 0; i < n; ++i) // 计算距离
        {
            const float dx = coords[2 * i] - coords[0];     // dx
            const float dy = coords[2 * i + 1] - coords[1]; // dy
            _dists[i] = (dx != 0.0) ? dx : dy;               // 选择 x 或 y 差
        }
        quicksort(_ids, _dists, 0, static_cast<int>(n) - 1);  // 排序
        hull.clear();                                         // 清空 hull
        hull.resize(n);                                       // 分配 hull
        uint32_t j = 0;                                       // hull 计数
        float d0 = -std::numeric_limits<float>::infinity(); // 上一次距离
        for (uint32_t i = 0; i < n; ++i)                      // 生成 hull
        {
            const uint32_t id = _ids[i]; // id
            const float d = _dists[id]; // 距离
            if (d > d0)                  // 去重
            {
                hull[j++] = id; // 添加
                d0 = d;         // 更新
            }
        }
        hull.resize(j);    // 裁剪 hull
        triangles.clear(); // 清空三角形
        halfedges.clear(); // 清空半边
        return;
    }

    if (orient2d(i0x, i0y, i1x, i1y, i2x, i2y) < 0.0) // 确保逆时针
    {
        const uint32_t ti = i1; // 交换索引
        const float tx = i1x;  // 交换 x
        const float ty = i1y;  // 交换 y
        i1 = i2;                // i1 <- i2
        i1x = i2x;              // i1x <- i2x
        i1y = i2y;              // i1y <- i2y
        i2 = ti;                // i2 <- old i1
        i2x = tx;               // i2x <- old i1x
        i2y = ty;               // i2y <- old i1y
    }

    const Vec2 center = circumcenter(i0x, i0y, i1x, i1y, i2x, i2y); // 外接圆心
    _cx = center.x;                                                 // 保存圆心 x
    _cy = center.y;                                                 // 保存圆心 y

    for (uint32_t i = 0; i < n; ++i)                                            // 计算到圆心距离
    {                                                                           
        _dists[i] = dist(coords[2 * i], coords[2 * i + 1], center.x, center.y); // 距离平方
    } 

    quicksort(_ids, _dists, 0, static_cast<int>(n) - 1); // 按距离排序

    _hullStart = i0;       // hull 起点
    uint32_t hullSize = 3; // hull 初始大小

    _hullNext[i0] = i1; // i0 -> i1
    _hullNext[i1] = i2; // i1 -> i2
    _hullNext[i2] = i0; // i2 -> i0

    _hullPrev[i0] = i2; // i0 <- i2
    _hullPrev[i1] = i0; // i1 <- i0
    _hullPrev[i2] = i1; // i2 <- i1

    _hullTri[i0] = 0; // tri 索引
    _hullTri[i1] = 1; // tri 索引
    _hullTri[i2] = 2; // tri 索引

    std::fill(_hullHash.begin(), _hullHash.end(), -1);        // 清空 hash
    _hullHash[_hashKey(i0x, i0y)] = static_cast<int32_t>(i0); // 记录 i0
    _hullHash[_hashKey(i1x, i1y)] = static_cast<int32_t>(i1); // 记录 i1
    _hullHash[_hashKey(i2x, i2y)] = static_cast<int32_t>(i2); // 记录 i2

    trianglesLen = 0;                     // 重置长度
    _addTriangle(i0, i1, i2, -1, -1, -1); // 添加种子三角形

    std::array<uint32_t, EDGE_STACK_SIZE> edgeStack{}; // 边栈

    float xp = 0.0;                           // 上一个 x
    float yp = 0.0;                           // 上一个 y
    for (uint32_t k = 0; k < _ids.size(); ++k) // 遍历点
    {                                          
        const uint32_t i = _ids[k];            // 当前点索引
        const float x = coords[2 * i];        // x
        const float y = coords[2 * i + 1];    // y

        if (k > 0 && std::abs(x - xp) <= EPSILON && std::abs(y - yp) <= EPSILON) // 近重复点
        {
            continue;
        }
        xp = x;
        yp = y;

        if (i == i0 || i == i1 || i == i2) // 跳过种子点
        {
            continue;
        }

        int32_t start = -1;                           // 起始边
        const uint32_t key = _hashKey(x, y);          // hash key
        for (uint32_t j = 0; j < _hashSize; ++j)      // 查找可见边
        {                                             
            start = _hullHash[(key + j) % _hashSize]; // 从 hash 取
            if (start != -1 && start != static_cast<int32_t>(_hullNext[start]))
                break; // 找到有效边
        } 

        if (start == -1) // 未找到
        {                
            continue;    // 跳过
        } 

        uint32_t e = _hullPrev[start]; // 从前驱开始
        uint32_t q = e;                // 临时变量
        while (true)                   // 寻找可见边
        {
            q = _hullNext[e];
            if (orient2d(x, y, coords[2 * e], coords[2 * e + 1], coords[2 * q], coords[2 * q + 1]) < 0.0)
                break; // 可见
            e = q;
            if (e == static_cast<uint32_t>(start)) // 绕回
            {
                e = UINT32_MAX; // 标记无效
                break;
            }
        }
        if (e == UINT32_MAX) // 无法插入
        {
            continue;
        }

        uint32_t t = _addTriangle(e, i, _hullNext[e], -1, -1, static_cast<int32_t>(_hullTri[e])); // 添加三角形
        _hullTri[i] = _legalize(t + 2, edgeStack);                                                // 法律化
        _hullTri[e] = t;                                                                          // 更新 hull tri
        hullSize++;                                                                               // hull 增长

        uint32_t n = _hullNext[e]; // 前进指针
        while (true)               // 向前扩展
        {                          
            q = _hullNext[n];      // 后继
            if (orient2d(x, y, coords[2 * n], coords[2 * n + 1], coords[2 * q], coords[2 * q + 1]) >= 0.0)
                break;                                                                                           // 不可见
            t = _addTriangle(n, i, q, static_cast<int32_t>(_hullTri[i]), -1, static_cast<int32_t>(_hullTri[n])); // 添加
            _hullTri[i] = _legalize(t + 2, edgeStack);                                                           // 更新
            _hullNext[n] = n;                                                                                    // 标记移除
            hullSize--;                                                                                          // hull 减少
            n = q;                                                                                               // 前进
        }

        if (e == static_cast<uint32_t>(start)) // 向后扩展
        {                                      
            while (true)                       // 循环
            {                                  
                q = _hullPrev[e];              // 前驱
                if (orient2d(x, y, coords[2 * q], coords[2 * q + 1], coords[2 * e], coords[2 * e + 1]) >= 0.0)
                    break;                                                                                           // 不可见
                t = _addTriangle(q, i, e, -1, static_cast<int32_t>(_hullTri[e]), static_cast<int32_t>(_hullTri[q])); // 添加
                _legalize(t + 2, edgeStack);                                                                         // 法律化
                _hullTri[q] = t;                                                                                     // 更新
                _hullNext[e] = e;                                                                                    // 标记移除
                hullSize--;                                                                                          // 减少
                e = q;                                                                                               // 回退
            }
        }

        _hullStart = _hullPrev[i] = e;   // 更新 hull start
        _hullNext[e] = _hullPrev[n] = i; // 连接
        _hullNext[i] = n;                // 连接

        _hullHash[_hashKey(x, y)] = static_cast<int32_t>(i);                             // 更新 hash
        _hullHash[_hashKey(coords[2 * e], coords[2 * e + 1])] = static_cast<int32_t>(e); // 更新 hash
    }

    hull.clear();                           // 清空 hull
    hull.resize(hullSize);                  // 分配 hull
    uint32_t e = _hullStart;                // 起点
    for (uint32_t i = 0; i < hullSize; ++i) // 构建 hull
    {                                       
        hull[i] = e;                        // 写入
        e = _hullNext[e];                   // 下一个
    } 

    triangles.assign(_triangles.begin(), _triangles.begin() + trianglesLen); // 截断三角形
    halfedges.assign(_halfedges.begin(), _halfedges.begin() + trianglesLen); // 截断半边
} 

uint32_t Delaunator::_hashKey(float x, float y) const                   // 哈希键
{                                                                         
    const float dx = x - _cx;                                            // dx
    const float dy = y - _cy;                                            // dy
    const float pa = pseudoAngle(dx, dy);                                // 伪角度
    return static_cast<uint32_t>(std::floor(pa * _hashSize)) % _hashSize; // 映射到桶
}

uint32_t Delaunator::_legalize(uint32_t a, std::array<uint32_t, EDGE_STACK_SIZE> &stack) // 合法化
{                                                                                        
    int i = 0;                                                                           // 栈指针
    uint32_t ar = 0;                                                                     // 右边索引
    while (true)                                                                         // 循环
    {                                                                                    
        const int32_t b = _halfedges[a];                                                 // 对边
        const uint32_t a0 = a - a % 3;                                                   // 三角形起点
        ar = a0 + (a + 2) % 3;                                                           // 右边
        if (b == -1)                                                                     // 凸包边
        {                                                                                
            if (i == 0)
                break;      
            a = stack[--i]; // 弹栈
            continue;       // 继续
        }

        const uint32_t b0 = static_cast<uint32_t>(b - b % 3);        // b 三角起点
        const uint32_t al = a0 + (a + 1) % 3;                        // 左边
        const uint32_t bl = b0 + (static_cast<uint32_t>(b) + 2) % 3; // b 左边

        const uint32_t p0 = _triangles[ar]; // p0
        const uint32_t pr = _triangles[a];  // pr
        const uint32_t pl = _triangles[al]; // pl
        const uint32_t p1 = _triangles[bl]; // p1

        const bool illegal = inCircle(           // 圆内测试
            coords[2 * p0], coords[2 * p0 + 1],  // p0
            coords[2 * pr], coords[2 * pr + 1],  // pr
            coords[2 * pl], coords[2 * pl + 1],  // pl
            coords[2 * p1], coords[2 * p1 + 1]); // p1

        if (illegal)            // 非 Delaunay
        {                       
            _triangles[a] = p1; // 交换
            _triangles[b] = p0; // 交换

            const int32_t hbl = _halfedges[bl]; // 对边
            if (hbl == -1)                      // 边在 hull
            {                                   
                uint32_t e = _hullStart;        // 从 hullStart 开始
                do                              // 循环
                {                               
                    if (_hullTri[e] == bl)      // 找到
                    {                           
                        _hullTri[e] = a;        // 更新
                        break;                  // 退出
                    } 
                    e = _hullPrev[e]; // 前驱
                } while (e != _hullStart); // 条件
            } 

            _link(a, hbl);                                   // 连接
            _link(static_cast<uint32_t>(b), _halfedges[ar]); // 连接
            _link(ar, static_cast<int32_t>(bl));             // 连接

            const uint32_t br = b0 + (static_cast<uint32_t>(b) + 1) % 3; // br
            if (i < static_cast<int>(EDGE_STACK_SIZE))                   // 入栈
            {                                                            
                stack[i++] = br;                                         // 压栈
            }
        } 
        else // 合法
        {    
            if (i == 0)
                break;   
            a = stack[--i]; // 弹栈
        } 
    }
    return ar;
}

void Delaunator::_link(uint32_t a, int32_t b) // 半边连接
{                                             
    _halfedges[a] = b;                        // 设置 a 的对边
    if (b != -1)
        _halfedges[static_cast<uint32_t>(b)] = static_cast<int32_t>(a); // 设置 b 的对边
}

uint32_t Delaunator::_addTriangle(uint32_t i0, uint32_t i1, uint32_t i2, int32_t a, int32_t b, int32_t c) // 添加三角形
{                                                                                                         
    const uint32_t t = trianglesLen;                                                                      // 起始索引
    _triangles[t] = i0;                                                                                   // 写入
    _triangles[t + 1] = i1;                                                                               // 写入
    _triangles[t + 2] = i2;                                                                               // 写入
    _link(t, a);                                                                                          // 连接
    _link(t + 1, b);                                                                                      // 连接
    _link(t + 2, c);                                                                                      // 连接
    trianglesLen += 3;                                                                                    // 更新长度
    return t;                                                                                             // 返回
}

float Delaunator::orient2d(float ax, float ay, float bx, float by, float cx, float cy) // 方向测试
{                                                                                             
    return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);                                     // 叉积符号
}

float Delaunator::pseudoAngle(float dx, float dy)     // 伪角度
{                                                        
    const float p = dx / (std::abs(dx) + std::abs(dy)); // 归一化
    return (dy > 0.0 ? 3.0 - p : 1.0 + p) * 0.25;        // 映射到 [0,1]
}

float Delaunator::dist(float ax, float ay, float bx, float by) // 距离平方
{                                                                   
    const float dx = ax - bx;                                      // dx
    const float dy = ay - by;                                      // dy
    return dx * dx + dy * dy;                                       // 返回平方
}

bool Delaunator::inCircle(float ax, float ay, float bx, float by, float cx, float cy, float px, float py) // 圆内测试
{                                                                                                                 
    const float dx = ax - px;                                                                                    // dx
    const float dy = ay - py;                                                                                    // dy
    const float ex = bx - px;                                                                                    // ex
    const float ey = by - py;                                                                                    // ey
    const float fx = cx - px;                                                                                    // fx
    const float fy = cy - py;                                                                                    // fy
    const float ap = dx * dx + dy * dy;                                                                          // ap
    const float bp = ex * ex + ey * ey;                                                                          // bp
    const float cp = fx * fx + fy * fy;                                                                          // cp
    return dx * (ey * cp - bp * fy) - dy * (ex * cp - bp * fx) + ap * (ex * fy - ey * fx) < 0.0;                  // 判定
} 

float Delaunator::circumradius(float ax, float ay, float bx, float by, float cx, float cy) // 外接圆半径平方
{                                                                                                 
    const float dx = bx - ax;                                                                    // dx
    const float dy = by - ay;                                                                    // dy
    const float ex = cx - ax;                                                                    // ex
    const float ey = cy - ay;                                                                    // ey
    const float bl = dx * dx + dy * dy;                                                          // bl
    const float cl = ex * ex + ey * ey;                                                          // cl
    const float d = 0.5 / (dx * ey - dy * ex);                                                   // 分母
    const float x = (ey * bl - dy * cl) * d;                                                     // x
    const float y = (dx * cl - ex * bl) * d;                                                     // y
    return x * x + y * y;                                                                         // 返回半径平方
} 

Vec2 Delaunator::circumcenter(float ax, float ay, float bx, float by, float cx, float cy) // 外接圆心
{                                                                                               
    const float dx = bx - ax;                                                                  // dx
    const float dy = by - ay;                                                                  // dy
    const float ex = cx - ax;                                                                  // ex
    const float ey = cy - ay;                                                                  // ey
    const float bl = dx * dx + dy * dy;                                                        // bl
    const float cl = ex * ex + ey * ey;                                                        // cl
    const float d = 0.5 / (dx * ey - dy * ex);                                                 // 分母
    const float x = ax + (ey * bl - dy * cl) * d;                                              // x
    const float y = ay + (dx * cl - ex * bl) * d;                                              // y
    return Vec2(static_cast<float>(x), static_cast<float>(y));                                  // 返回 Vec2
} 

void Delaunator::quicksort(std::vector<uint32_t> &ids, std::vector<float> &dists, int left, int right) // 快速排序
{                                                                                                       
    if (right - left <= 20)                                                                             // 小数组插入排序
    {                                                                                                   
        for (int i = left + 1; i <= right; ++i)                                                         // 插入排序
        {                                                                                               
            const uint32_t temp = ids[i];                                                               // 临时
            const float tempDist = dists[temp];                                                        // 距离
            int j = i - 1;                                                                              // j
            while (j >= left && dists[ids[j]] > tempDist)                                               // 移动
            {                                                                                           
                ids[j + 1] = ids[j];                                                                    // 右移
                --j;                                                                                    // 减少
            } 
            ids[j + 1] = temp; // 插入
        } 
    } 
    else                                        // 大数组快速排序
    {                                           
        const int median = (left + right) >> 1; // 中位数
        int i = left + 1;                       // i
        int j = right;                          // j
        swap(ids, median, i);                   // 交换
        if (dists[ids[left]] > dists[ids[right]])
            swap(ids, left, right); // 交换
        if (dists[ids[i]] > dists[ids[right]])
            swap(ids, i, right); // 交换
        if (dists[ids[left]] > dists[ids[i]])
            swap(ids, left, i); // 交换

        const uint32_t temp = ids[i];        // pivot
        const float tempDist = dists[temp]; // pivot distance
        while (true)                         // 分区
        {                                    
            do
            {
                ++i;
            } while (dists[ids[i]] < tempDist); // 左扫描
            do
            {
                --j;
            } while (dists[ids[j]] > tempDist); // 右扫描
            if (j < i)
                break;       // 完成
            swap(ids, i, j); // 交换
        } 
        ids[left + 1] = ids[j]; // 归位
        ids[j] = temp;          // 放回 pivot

        if (right - i + 1 >= j - left) // 先排大区
        {
            quicksort(ids, dists, i, right);    // 排右
            quicksort(ids, dists, left, j - 1); // 排左
        }
        else // 先排小区
        {
            quicksort(ids, dists, left, j - 1); // 排左
            quicksort(ids, dists, i, right);    // 排右
        }
    }
}

void Delaunator::swap(std::vector<uint32_t> &arr, int i, int j) // 交换
{
    const uint32_t tmp = arr[i]; // tmp
    arr[i] = arr[j];             // swap
    arr[j] = tmp;                // swap
}
