#include "../include/Simulation.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Simulation::Simulation()
    : locationTable(23), faultTable(17),
      nextFaultId(101), nextEventId(1),
      simulationTimeMinutes(0), mainPowerStationId(1),
      repairDepotLocationId(1) {
    loadSampleNetwork();
}

Simulation::~Simulation() {
}

std::string Simulation::getCurrentTimeStr() const {
    int hours = (simulationTimeMinutes / 60) % 24;
    int mins = simulationTimeMinutes % 60;
    std::ostringstream oss;
    oss << "T+" << std::setfill('0') << std::setw(2) << hours
        << ":" << std::setfill('0') << std::setw(2) << mins;
    return oss.str();
}

void Simulation::logEvent(const std::string& desc, int faultId, const std::string& eventType) {
    IncidentEvent ev(nextEventId++, desc, faultId, eventType, getCurrentTimeStr());
    incidentHistory.append(ev);
}

void Simulation::loadSampleNetwork() {
    // 14 Predefined Sample Locations
    // 1 Main Power Station, 3 Substations, 2 Hospitals, 2 Universities, 4 Residential, 2 Commercial
    Location locs[] = {
        Location(1,  "Metro Central Power Station", POWER_STATION, 0,     CRITICAL),
        Location(2,  "North Substation Alpha",      SUBSTATION,    0,     HIGH),
        Location(3,  "Central Substation Beta",     SUBSTATION,    0,     HIGH),
        Location(4,  "South Substation Gamma",      SUBSTATION,    0,     HIGH),
        Location(5,  "City General Hospital",       HOSPITAL,      1200,  CRITICAL),
        Location(6,  "St. Jude Trauma Center",      HOSPITAL,      850,   CRITICAL),
        Location(7,  "Metro Tech University",       UNIVERSITY,    18500, MEDIUM),
        Location(8,  "State University Campus",     UNIVERSITY,    14200, MEDIUM),
        Location(9,  "Riverside Residential",       RESIDENTIAL,   24000, LOW),
        Location(10, "Highland Gardens Community",  RESIDENTIAL,   16500, LOW),
        Location(11, "Maple Grove Residential",     RESIDENTIAL,   19000, LOW),
        Location(12, "Oakridge Suburb",             RESIDENTIAL,   11000, LOW),
        Location(13, "Downtown Financial Center",   COMMERCIAL,    32000, HIGH),
        Location(14, "Harbor Tech Commercial Park", COMMERCIAL,    15500, MEDIUM)
    };

    for (int i = 0; i < 14; ++i) {
        powerGrid.addLocation(locs[i]);
        locationTable.insert(locs[i].id, locs[i]);
    }

    // 18 Power Transmission Lines (Combining Redundant and Non-redundant paths)
    PowerLine lines[] = {
        PowerLine(1,  1,  2, 12000.0, 12.0), // Main -> North Sub
        PowerLine(2,  1,  3, 10000.0, 10.0), // Main -> Central Sub
        PowerLine(3,  1,  4, 15000.0, 15.0), // Main -> South Sub
        PowerLine(4,  2,  3,  8000.0,  8.0), // North Sub <-> Central Sub (Redundant backbone)
        PowerLine(5,  3,  4,  9000.0,  9.0), // Central Sub <-> South Sub (Redundant backbone)
        PowerLine(6,  2,  5,  7500.0,  6.0), // North Sub -> Hospital 5
        PowerLine(7,  3,  5,  7000.0,  7.0), // Central Sub -> Hospital 5 (Redundant feed to Hospital 5!)
        PowerLine(8,  2,  7,  6500.0, 11.0), // North Sub -> Metro Tech Univ
        PowerLine(9,  2,  9,  5500.0,  8.0), // North Sub -> Riverside Residential
        PowerLine(10, 3, 13,  9500.0,  5.0), // Central Sub -> Downtown Financial
        PowerLine(11, 3, 10,  6000.0,  7.0), // Central Sub -> Highland Gardens
        PowerLine(12, 10, 11, 4500.0,  4.0), // Highland Gardens <-> Maple Grove (Neighborhood tie)
        PowerLine(13, 3, 11,  6200.0,  6.0), // Central Sub -> Maple Grove
        PowerLine(14, 4,  6, 11000.0,  8.0), // South Sub -> St. Jude Trauma (Non-redundant hospital line!)
        PowerLine(15, 4,  8,  6800.0, 10.0), // South Sub -> State Univ
        PowerLine(16, 4, 14,  7800.0, 12.0), // South Sub -> Harbor Tech Park
        PowerLine(17, 4, 12,  5000.0, 14.0), // South Sub -> Oakridge Suburb (Non-redundant suburb line!)
        PowerLine(18, 14, 13, 6200.0,  9.0)  // Harbor Tech <-> Downtown Financial (Commercial tie)
    };

    for (int i = 0; i < 18; ++i) {
        powerGrid.addPowerLine(lines[i]);
    }

    // Road Network (Independent travel routes for repair crews)
    // Connecting all nodes with realistic travel weights (minutes)
    roadNetwork.addRoad(1, 2, 12.0);
    roadNetwork.addRoad(1, 3, 10.0);
    roadNetwork.addRoad(1, 4, 15.0);
    roadNetwork.addRoad(2, 3, 8.0);
    roadNetwork.addRoad(3, 4, 9.0);
    roadNetwork.addRoad(2, 5, 6.0);
    roadNetwork.addRoad(3, 5, 7.0);
    roadNetwork.addRoad(2, 7, 11.0);
    roadNetwork.addRoad(2, 9, 8.0);
    roadNetwork.addRoad(3, 13, 5.0);
    roadNetwork.addRoad(3, 10, 7.0);
    roadNetwork.addRoad(10, 11, 4.0);
    roadNetwork.addRoad(3, 11, 6.0);
    roadNetwork.addRoad(4, 6, 8.0);
    roadNetwork.addRoad(4, 8, 10.0);
    roadNetwork.addRoad(4, 14, 12.0);
    roadNetwork.addRoad(4, 12, 14.0);
    roadNetwork.addRoad(14, 13, 9.0);
    // Additional cross roads
    roadNetwork.addRoad(5, 7, 5.0);
    roadNetwork.addRoad(7, 9, 6.0);
    roadNetwork.addRoad(6, 8, 7.0);
    roadNetwork.addRoad(8, 12, 8.0);

    logEvent("GridGuard Simulation initialized with 14 locations and 18 power lines.", -1, "SYSTEM_INIT");
}

