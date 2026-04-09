#include "earcut2D.h"
#include "geometry2d-common.h"
#include <stdio.h>
#include "../../../sqstd/StackTempArenaAllocator.h"

struct LinkNode;
sqstd::Array<LinkNode *> nodes;

struct LinkNode
{
    Vec2 *point = NULL;

    // 记录这个点在原来的数组中位于哪个位置
    int i = -1;

    // zOrder值
    int z = 0;

    bool steiner = false;

    LinkNode *next = NULL;
    LinkNode *prev = NULL;
    LinkNode *prevZ = NULL;
    LinkNode *nextZ = NULL;

    LinkNode()
    {
        nodes.push(this);
    }
};

/**
 * EarCut中需要使用到的双向链表(即第一个和最后一个都需要链接起来)
 */
class EarcutLink
{
public:
    struct LinkNode
    {
        Vec2 *point = NULL;

        // 记录这个点在原来的数组中位于哪个位置
        int i = -1;

        // zOrder值
        int z = 0;

        bool steiner = false;

        LinkNode *next = NULL;
        LinkNode *prev = NULL;
        LinkNode *prevZ = NULL;
        LinkNode *nextZ = NULL;
    };

    sqstd::Array<LinkNode> nodes;
    LinkNode *root = nullptr;
    LinkNode *last = nullptr;

    EarcutLink(int allocatePoints) : nodes(sqstd::StackTempArenaAllocator::getInstance()) {}

    void setPoints(Vec2 **points, int numberPoint)
    {
        nodes.clear();
        if (nodes.getCapacity() < numberPoint)
        {
            nodes.resize(numberPoint);
        }

        /**
         * 必须保证在链表中顶点按照逆时针排序。因为EarCut算法接下来都是按照逆时针来计算的
         */

        for (int i = 0; i < numberPoint; ++i)
        {
            LinkNode *node = insertPointLast(points[i]);
            node->i = i;
        }
    }

    LinkNode *insertPointLast(Vec2 *point)
    {
        return insertAfterPoint(point, last);
    }

    LinkNode *insertAfterPoint(Vec2 *point, LinkNode *insertNode)
    {
        if (root == nullptr)
        {
            SQ_ASSERT(insertNode == nullptr);

            root = nodes.Add();
            root->point = point;

            // 构建一个双向链表，因为多边形本来就是闭合的
            last = root;
            last->prev = root;
            root->next = last;
            return root;
        }

        LinkNode *newNode = nodes.Add();
        insertNode = insertNode == nullptr ? last : insertNode;

        if (insertNode->next)
        {
            insertNode->next->prev = newNode;
        }

        newNode->next = insertNode->next;
        newNode->prev = insertNode;
        insertNode->next = newNode;

        if (insertNode == last)
        {
            last = newNode;
        }
        return newNode;
    }

    void removeNode(LinkNode *node)
    {
        if (node->next)
            node->next->prev = node->prev;

        if (node->prev)
            node->prev->next = node->next;

        if (node->prevZ)
        {
            node->prevZ->nextZ = node->nextZ;
        }

        if (node->nextZ)
        {
            node->nextZ->prevZ = node->prevZ;
        }
    }
};

/**
 * EarCut中需要使用到的双向链表
 */
static LinkNode *insertNode(Vec2 *point, int i, LinkNode *last)
{
    LinkNode *node = new LinkNode();
    node->point = point;
    node->i = i;
    if (last)
    {
        node->next = last->next;
        node->prev = last;
        if (last->next)
        {
            last->next->prev = node;
        }
        last->next = node;
    }
    else
    {
        node->prev = node;
        node->next = node;
    }
    return node;
}

static void removeNode(LinkNode *node)
{
    if (node->next)
        node->next->prev = node->prev;

    if (node->prev)
        node->prev->next = node->next;

    if (node->prevZ)
    {
        node->prevZ->nextZ = node->nextZ;
    }

    if (node->nextZ)
    {
        node->nextZ->prevZ = node->prevZ;
    }
}

