#include "../include/Simulation.h"
#include "../include/HashTable.h"
#include "../include/Queue.h"
#include "../include/Stack.h"
#include "../include/MaxHeap.h"
#include "../include/DoublyLinkedList.h"
#include "../include/CircularQueue.h"
#include "../include/ArrayStack.h"
#include "../include/Sorting.h"
#include "../include/SearchAlgorithms.h"
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

// TEST 1: Normal network operation. All 44 Karachi municipal locations should receive power.
void test1_NormalOperation() {
    printTestHeader("TEST 1: Karachi Baseline Normal Network Operation");
    Simulation sim;
    CustomVector<int> connected, disconnected;
    sim.getPowerGrid().analyzeConnectivity(sim.getMainStationId(), connected, disconnected);

    std::cout << "Connected count: " << connected.size() << ", Disconnected count: " << disconnected.size() << "\n";
    assert(connected.size() == 44);
    assert(disconnected.empty());
    printPass("All 44 Karachi locations are connected and powered in baseline state.");
}

// TEST 2: Non-critical power line failure. Malir neighborhood becomes isolated.
void test2_NonCriticalFailure() {
    printTestHeader("TEST 2: Non-Critical Power Line Failure (Malir Outage)");
    Simulation sim;
    // Power Line 48 connects East Hub (3) to Malir (31).
    // Malir has single radial feed.
    bool ok = sim.simulatePowerLineFailure(48, 2, 45);
    assert(ok);

    CustomVector<int> connected, disconnected;
    sim.getPowerGrid().analyzeConnectivity(sim.getMainStationId(), connected, disconnected);

    assert(disconnected.size() == 1);
    assert(disconnected[0] == 31);
    const Location* malir = sim.getPowerGrid().getLocation(31);
    assert(malir != nullptr);
    assert(!malir->isPowered);
    printPass("Line #48 failure properly isolated Location 31 (Malir).");
}

// TEST 3: Redundant connection. Aga Khan University Hospital remains powered via alternate feed.
void test3_RedundantConnection() {
    printTestHeader("TEST 3: Redundant Connection (Aga Khan Hospital Dual Feed)");
    Simulation sim;
    // Aga Khan University Hospital (10) has dual feeds:
    // Line 17 from Central Hub (1) and Line 18 from Gulshan Dist Node (6)
    bool ok = sim.simulatePowerLineFailure(17, 3, 30);
    assert(ok);

    CustomVector<int> connected, disconnected;
    sim.getPowerGrid().analyzeConnectivity(sim.getMainStationId(), connected, disconnected);

    // Because Line 18 is still operational, AKU should remain CONNECTED
    const Location* aku = sim.getPowerGrid().getLocation(10);
    assert(aku != nullptr);
    assert(aku->isPowered);
    assert(disconnected.empty());
    printPass("Aga Khan Hospital remained powered via redundant Line #18 after Line #17 failed.");
}

// TEST 4: Hospital outage. South City Hospital loses power and triggers elevated repair priority.
void test4_HospitalOutage() {
    printTestHeader("TEST 4: Critical Hospital Outage & Priority Elevation (South City Hospital)");
    Simulation sim;
    // Line 25 connects South Hub (4) to South City Hospital (14).
    // South City Hospital has single radial feed.
    bool ok = sim.simulatePowerLineFailure(25, 5, 120);
    assert(ok);

    const Location* hosp = sim.getPowerGrid().getLocation(14);
    assert(hosp != nullptr);
    assert(!hosp->isPowered);
    assert(hosp->isCriticalFacility());

    // Priority formula: (Severity * 10) + (CritFac * 20) + (Pop / 100) + (Duration * 2)
    // Severity=5 (*10=50), CritFac=1 (*20=20), Pop=1400 (/100=14.0), Duration=120 (*2=240)
    // Score = 50 + 20 + 14.0 + 240 = 324.0
    Fault topFault = sim.getRepairQueue().peekMax();
    assert(topFault.criticalFacilitiesAffected >= 1);
    assert(topFault.priorityScore >= 300.0);

    printPass("South City Hospital outage correctly identified critical facility and assigned elevated score.");
}

// TEST 5: Multiple simultaneous faults across Karachi. Max-Heap maintains priority order.
void test5_MultipleSimultaneousFaults() {
    printTestHeader("TEST 5: Multiple Simultaneous Faults & Heap Order");
    Simulation sim;
    // Fault A: Non-critical suburb failure (Line 48, Malir, Severity 1, Duration 30)
    sim.simulatePowerLineFailure(48, 1, 30);

    // Fault B: Hospital failure (Line 25, South City Hospital, Severity 5, Duration 120)
    sim.simulatePowerLineFailure(25, 5, 120);

    // Fault C: University line failure (Line 29, FAST-NUCES, Severity 2, Duration 30)
    sim.simulatePowerLineFailure(29, 2, 30);

    assert(sim.getRepairQueue().size() == 3);

    // Hospital fault must have highest priority and be at root of Max-Heap
    Fault highest = sim.getRepairQueue().peekMax();
    assert(highest.powerLineId == 25); // Hospital line #25 must be first!
    printPass("Max-Heap correctly prioritized critical Hospital fault #25 above all other simultaneous faults.");
}

