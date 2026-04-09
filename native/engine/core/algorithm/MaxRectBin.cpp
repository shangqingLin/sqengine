#include "MaxRectBin.h"
#include <algorithm>

using namespace algorithm;

bool sortNode(MaxRectPackerNode *a, MaxRectPackerNode *b)
{
    int result = std::max(b->width, b->height) - std::max(a->width, a->height);
    return result < 0;
}

MaxRectBin::MaxRectBin(int maxWidth, int maxHeight, MaxRectPackerOptions *option)
    : width(maxWidth), height(maxHeight), option(option)
{
    freeRects.emplace_back(0, 0, maxWidth, maxHeight);
}

bool MaxRectBin::place(int width, int height, MaxRectPackerNode &result)
{

    bool find = this->findNode(width, height, option->allowRotation, result);
    if (find)
    {
        Rect<int> node;
        node.set(result.x, result.y, result.width, result.height);
        int numRectToProcess = freeRects.size();
        int i = 0;
        while (i < numRectToProcess)
        {
            if (splitNode(freeRects[i], node))
            {
                freeRects.erase(freeRects.begin() + i);
                numRectToProcess--;
            }
            else
            {
                ++i;
            }
        }
        pruneFreeList();
    }
    return find;
}

bool MaxRectBin::splitNode(Rect<int> &freeNode, Rect<int> &insertRect)
{
    /**
     * 这方法就是实现MaxRect核心逻辑，按照最大区域拆分空闲区域
     */

    // 其实就是判断插入的节点是否在插入到当前的空闲区域，如果不是插入到当前的空闲区域中，则不需要分割
    if (!freeNode.contain(insertRect))
        return false;

    /**
     * 这里进行垂直方向的拆分
     */

    // 判断insertRect的 x+width 是否在freeNode矩形内
    if (insertRect.x < freeNode.x + freeNode.width && insertRect.x + insertRect.width > freeNode.x)
    {

        // 对于位于insertRect顶部下面的空闲区域进行分割
        if (insertRect.y > freeNode.y && insertRect.y < freeNode.y + freeNode.height)
        {
            freeRects.emplace_back(
                freeNode.x,
                freeNode.y,
                freeNode.width,
                insertRect.y - freeNode.y);
        }

        // 对位于insertRect顶部上的空闲区域进行分割
        if (insertRect.y + insertRect.height < freeNode.y + freeNode.height)
        {
            freeRects.emplace_back(
                freeNode.x,
                insertRect.y + insertRect.height,
                freeNode.width,
                freeNode.y + freeNode.height - (insertRect.y + insertRect.height));
        }
    }

    /**
     * 水平方向的分割
     */

    if (insertRect.y < freeNode.y + freeNode.height &&
        insertRect.y + insertRect.height > freeNode.y)
    {
        // New node at the left side of the used node.
        if (insertRect.x > freeNode.x && insertRect.x < freeNode.x + freeNode.width)
        {
            freeRects.emplace_back(
                freeNode.x,
                freeNode.y,
                insertRect.x - freeNode.x,
                freeNode.height);
        }

        // New node at the right side of the used node.
        if (insertRect.x + insertRect.width < freeNode.x + freeNode.width)
        {
            freeRects.emplace_back(
                insertRect.x + insertRect.width,
                freeNode.y,
                freeNode.x + freeNode.width - (insertRect.x + insertRect.width),
                freeNode.height);
        }
    }
    return true;
}

/**
 * 遍历所有的空闲矩形空间，删除多余的空间
 * 所谓的多余是：
 *     两个相邻连在一起的空间就合并为一个矩形就可以了，没必要分割，这样保证每个空闲空间是最大的，插入的时候就能够提升空间利用率
 */
void MaxRectBin::pruneFreeList()
{
    int i = 0;
    int j = 0;
    int len = freeRects.size();
    while (i < len)
    {
        j = i + 1;
        Rect<int> &tmpRect1 = freeRects[i];
        while (j < len)
        {
            Rect<int> &tmpRect2 = freeRects[j];
            if (tmpRect2.contain(tmpRect1))
            {
                freeRects.erase(freeRects.begin() + i);
                i--;
                len--;
                break;
            }
            if (tmpRect1.contain(tmpRect2))
            {
                freeRects.erase(freeRects.begin() + j);
                j--;
                len--;
            }
            j++;
        }
        i++;
    }
}

bool MaxRectBin::findNode(int width, int height, bool allowRotation, MaxRectPackerNode &bestNode)
{
    int areaFit = 0;
    int score = 2147483647;
    int bestNodeIndex = -1;
    bool find = false;
    /**
     * 找到一块最好的空闲区域存放，保证插入的插入的块能够更接近地填满所选的区域
     * 怎么定义最好？有两种选择方式
     * 1、基于面积。如果空闲区域的面积和插入的面积相差最小表示最好
     * 2、基于宽高。 其实也是一样找到面积相差不大的块
     */

    for (int i = 0; i < freeRects.size(); ++i)
    {
        Rect<int> &r = freeRects[i];
        if (r.width >= width && r.height >= height)
        {
            if (option->logic == MAX_RECT_PACKING_LOGIC::MAX_AREA)
            {
                areaFit = r.width * r.height - width * height;
            }
            else
            {
                areaFit = std::min(r.width - width, r.height - height);
            }

            if (areaFit < score)
            {
                bestNodeIndex = i;
                score = areaFit;
                bestNode.x = r.x;
                bestNode.y = r.y;
                bestNode.width = width;
                bestNode.height = height;
                bestNode.rotation = false;
                find = true;
            }

            if (allowRotation)
            {
                if (option->logic == MAX_RECT_PACKING_LOGIC::MAX_AREA)
                {
                    areaFit = r.width * r.height - width * height;
                }
                else
                {
                    areaFit = std::min(r.width - height, r.height - width);
                }

                if (areaFit < score)
                {
                    bestNodeIndex = i;
                    score = areaFit;
                    bestNode.x = r.x;
                    bestNode.y = r.y;
                    bestNode.width = height;
                    bestNode.height = width;
                    bestNode.rotation = true;
                    find = true;
                }
            }
        }
    }

    return find;
}

MaxRectPackerNode *MaxRectBin::add(int width, int height)
{
    MaxRectPackerNode *result = new MaxRectPackerNode();
    if (this->place(width, height, *result))
    {
        if (useRects.size() + 1 < useRects.capacity())
        {
            useRects.resize(useRects.size() + 100);
        }
        useRects.push_back(result);
        return result;
    }
    else
    {
        delete result;
        return NULL;
    }
}

void MaxRectBin::repack()
{
    freeRects.clear();
    freeRects.emplace_back(0, 0, width, height);
    sort(useRects.begin(), useRects.end(), sortNode);
    for (int i = 0; i < useRects.size(); ++i)
    {
        MaxRectPackerNode *node = useRects[i];
        place(node->width, node->height, *node);
    }
}

void MaxRectBin::remove(MaxRectPackerNode *node)
{
    bool find = false;
    for (int i = 0; i < useRects.size(); ++i)
    {
        if (useRects[i] == node)
        {
            useRects.erase(useRects.begin() + i);
            find = true;
            break;
            ;
        }
    }

    if (find)
    {
        freeRects.emplace_back(node->x, node->y, node->width, node->height);
        delete node;
    }
}

MaxRectBin::~MaxRectBin()
{
    for (int i = 0; i < useRects.size(); ++i)
    {
        delete useRects[i];
    }
    useRects.clear();
}