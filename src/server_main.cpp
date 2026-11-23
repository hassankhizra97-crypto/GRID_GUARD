#include "../include/GridGuardServer.h"
#include "../include/Simulation.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        std::cout << "[main] Creating Simulation..." << std::endl;
        Simulation sim;
        std::cout << "[main] Simulation created." << std::endl;
        
        std::cout << "[main] Creating server on port 8081..." << std::endl;
        GridGuardServer server(sim, 8081);
        
        std::cout << "[main] Starting server..." << std::endl;
        if (!server.start()) {
            std::cerr << "Failed to start GridGuard HTTP server.\n";
            return 1;
        }

        std::cout << "GridGuard Backend Server running at http://localhost:8081\n";
        std::cout << "Press Ctrl+C to stop.\n\n";

        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception\n";
        return 1;
    }
    return 0;
}