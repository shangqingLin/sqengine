#include "MarchingSquaresPathGenerator.h"
#include "../../StackTempArenaAllocator.h"

using namespace sqstd;

MarchingSquaresPathGenerator::Path::Path()
    : points(StackTempArenaAllocator::getInstance())
{
    points.resize(100);
}

MarchingSquaresPathGenerator::Path::~Path()
{
    // printf("++++++++++++++++delete path \n");
}

void MarchingSquaresPathGenerator::Path::addLine(float x, float y, float x1, float y1)
{
    // printf("add b %f %f e %f %f \n", x, y, x1, y1);

    if (points.getCount() > 0)
    {

        // 判断是否在同一条直线上，同一条直线上的话就不需要了

        int pointNum = points.getCount();
        Vec2 *begin = points.get(pointNum - 2);
        Vec2 *end = points.get(pointNum - 1);

        Vec2 line;
        line.sub2(*end, *begin);

        Vec2 line2(x - x1, y - y1);

        // printf("ccc dot %f  cross %f \n", Vec2::dot(line, line2), Vec2::cross(line, line2));

        if (Vec2::cross(line, line2) != 0)
        {

            // 不共线
            points.push(Vec2(x, y));

            // 开始点和结束点是否一样
            Vec2 *pathBeginPoint = points.get(0);
            line.x = x1;
            line.y = y1;
            line.sub(*pathBeginPoint);
            if (line.lengthSqr() >= 0.00001)
            {
                points.push(Vec2(x1, y1));
            }
        }
        else
        {

            if (begin->y <= y1)
            {
                // 插入的新线段在当前线段的后端
                begin->x = x;
                begin->y = y;

                // printf("fuck you a %f %f %f %f \n", begin->x, begin->y, end->x, end->y);
            }
            else
            {
                end->x = x1;
                end->y = y1;

                // printf("fuck you b %f %f %f %f \n", begin->x, begin->y, end->x, end->y);
            }
        }
    }
    else
    {
        Vec2 point;
        point.x = x;
        point.y = y;
        points.push(point);

        point.x = x1;
        point.y = y1;

        points.push(point);
    }
}

bool MarchingSquaresPathGenerator::isProcess(int cellIndex)
{
    return process.find(cellIndex) != process.end();
}