// TEST 6: Repair operation. Selected fault repaired and electricity restored to Malir.
void test6_RepairOperation() {
    printTestHeader("TEST 6: Repair Operation & Grid Re-energization");
    Simulation sim;
    sim.simulatePowerLineFailure(48, 3, 45); // Malir (31) offline
    assert(!sim.getPowerGrid().getLocation(31)->isPowered);

    // Perform repair
    bool repOk = sim.processNextRepair();
    assert(repOk);

    // Location 31 should now be restored
    assert(sim.getPowerGrid().getLocation(31)->isPowered);
    assert(sim.getRepairQueue().isEmpty());
    printPass("Repair successfully completed, re-energized Location 31 (Malir), and emptied repair queue.");
}

// TEST 7: Undo operation. A failed power line is restored via Stack.
void test7_UndoOperation() {
    printTestHeader("TEST 7: Undo Mechanism via Stack");
    Simulation sim;
    sim.simulatePowerLineFailure(48, 3, 45);
    assert(!sim.getPowerGrid().getLocation(31)->isPowered);
    assert(sim.getUndoStackSize() == 1);

    // Undo the failure
    bool undoOk = sim.undoLastOperation();
    assert(undoOk);

    // Grid should now be restored to operational baseline
    assert(sim.getPowerGrid().getLocation(31)->isPowered);
    assert(sim.getPowerGrid().isPowerLineOperational(48));
    assert(sim.getUndoStackSize() == 0);
    printPass("Undo successfully popped stack and reversed power line failure back to operational.");
}

// TEST 8: Dijkstra route. Minimum-cost repair route across Karachi road network.
void test8_DijkstraRoute() {
    printTestHeader("TEST 8: Dijkstra Shortest Route Calculation (Karachi Arteries)");
    Simulation sim;
    CustomVector<int> path;
    double cost = 0.0;
    // Find path from Central Hub Depot (1) to Indus Hospital Korangi (11)
    bool found = sim.getRoadNetwork().findShortestPath(1, 11, path, cost);
    assert(found);
    assert(!path.empty());
    assert(path[0] == 1);
    assert(path[path.size() - 1] == 11);
    assert(cost > 0.0);

    std::cout << "Dijkstra optimal path from 1 to 11 (Indus Hospital): ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i] << (i + 1 < path.size() ? " -> " : "");
    }
    std::cout << " (Total time: " << cost << " mins)\n";
    printPass("Dijkstra computed shortest route across Karachi road arteries with non-negative travel weights.");
}

// TEST 9: Hash table collision. Multiple records sharing a hash index remain searchable.
void test9_HashTableCollision() {
    printTestHeader("TEST 9: Hash Table Separate Chaining & Collision Handling");
    HashTable<int, std::string> table(5); // Small capacity of 5 guarantees collisions

    // Keys 5, 10, 15 all hash to 0 (5 % 5 == 0, 10 % 5 == 0, 15 % 5 == 0)
    table.insert(5, "JPMC Cantt");
    table.insert(10, "Aga Khan Hospital");
    table.insert(15, "Indus Hospital Korangi");

    assert(table.getCollisionCount() >= 2);

    std::string val1, val2, val3;
    assert(table.search(5, val1) && val1 == "JPMC Cantt");
    assert(table.search(10, val2) && val2 == "Aga Khan Hospital");
    assert(table.search(15, val3) && val3 == "Indus Hospital Korangi");

    printPass("Separate chaining resolved collisions; all 3 items retrieved accurately.");
}

// TEST 10: Incident history. Events can be traversed in both directions.
void test10_IncidentHistory() {
    printTestHeader("TEST 10: Doubly Linked List Forward & Backward Traversal");
    DoublyLinkedList<IncidentEvent> list;

    list.append(IncidentEvent(1, "Karachi Grid Initialized", 101, "INIT", "T+00:00"));
    list.append(IncidentEvent(2, "Failure on Line 25 to South City Hospital", 102, "FAILURE", "T+00:15"));
    list.append(IncidentEvent(3, "Crew Dispatched on Shahrah-e-Faisal / Korangi", 103, "REPAIR", "T+00:30"));

    assert(list.size() == 3);
    const IncidentEvent* latest = list.getLatest();
    assert(latest != nullptr);
    assert(latest->eventId == 3);
    assert(list.removeFirst());
    assert(list.removeLast());
    assert(list.size() == 1);

    printPass("Doubly Linked List correctly maintained bidirectional node pointers.");
}

