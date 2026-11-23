CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Iinclude
WS2_LIBS = -lws2_32

# Source files
SRCS        = src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp
SERVER_SRCS = $(SRCS) src/GridGuardServer.cpp src/server_main.cpp

# ───────────────────────────────────────────────────────────────
#  Default: build everything (console app + server + tests)
# ───────────────────────────────────────────────────────────────
all: bin/GridGuard.exe bin/GridGuardServer.exe bin/test_suite.exe

bin/GridGuard.exe: main.cpp $(SRCS)
	@if not exist bin mkdir bin
	$(CXX) $(CXXFLAGS) main.cpp $(SRCS) -o bin/GridGuard.exe

bin/GridGuardServer.exe: $(SERVER_SRCS)
	@if not exist bin mkdir bin
	$(CXX) $(CXXFLAGS) $(SERVER_SRCS) -o bin/GridGuardServer.exe $(WS2_LIBS)

bin/test_suite.exe: tests/test_suite.cpp $(SRCS)
	@if not exist bin mkdir bin
	$(CXX) $(CXXFLAGS) tests/test_suite.cpp $(SRCS) -o bin/test_suite.exe

# ───────────────────────────────────────────────────────────────
#  Individual targets
# ───────────────────────────────────────────────────────────────
server: bin/GridGuardServer.exe
	bin\GridGuardServer.exe

console: bin/GridGuard.exe
	bin\GridGuard.exe

test: bin/test_suite.exe
	bin\test_suite.exe

# ───────────────────────────────────────────────────────────────
#  Full-stack: build backend then launch frontend dev server
#  Usage:  make dev
# ───────────────────────────────────────────────────────────────
dev: bin/GridGuardServer.exe
	@echo.
	@echo  =====================================================
	@echo   GridGuard Full-Stack — starting both servers
	@echo  =====================================================
	@echo   Backend  : http://localhost:8081
	@echo   Frontend : http://localhost:5173
	@echo  =====================================================
	@echo.
	@start "GridGuard Backend" cmd /k "bin\GridGuardServer.exe"
	@cd frontend && npm run dev

# ───────────────────────────────────────────────────────────────
clean:
	@if exist bin rmdir /s /q bin
