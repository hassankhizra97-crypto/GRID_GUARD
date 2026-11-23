@echo off
REM ============================================================
REM  GridGuard Production Build Script
REM  Builds the C++ backend AND bundles the React frontend.
REM  Run this from the dsa-project root directory.
REM ============================================================

echo.
echo  =========================================================
echo    GridGuard ^| Production Build
echo  =========================================================
echo.

if not exist bin mkdir bin

REM ---- Build C++ backend -------------------------------------
echo [1/2] Building C++ backend...
g++ -std=c++17 -O2 -Wall -Iinclude ^
    src/Graph.cpp ^
    src/RoadNetwork.cpp ^
    src/Simulation.cpp ^
    src/Sorting.cpp ^
    src/GridGuardServer.cpp ^
    src/server_main.cpp ^
    -o bin/GridGuardServer.exe ^
    -lws2_32

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Backend build failed.
    pause
    exit /b 1
)
echo [OK] Backend compiled: bin\GridGuardServer.exe

REM ---- Build React frontend into frontend\dist ---------------
echo.
echo [2/2] Building React frontend...
cd frontend
call npm install
call npm run build
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Frontend build failed.
    pause
    exit /b 1
)
cd ..
echo [OK] Frontend bundled: frontend\dist\

echo.
echo  =========================================================
echo    Build complete!
echo.
echo    To run:
echo      bin\GridGuardServer.exe
echo    Then open: http://localhost:8081
echo  =========================================================
pause
