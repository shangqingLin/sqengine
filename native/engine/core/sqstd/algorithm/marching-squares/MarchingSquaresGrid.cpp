#include "MarchingSquaresGrid.h"
#include <stdio.h>

using namespace sqstd;

void MarchingSquaresGrid::setGridSize(int x, int y)
{
    xNum = x;
    yNum = y;
}

int MarchingSquaresGrid::toCellIndex(int cellX, int cellY)
{
    return cellY * xNum + cellX;
}

void MarchingSquaresGrid::cellIndexToColumnRow(int cellIndex, int &column, int &row)
{
    row = cellIndex / xNum;
    column = cellIndex % xNum;
}

int MarchingSquaresGrid::getCellIndex(int cellX, int cellY, int offsetX, int offsetY)
{
    int column = cellX + offsetX;

    if (column < 0 || column >= xNum)
        return -1;

    int row = cellY + offsetY;

    if (row < 0 || row >= yNum)
        return -1;
    return toCellIndex(column, row);
}

//================================
void MarchingSquaresOuterGrid::clear()
{
    gridCells.clear();
}

void MarchingSquaresOuterGrid::getCellValue(int cellX, int cellY, float &x, float &y)
{
    x = valueMinX + cellX * valueSizeX;
    y = valueMinY + cellY * valueSizeY;
}

void MarchingSquaresOuterGrid::setGridValue(float minXValue, float minYValue, float maxXValue, float maxYValue)
{
    valueMinX = minXValue;
    valueMinY = minYValue;
    valueSizeX = (maxXValue - valueMinX) / xNum;
    valueSizeY = (maxYValue - valueMinY) / yNum;
}

void MarchingSquaresOuterGrid::setCellIsoValue(int cellX, int cellY, float valueX, float valueY)
{
    int cellIndex = cellY * xNum + cellX;
    Cell cell;
    cell.isoValueX = valueX;
    cell.isoValueY = valueY;
    gridCells[cellIndex] = cell;
}

void MarchingSquaresOuterGrid::getCellIsoValue(int cellX, int cellY, float &x, float &y)
{
    int cellIndex = toCellIndex(cellX, cellY);
    std::unordered_map<int, MarchingSquaresOuterGrid::Cell>::iterator it = gridCells.find(cellIndex);

    if (it != gridCells.end())
    {
        x = it->second.isoValueX;
        y = it->second.isoValueY;
    }
    else
    {
        // 因为大于才算激活顶点的，所以这计算最永远都小于的值，即不激活的格子就使用该isovalue
        x = valueMinX - 1.0f;
        y = valueMinY - 1.0f;
    }
}

//=============================

static void getBottomEdgeVertex(float points[4][2], float isoValue[4][2], float &x, float &y)
{
    x = isoValue[0][0] == points[0][0] ? points[0][0] + 0.5f : isoValue[0][0];
    y = points[0][1];
}

static void getRightEdgeVertex(float points[][2], float isoValue[4][2], float &x, float &y)
{
    x = points[1][0];
    y = isoValue[0][1] == points[1][1] ? points[1][1] + 0.5 : isoValue[0][1];
}

static void getTopEdgeVertex(float points[][2], float isoValue[4][2], float &x, float &y)
{
    x = isoValue[0][0] == points[3][0] ? points[3][0] + 0.5 : isoValue[0][0];
    y = points[2][1];
}

static void getLeftEdgeVertex(float points[4][2], float isoValue[4][2], float &x, float &y)
{
    x = points[0][0];
    y = isoValue[0][1] == points[0][1] ? points[0][1] + 0.5 : isoValue[0][1];
}

MarchingSquaresSubGrid::MarchingSquaresSubGrid(MarchingSquaresOuterGrid *grid) : outerGrid(grid)
{
}

void MarchingSquaresSubGrid::clear()
{
    gridCells.clear();
}

void MarchingSquaresSubGrid::getCornerData2(int subCellX, int subCellY, int corner, float values[4][2], float isovalue[4][2])
{

    if (corner == 0)
    {
        // 左下角
    }
    else if (corner == 1)
    {
        // 右下角
        subCellX = subCellX + 1;
    }
    else if (corner == 2)
    {
        // 右上角
        subCellX += 1;
        subCellY += 1;
    }
    else if (corner)
    {
        // 左上角
        subCellY += 1;
    }

    outerGrid->getCellIsoValue(subCellX, subCellY, isovalue[corner][0], isovalue[corner][1]);
    outerGrid->getCellValue(subCellX, subCellY, values[corner][0], values[corner][1]);
    // printf("corner %d cell: %d %d value %f %f \n", corner, subCellX, subCellY, values[corner][0], values[corner][1]);
}

