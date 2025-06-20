@echo off
mkdir build
cd build
cmake -T ClangCL -G "Visual Studio 17 2022" ..
pause