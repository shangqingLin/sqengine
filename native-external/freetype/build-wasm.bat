@REM @echo off
set EMSDK_TOO_PATH=E:\\emsdk-main
set CMAKE_ROOT_DIR=%~dp0
cd/d %EMSDK_TOO_PATH%
call emsdk activate
cd/d %CMAKE_ROOT_DIR%
set buildDir=./cmake-build
call emcmake cmake %CMAKE_ROOT_DIR%  -B %buildDir% -G "CodeBlocks - MinGW Makefiles"
call cmake --build %buildDir%