bool Simulation::simulatePowerLineFailure(int lineId, int severity, int durationMinutes) {
    // STEP 1: Select power line
    PowerLine* line = powerGrid.getPowerLine(lineId);
    if (!line) {
        std::cout << "[ERROR] Power line #" << lineId << " does not exist in the network.\n";
        return false;
    }
    if (!line->isOperational) {
        std::cout << "[WARNING] Power line #" << lineId << " has ALREADY failed.\n";
        return false;
    }

    if (severity < 1 || severity > 5) {
        severity = 3; // Clamp to reasonable default
    }

    simulationTimeMinutes += 15;

    // Analyze reachable locations BEFORE disabling to isolate impact of this specific line
    CustomVector<int> beforeConnected, beforeDisconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, beforeConnected, beforeDisconnected);

    // STEP 2: Mark selected power line as FAILED
    powerGrid.disablePowerLine(lineId);

    // STEP 3: Create fault record and assign unique fault ID
    int fId = nextFaultId++;

    // STEP 4: Record operation in undo stack
    undoStack.push(UndoAction(ACTION_LINE_DISABLED, lineId, fId,
                              "Simulated failure on Power Line #" + std::to_string(lineId)));

    // STEP 5: Run BFS from main power station
    CustomVector<int> reachable;
    CustomVector<int> disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, reachable, disconnected);

    // Update location powered statuses
    for (size_t i = 0; i < reachable.size(); ++i) {
        Location* loc = powerGrid.getLocation(reachable[i]);
        if (loc) {
            loc->isPowered = true;
            locationTable.update(loc->id, *loc);
        }
    }
    for (size_t i = 0; i < disconnected.size(); ++i) {
        Location* loc = powerGrid.getLocation(disconnected[i]);
        if (loc) {
            loc->isPowered = false;
            locationTable.update(loc->id, *loc);
        }
    }

    // Identify locations specifically disconnected by THIS line failure (beforeConnected \ reachable)
    CustomVector<int> lineImpactLocations;
    bool isStillReachable[64] = {false};
    for (size_t i = 0; i < reachable.size(); ++i) {
        if (reachable[i] >= 0 && reachable[i] < 64) {
            isStillReachable[reachable[i]] = true;
        }
    }

    int critCount = 0;
    int popCount = 0;
    for (size_t i = 0; i < beforeConnected.size(); ++i) {
        int u = beforeConnected[i];
        if (!isStillReachable[u]) {
            lineImpactLocations.push_back(u);
            const Location* loc = powerGrid.getLocation(u);
            if (loc) {
                popCount += loc->population;
                if (loc->isCriticalFacility()) {
                    critCount++;
                }
            }
        }
    }

    // STEP 8: Calculate fault priority score
    Fault faultRecord(fId, lineId, severity, critCount, popCount, durationMinutes);

    // STEP 9: Insert fault into max-heap
    repairQueue.insert(faultRecord);

    // STEP 10: Insert fault record into hash table
    faultTable.insert(fId, faultRecord);

    // STEP 11: Add failure event to incident history
    std::string desc = "Power Line #" + std::to_string(lineId) + " (" +
                       powerGrid.getLocation(line->fromLocationId)->name + " <-> " +
                       powerGrid.getLocation(line->toLocationId)->name + ") FAILED. " +
                       std::to_string(lineImpactLocations.size()) + " location(s) newly isolated.";
    logEvent(desc, fId, "FAILURE_DETECTED");

    // STEP 12: Display outage report
    std::cout << "\n========================================================================================\n";
    std::cout << "                              GRIDGUARD INCIDENT OUTAGE REPORT                          \n";
    std::cout << "========================================================================================\n";
    std::cout << "Fault ID Registered : " << fId << "\n";
    std::cout << "Failed Power Line   : Line #" << lineId << " ["
              << powerGrid.getLocation(line->fromLocationId)->name << " <---> "
              << powerGrid.getLocation(line->toLocationId)->name << "]\n";
    std::cout << "Fault Severity      : " << severity << " / 5\n";
    std::cout << "Outage Duration Est : " << durationMinutes << " minutes\n";
    std::cout << "Priority Score      : " << std::fixed << std::setprecision(1) << faultRecord.priorityScore << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "DIRECTLY ISOLATED LOCATIONS BY THIS LINE (" << lineImpactLocations.size() << "):\n";
    if (lineImpactLocations.empty()) {
        std::cout << "   None! Grid redundancy preserved power flow through alternate transmission lines.\n";
    } else {
        for (size_t i = 0; i < lineImpactLocations.size(); ++i) {
            const Location* loc = powerGrid.getLocation(lineImpactLocations[i]);
            if (loc) {
                std::cout << "   * [" << std::setw(2) << loc->id << "] "
                          << std::setw(28) << std::left << loc->name
                          << " | Type: " << std::setw(18) << loc->getTypeString()
                          << " | Pop: " << std::setw(6) << loc->population
                          << " | Crit: " << loc->getCriticalityString() << "\n";
            }
        }
    }
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "TOTAL GRID STATUS            : " << reachable.size() << " Online / "
              << disconnected.size() << " Offline\n";
    std::cout << "POPULATION IMPACTED BY LINE  : " << popCount << " residents\n";
    std::cout << "CRITICAL FACILITIES IMPACTED : " << critCount << "\n";
    std::cout << "========================================================================================\n";

    return true;
}