// TEST 11: Circular array queue preserves FIFO order through index wraparound.
void test11_CircularQueueWraparound() {
    printTestHeader("TEST 11: Circular Queue FIFO & Wraparound");
    CircularQueue<int> queue(3);
    queue.enqueue(10); queue.enqueue(20); queue.enqueue(30);
    assert(queue.isFull());
    assert(queue.dequeue() == 10);
    queue.enqueue(40); // writes into the slot freed at the beginning
    assert(queue.dequeue() == 20);
    assert(queue.dequeue() == 30);
    assert(queue.dequeue() == 40);
    assert(queue.isEmpty());
    printPass("Circular queue maintained FIFO order across rear/front wraparound.");
}

// TEST 12: Array stack is LIFO and expands safely when its initial capacity fills.
void test12_ArrayStackAndUndo() {
    printTestHeader("TEST 12: Array Stack LIFO & Dynamic Growth");
    ArrayStack<int> stack(2);
    stack.push(1); stack.push(2); stack.push(3);
    assert(stack.capacity() >= 3);
    assert(stack.pop() == 3 && stack.peek() == 2 && stack.pop() == 2 && stack.pop() == 1);
    assert(stack.isEmpty());
    Simulation sim;
    sim.simulatePowerLineFailure(48, 2, 30);
    assert(sim.undoLastOperation());
    assert(sim.getPowerGrid().isPowerLineOperational(48));
    printPass("Array stack provides LIFO operations and backs simulation undo.");
}

static bool isDescendingBySeverity(const Fault values[], int count) {
    for (int i = 1; i < count; ++i) if (values[i - 1].severity < values[i].severity) return false;
    return true;
}

// TEST 13: Every required manual sort and the manual searches return expected results.
void test13_SortingAndSearching() {
    printTestHeader("TEST 13: Manual Sorting & Searching");
    Fault original[4] = { Fault(104, 4, 2, 10, 0, 10), Fault(101, 1, 5, 10, 0, 10),
                          Fault(103, 3, 1, 10, 0, 10), Fault(102, 2, 4, 10, 0, 10) };
    Fault bubble[4], selection[4], insertion[4], quick[4], merge[4];
    for (int i = 0; i < 4; ++i) bubble[i] = selection[i] = insertion[i] = quick[i] = merge[i] = original[i];
    Sorting::bubbleSort(bubble, 4, SORT_BY_SEVERITY);
    Sorting::selectionSort(selection, 4, SORT_BY_SEVERITY);
    Sorting::insertionSort(insertion, 4, SORT_BY_SEVERITY);
    Sorting::quickSort(quick, 0, 3, SORT_BY_SEVERITY);
    Sorting::mergeSort(merge, 0, 3, SORT_BY_SEVERITY);
    assert(isDescendingBySeverity(bubble, 4) && isDescendingBySeverity(selection, 4));
    assert(isDescendingBySeverity(insertion, 4) && isDescendingBySeverity(quick, 4) && isDescendingBySeverity(merge, 4));
    // Insertion-sort a copy by ID before binary search, fulfilling its sorted-input prerequisite.
    for (int i = 1; i < 4; ++i) { Fault key = original[i]; int j = i - 1; while (j >= 0 && original[j].faultId > key.faultId) { original[j + 1] = original[j]; --j; } original[j + 1] = key; }
    assert(SearchAlgorithms::linearSearchFaultById(original, 4, 103) >= 0);
    assert(SearchAlgorithms::binarySearchFaultById(original, 4, 103) >= 0);
    printPass("All five manual sorts and both manual searches produced correct results.");
}

// TEST 14: Deleting a chained hash entry preserves the other colliding entries.
void test14_HashDelete() {
    printTestHeader("TEST 14: Hash Table Delete in Collision Chain");
    HashTable<int, int> table(5);
    table.insert(5, 50); table.insert(10, 100); table.insert(15, 150);
    assert(table.remove(10));
    int value = 0;
    assert(!table.search(10, value));
    assert(table.search(5, value) && value == 50);
    assert(table.search(15, value) && value == 150);
    printPass("Hash-table deletion repaired a collision chain without losing neighbours.");
}

int main() {
    std::cout << "\n========================================================================\n";
    std::cout << "       GRIDGUARD KARACHI ACADEMIC VERIFICATION TEST HARNESS             \n";
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
    test11_CircularQueueWraparound();
    test12_ArrayStackAndUndo();
    test13_SortingAndSearching();
    test14_HashDelete();

    std::cout << "\n========================================================================\n";
    std::cout << "   ALL 14 KARACHI VERIFICATION TESTS PASSED SUCCESSFULLY! (14/14)       \n";
    std::cout << "========================================================================\n\n";

    return 0;
}
