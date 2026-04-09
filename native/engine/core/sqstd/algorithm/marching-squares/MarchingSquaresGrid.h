#pragma once
#include <unordered_map>

namespace sqstd
{

    class MarchingSquaresGrid
    {
    protected:
        /**
         * 网格大小，指定的是单元格。
         * 网格的原点为左下角
         */
        int xNum;
        int yNum;
        int toCellIndex(int cellX, int cellY);

    public:
        void setGridSize(int x, int y);
        void cellIndexToColumnRow(int cellIndex, int &column, int &row);
        int getCellIndex(int cellX, int cellY, int offsetX = 0, int offsetY = 0);
    };

    class MarchingSquaresOuterGrid : public MarchingSquaresGrid
    {

    private:
        // 网格值的范围，通过这个计算每个格子的值
        float valueMinX;
        float valueMinY;
        float valueSizeX;
        float valueSizeY;

    public:
        friend class MarchingSquaresSubGrid;
        struct Cell
        {
            float isoValueX{0.f};
            float isoValueY{0.f};
        };
        std::unordered_map<int, Cell> gridCells;
        void clear();
        void setCellIsoValue(int cellX, int cellY, float valueX, float valueY);
        void getCellIsoValue(int cellX, int cellY, float &x, float &y);
        void getCellValue(int cellX, int cellY, float &x, float &y);
        void setGridValue(float minXValue, float minYValue, float maxXValue, float maxYValue);
    };

    class MarchingSquaresSubGrid : public MarchingSquaresGrid
    {
    private:
        MarchingSquaresOuterGrid *outerGrid;
        void getCornerData2(int subCellX, int subCellY, int corner, float values[4][2], float isovalue[4][2]);
        void getCornerData(int subCellX, int subCellY, float values[4][2], float isovalue[4][2]);
        int getCase(float values[4][2], float isovalue[4][2]);
        void clear();
        void generator();

    public:
        friend class MarchingSquares;
        friend class MarchingSquaresPathGenerator;
        struct Line
        {
            float x1{0.f};
            float y1{0.f};
            float x2{0.f};
            float y2{0.f};
        };

        struct Cell
        {
            // 一个格子最多只有2条线
            Line lines[2];
            int bits{0};
        };

        std::unordered_map<int, MarchingSquaresSubGrid::Cell> gridCells;
        bool isBoundaryCell(int cellIndex);
        MarchingSquaresSubGrid(MarchingSquaresOuterGrid *);
    };

}