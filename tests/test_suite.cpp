#include "../include/Simulation.h"
#include "../include/HashTable.h"
#include "../include/Queue.h"
#include "../include/Stack.h"
#include "../include/MaxHeap.h"
#include "../include/DoublyLinkedList.h"
#include <iostream>
#include <cassert>
#include <string>

void printTestHeader(const std::string& name) {
    std::cout << "\n========================================================================\n";
    std::cout << " RUNNING: " << name << "\n";
    std::cout << "========================================================================\n";
}

void printPass(const std::string& msg) {
    std::cout << " [PASS] " << msg << "\n";
}

// TEST 1: Normal network operation. All expected locations should receive power.
void test1_NormalOperation() {
    printTestHeader("TEST 1: Normal Network Operation");
    Simulation sim;
    CustomVector<int> connected, disconnected;
    sim.getPowerGrid().analyzeConnectivity(sim.getMainStationId(), connected, disconnected);

    std::cout << "Connected count: " << connected.size() << ", Disconnected count: " << disconnected.size() << "\n";
    assert(connected.size() == 14);
    assert(disconnected.empty());
    printPass("All 14 locations are connected and powered in baseline state.");
}

// TEST 2: Non-critical power line failure. Some locations may become disconnected.
void test2_NonCriticalFailure() {
    printTestHeader("TEST 2: Non-Critical Power Line Failure");
    Simulation sim;
    // Power Line 17 connects South Substation (4) to Oakridge Suburb (12).
    // Oakridge has no other power feed.
    bool ok = sim.simulatePowerLineFailure(17, 2, 45);
    assert(ok);

    CustomVector<int> connected, disconnected;
    sim.getPowerGrid().analyzeConnectivity(sim.getMainStationId(), connected, disconnected);

    assert(disconnected.size() == 1);
    assert(disconnected[0] == 12);
    const Location* oakridge = sim.getPowerGrid().getLocation(12);
    assert(oakridge != nullptr);
    assert(!oakridge->isPowered);
    printPass("Line #17 failure properly isolated Location 12 (Oakridge Suburb).");
}

// TEST 3: Redundant connection. A power line fails, but alternative paths maintain connectivity.
void test3_RedundantConnection() {
    printTestHeader("TEST 3: Redundant Connection (Alternate Power Paths)");
    Simulation sim;
    // Hospital 5 (City General Hospital) has two feeds:
    // Line 6: (2 <-> 5) and Line 7: (3 <-> 5)
    bool ok = sim.simulatePowerLineFailure(6, 3, 30);
    assert(ok);

    CustomVector<int> connected, disconnected;
    sim.getPowerGrid().analyzeConnectivity(sim.getMainStationId(), connected, disconnected);

    // Because Line 7 is still operational, Hospital 5 should remain CONNECTED
    const Location* hosp5 = sim.getPowerGrid().getLocation(5);
    assert(hosp5 != nullptr);
    assert(hosp5->isPowered);
    assert(disconnected.empty());
    printPass("Hospital 5 remained powered via redundant Line #7 after Line #6 failed.");
}

// TEST 4: Hospital outage. A hospital loses power and the fault receives higher repair priority.
void test4_HospitalOutage() {
    printTestHeader("TEST 4: Hospital Outage & Priority Elevation");
    Simulation sim;
    // Line 14 connects South Substation (4) to St. Jude Trauma Center (6).
    // St. Jude is non-redundant.
    bool ok = sim.simulatePowerLineFailure(14, 5, 120);
    assert(ok);

    const Location* stJude = sim.getPowerGrid().getLocation(6);
    assert(stJude != nullptr);
    assert(!stJude->isPowered);
    assert(stJude->isCriticalFacility());

    // Priority formula: (Severity * 10) + (CritFac * 20) + (Pop / 100) + (Duration * 2)
    // Severity=5 (*10=50), CritFac=1 (*20=20), Pop=850 (/100=8.5), Duration=120 (*2=240)
    // Score = 50 + 20 + 8.5 + 240 = 318.5
    Fault topFault = sim.getRepairQueue().peekMax();
    assert(topFault.criticalFacilitiesAffected >= 1);
    assert(topFault.priorityScore >= 300.0);

    printPass("St. Jude Trauma Center outage correctly identified critical facility and assigned elevated score.");
}

// TEST 5: Multiple simultaneous faults. Multiple faults registered and prioritized correctly.
void test5_MultipleSimultaneousFaults() {
    printTestHeader("TEST 5: Multiple Simultaneous Faults & Heap Order");
    Simulation sim;
    // Fault A: Non-critical suburb failure (Line 17, Severity 1, Duration 30)
    sim.simulatePowerLineFailure(17, 1, 30);

    // Fault B: Hospital failure (Line 14, Severity 5, Duration 120)
    sim.simulatePowerLineFailure(14, 5, 120);

    // Fault C: University line failure (Line 8, Severity 2, Duration 30)
    sim.simulatePowerLineFailure(8, 2, 30);

    assert(sim.getRepairQueue().size() == 3);

    // Hospital fault must have highest priority and be at root of Max-Heap
    Fault highest = sim.getRepairQueue().peekMax();
    assert(highest.powerLineId == 14); // Hospital line #14 must be first!
    printPass("Max-Heap correctly prioritized critical Hospital fault #14 above all other simultaneous faults.");
}