bool Simulation::processNextRepair(int specificFaultId) {
    if (repairQueue.isEmpty()) {
        std::cout << "[INFO] No pending faults in the repair queue. All lines operational!\n";
        return false;
    }

    Fault faultToRepair;
    if (specificFaultId != -1) {
        Fault* fPtr = faultTable.searchPtr(specificFaultId);
        if (!fPtr) {
            std::cout << "[ERROR] Fault ID #" << specificFaultId << " not found in records.\n";
            return false;
        }
        if (fPtr->status == REPAIRED) {
            std::cout << "[WARNING] Fault ID #" << specificFaultId << " has ALREADY been repaired.\n";
            return false;
        }
        faultToRepair = *fPtr;
        // Also extract from heap if it matches top or re-heapify
        if (!repairQueue.isEmpty() && repairQueue.peekMax().faultId == specificFaultId) {
            repairQueue.extractMax();
        }
    } else {
        // STEP 1: Select highest-priority fault from Max-Heap
        faultToRepair = repairQueue.extractMax();
    }

    int fId = faultToRepair.faultId;
    int lineId = faultToRepair.powerLineId;
    PowerLine* line = powerGrid.getPowerLine(lineId);
    if (!line) {
        std::cout << "[ERROR] Corrupted power line ID in fault record.\n";
        return false;
    }

    simulationTimeMinutes += 30;

    std::cout << "\n========================================================================================\n";
    std::cout << "                            EMERGENCY REPAIR OPERATION INITIATED                        \n";
    std::cout << "========================================================================================\n";
    std::cout << "Processing Fault ID: #" << fId << " (Priority Score: "
              << std::fixed << std::setprecision(1) << faultToRepair.priorityScore << ")\n";
    std::cout << "Target Damaged Line: Line #" << lineId << " ["
              << powerGrid.getLocation(line->fromLocationId)->name << " <---> "
              << powerGrid.getLocation(line->toLocationId)->name << "]\n";

    // STEP 2: Use Dijkstra's algorithm to calculate repair crew route
    int destination = line->toLocationId;
    CustomVector<int> route;
    double travelMinutes = 0.0;
    bool routeFound = roadNetwork.findShortestPath(repairDepotLocationId, destination, route, travelMinutes);

    // STEP 3: Display route and simulated travel cost
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "REPAIR CREW DISPATCH ROUTE (DIJKSTRA'S ALGORITHM):\n";
    std::cout << "   Origin (Depot): [" << repairDepotLocationId << "] "
              << powerGrid.getLocation(repairDepotLocationId)->name << "\n";
    std::cout << "   Destination   : [" << destination << "] "
              << powerGrid.getLocation(destination)->name << "\n";
    if (routeFound && !route.empty()) {
        std::cout << "   Optimal Path  : ";
        for (size_t i = 0; i < route.size(); ++i) {
            const Location* loc = powerGrid.getLocation(route[i]);
            std::cout << "[" << route[i] << "] " << (loc ? loc->name : "Loc");
            if (i + 1 < route.size()) std::cout << "  ==>  ";
        }
        std::cout << "\n   Travel Time   : " << std::fixed << std::setprecision(1)
                  << travelMinutes << " minutes (Simulated Road Network)\n";
    } else {
        std::cout << "   [Notice] Destination reached directly at location depot.\n";
    }
    std::cout << "----------------------------------------------------------------------------------------\n";

    // STEP 4: Mark repair as started
    faultToRepair.status = IN_PROGRESS;
    faultTable.update(fId, faultToRepair);
    logEvent("Repair crew dispatched to Line #" + std::to_string(lineId), fId, "REPAIR_STARTED");

    // STEP 5: Simulate repair operation
    std::cout << "Executing repairs on Line #" << lineId << "... [Simulated Cost: $"
              << std::fixed << std::setprecision(0) << line->repairCost << "]\n";

    // STEP 6: Restore failed power line
    powerGrid.restorePowerLine(lineId);

    // Push to undo stack
    undoStack.push(UndoAction(ACTION_LINE_RESTORED, lineId, fId,
                              "Completed repair of Line #" + std::to_string(lineId)));

    // STEP 7: Run BFS again
    CustomVector<int> connected;
    CustomVector<int> disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);

    // Update power status
    CustomVector<int> newlyRestored;
    for (size_t i = 0; i < connected.size(); ++i) {
        Location* loc = powerGrid.getLocation(connected[i]);
        if (loc) {
            if (!loc->isPowered) {
                newlyRestored.push_back(loc->id);
            }
            loc->isPowered = true;
            locationTable.update(loc->id, *loc);
        }
    }
    for (size_t i = 0; i < disconnected.size(); ++i) {
        Location* loc = powerGrid.getLocation(disconnected[i]);
        if (loc) {
            loc->isPowered = false;
            locationTable.update(loc->id, *loc);
        }
    }

    // STEP 8: Display newly restored locations
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "ELECTRICITY RESTORATION RESULTS (POST-REPAIR BFS ANALYSIS):\n";
    if (newlyRestored.empty()) {
        std::cout << "   Line #" << lineId << " restored. Network redundancy and stability reinforced.\n";
    } else {
        std::cout << "   " << newlyRestored.size() << " Location(s) successfully re-energized:\n";
        for (size_t i = 0; i < newlyRestored.size(); ++i) {
            const Location* loc = powerGrid.getLocation(newlyRestored[i]);
            if (loc) {
                std::cout << "   + [" << std::setw(2) << loc->id << "] "
                          << std::setw(28) << std::left << loc->name
                          << " (Pop: " << loc->population << " restored)\n";
            }
        }
    }

    // STEP 9: Update fault status
    faultToRepair.status = REPAIRED;
    faultTable.update(fId, faultToRepair);

    // STEP 10: Record repair and restoration events in incident history
    logEvent("Line #" + std::to_string(lineId) + " successfully repaired and re-energized.", fId, "REPAIR_COMPLETED");
    logEvent("Electricity restored to " + std::to_string(newlyRestored.size()) + " location(s).", fId, "ELECTRICITY_RESTORED");

    std::cout << "========================================================================================\n";
    std::cout << "Repair complete. Fault #" << fId << " marked as REPAIRED.\n";
    return true;
}