static LinkNode *buildLinkList(sqstd::Array<Vec2 *> &points)
{
    LinkNode *last = NULL;

    // 保证在链表中顶点按照逆时针排序。因为算法接下来都是按照逆时针来计算的
    if (geometry2d::polygonVertSeq(points))
    {
        // printf("顺时针\n");
        for (int i = points.getCount() - 1; i >= 0; --i)
        {
            last = insertNode(points[i], i, last);
        }
    }
    else
    {
        //  printf("逆时针\n");
        for (int i = 0; i < points.getCount(); ++i)
        {
            last = insertNode(points[i], i, last);
        }
    }

    if (last && last->point->equals(*last->next->point))
    {
        removeNode(last);
        last = last->next;
    }
    return last;
}

//---------------------------------------------------------------
static int zOrder(int x, int y, float minX, float minY, float size)
{

    x = 32767 * (x - minX) / size;
    y = 32767 * (y - minY) / size;

    x = (x | (x << 8)) & 0x00FF00FF;
    x = (x | (x << 4)) & 0x0F0F0F0F;
    x = (x | (x << 2)) & 0x33333333;
    x = (x | (x << 1)) & 0x55555555;

    y = (y | (y << 8)) & 0x00FF00FF;
    y = (y | (y << 4)) & 0x0F0F0F0F;
    y = (y | (y << 2)) & 0x33333333;
    y = (y | (y << 1)) & 0x55555555;

    return x | (y << 1);
}

static LinkNode *sortLinked(LinkNode *list)
{
    LinkNode *p = NULL;
    LinkNode *q = NULL;
    LinkNode *tail = NULL;
    LinkNode *e = NULL;
    int i, numMerges, pSize, qSize, inSize = 1;

    do
    {
        p = list;
        list = NULL;
        tail = NULL;
        numMerges = 0;
        while (p)
        {
            numMerges++;
            q = p;
            pSize = 0;
            for (i = 0; i < inSize; ++i)
            {
                ++pSize;
                q = q->nextZ;
                if (!q)
                    break;
            }
            qSize = inSize;

            while (pSize > 0 || (qSize > 0 && q))
            {
                if (pSize == 0)
                {
                    e = q;
                    q = q->nextZ;
                    --qSize;
                }
                else if (qSize == 0 || !q)
                {
                    e = p;
                    p = p->nextZ;
                    --pSize;
                }
                else if (p->z <= q->z)
                {
                    e = p;
                    p = p->nextZ;
                    pSize--;
                }
                else
                {
                    e = q;
                    q = q->nextZ;
                    --qSize;
                }

                if (tail)
                {
                    tail->nextZ = e;
                }
                else
                {
                    list = e;
                }

                e->prevZ = tail;
                tail = e;
            }

            p = q;
        }

        tail->nextZ = NULL;
        inSize *= 2;

    } while (numMerges > 1);

    return list;
}

static void indexCurve(LinkNode *start, float minX, float minY, float invSize)
{
    LinkNode *p = start;
    do
    {
        if (!p->z)
            p->z = zOrder((int)p->point->x, (int)p->point->y, minX, minY, invSize);
        p->prevZ = p->prev;
        p->nextZ = p->next;
        p = p->next;
    } while (p != start);

    p->prevZ->nextZ = NULL;
    p->prevZ = NULL;
    sortLinked(p);
}

/**
 * 如果是使用Z-order，则使用Z-Order方式判断是否是耳朵
 */
