@echo off
echo ========================================================
echo Compiling GridGuard Simulator and Test Suite...
echo ========================================================

if not exist bin mkdir bin

echo Compiling GridGuard.exe...
g++ -std=c++14 -O2 -Iinclude main.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/GridGuard.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile GridGuard.exe
    exit /b %ERRORLEVEL%
)

echo Compiling test_suite.exe...
g++ -std=c++14 -O2 -Iinclude tests/test_suite.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/test_suite.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile test_suite.exe
    exit /b %ERRORLEVEL%
)

echo [SUCCESS] Compilation complete. Binaries placed in ./bin/
echo Run ./bin/GridGuard.exe for the interactive simulation.
echo Run ./bin/test_suite.exe to execute all verification tests.