bool Simulation::restorePowerLineDirectly(int lineId) {
    PowerLine* line = powerGrid.getPowerLine(lineId);
    if (!line) {
        std::cout << "[ERROR] Power line #" << lineId << " does not exist.\n";
        return false;
    }
    if (line->isOperational) {
        std::cout << "[INFO] Power line #" << lineId << " is ALREADY operational.\n";
        return false;
    }

    powerGrid.restorePowerLine(lineId);
    undoStack.push(UndoAction(ACTION_LINE_RESTORED, lineId, -1,
                              "Manual direct restoration of Line #" + std::to_string(lineId)));

    // Re-run BFS to restore status
    CustomVector<int> connected, disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);

    for (size_t i = 0; i < connected.size(); ++i) {
        Location* loc = powerGrid.getLocation(connected[i]);
        if (loc) {
            loc->isPowered = true;
            locationTable.update(loc->id, *loc);
        }
    }

    logEvent("Line #" + std::to_string(lineId) + " manually restored.", -1, "MANUAL_RESTORATION");
    std::cout << "[SUCCESS] Power line #" << lineId << " restored to operational status.\n";
    return true;
}

bool Simulation::undoLastOperation() {
    if (undoStack.isEmpty()) {
        std::cout << "[INFO] Undo Stack is empty. No reversible operations to undo.\n";
        return false;
    }

    UndoAction action = undoStack.pop();
    simulationTimeMinutes += 5;

    std::cout << "\n========================================================================================\n";
    std::cout << "                              REVERSING LAST OPERATION                                  \n";
    std::cout << "========================================================================================\n";
    std::cout << "Reversing: " << action.description << " (Target ID: " << action.targetId << ")\n";

    if (action.type == ACTION_LINE_DISABLED) {
        // Line was disabled: Undo by restoring the line!
        powerGrid.restorePowerLine(action.targetId);

        // Update fault record if it was generated
        if (action.secondaryId != -1) {
            Fault* f = faultTable.searchPtr(action.secondaryId);
            if (f) {
                f->status = REPAIRED;
            }
        }

        // Re-analyze power grid
        CustomVector<int> connected, disconnected;
        powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);
        for (size_t i = 0; i < connected.size(); ++i) {
            Location* loc = powerGrid.getLocation(connected[i]);
            if (loc) {
                loc->isPowered = true;
                locationTable.update(loc->id, *loc);
            }
        }

        logEvent("Reversed failure of Line #" + std::to_string(action.targetId) + "; line re-enabled.",
                 action.secondaryId, "OPERATION_UNDONE");

        std::cout << "[STATUS] Line #" << action.targetId << " re-enabled. Grid connectivity restored.\n";
    } else if (action.type == ACTION_LINE_RESTORED) {
        // Line was restored: Undo by disabling the line!
        powerGrid.disablePowerLine(action.targetId);

        if (action.secondaryId != -1) {
            Fault* f = faultTable.searchPtr(action.secondaryId);
            if (f) {
                f->status = PENDING;
                repairQueue.insert(*f);
            }
        }

        // Re-analyze power grid
        CustomVector<int> connected, disconnected;
        powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);
        for (size_t i = 0; i < disconnected.size(); ++i) {
            Location* loc = powerGrid.getLocation(disconnected[i]);
            if (loc) {
                loc->isPowered = false;
                locationTable.update(loc->id, *loc);
            }
        }

        logEvent("Reversed repair of Line #" + std::to_string(action.targetId) + "; line re-disabled.",
                 action.secondaryId, "OPERATION_UNDONE");

        std::cout << "[STATUS] Line #" << action.targetId << " re-disabled. Grid state returned to outage.\n";
    }

    std::cout << "========================================================================================\n";
    return true;
}

