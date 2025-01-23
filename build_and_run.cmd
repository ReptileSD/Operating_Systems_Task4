@echo off
if exist build (
    rd /s /q build
)
if exist logs (
    rd /s /q logs
)

mkdir build

cd build

cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" ..

REM Собираем проект
cmake --build .

REM mock_device.exe
REM main.exe

REM Для отладки, чтобы окно не закрывалось сразу
pause