static bool isEarHashed(LinkNode *ear, float &minX, float &minY, float &size)
{
    Vec2 &a = *ear->prev->point;
    Vec2 &b = *ear->point;
    Vec2 &c = *ear->next->point;

    if (geometry2d::isTriangleReflex(a, b, c))
    {
        return false;
    } // reflex, can't be an ear

    // triangle bbox; min & max are calculated like this for speed
    float minTX = a.x < b.x ? (a.x < c.x ? a.x : c.x) : (b.x < c.x ? b.x : c.x);
    float minTY = a.y < b.y ? (a.y < c.y ? a.y : c.y) : (b.y < c.y ? b.y : c.y);
    float maxTX = a.x > b.x ? (a.x > c.x ? a.x : c.x) : (b.x > c.x ? b.x : c.x);
    float maxTY = a.y > b.y ? (a.y > c.y ? a.y : c.y) : (b.y > c.y ? b.y : c.y);

    // z-order range for the current triangle bbox;
    int minZ = zOrder(minTX, minTY, minX, minY, size);
    int maxZ = zOrder(maxTX, maxTY, minX, minY, size);

    // first look for points inside the triangle in increasing z-order
    LinkNode *p = ear->nextZ;

    // 如果链表中的顶点是杂乱无章的，可能包含在当前三角形中的顶点位于链表的最后面，造成这里遍历的次数过多。
    // 我们可以基于这样的事实：包含在a,b,c组成的三角形中的顶点，肯定是和a,b,c三个顶点相邻的顶点，
    // 所以，为了更快地退出当前循环，需要根据空间位置对链表中的顶点进行排序，使用Z-Order就是最好的算法根据空间位置排序出来的顶点是相邻的顺序的
    while (p && p->z <= maxZ)
    {
        if (p != ear->prev && p != ear->next && geometry2d::pointInTriangle(a, b, c, *p->point) && geometry2d::isTriangleReflex(*p->prev->point, *p->point, *p->next->point))
        {
            return false;
        }
        p = p->nextZ;
    }

    // then look for points in decreasing z-order
    p = ear->prevZ;

    while (p && p->z >= minZ)
    {
        if (p != ear->prev && p != ear->next && geometry2d::pointInTriangle(a, b, c, *p->point) && geometry2d::isTriangleReflex(*p->prev->point, *p->point, *p->next->point))
        {
            return false;
        }
        p = p->prevZ;
    }
    return true;
}

// check if a polygon diagonal is locally inside the polygon
static bool locallyInside(LinkNode *a, LinkNode *b)
{
    return geometry2d::triangleArea(*a->prev->point, *a->point, *a->next->point) < 0
               ? geometry2d::triangleArea(*a->point, *b->point, *a->next->point) >= 0 && geometry2d::triangleArea(*a->point, *a->prev->point, *b->point) >= 0
               : geometry2d::triangleArea(*a->point, *b->point, *a->prev->point) < 0 || geometry2d::triangleArea(*a->point, *a->next->point, *b->point) < 0;
}

static bool middleInside(LinkNode *a, LinkNode *b)
{
    LinkNode *p = a;
    bool inside = false;
    float px = (a->point->x + b->point->x) / 2;
    float py = (a->point->y + b->point->y) / 2;
    do
    {
        if (((p->point->y > py) != (p->next->point->y > py)) && (px < (p->next->point->x - p->point->x) * (py - p->point->y) / (p->next->point->y - p->point->y) + p->point->x))
        {
            inside = !inside;
        }
        p = p->next;
    } while (p != a);

    return inside;
}

static bool intersectsPolygon(LinkNode *a, LinkNode *b)
{
    LinkNode *p = a;
    do
    {
        if (p->i != a->i && p->next->i != a->i && p->i != b->i && p->next->i != b->i && geometry2d::segmentsIntersects(*p->point, *p->next->point, *a->point, *b->point))
        {
            return true;
        }
        p = p->next;
    } while (p != a);
    return false;
}

static bool isValidDiagonal(LinkNode *a, LinkNode *b)
{
    return a->next->i != b->i && a->prev->i != b->i && !intersectsPolygon(a, b) && locallyInside(a, b) && locallyInside(b, a) && middleInside(a, b);
}

// go through all polygon nodes and cure small local self-intersections
static LinkNode *cureLocalIntersections(LinkNode *start, sqstd::Array<unsigned short> &triangles)
{
    LinkNode *p = start;
    do
    {
        LinkNode *a = p->prev;
        LinkNode *b = p->next->next;

        if (!a->point->equals(*b->point) && geometry2d::segmentsIntersects(*a->point, *p->point, *p->next->point, *b->point) && locallyInside(a, b) && locallyInside(b, a))
        {

            triangles.push(a->i);
            triangles.push(p->i);
            triangles.push(b->i);

            // remove two nodes involved
            removeNode(p);
            removeNode(p->next);
            p = start = b;
        }
        p = p->next;
    } while (p != start);
    return p;
}