void Simulation::displayPowerNetwork() const {
    powerGrid.displayNetwork();
}

void Simulation::displayAllLocations() const {
    powerGrid.displayAllLocations();
}

void Simulation::displayLocationNeighbors(int locationId) const {
    powerGrid.displayNeighbors(locationId);
}

void Simulation::displayRepairQueue() const {
    repairQueue.displayQueue();
}

void Simulation::displayAllFaults() const {
    std::cout << "========================================================================================\n";
    std::cout << "                                REGISTERED FAULT DATABASE                               \n";
    std::cout << "========================================================================================\n";
    if (faultTable.size() == 0) {
        std::cout << "   [No faults registered in database]\n";
    } else {
        for (int id = 101; id < nextFaultId; ++id) {
            Fault f;
            if (faultTable.search(id, f)) {
                f.display();
            }
        }
    }
    std::cout << "========================================================================================\n";
}

void Simulation::displayIncidentHistoryForward() const {
    incidentHistory.displayForward();
}

void Simulation::displayIncidentHistoryBackward() const {
    incidentHistory.displayBackward();
}

void Simulation::searchIncidentHistory(const std::string& keyword) const {
    incidentHistory.searchEvents(keyword);
}

void Simulation::displayUndoHistory() const {
    undoStack.displayHistory();
}

