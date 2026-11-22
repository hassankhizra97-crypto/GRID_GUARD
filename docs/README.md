# GridGuard: Intelligent Karachi Power Grid Fault Detection & Restoration Simulator

**GridGuard (Karachi Edition)** is an academic-grade, simulation system developed in modern C++ and TypeScript/React to model real-world municipal power distribution networks across Karachi, Pakistan. The system simulates power line failures across real geographical coordinates, dynamically analyzes electrical grid connectivity using custom Breadth-First Search, prioritizes emergency repair operations using custom binary Max-Heaps, calculates optimal repair crew transit routes along Karachi road arteries via Dijkstra's algorithm, and maintains a completely reversible operational state using custom LIFO stacks.

This project was built from scratch to demonstrate fundamental **Data Structures and Algorithms (DSA)** without relying on built-in standard template library (STL) containers for core algorithmic operations.

---

## 1. Problem Overview & Karachi Grid Domain

Karachi is a megacity of over 15 million residents with critical infrastructure distributed across diverse municipal zones. GridGuard models the city with **44 verified geographical locations**, **61 simulated power lines**, and an interconnected road network:

- **Simulated Supply Hubs & Substations (8 locations):**
  - Central Supply Hub (Civic Centre), North Hub (Manghopir), East Hub (Bin Qasim), South Hub (Korangi Creek)
  - Saddar, Gulshan, Clifton, and Federal B Distribution Substations
- **Major Hospitals (8 locations):**
  - Jinnah Postgraduate Medical Centre (JPMC), Aga Khan University Hospital, Indus Hospital (Korangi), Civil Hospital Karachi, Liaquat National Hospital, South City Hospital, SIUT, Abbasi Shaheed Hospital
- **Prestigious Universities (7 locations):**
  - FAST-NUCES Karachi, NED University of Engineering & Technology, University of Karachi, IBA Karachi, Habib University, Dow University of Health Sciences (Ojha Campus), Bahria University
- **Residential Neighborhoods (14 sectors):**
  - Gulshan-e-Iqbal, Gulistan-e-Jauhar, North Nazimabad, Federal B Area, Clifton, DHA, Saddar Residential, Malir, Korangi, Landhi, PECHS, Nazimabad, Lyari, Shah Faisal Colony
- **Commercial & Industrial Zones (7 zones):**
  - Saddar Commercial, Tariq Road, Clifton Commercial, Korangi Industrial Area, SITE Industrial Area, Landhi Industrial Area, Port Qasim Industrial Area

When severe weather, line tripping, or physical damage severs transmission lines, grid operators can:
1. Detect isolated and de-energized sectors across complex looped and radial topologies via BFS.
2. Quantify civic impact (affected residents, offline trauma centers).
3. Prioritize emergency repair work orders according to objective triage scoring formulas via a Max-Heap.
4. Dispatch repair crews along optimal ground transit corridors using Dijkstra's shortest path algorithm.
5. Re-energize the network systematically and maintain a reversible, auditable operational ledger via Stacks and Doubly Linked Lists.
6. Visualize everything on an interactive **Geographical Map of Karachi** powered by Leaflet and OpenStreetMap.

---

## 2. Technical Stack & Design Highlights

- **Backend Programming Language:** C++ (C++11 / C++14 compliant)
- **Compiler:** MinGW GCC / Clang / MSVC
- **Frontend Dashboard:** React 18, TypeScript, Tailwind CSS, React Flow, React Leaflet, Lucide Icons, Recharts
- **Zero-STL Container Policy:** Every foundational data structure (Adjacency List Graph, FIFO Queue, LIFO Stack, Binary Max-Heap, Separate-Chaining Hash Table, Doubly Linked List, and Dynamic Array) is manually engineered with raw pointers, dynamic memory allocation, and explicit rule-of-three memory management.
- **Realistic Engineering Distinctions:** Simulated municipal distribution infrastructure is clearly categorized and distinguished from physical utility infrastructure. Feeder-level population calibration ensures realistic prioritization.

---

## 3. Custom Data Structures Summary

