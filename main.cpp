#include "include/Simulation.h"
#include <iostream>
#include <limits>
#include <string>

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getValidInt(const std::string& prompt, int minVal, int maxVal) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            if (val >= minVal && val <= maxVal) {
                clearInputBuffer();
                return val;
            }
            std::cout << "[Invalid] Please enter a value between " << minVal << " and " << maxVal << ".\n";
        } else {
            std::cout << "[Invalid Input] Please enter a valid number.\n";
            clearInputBuffer();
        }
    }
}

void printMainMenu() {
    std::cout << "\n";
    std::cout << "========================================================================================\n";
    std::cout << "                                     GRIDGUARD                                          \n";
    std::cout << "             Karachi Power Grid Fault Detection & Restoration Simulator                 \n";
    std::cout << "========================================================================================\n";
    std::cout << " 1. Display Power Network                  11. Search Fault Record by ID\n";
    std::cout << " 2. View All Locations                     12. Sort Fault Records\n";
    std::cout << " 3. Simulate Power Line Failure            13. Display Incident History (Chronological)\n";
    std::cout << " 4. View Affected Locations (Outages)      14. View Incident History Backward (Reverse)\n";
    std::cout << " 5. View Critical Facility Outages         15. Undo Last Operation\n";
    std::cout << " 6. View Repair Priority Queue (Max-Heap)  16. Run BFS Traversal (Custom Queue)\n";
    std::cout << " 7. Process Next Repair (Dijkstra + Fix)   17. Run DFS Traversal (Recursive)\n";
    std::cout << " 8. Find Repair Crew Route (Dijkstra)      18. View Network Connectivity Status\n";
    std::cout << " 9. View All Fault Records                 19. Restore a Power Line Manually\n";
    std::cout << "10. Search Location by ID (Hash Table)     20. Exit Simulator\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "21. [SPECIAL] Run Automated Test Suite (All 10 Academic Validation Tests)\n";
    std::cout << "22. [SPECIAL] Display Hash Table Collision & Bucket Diagnostics\n";
    std::cout << "23. [SPECIAL] Display Undo Stack Contents\n";
    std::cout << "========================================================================================\n";
}

int main(int argc, char* argv[]) {
    // If command line argument --test passed, run automated test suite directly
    if (argc > 1 && std::string(argv[1]) == "--test") {
        std::cout << "Running in automated verification mode...\n";
        // Can invoke test function or exit
    }

    Simulation sim;
    bool running = true;

    std::cout << "\n>>> GridGuard Karachi Power Grid Initialized with 44 Municipal Locations <<<\n";

    while (running) {
        printMainMenu();
        int choice = getValidInt("Select an option (1-23): ", 1, 23);

        switch (choice) {
            case 1:
                sim.displayPowerNetwork();
                break;
            case 2:
                sim.displayAllLocations();
                break;
            case 3: {
                std::cout << "\n--- Simulate Power Line Failure ---\n";
                int lineId = getValidInt("Enter Power Line ID to fail (1-61): ", 1, 61);
                int severity = getValidInt("Enter Fault Severity (1 = Minor to 5 = Catastrophic): ", 1, 5);
                int duration = getValidInt("Enter Estimated Outage Duration in minutes (15-600): ", 15, 600);
                sim.simulatePowerLineFailure(lineId, severity, duration);
                break;
            }
            case 4:
                sim.displayOutageReportSummary();
                break;
            case 5:
                sim.displayCriticalFacilitiesStatus();
                break;
            case 6:
                sim.displayRepairQueue();
                break;
            case 7: {
                std::cout << "\n--- Process Next Repair ---\n";
                std::cout << "1. Auto-select highest priority fault from Max-Heap\n";
                std::cout << "2. Manually select specific Fault ID\n";
                int sub = getValidInt("Choice (1-2): ", 1, 2);
                if (sub == 1) {
                    sim.processNextRepair();
                } else {
                    int fId = getValidInt("Enter Fault ID: ", 101, 999);
                    sim.processNextRepair(fId);
                }
                break;
            }
            case 8: {
                std::cout << "\n--- Find Repair Crew Route (Dijkstra's Road Algorithm) ---\n";
                int target = getValidInt("Enter Destination Location ID (1-44): ", 1, 44);
                sim.findRepairCrewRoute(target);
                break;
            }
            case 9:
                sim.displayAllFaults();
                break;
            case 10: {
                int locId = getValidInt("Enter Location ID to search (1-44): ", 1, 44);
                sim.searchLocationById(locId);
                break;
            }
            case 11: {
                int fId = getValidInt("Enter Fault ID to search (e.g. 101): ", 1, 9999);
                sim.searchFaultById(fId);
                break;
            }
            case 12: {
                std::cout << "\n--- Sort Fault Records (Manual QuickSort) ---\n";
                std::cout << "1. Sort by Priority Score (Descending)\n";
                std::cout << "2. Sort by Severity Level (Descending)\n";
                std::cout << "3. Sort by Population Affected (Descending)\n";
                std::cout << "4. Sort by Outage Duration (Descending)\n";
                int sChoice = getValidInt("Select sorting criterion (1-4): ", 1, 4);
                SortCriterion crit = SORT_BY_PRIORITY_SCORE;
                if (sChoice == 2) crit = SORT_BY_SEVERITY;
                else if (sChoice == 3) crit = SORT_BY_POPULATION;
                else if (sChoice == 4) crit = SORT_BY_OUTAGE_DURATION;

                sim.sortAndDisplayFaults(crit, false);
                break;
            }
            case 13:
                sim.displayIncidentHistoryForward();
                break;
            case 14:
                sim.displayIncidentHistoryBackward();
                break;
            case 15:
                sim.undoLastOperation();
                break;
            case 16: {
                std::cout << "\n--- Run BFS Traversal (Custom Queue) ---\n";
                int startId = getValidInt("Enter Start Location ID (Default 1 for Central Supply Hub): ", 1, 44);
                sim.runBFSTraversal(startId);
                break;
            }
            case 17: {
                std::cout << "\n--- Run DFS Traversal (Recursive Graph Exploration) ---\n";
                int startId = getValidInt("Enter Start Location ID (Default 1 for Central Supply Hub): ", 1, 44);
                sim.runDFSTraversal(startId);
                break;
            }
            case 18:
                sim.analyzeAndDisplayConnectivity();
                break;
            case 19: {
                int lineId = getValidInt("Enter Power Line ID to restore directly (1-61): ", 1, 61);
                sim.restorePowerLineDirectly(lineId);
                break;
            }
            case 20:
                std::cout << "\nExiting GridGuard Simulator. Thank you for using GridGuard!\n";
                running = false;
                break;
            case 21: {
                std::cout << "\n[INFO] Please run the standalone test runner: bin/test_suite.exe\n";
                std::cout << "Running quick built-in test demonstration...\n";
                Simulation testSim;
                testSim.simulatePowerLineFailure(14, 5, 120); // Hospital line fails
                testSim.displayRepairQueue();
                testSim.processNextRepair();
                testSim.undoLastOperation();
                break;
            }
            case 22:
                sim.displayHashTableDiagnostics();
                break;
            case 23:
                sim.displayUndoHistory();
                break;
        }

        if (running) {
            std::cout << "\nPress Enter to return to the Main Menu...";
            std::string dummy;
            std::getline(std::cin, dummy);
        }
    }

    return 0;
}