bool Simulation::searchLocationById(int locId) const {
    Location loc;
    if (locationTable.search(locId, loc)) {
        std::cout << "\n--- Location Found (Hash Table Lookup) ---\n";
        loc.display();
        return true;
    }
    std::cout << "[NOT FOUND] Location ID #" << locId << " does not exist in Hash Table.\n";
    return false;
}

bool Simulation::searchFaultById(int faultId) const {
    Fault f;
    if (faultTable.search(faultId, f)) {
        std::cout << "\n--- Fault Record Found (Hash Table Lookup) ---\n";
        f.display();
        return true;
    }
    std::cout << "[NOT FOUND] Fault ID #" << faultId << " does not exist in Hash Table.\n";
    return false;
}

void Simulation::displayHashTableDiagnostics() const {
    std::cout << "\n========================================================================================\n";
    std::cout << "                          HASH TABLE SEPARATE CHAINING DIAGNOSTICS                      \n";
    std::cout << "========================================================================================\n";
    std::cout << "--- Location Hash Table ---\n";
    locationTable.displayBucketDiagnostics();
    std::cout << "\n--- Fault Hash Table ---\n";
    faultTable.displayBucketDiagnostics();
    std::cout << "========================================================================================\n";
}

void Simulation::runBFSTraversal(int startId) const {
    if (startId == -1) startId = mainPowerStationId;
    CustomVector<int> reachable;
    powerGrid.runBFS(startId, reachable, true);

    std::cout << "\n--- Breadth-First Search (BFS) Traversal using Custom Queue ---\n";
    std::cout << "Starting from Location [" << startId << "] "
              << powerGrid.getLocation(startId)->name << "\n";
    std::cout << "Reachable Locations Traversal Order (" << reachable.size() << " nodes):\n   ";
    for (size_t i = 0; i < reachable.size(); ++i) {
        std::cout << reachable[i];
        if (i + 1 < reachable.size()) std::cout << " -> ";
    }
    std::cout << "\n";
}