| Data Structure | Implementation Details | Role in GridGuard | Primary Operations |
| :--- | :--- | :--- | :--- |
| **Graph** | Manual Adjacency List with `AdjEdgeNode*` | Models Karachi power distribution topology & road transit network | `addLocation`, `addPowerLine`, `disablePowerLine`, `restorePowerLine`, `analyzeConnectivity` |
| **Queue** | Singly Linked List FIFO with `front` & `rear` pointers | Powers Breadth-First Search (BFS) to identify de-energized grid sectors | `enqueue`, `dequeue`, `peek`, `isEmpty`, `size` |
| **Priority Queue (Max-Heap)** | Dynamically-resized binary heap with `heapifyUp` / `heapifyDown` | Schedules and triages emergency repair work orders by priority score | `insert`, `extractMax`, `peekMax`, `heapifyUp`, `heapifyDown`, `displayQueue` |
| **Hash Table** | Separate Chaining with linked-list bucket collision resolution | Instant $O(1)$ average-time lookup for locations and incident fault records | `insert`, `search`, `update`, `remove`, `getCollisionCount` |
| **Doubly Linked List** | Bidirectional node links (`prev` & `next` pointers) | Chronological and reverse-chronological incident history logging | `append`, `displayForward`, `displayBackward`, `searchEvents`, `getLatest` |
| **Stack** | Singly Linked List LIFO with `top` pointer | Enables undoing reversible state-changing actions (failures & repairs) | `push`, `pop`, `peek`, `isEmpty`, `displayHistory` |

---

## 4. Algorithms Implemented

1. **Breadth-First Search (BFS):** Explores operational transmission lines starting at the Central Supply Hub to partition Karachi into powered versus disconnected sectors. Uses the custom `Queue`.
2. **Depth-First Search (DFS):** Traverses the network recursively to inspect topological reachability and display traversal ordering.
3. **Dijkstra’s Shortest Path Algorithm:** Operates over an independent ground road network connecting Karachi's major arteries (Shahrah-e-Faisal, University Road, Korangi Road, M-9, Hub River Road) with positive travel weights to find the fastest dispatch path for repair crews.
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
│   ├── Location.h          # Karachi location representation with lat/lon & area
│   ├── MaxHeap.h           # Binary max-heap priority queue
│   ├── PowerLine.h         # Transmission line edge representation
│   ├── Queue.h             # Linked-list FIFO queue
│   ├── RoadNetwork.h       # Karachi road graph for Dijkstra routing
│   ├── Simulation.h        # Central simulation engine and coordinator
│   ├── Sorting.h           # Manual QuickSort and MergeSort algorithms
│   └── Stack.h             # Linked-list LIFO undo stack
├── src/
│   ├── Graph.cpp           # Graph methods, BFS, DFS, connectivity
│   ├── RoadNetwork.cpp     # Road network methods and Dijkstra
│   ├── Simulation.cpp      # Karachi network initialization (44 nodes, 61 lines)
│   └── Sorting.cpp         # QuickSort and MergeSort implementation
├── tests/
│   └── test_suite.cpp      # Automated verification suite (10 Karachi test scenarios)
├── docs/
│   ├── README.md           # System documentation
│   ├── DSA_Explanation.md  # In-depth DSA technical breakdown
│   └── Test_Cases.md       # Test scenarios and validation results
├── frontend/               # Professional React + Leaflet web dashboard
│   ├── src/
│   │   ├── components/     # KarachiMap, Sidebar, Header, Modals, etc.
│   │   ├── pages/          # OverviewPage, PowerNetworkPage, Faults, Repairs, etc.
│   │   ├── data/           # Synchronized Karachi location and line data
│   │   └── services/       # API layer with Dijkstra coordinates and fault simulation
├── bin/                    # Compiled binary output directory
├── main.cpp                # Interactive console UI entry point
└── Makefile                # Build automation script
```

---

## 6. How to Compile and Run

### C++ Console Application & Test Suite

1. **Compile and run the verification test suite:**
   ```bash
   g++ -std=c++14 -O2 -Iinclude tests/test_suite.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/test_suite.exe
   ./bin/test_suite.exe
   ```
   *Expected outcome: All 10 test scenarios pass successfully.*

2. **Compile and run the interactive console simulator:**
   ```bash
   g++ -std=c++14 -O2 -Iinclude main.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/GridGuard.exe
   ./bin/GridGuard.exe
   ```

### Web Dashboard (React + Leaflet)

1. **Launch the development server:**
   ```bash
   cd frontend
   npm install
   npm run dev
   ```
2. Open `http://localhost:5173` in any modern web browser to access the interactive Karachi Map, React Flow topological graph, Max-Heap triage scheduler, and incident ledger.
