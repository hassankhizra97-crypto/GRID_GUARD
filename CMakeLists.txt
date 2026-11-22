cmake_minimum_required(VERSION 3.10)
project(GridGuard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# GridGuard Main Simulator Executable
add_executable(GridGuard
    main.cpp
    src/Graph.cpp
    src/RoadNetwork.cpp
    src/Simulation.cpp
    src/Sorting.cpp
)

# Automated Test Suite Executable
add_executable(test_suite
    tests/test_suite.cpp
    src/Graph.cpp
    src/RoadNetwork.cpp
    src/Simulation.cpp
    src/Sorting.cpp
)
