@echo off
REM ============================================================
REM  GridGuard Full-Stack Startup Script (Development Mode)
REM  Run this script from the dsa-project root directory.
REM ============================================================

echo.
echo  =========================================================
echo    GridGuard ^| Karachi Power Grid Simulator
echo    Full-Stack Development Launcher
echo  =========================================================
echo.

REM ---- Step 1: Build the C++ backend -------------------------
echo [1/3] Building C++ backend...
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
    echo [ERROR] C++ backend build failed. Make sure g++ is installed.
    echo         Install MinGW-w64: https://www.mingw-w64.org/
    pause
    exit /b 1
)
echo [OK] Backend built successfully.
echo.

REM ---- Step 2: Install frontend dependencies if needed --------
if not exist "frontend\node_modules" (
    echo [2/3] Installing frontend dependencies (first run only)...
    cd frontend
    npm install
    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] npm install failed. Make sure Node.js is installed.
        echo         Download Node.js: https://nodejs.org/
        pause
        exit /b 1
    )
    cd ..
    echo [OK] Dependencies installed.
) else (
    echo [2/3] Frontend dependencies already installed. Skipping npm install.
)
echo.

REM ---- Step 3: Launch both servers ----------------------------
echo [3/3] Launching both servers...
echo.
echo  Backend  -^> http://localhost:8081  (C++ HTTP API)
echo  Frontend -^> http://localhost:5173  (React Dashboard)
echo.
echo  Open your browser at: http://localhost:5173
echo.
echo  Press Ctrl+C in each window to stop.
echo.

REM Launch backend in a new console window
start "GridGuard C++ Backend (port 8081)" cmd /k "bin\GridGuardServer.exe"

REM Small delay so backend is ready before frontend starts
timeout /t 2 /nobreak >nul

REM Launch frontend dev server in this window
cd frontend
npm run dev