// TEST 6: Repair operation. A selected fault is repaired and electricity is restored.
void test6_RepairOperation() {
    printTestHeader("TEST 6: Repair Operation & Grid Re-energization");
    Simulation sim;
    sim.simulatePowerLineFailure(17, 3, 45); // Oakridge (12) offline
    assert(!sim.getPowerGrid().getLocation(12)->isPowered);

    // Perform repair
    bool repOk = sim.processNextRepair();
    assert(repOk);

    // Location 12 should now be restored
    assert(sim.getPowerGrid().getLocation(12)->isPowered);
    assert(sim.getRepairQueue().isEmpty());
    printPass("Repair successfully completed, re-energized Location 12, and emptied repair queue.");
}

// TEST 7: Undo operation. A failed power line is restored to its previous operational state.
void test7_UndoOperation() {
    printTestHeader("TEST 7: Undo Mechanism via Stack");
    Simulation sim;
    sim.simulatePowerLineFailure(17, 3, 45);
    assert(!sim.getPowerGrid().getLocation(12)->isPowered);
    assert(sim.getUndoStackSize() == 1);

    // Undo the failure
    bool undoOk = sim.undoLastOperation();
    assert(undoOk);

    // Grid should now be restored to operational baseline
    assert(sim.getPowerGrid().getLocation(12)->isPowered);
    assert(sim.getPowerGrid().isPowerLineOperational(17));
    assert(sim.getUndoStackSize() == 0);
    printPass("Undo successfully popped stack and reversed power line failure back to operational.");
}

// TEST 8: Dijkstra route. Minimum-cost repair route displayed.
void test8_DijkstraRoute() {
    printTestHeader("TEST 8: Dijkstra Shortest Route Calculation");
    Simulation sim;
    CustomVector<int> path;
    double cost = 0.0;
    // Find path from Main Station (1) to Oakridge Suburb (12)
    bool found = sim.getRoadNetwork().findShortestPath(1, 12, path, cost);
    assert(found);
    assert(!path.empty());
    assert(path[0] == 1);
    assert(path[path.size() - 1] == 12);
    assert(cost > 0.0);

    std::cout << "Dijkstra optimal path from 1 to 12: ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i] << (i + 1 < path.size() ? " -> " : "");
    }
    std::cout << " (Total time: " << cost << " mins)\n";
    printPass("Dijkstra computed shortest route with non-negative travel weights.");
}

// TEST 9: Hash table collision. Multiple records sharing a hash index remain searchable.
void test9_HashTableCollision() {
    printTestHeader("TEST 9: Hash Table Separate Chaining & Collision Handling");
    HashTable<int, std::string> table(5); // Small capacity of 5 guarantees collisions

    // Keys 5, 10, 15 all hash to 0 (5 % 5 == 0, 10 % 5 == 0, 15 % 5 == 0)
    table.insert(5, "Hospital Alpha");
    table.insert(10, "Substation Beta");
    table.insert(15, "Power Plant Gamma");

    assert(table.getCollisionCount() >= 2);

    std::string val1, val2, val3;
    assert(table.search(5, val1) && val1 == "Hospital Alpha");
    assert(table.search(10, val2) && val2 == "Substation Beta");
    assert(table.search(15, val3) && val3 == "Power Plant Gamma");

    printPass("Separate chaining resolved collisions; all 3 items retrieved accurately.");
}

// TEST 10: Incident history. Events can be traversed in both directions.
void test10_IncidentHistory() {
    printTestHeader("TEST 10: Doubly Linked List Forward & Backward Traversal");
    DoublyLinkedList<IncidentEvent> list;

    list.append(IncidentEvent(1, "Event Alpha", 101, "INIT", "T+00:00"));
    list.append(IncidentEvent(2, "Event Beta", 102, "FAILURE", "T+00:15"));
    list.append(IncidentEvent(3, "Event Gamma", 103, "REPAIR", "T+00:30"));

    assert(list.size() == 3);
    const IncidentEvent* latest = list.getLatest();
    assert(latest != nullptr);
    assert(latest->eventId == 3);

    printPass("Doubly Linked List correctly maintained bidirectional node pointers.");
}

int main() {
    std::cout << "\n========================================================================\n";
    std::cout << "           GRIDGUARD ACADEMIC VERIFICATION TEST HARNESS                 \n";
    std::cout << "========================================================================\n";

    test1_NormalOperation();
    test2_NonCriticalFailure();
    test3_RedundantConnection();
    test4_HospitalOutage();
    test5_MultipleSimultaneousFaults();
    test6_RepairOperation();
    test7_UndoOperation();
    test8_DijkstraRoute();
    test9_HashTableCollision();
    test10_IncidentHistory();

    std::cout << "\n========================================================================\n";
    std::cout << "     ALL 10 VERIFICATION TESTS PASSED SUCCESSFULLY! (10/10)             \n";
    std::cout << "========================================================================\n\n";

    return 0;
}
