#include "MarchingSquares.h"
#include "../../../math/math.h"

using namespace sqstd;

//========================
MarchingSquares::MarchingSquares(int xNum, int yNum, float valueMinX, float valueMinY, float valueMaxX, float valueMaxY)
    : subGrid(&outerGrid)
{
    outerGrid.setGridSize(xNum, yNum);
    subGrid.setGridSize(xNum - 1, yNum - 1);
    outerGrid.setGridValue(valueMinX, valueMinY, valueMaxX, valueMaxY);
}

void MarchingSquares::clear()
{
    outerGrid.clear();
    subGrid.clear();
}

void MarchingSquares::setCellIsoValue(int cellX, int cellY, float valueX, float valueY)
{
    outerGrid.setCellIsoValue(cellX, cellY, valueX, valueY);
    printf("partciel p %u %u %f %f \n", cellX, cellY, valueX, valueY);
}

void MarchingSquares::setCellIsoForCellValue(int cellX, int cellY)
{
    float x, y;
    outerGrid.getCellValue(cellX, cellY, x, y);
    // printf("fuck %d %d %f %f \n",cellX,cellY,x,y);
    setCellIsoValue(cellX, cellY, x, y);
}

void MarchingSquares::generator(sqstd::Array<MarchingSquaresPathGenerator::Path> &paths)
{

    if (outerGrid.gridCells.size() <= 0)
        return;

    subGrid.generator();
    pathGenerator.generator(&subGrid, paths);

    /*
    for (int c = checkBoundMinX; c < checkBoundMaxX; ++c)
    {

        for (int r = checkBoundMinY; r < checkBoundMaxY; ++r)
        {
            int subIndex = r * subGridX + c;
            getSubCellVertex(subIndex, points, isovalues);
            int bits = getCase(points, isovalues);

            switch (bits)
            {
            case 0: // 0000
            {
                break;
            }
            case 1: // 0001
            {

                getBottomEdgeVertex(points, isovalues, vertexPoints);
                // a = 1;
                vertexPoints.clear();
                // getLeftEdgeVertex(points, isovalues, vertexPoints);
                break;
            }
            case 2: // 0010
                // getBottomEdgeVertex(points, isovalues, vertexPoints);
                // vertexPoints.push(Vec2(0, 0));
                // getRightEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 3: // 0011
                // getLeftEdgeVertex(points, isovalues, vertexPoints);
                // getRightEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 4: // 0100
                // getRightEdgeVertex(points, isovalues, vertexPoints);
                // getTopEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 5: // 0101
                // getRightEdgeVertex(points, isovalues, vertexPoints);
                // getTopEdgeVertex(points, isovalues, vertexPoints);
                getBottomEdgeVertex(points, isovalues, vertexPoints);
                // getLeftEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 6: // 0110
                getBottomEdgeVertex(points, isovalues, vertexPoints);
                // getTopEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 7: // 0111
                // getTopEdgeVertex(points, isovalues, vertexPoints);
                // getLeftEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 8: // 1000
                // getTopEdgeVertex(points, isovalues, vertexPoints);
                // getLeftEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 9: // 1001
                getBottomEdgeVertex(points, isovalues, vertexPoints);
                // getTopEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 10: // 1010
                // getLeftEdgeVertex(points, isovalues, vertexPoints);
                // getTopEdgeVertex(points, isovalues, vertexPoints);
                getBottomEdgeVertex(points, isovalues, vertexPoints);
                // getRightEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 11: // 1011
                // getRightEdgeVertex(points, isovalues, vertexPoints);
                // getTopEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 12: // 1100
                // getLeftEdgeVertex(points, isovalues, vertexPoints);
                // getRightEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 13: // 1101
                // getBottomEdgeVertex(points, isovalues, vertexPoints);
                // getRightEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 14: // 1110
                // getBottomEdgeVertex(points, isovalues, vertexPoints);
                // getLeftEdgeVertex(points, isovalues, vertexPoints);
                break;
            case 15: // 1111
                break;
            }
        }
    }*/
}