static LinkNode *splitPolygon(LinkNode *a, LinkNode *b)
{
    LinkNode *a2 = new LinkNode();
    a2->i = a->i;
    a2->point = a->point;

    LinkNode *b2 = new LinkNode();
    b2->i = b->i;
    b2->point = b->point;

    LinkNode *an = a->next;
    LinkNode *bp = b->prev;

    a->next = b;
    b->prev = a;

    a2->next = an;
    an->prev = a2;

    b2->next = a2;
    a2->prev = b2;

    bp->next = b2;
    b2->prev = bp;

    return b2;
}

//---------------------------------------------------------------

/**
 * 判断顶点是否是耳朵
 */
static bool isEar(LinkNode *node)
{

    const Vec2 &a = *node->prev->point;
    const Vec2 &b = *node->point;
    const Vec2 &c = *node->next->point;

    // printf("check point (%f,%f) (%f,%f) (%f,%f) Reflex %d\n",
    //        a.x, a.y,
    //        b.x, b.y,
    //        c.x, c.y,
    //        geometry2d::isTriangleReflex(a, b, c)
    //     );

    // 如果是反射三角形，表示b点是凹进多边形中的，所以不是耳顶点
    if (geometry2d::isTriangleReflex(a, b, c))
    {
        return false;
    }

    // 判断这个三角形中是否包含多边形中其他的顶点，如果包含就不是耳顶点

    LinkNode *checkNode = node->next->next;
    while (checkNode != node->prev)
    {

        if (geometry2d::pointInTriangle(a, b, c, *checkNode->point) &&
            geometry2d::isTriangleReflex(*checkNode->prev->point, *checkNode->point, *checkNode->next->point))
        {
            return false;
        }
        checkNode = checkNode->next;
    }
    return true;
}

static LinkNode *filterPoints(LinkNode *start, LinkNode *end)
{
    if (!start)
    {
        return start;
    }

    if (!end)
    {
        end = start;
    }

    LinkNode *p = start;
    bool again = false;
    do
    {
        again = false;

        if (!p->steiner && (p->point->equals(*p->next->point) || geometry2d::triangleArea(*p->prev->point, *p->point, *p->next->point) == 0))
        {
            removeNode(p);
            p = end = p->prev;
            if (p == p->next)
            {
                return nullptr;
            }
            again = true;
        }
        else
        {
            p = p->next;
        }
    } while (again || p != end);

    return end;
}

static void splitEarcut(LinkNode *start, sqstd::Array<unsigned short> &triangles, float minX, float minY, float size, int);

static void earcutLinked(LinkNode *ear, sqstd::Array<unsigned short> &triangles, float minX, float minY, float size, int baseVertex, int pass = 0)
{

    LinkNode *stop = ear;
    LinkNode *prev = NULL;
    LinkNode *next = NULL;

    // iterate through ears, slicing them one by one
    while (ear->prev != ear->next)
    {
        prev = ear->prev;
        next = ear->next;

        // printf("check point (%f,%f) (%f,%f) (%f,%f) %d\n",
        //     prev->point->x,prev->point->y,
        //     ear->point->x,ear->point->y ,
        //     next->point->x,next->point->y,
        //     isEar(ear)
        // );

        if (size ? isEarHashed(ear, minX, minY, size) : isEar(ear))
        {
            // cut off the triangle

            // printf("ear\n");
            if (baseVertex > 0)
            {
                triangles.push(baseVertex + prev->i);
                triangles.push(baseVertex + ear->i);
                triangles.push(baseVertex + next->i);
            }
            else
            {
                triangles.push(prev->i);
                triangles.push(ear->i);
                triangles.push(next->i);
            }
            removeNode(ear);

            // skipping the next vertices leads to less sliver triangles
            ear = next->next;
            stop = next->next;
            continue;
        }

        ear = next;

        /**
         * 移除所有“耳朵”后，应该能将多边形完全剖分成一系列不重叠的三角形。然而，如果在整个多边形中找不到更多的“耳朵”，需要考虑以下几种情况：
         * 1、多边形包含反射点（凹点）：反射点是指多边形中的一个顶点，其内部角度大于180度。对于存在反射点的多边形，可能无法直接通过耳切法（ear clipping）进行三角剖分。
         * 2、多边形自交或不简单：如果多边形有自交，即边界线相交，则可能无法找到合法的“耳朵”进行剖分。
         */
        if (ear == stop)
        {
            // try filtering points and slicing again
            if (!pass)
            {
                earcutLinked(filterPoints(ear, nullptr), triangles, minX, minY, size, baseVertex, 1);

                // if this didn't work, try curing all small self-intersections locally
            }
            else if (pass == 1)
            {
                ear = cureLocalIntersections(ear, triangles);
                earcutLinked(ear, triangles, minX, minY, size, baseVertex, 2);

                // as a last resort, try splitting the remaining polygon into two
            }
            else if (pass == 2)
            {
                splitEarcut(ear, triangles, minX, minY, size, baseVertex);
            }
            break;
        }
    }
}