void Simulation::runDFSTraversal(int startId) const {
    if (startId == -1) startId = mainPowerStationId;
    CustomVector<int> order;
    powerGrid.runDFS(startId, order, true);

    std::cout << "\n--- Depth-First Search (DFS) Traversal (Recursive) ---\n";
    std::cout << "Starting from Location [" << startId << "] "
              << powerGrid.getLocation(startId)->name << "\n";
    std::cout << "DFS Traversal Order (" << order.size() << " nodes):\n   ";
    for (size_t i = 0; i < order.size(); ++i) {
        std::cout << order[i];
        if (i + 1 < order.size()) std::cout << " -> ";
    }
    std::cout << "\n";
}

void Simulation::analyzeAndDisplayConnectivity() const {
    CustomVector<int> connected, disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);

    std::cout << "\n========================================================================================\n";
    std::cout << "                           GRID CONNECTIVITY & POWER FLOW STATUS                        \n";
    std::cout << "========================================================================================\n";
    std::cout << "Main Power Source: [" << mainPowerStationId << "] "
              << powerGrid.getLocation(mainPowerStationId)->name << "\n";
    std::cout << "Total Locations Powered    : " << connected.size() << " / " << powerGrid.getLocationCount() << "\n";
    std::cout << "Total Locations Disconnected : " << disconnected.size() << " / " << powerGrid.getLocationCount() << "\n";

    if (!disconnected.empty()) {
        std::cout << "\nDISCONNECTED CRITICAL FACILITIES & RESIDENTIAL AREAS:\n";
        for (size_t i = 0; i < disconnected.size(); ++i) {
            const Location* loc = powerGrid.getLocation(disconnected[i]);
            if (loc) {
                std::cout << "   ! [" << std::setw(2) << loc->id << "] "
                          << std::setw(28) << std::left << loc->name
                          << " | " << loc->getTypeString()
                          << " | Pop: " << loc->population
                          << " | " << loc->getCriticalityString() << "\n";
            }
        }
    } else {
        std::cout << "\n[ALL SYSTEMS NORMAL] Complete grid connectivity. Zero outages.\n";
    }
    std::cout << "========================================================================================\n";
}

bool Simulation::findRepairCrewRoute(int targetLocationId) const {
    CustomVector<int> path;
    double travelMinutes = 0.0;
    bool found = roadNetwork.findShortestPath(repairDepotLocationId, targetLocationId, path, travelMinutes);

    std::cout << "\n========================================================================================\n";
    std::cout << "                        DIJKSTRA'S SHORTEST ROAD ROUTE CALCULATION                      \n";
    std::cout << "========================================================================================\n";
    std::cout << "Origin Depot     : [" << repairDepotLocationId << "] "
              << powerGrid.getLocation(repairDepotLocationId)->name << "\n";
    const Location* destLoc = powerGrid.getLocation(targetLocationId);
    std::cout << "Destination Node : [" << targetLocationId << "] "
              << (destLoc ? destLoc->name : "Unknown Location") << "\n";

    if (!found || path.empty()) {
        std::cout << "[ERROR] No viable road path found to location ID " << targetLocationId << ".\n";
        return false;
    }

    std::cout << "Optimal Path     : ";
    for (size_t i = 0; i < path.size(); ++i) {
        const Location* loc = powerGrid.getLocation(path[i]);
        std::cout << "[" << path[i] << "] " << (loc ? loc->name : "Node");
        if (i + 1 < path.size()) std::cout << " ==> ";
    }
    std::cout << "\nTotal Travel Time: " << std::fixed << std::setprecision(1) << travelMinutes << " minutes.\n";
    std::cout << "========================================================================================\n";
    return true;
}

