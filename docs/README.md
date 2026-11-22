# GridGuard: Intelligent Power Grid Fault Detection & Restoration Simulator

**GridGuard** is an academic-grade, console-based simulation system developed in modern C++ to model power distribution networks, simulate power line failures, dynamically analyze grid connectivity, prioritize emergency repair operations using max-heaps, calculate optimal repair crew transit routes via Dijkstra's algorithm, and maintain reversible operational state via stack-based undo mechanisms.

This project was built from scratch to demonstrate fundamental **Data Structures and Algorithms (DSA)** without relying on built-in standard template library (STL) containers for core algorithmic operations.

---

## 1. Problem Overview & Simulation Domain

Modern municipal electricity grids rely on interconnected distribution networks featuring:
- **Generation Sources:** Main central power generation stations.
- **Substations:** High-voltage stepping and distribution hubs.
- **Critical Facilities:** Trauma centers, general hospitals, and emergency services requiring uninterrupted electricity.
- **Civic Infrastructure:** Universities, transit networks, and commercial districts.
- **Residential Zones:** High-density population neighborhoods.

When severe weather, mechanical breakdown, or physical damage severs transmission lines, grid operators must instantly:
1. Detect isolated and de-energized sectors across complex looped and radial topologies.
2. Quantify social and civic harm (population affected, offline hospitals).
3. Prioritize emergency repair work orders according to objective triage scoring formulas.
4. Dispatch repair crews along optimal ground transit routes that remain accessible even when power lines fail.
5. Re-energize the network systematically and maintain a reversible, auditable operational ledger.

---

## 2. Technical Stack & Design Highlights

- **Programming Language:** C++ (C++11 / C++14 compliant)
- **Compiler:** MinGW GCC / Clang / MSVC
- **Architecture:** Modular Object-Oriented Architecture (Separation of Concerns: Data structures, Domain entities, Algorithms, Simulation coordinator, Console UI)
- **Zero-STL Container Policy:** Every foundational data structure (Adjacency List Graph, FIFO Queue, LIFO Stack, Binary Max-Heap, Separate-Chaining Hash Table, Doubly Linked List, and Dynamic Array) is manually engineered with raw pointers, dynamic memory allocation, and explicit rule-of-three memory management.

---

## 3. Custom Data Structures Summary

| Data Structure | Implementation Details | Role in GridGuard | Primary Operations |
| :--- | :--- | :--- | :--- |
| **Graph** | Manual Adjacency List with `AdjEdgeNode*` | Models power distribution topology & road transit network | `addLocation`, `addPowerLine`, `disablePowerLine`, `restorePowerLine`, `analyzeConnectivity` |
| **Queue** | Singly Linked List FIFO with `front` & `rear` pointers | Powers Breadth-First Search (BFS) to identify de-energized grid sectors | `enqueue`, `dequeue`, `peek`, `isEmpty`, `size` |
| **Priority Queue (Max-Heap)** | Dynamically-resized binary heap with `heapifyUp` / `heapifyDown` | Schedules and triages emergency repair work orders by priority score | `insert`, `extractMax`, `peekMax`, `heapifyUp`, `heapifyDown`, `displayQueue` |
| **Hash Table** | Separate Chaining with linked-list bucket collision resolution | Instant $O(1)$ average-time lookup for locations and incident fault records | `insert`, `search`, `update`, `remove`, `getCollisionCount` |
| **Doubly Linked List** | Bidirectional node links (`prev` & `next` pointers) | Chronological and reverse-chronological incident history logging | `append`, `displayForward`, `displayBackward`, `searchEvents`, `getLatest` |
| **Stack** | Singly Linked List LIFO with `top` pointer | Enables undoing reversible state-changing actions (failures & repairs) | `push`, `pop`, `peek`, `isEmpty`, `displayHistory` |

---

## 4. Algorithms Implemented

1. **Breadth-First Search (BFS):** Explores operational transmission lines starting at the Main Power Station to partition the city into powered versus disconnected sectors. Uses the custom `Queue`.
2. **Depth-First Search (DFS):** Traverses the network recursively to inspect topological reachability and display traversal ordering.
3. **Dijkstra’s Shortest Path Algorithm:** Operates over an independent ground road network with positive travel weights to find the fastest dispatch path for repair crews.
4. **Max-Heap Priority Scheduling:** Prioritizes repairs according to:
   $$\text{Priority Score} = (\text{Severity} \times 10) + (\text{Critical Facilities} \times 20) + \left(\frac{\text{Population}}{100}\right) + (\text{Outage Duration} \times 2)$$
5. **QuickSort & MergeSort:** Enables operators to sort fault ledgers across multiple criteria (Priority, Severity, Population, Outage Duration).
6. **Separate-Chaining Hashing:** Employs polynomial rolling hash (djb2) and modulo arithmetic with dynamic linked-list bucket chains.

---

## 5. Directory Structure