void MarchingSquaresSubGrid::getCornerData(int subCellX, int subCellY, float values[4][2], float isovalue[4][2])
{

    /**
     * 网格原点在左下角
     * value[0]:左下角
     * value[1]:右下角
     * value[2]:右上角
     * value[3]:左下角
     */
    getCornerData2(subCellX, subCellY, 0, values, isovalue);
    getCornerData2(subCellX, subCellY, 1, values, isovalue);
    getCornerData2(subCellX, subCellY, 2, values, isovalue);
    getCornerData2(subCellX, subCellY, 3, values, isovalue);
}

int MarchingSquaresSubGrid::getCase(float values[4][2], float isovalue[4][2])
{
    int bits = 0;
    for (int i = 0; i < 4; i++)
    {
        // printf("case i %d value %f %f isovalue %f %f \n", i, values[i][0], values[i][1], isovalue[i][0], isovalue[i][1]);
        if (values[i][0] <= isovalue[i][0] && values[i][1] <= isovalue[i][1])
            bits |= 1 << i;
    }
    return bits;
}

bool MarchingSquaresSubGrid::isBoundaryCell(int cellIndex)
{
    std::unordered_map<int, MarchingSquaresSubGrid::Cell>::iterator it = gridCells.find(cellIndex);
    return it != gridCells.end() ? it->second.bits != 0 && it->second.bits != 15 : false;
}

void MarchingSquaresSubGrid::generator()
{
    float isovalues[4][2];
    float points[4][2];

    std::unordered_map<int, MarchingSquaresOuterGrid::Cell>::iterator it = outerGrid->gridCells.begin();

    // 先计算所有的Cell的Bit
    while (it != outerGrid->gridCells.end())
    {
        int subCellX, subCellY;
        outerGrid->cellIndexToColumnRow(it->first, subCellX, subCellY);
        ++it;

        if (subCellX == outerGrid->xNum - 1 || subCellY == outerGrid->yNum - 1)
        {
            continue;
        }

        getCornerData(subCellX, subCellY, points, isovalues);

        MarchingSquaresSubGrid::Cell *cell = nullptr;
        int cellIndex = getCellIndex(subCellX, subCellY, 0, 0);
        std::unordered_map<int, MarchingSquaresSubGrid::Cell>::iterator subIt = gridCells.find(cellIndex);
        if (subIt == gridCells.end())
        {
            gridCells[cellIndex] = {};
            cell = &gridCells[cellIndex];
        }
        else
        {
            cell = &subIt->second;
        }

        printf("===== generator %d %d \n", subCellX, subCellY);

        cell->bits = getCase(points, isovalues);

        int cc = 1;
        switch (cell->bits)
        {
        case 0:  // 0000
        case 15: // 1111
        {
            cc = 0;
            break;
        }
        case 1: // 0001
        {
            // 按逆时针排列

            getLeftEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getBottomEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        }
        case 2: // 0010
            getBottomEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getRightEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 3: // 0011
            getLeftEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getRightEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 4: // 0100
            getTopEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getRightEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 5: // 0101
            getTopEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getRightEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);

            getLeftEdgeVertex(points, isovalues, cell->lines[1].x1, cell->lines[1].y1);
            getBottomEdgeVertex(points, isovalues, cell->lines[1].x2, cell->lines[1].y2);
            cc = 2;
            break;
        case 6: // 0110
            getTopEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getBottomEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 7: // 0111
            getLeftEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getTopEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 8: // 1000
            getLeftEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getTopEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 9: // 1001
            getTopEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getBottomEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 10: // 1010
            getTopEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getLeftEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);

            getBottomEdgeVertex(points, isovalues, cell->lines[1].x1, cell->lines[1].y1);
            getRightEdgeVertex(points, isovalues, cell->lines[1].x2, cell->lines[1].y2);
            cc = 2;
            break;
        case 11: // 1011
            getTopEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getRightEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 12: // 1100
            getLeftEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getRightEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 13: // 1101
            getBottomEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getRightEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        case 14: // 1110
            getLeftEdgeVertex(points, isovalues, cell->lines[0].x1, cell->lines[0].y1);
            getBottomEdgeVertex(points, isovalues, cell->lines[0].x2, cell->lines[0].y2);
            break;
        }

        for (int c = 0; c < cc; ++c)
        {
            printf("genertor tag %d vb %f %f ve %f %f \n", 
                   cell->bits,
                   cell->lines[c].x1, cell->lines[c].y1,
                   cell->lines[c].x2, cell->lines[c].y2);
        }
    }
}