void Simulation::sortAndDisplayFaults(SortCriterion criterion, bool ascending) {
    int totalFaults = nextFaultId - 101;
    if (totalFaults <= 0) {
        std::cout << "[INFO] No faults recorded yet to sort.\n";
        return;
    }

    // Collect faults into dynamic array for sorting
    Fault* arr = new Fault[totalFaults];
    int count = 0;
    for (int id = 101; id < nextFaultId; ++id) {
        Fault f;
        if (faultTable.search(id, f)) {
            arr[count++] = f;
        }
    }

    if (count == 0) {
        std::cout << "[INFO] No active faults found.\n";
        delete[] arr;
        return;
    }

    // Execute Manual QuickSort
    Sorting::quickSort(arr, 0, count - 1, criterion, ascending);

    std::cout << "\n========================================================================================\n";
    std::cout << "            SORTED FAULT RECORDS BY: "
              << Sorting::getCriterionName(criterion)
              << (ascending ? " (ASCENDING)" : " (DESCENDING)") << "\n";
    std::cout << "========================================================================================\n";
    std::cout << std::left
              << std::setw(10) << "Fault ID"
              << std::setw(10) << "Line ID"
              << std::setw(10) << "Severity"
              << std::setw(12) << "Crit Fac"
              << std::setw(14) << "Pop Affected"
              << std::setw(12) << "Duration"
              << std::setw(16) << "Priority Score"
              << std::setw(12) << "Status"
              << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    for (int i = 0; i < count; ++i) {
        std::cout << std::left
                  << std::setw(10) << arr[i].faultId
                  << std::setw(10) << arr[i].powerLineId
                  << std::setw(10) << arr[i].severity
                  << std::setw(12) << arr[i].criticalFacilitiesAffected
                  << std::setw(14) << arr[i].populationAffected
                  << (std::to_string(arr[i].outageDuration) + "m     ").substr(0, 12)
                  << std::setw(16) << std::fixed << std::setprecision(1) << arr[i].priorityScore
                  << std::setw(12) << arr[i].getStatusString()
                  << "\n";
    }
    std::cout << "========================================================================================\n";

    delete[] arr;
}

void Simulation::displayOutageReportSummary() const {
    CustomVector<int> connected, disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);

    int critCount = 0;
    int popCount = 0;
    for (size_t i = 0; i < disconnected.size(); ++i) {
        const Location* loc = powerGrid.getLocation(disconnected[i]);
        if (loc) {
            popCount += loc->population;
            if (loc->isCriticalFacility()) {
                critCount++;
            }
        }
    }

    std::cout << "\n========================================================================================\n";
    std::cout << "                           CURRENT ACTIVE OUTAGE SUMMARY                                \n";
    std::cout << "========================================================================================\n";
    std::cout << "Total Disconnected Locations : " << disconnected.size() << "\n";
    std::cout << "Total Disconnected Residents : " << popCount << "\n";
    std::cout << "Critical Facilities Affected : " << critCount << "\n";
    std::cout << "Pending Faults in Heap       : " << repairQueue.size() << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    if (disconnected.empty()) {
        std::cout << "All 14 locations receiving power. No active outages.\n";
    } else {
        for (size_t i = 0; i < disconnected.size(); ++i) {
            const Location* loc = powerGrid.getLocation(disconnected[i]);
            if (loc) {
                std::cout << "   ! [" << std::setw(2) << loc->id << "] "
                          << std::setw(28) << std::left << loc->name
                          << " | Pop: " << std::setw(6) << loc->population
                          << " | Crit: " << loc->getCriticalityString() << "\n";
            }
        }
    }
    std::cout << "========================================================================================\n";
}

void Simulation::displayCriticalFacilitiesStatus() const {
    std::cout << "\n========================================================================================\n";
    std::cout << "                         CRITICAL FACILITIES MONITORING REPORT                          \n";
    std::cout << "========================================================================================\n";
    for (int i = 1; i <= 14; ++i) {
        const Location* loc = powerGrid.getLocation(i);
        if (loc && loc->isCriticalFacility()) {
            std::cout << "Facility: [" << std::setw(2) << loc->id << "] "
                      << std::setw(28) << std::left << loc->name
                      << " | Type: " << std::setw(14) << loc->getTypeString()
                      << " | Status: " << (loc->isPowered ? "[ONLINE - ENERGIZED]" : "[*** OFFLINE - EMERGENCY ***]")
                      << "\n";
        }
    }
    std::cout << "========================================================================================\n";
}