```
dsa-project/
├── include/
│   ├── CustomVector.h      # Custom dynamic array template
│   ├── DoublyLinkedList.h  # Doubly linked list for incident logging
│   ├── Fault.h             # Fault data entity & priority formula
│   ├── Graph.h             # Power grid graph (manual adjacency list)
│   ├── HashTable.h         # Separate-chaining hash table template
│   ├── IncidentEvent.h     # Simulation log event structure
│   ├── Location.h          # City facility location representation
│   ├── MaxHeap.h           # Binary max-heap priority queue
│   ├── PowerLine.h         # Transmission line edge representation
│   ├── Queue.h             # Linked-list FIFO queue
│   ├── RoadNetwork.h       # Independent road graph for Dijkstra
│   ├── Simulation.h        # Central simulation engine and coordinator
│   ├── Sorting.h           # Manual QuickSort and MergeSort algorithms
│   └── Stack.h             # Linked-list LIFO undo stack
├── src/
│   ├── Graph.cpp           # Graph methods, BFS, DFS, connectivity
│   ├── RoadNetwork.cpp     # Road network methods and Dijkstra
│   ├── Simulation.cpp      # Workflows, incident logging, undo dispatch
│   └── Sorting.cpp         # QuickSort and MergeSort implementation
├── tests/
│   └── test_suite.cpp      # Automated verification suite (10 test scenarios)
├── docs/
│   ├── README.md           # System documentation
│   ├── DSA_Explanation.md  # In-depth DSA technical breakdown
│   └── Test_Cases.md       # Test scenarios and validation results
├── bin/                    # Compiled binary output directory
├── main.cpp                # Interactive console UI entry point
└── Makefile                # Build automation script
```

---

## 6. How to Compile and Run

### Prerequisites
A C++ compiler supporting C++11 or higher (such as `g++` from MinGW, Clang, or MSVC).

### Compilation Commands

#### 1. Compile the Interactive GridGuard Simulator:
```powershell
g++ -std=c++14 -O2 -Iinclude main.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/GridGuard.exe
```

#### 2. Compile the Automated Academic Verification Suite:
```powershell
g++ -std=c++14 -O2 -Iinclude tests/test_suite.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/test_suite.exe
```

### Running the Programs

#### Launch Interactive Simulator:
```powershell
./bin/GridGuard.exe
```

#### Run Automated Test Harness:
```powershell
./bin/test_suite.exe
```

---

## 7. Interactive Menu Options

```
========================================================================================
                                     GRIDGUARD                                          
             Intelligent Power Grid Fault Detection & Restoration Simulator             
========================================================================================
 1. Display Power Network                  11. Search Fault Record by ID
 2. View All Locations                     12. Sort Fault Records
 3. Simulate Power Line Failure            13. Display Incident History (Chronological)
 4. View Affected Locations (Outages)      14. View Incident History Backward (Reverse)
 5. View Critical Facility Outages         15. Undo Last Operation
 6. View Repair Priority Queue (Max-Heap)  16. Run BFS Traversal (Custom Queue)
 7. Process Next Repair (Dijkstra + Fix)   17. Run DFS Traversal (Recursive)
 8. Find Repair Crew Route (Dijkstra)      18. View Network Connectivity Status
 9. View All Fault Records                 19. Restore a Power Line Manually
10. Search Location by ID (Hash Table)     20. Exit Simulator
----------------------------------------------------------------------------------------
21. [SPECIAL] Run Automated Test Suite (All 10 Academic Validation Tests)
22. [SPECIAL] Display Hash Table Collision & Bucket Diagnostics
23. [SPECIAL] Display Undo Stack Contents
========================================================================================
```

---

## 8. Simulation Workflow Walkthrough

### Simulating a Transmission Line Failure
1. The operator selects a power line (e.g., Line #14 to St. Jude Trauma Center).
2. The line is flagged as failed (`isOperational = false`).
3. An undo record is pushed onto the custom LIFO stack.
4. Breadth-First Search (BFS) is executed from the Main Power Station via the custom Queue.
5. Reachable vs. isolated locations are partitioned; affected population and critical facilities are summed.
6. The fault's priority score is calculated.
7. The fault is enqueued into the Binary Max-Heap and cached in the Hash Table.
8. A failure event is appended to the Doubly Linked List incident log.
9. A comprehensive outage report is printed to the console.

### Processing a Repair Work Order
1. The highest-priority fault is extracted from the root of the Max-Heap.
2. Dijkstra's algorithm computes the shortest ground transit route on the Road Network from the crew depot to the destination node.
3. The transmission line is restored in the Graph.
4. BFS is re-run to confirm newly re-energized locations.
5. The fault status is updated to `REPAIRED` in the Hash Table.
6. Restoration events are logged in the Doubly Linked List.
7. An undo action (`ACTION_LINE_RESTORED`) is pushed to the stack to enable reversible operations.
