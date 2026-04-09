#pragma once

#include <unordered_map>
#include "../../Array.h"
#include "../../../math/Vec2.h"
#include "./MarchingSquaresGrid.h"
#include "MarchingSquaresPathGenerator.h"

namespace sqstd
{

    /**
     * 只能满足连续的数值场生成连续的网格，而能满足离散的数值场
     * 即MarchingSquares算法只能满足可以生成闭合空间的，不是闭合的只能生成线段
     */
    class MarchingSquares
    {
    private:
        MarchingSquaresOuterGrid outerGrid;
        MarchingSquaresSubGrid subGrid;
        MarchingSquaresPathGenerator pathGenerator;

    public:
        MarchingSquares(int xNum, int yNum, float valueMinX, float valueMinY, float valueMaxX, float valueMaxY);

        void generator(sqstd::Array<MarchingSquaresPathGenerator::Path> &paths);
        void clear();

        /**
         *  设置的是大网格cell 的isovalue值，通过这个值来判断顶点是否被“点亮”
         */
        void setCellIsoValue(int cellX, int cellY, float valueX, float valueY);
        void setCellIsoForCellValue(int cellX, int cellY);
    };
}