static void splitEarcut(LinkNode *start, sqstd::Array<unsigned short> &triangles, float minX, float minY, float size, int baseVertex)
{
    // look for a valid diagonal that divides the polygon into two
    LinkNode *a = start;
    do
    {
        LinkNode *b = a->next->next;
        while (b != a->prev)
        {
            if (a->i != b->i && isValidDiagonal(a, b))
            {
                // split the polygon in two by the diagonal
                LinkNode *c = splitPolygon(a, b);

                // filter colinear points around the cuts
                a = filterPoints(a, a->next);
                c = filterPoints(c, c->next);

                // run earcut on each half
                earcutLinked(a, triangles, minX, minY, size, baseVertex);
                earcutLinked(c, triangles, minX, minY, size, baseVertex);
                return;
            }
            b = b->next;
        }
        a = a->next;
    } while (a != start);
}

static void deleteNode()
{
    for (int i = 0; i < nodes.getCount(); ++i)
    {
        delete nodes[i];
    }
    nodes.clear();
}

void earcut(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangles, int baseVertex, bool passZOrder)
{
    sqstd::Array<Vec2 *> ppoints(sqstd::StackTempArenaAllocator::getInstance());

    for (int i = 0; i < points.getCount(); ++i)
    {
        ppoints.push(&points[i]);
    }
    earcut(ppoints, triangles, baseVertex, passZOrder);
}

void earcut(sqstd::Array<Vec2 *> &points, sqstd::Array<unsigned short> &triangles, int baseVertex, bool passZOrder)
{
    LinkNode *node = buildLinkList(points);
    if (!node)
    {
        deleteNode();
        return;
    }

    float minX = 0;
    float minY = 0;
    float maxX = 0;
    float maxY = 0;
    float size = 0;

    // 如果是一个顶点非常多的复杂多边形，我们使用Z-Order算法来对输入的顶点进行排序
    // 来提升earcut的效率，因为earcut需要获取相邻的顶点来判断顶点是否是耳朵
    if (!passZOrder && points.getCount() > 80)
    {
        float x = 0;
        float y = 0;

        minX = maxX = points[0]->x;
        minY = maxY = points[0]->y;

        // 计算多边形的包围盒
        for (int i = 1; i < points.getCount(); ++i)
        {
            x = points[i]->x;
            y = points[i]->y;
            if (x < minX)
            {
                minX = x;
            }
            if (y < minY)
            {
                minY = y;
            }
            if (x > maxX)
            {
                maxX = x;
            }
            if (y > maxY)
            {
                maxY = y;
            }
        }
        size = std::max(maxX - minX, maxY - minY);
        if (size > 0)
        {
            indexCurve(node, minX, minY, size);
        }
    }
    earcutLinked(node, triangles, minX, minY, size, baseVertex);
    deleteNode();
}
