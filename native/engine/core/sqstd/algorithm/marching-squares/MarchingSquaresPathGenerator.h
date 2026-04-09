#pragma once
#include "MarchingSquaresGrid.h"
#include "../../Array.h"
#include "../../../math/Vec2.h"
#include "../../../geom/2d/misc/line2D.h"

namespace sqstd
{
    class MarchingSquaresPathGenerator
    {
    private:
        std::unordered_map<int, bool> process;

        bool isProcess(int cellIndex);
        int findCellNearClockwise8Boundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCellNearCounterclockwise8Boundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase1NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase2NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase3NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase4NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase5NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase6NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase7NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase8NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase9NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase10NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase11NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase12NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase13NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);
        int findCase14NextBoundary(MarchingSquaresSubGrid *grid, int cellIndex);

    public:
        friend class MarchingSquares;
        struct Path
        {
            sqstd::Array<Vec2> points;
            Path();
            ~Path();

        private:
            friend class MarchingSquaresPathGenerator;
            void addLine(float x, float y, float x1, float y1);
        };
        void generator(MarchingSquaresSubGrid *grid, sqstd::Array<Path> &paths);
    };

}