int MarchingSquaresPathGenerator::findCellNearClockwise8Boundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    // 顺时针取四周的Cell

    int cellX, cellY;
    grid->cellIndexToColumnRow(cellIndex, cellX, cellY);

    // 右边
    cellIndex = grid->getCellIndex(cellX, cellY, 1, 0);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 右下角
    cellIndex = grid->getCellIndex(cellX, cellY, 1, -1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 正下方
    cellIndex = grid->getCellIndex(cellX, cellY, 0, -1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 左下角
    cellIndex = grid->getCellIndex(cellX, cellY, -1, -1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 左边
    cellIndex = grid->getCellIndex(cellX, cellY, -1, 0);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 左上角
    cellIndex = grid->getCellIndex(cellX, cellY, -1, 1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 正上方
    cellIndex = grid->getCellIndex(cellX, cellY, 0, 1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 右上角
    cellIndex = grid->getCellIndex(cellX, cellY, 1, 1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    return -1;
}

int MarchingSquaresPathGenerator::findCellNearCounterclockwise8Boundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    // 逆时针取四周的Cell

    int cellX, cellY;
    grid->cellIndexToColumnRow(cellIndex, cellX, cellY);

    // 右边
    cellIndex = grid->getCellIndex(cellX, cellY, 1, 0);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 右上角
    cellIndex = grid->getCellIndex(cellX, cellY, 1, 1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 正上方
    cellIndex = grid->getCellIndex(cellX, cellY, 0, 1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 左上角
    cellIndex = grid->getCellIndex(cellX, cellY, -1, 1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 左边
    cellIndex = grid->getCellIndex(cellX, cellY, -1, 0);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 左下角
    cellIndex = grid->getCellIndex(cellX, cellY, -1, -1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 正下方
    cellIndex = grid->getCellIndex(cellX, cellY, 0, -1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    // 右下角
    cellIndex = grid->getCellIndex(cellX, cellY, 1, -1);
    if (cellIndex != -1 && !isProcess(cellIndex) && grid->isBoundaryCell(cellIndex))
    {
        return cellIndex;
    }

    return -1;
}

int MarchingSquaresPathGenerator::findCase1NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearClockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase2NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearClockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase3NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearClockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase4NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearCounterclockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase5NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearCounterclockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase6NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearCounterclockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase7NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearCounterclockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase8NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearClockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase9NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearClockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase10NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearClockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase11NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearCounterclockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase12NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearCounterclockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase13NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearCounterclockwise8Boundary(grid, cellIndex);
}

int MarchingSquaresPathGenerator::findCase14NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex)
{
    return findCellNearCounterclockwise8Boundary(grid, cellIndex);
}

void MarchingSquaresPathGenerator::generator(MarchingSquaresSubGrid *grid, sqstd::Array<Path> &paths)
{
    // 按照逆时针的顺序构建Path

    std::unordered_map<int, MarchingSquaresSubGrid::Cell>::iterator it = grid->gridCells.begin();

    process.clear();
    process.reserve(grid->gridCells.size());

    int cellX, cellY;
    while (it != grid->gridCells.end())
    {
        int cellIndex = it->first;
        if (isProcess(cellIndex))
        {
            ++it;
            continue;
        }
        ++it;

        MarchingSquaresPathGenerator::Path *path = paths.Add();

        while (cellIndex != -1)
        {
            std::unordered_map<int, MarchingSquaresSubGrid::Cell>::iterator pIt = grid->gridCells.find(cellIndex);
            process[cellIndex] = true;

            // printf(" path generator cellIndex %d bit %d \n", cellIndex, pIt->second.bits);

            int nextCellIndex = -1;
            switch (pIt->second.bits)
            {
            case 0:  // 0000
            case 15: // 1111
            {
                cellIndex = -1;
                nextCellIndex = -1;
                break;
            }
            case 1: // 0001
            {
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase1NextBoundary(grid, cellIndex);
                break;
            }
            case 2: // 0010
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase2NextBoundary(grid, cellIndex);
                break;
            case 3: // 0011
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase3NextBoundary(grid, cellIndex);
                break;
            case 4: // 0100
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase4NextBoundary(grid, cellIndex);
                break;
            case 5: // 0101
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                path->addLine(pIt->second.lines[1].x1, pIt->second.lines[1].y1, pIt->second.lines[1].x2, pIt->second.lines[1].y2);
                nextCellIndex = findCase5NextBoundary(grid, cellIndex);
                break;
            case 6: // 0110
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase6NextBoundary(grid, cellIndex);
                break;
            case 7: // 0111
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase7NextBoundary(grid, cellIndex);
                break;
            case 8: // 1000
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase8NextBoundary(grid, cellIndex);
                break;
            case 9: // 1001
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase9NextBoundary(grid, cellIndex);
                break;
            case 10: // 1010
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                path->addLine(pIt->second.lines[1].x1, pIt->second.lines[1].y1, pIt->second.lines[1].x2, pIt->second.lines[1].y2);

                nextCellIndex = findCase10NextBoundary(grid, cellIndex);
                break;
            case 11: // 1011
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase11NextBoundary(grid, cellIndex);
                break;
            case 12: // 1100
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase12NextBoundary(grid, cellIndex);
                break;
            case 13: // 1101
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase13NextBoundary(grid, cellIndex);
                break;
            case 14: // 1110
                path->addLine(pIt->second.lines[0].x1, pIt->second.lines[0].y1, pIt->second.lines[0].x2, pIt->second.lines[0].y2);
                nextCellIndex = findCase14NextBoundary(grid, cellIndex);
                break;
            }

            // if (cellIndex != -1 && nextCellIndex == -1)
            // {
            //     // 表示是属于Path中最后一个Cell了
            //     // 看看是否是闭合路径
            // }

            cellIndex = nextCellIndex;
        }
    }
}