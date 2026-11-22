CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall -Iinclude

SRCS = src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp

all: bin/GridGuard.exe bin/test_suite.exe

bin/GridGuard.exe: main.cpp $(SRCS)
	@if not exist bin mkdir bin
	$(CXX) $(CXXFLAGS) main.cpp $(SRCS) -o bin/GridGuard.exe

bin/test_suite.exe: tests/test_suite.cpp $(SRCS)
	@if not exist bin mkdir bin
	$(CXX) $(CXXFLAGS) tests/test_suite.cpp $(SRCS) -o bin/test_suite.exe

clean:
	@if exist bin rmdir /s /q bin

test: bin/test_suite.exe
	./bin/test_suite.exe
