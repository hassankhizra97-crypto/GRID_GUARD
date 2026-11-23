# GridGuard: Intelligent Karachi Power Grid Fault Detection & Restoration Simulator

[![C++](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=flat&logo=c%2B%2B)](https://en.cppreference.com/)
[![React](https://img.shields.io/badge/Frontend-React%2018%20%7C%20TypeScript-61DAFB?style=flat&logo=react)](https://reactjs.org/)
[![Vite](https://img.shields.io/badge/Bundler-Vite%205-646CFF?style=flat&logo=vite)](https://vitejs.dev/)
[![TailwindCSS](https://img.shields.io/badge/UI-Tailwind%20CSS-38B2AC?style=flat&logo=tailwind-css)](https://tailwindcss.com/)
[![Leaflet](https://img.shields.io/badge/Mapping-Leaflet%20%7C%20OpenStreetMap-199900?style=flat&logo=leaflet)](https://leafletjs.com/)
[![Tests](https://img.shields.io/badge/Tests-14%2F14%20Passing-success)](tests/)

**GridGuard** is an academic-grade, full-stack power grid simulation platform engineered in **modern C++** and **TypeScript/React**. It models the municipal electricity distribution and road transit infrastructure of **Karachi, Pakistan**, across 44 verified geographical locations and 61 transmission lines.

The platform simulates power line tripping and physical faults, dynamically analyzes grid connectivity using a **custom Breadth-First Search (BFS)**, prioritizes emergency restoration work orders via a **custom binary Max-Heap**, calculates shortest transit routes for repair crews across Karachi's arterial corridors via **Dijkstra's algorithm**, and supports reversible simulation operations using **custom LIFO stacks**.

---

## 1. Key Features

- **Geographical Karachi Topology:** Real coordinates for 44 municipal locations:
  - 4 Supply Hubs (PECHS, Manghopir, Bin Qasim, Korangi Creek)
  - 4 Major Substations (Saddar, Gulshan, Clifton, Federal B)
  - 8 Major Hospitals (JPMC, Aga Khan Hospital, Indus Hospital, Civil Hospital, Liaquat National, South City, SIUT, Abbasi Shaheed)
  - 7 Universities (FAST-NUCES, NED, UoK, IBA, Habib University, DUHS Ojha, Bahria University)
  - 14 Residential Neighborhoods (Gulshan-e-Iqbal, DHA, Clifton, North Nazimabad, PECHS, Malir, etc.)
  - 7 Commercial and Industrial Hubs (S.I.T.E., Korangi Industrial, Port Qasim, Tariq Road, etc.)
- **Interactive Dark Map UI:** Leaflet-powered dark-theme Karachi map showing live line operational statuses, pulsing offline nodes, interactive fault triggering, and inspection modal popups.
- **Zero-STL Algorithmic Core:** Every foundational data structure is manually implemented using raw pointers, dynamic arrays, and linked nodes — no `std::vector`, `std::queue`, `std::stack`, `std::map`, `std::priority_queue`, or `std::sort` in core algorithmic paths.
- **Dynamic Outage Impact Analysis:** Quantifies direct civilian impact (affected population count) and flags critical facilities (hospitals, universities) isolated from supply hubs.
- **Objective Repair Triage Scoring:** Calculates triage priority scores incorporating severity, estimated duration, isolated population, and facility criticality multipliers.
- **Emergency Dispatch Navigation:** Evaluates optimal crew routing from dispatch depots to fault locations using Dijkstra's shortest-path algorithm across the Karachi road network.
- **Reversible Simulation Ledger:** LIFO operation undo stack paired with a bidirectional Doubly Linked List incident history ledger.
- **Dual Execution Architecture:** Seamlessly operates as a standalone C++ terminal application or as a full-stack platform with a live C++ Winsock REST API backend and React dashboard.

---

## 2. Custom Data Structures & Algorithms

Every core DSA component was built manually for university laboratory compliance:

| Data Structure | Implementation Details | Role in GridGuard | Complexity |
|:---|:---|:---|:---:|
| **Graph** | Adjacency List (`AdjEdgeNode*`) | Models electrical transmission lines & road corridors | $O(V + E)$ |
| **Queue** | Singly Linked List (`front`, `rear`) | Powers Breadth-First Search (BFS) reachability | $O(1)$ ops |
| **Circular Queue** | Fixed/dynamic array with modulo wrap | Array-based FIFO alternative for task pipelines | $O(1)$ ops |
| **Max-Heap** | Binary Heap (`heapifyUp`, `heapifyDown`) | Priority Queue for emergency repair triage | $O(\log n)$ |
| **Hash Table** | Separate Chaining (bucket linked lists) | $O(1)$ average lookup for locations and faults | $O(1)$ avg |
| **Doubly Linked List** | Bidirectional (`prev`, `next`) | Chronological and reverse incident event ledger | $O(1)$ insert |
| **Stack** | Singly Linked List LIFO | Enables undoing state-changing actions | $O(1)$ ops |
| **Array Stack** | Dynamic array with $2\times$ resizing | Contiguous LIFO stack with minimal pointer overhead | $O(1)$ amortized |
| **Custom Vector** | Geometric doubling dynamic array | Replaces `std::vector` for dynamic storage | $O(1)$ amortized |

### Algorithms
- **Breadth-First Search (BFS):** Evaluates grid connectivity starting from the Central Supply Hub to partition Karachi into powered vs. isolated sectors.
- **Dijkstra's Shortest Path:** Computes quickest travel times across the Karachi arterial road network for repair crew dispatch.
- **Sorting Algorithms:** QuickSort, MergeSort, BubbleSort, SelectionSort, and InsertionSort for customizable work order ordering.
- **Search Algorithms:** LinearSearch and BinarySearch for rapid entity lookups.

---

## 3. Technology Stack

### Backend
- **Language:** C++17
- **Networking:** Winsock2 (`ws2_32`) for HTTP REST server
- **Architecture:** Object-Oriented Simulation Engine with custom memory management
- **Build Systems:** MinGW GCC / Clang / MSVC, GNU Make, CMake

### Frontend
- **Framework:** React 18 with TypeScript
- **Tooling:** Vite 5
- **Styling:** Tailwind CSS (custom dark electricity operations palette)
- **Mapping:** Leaflet & React-Leaflet with OpenStreetMap tiles and custom dark filters
- **Data Visualization:** Recharts (pie/donut distribution and activity bar charts)
- **Icons:** Lucide React

---

## 4. Architecture Overview

```
                      +----------------------------------+
                      |         Web Browser              |
                      |   http://localhost:5173 (Dev)    |
                      |   http://localhost:8081 (Prod)   |
                      +-----------------+----------------+
                                        |
                 +----------------------+----------------------+
                 | (Dev Mode Proxy)                            | (Production Direct)
                 v                                             v
      +---------------------+                       +---------------------+
      |   Vite Dev Server   |                       |   C++ HTTP Server   |
      |   localhost:5173    |                       |   localhost:8081    |
      |   /api -> Proxy ----+---------------------> |  (GridGuardServer)  |
      +---------------------+                       +----------+----------+
                                                               |
                                        +----------------------+----------------------+
                                        |                      |                      |
                                        v                      v                      v
                                 +--------------+       +--------------+       +--------------+
                                 |  Graph (BFS) |       |  Max-Heap    |       |   Dijkstra   |
                                 | Connectivity |       | Triage Queue |       | Road Routing |
                                 +--------------+       +--------------+       +--------------+
```

---

## 5. Project Structure

```
gridguard/
├── bin/                       # Output directory for compiled binaries (git-ignored)
├── docs/                      # Architectural & verification documentation
│   ├── DSA_Explanation.md     # Detailed architectural overview of manual DSA
│   ├── MANUAL_DSA_IMPLEMENTATIONS.md # Reference manual for custom data structures
│   └── Test_Cases.md          # Formal verification test specifications
├── frontend/                  # React + TypeScript dashboard
│   ├── src/
│   │   ├── components/        # KarachiMap, Sidebar, Header, Modals, Panels
│   │   ├── pages/             # Overview, PowerNetwork, Faults, Repairs, History
│   │   ├── services/          # REST API connector & synchronized state mirror
│   │   ├── data/              # 44 Karachi locations, 61 lines, road corridors
│   │   └── types/             # TypeScript domain definitions
│   ├── index.html
│   ├── package.json
│   ├── tailwind.config.js
│   └── vite.config.ts
├── include/                   # C++ Header files (custom DSA classes & interfaces)
├── src/                       # C++ Source implementations
│   ├── Graph.cpp              # Adjacency list & BFS
│   ├── RoadNetwork.cpp        # Dijkstra road routing
│   ├── Simulation.cpp         # Main simulation & triage orchestrator
│   ├── Sorting.cpp            # 5 manual sorting algorithms
│   ├── GridGuardServer.cpp    # HTTP REST & static file server
│   └── server_main.cpp        # Backend server entry point
├── tests/
│   └── test_suite.cpp         # 14-test verification harness
├── CMakeLists.txt             # CMake configuration
├── Makefile                   # GNU Make build targets
├── HOW_TO_RUN.md              # Detailed execution guide
├── start-dev.bat              # One-click launcher (backend + frontend dev)
├── build-production.bat       # Full production compiler & bundler
└── main.cpp                   # Interactive console simulation entry point
```

---

## 6. Prerequisites

Ensure the following tools are installed and accessible in your system `PATH`:

| Tool | Recommended Version | Download Link |
|:---|:---|:---|
| **g++ / MinGW-w64** | GCC 9.0+ (with C++17 support) | [MinGW-w64](https://www.mingw-w64.org/) / [MSYS2](https://www.msys2.org/) |
| **Node.js** | v18.0+ | [NodeJS.org](https://nodejs.org/) |
| **npm** | v9.0+ (bundled with Node.js) | — |

---

## 7. How to Run

### Option A — One-Click Development Mode (Recommended)

Double-click `start-dev.bat` or run:

```bat
start-dev.bat
```

This script will:
1. Compile the C++ backend server into `bin/GridGuardServer.exe`.
2. Install frontend dependencies automatically if not present.
3. Launch the C++ backend on `http://localhost:8081`.
4. Launch the Vite development server on `http://localhost:5173`.

Open your browser at **`http://localhost:5173`**.

---

### Option B — Manual Step-by-Step (Two Terminals)

**Terminal 1 (Backend):**
```powershell
# Compile and run the C++ server
g++ -std=c++17 -O2 -Wall -Iinclude src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp src/GridGuardServer.cpp src/server_main.cpp -o bin/GridGuardServer.exe -lws2_32
bin\GridGuardServer.exe
```

**Terminal 2 (Frontend):**
```powershell
cd frontend
npm install
npm run dev
```

Navigate to **`http://localhost:5173`**.

---

### Option C — Production Single-Port Deployment

To compile the C++ binary and bundle the React frontend into static assets served on a single port (`8081`):

```bat
build-production.bat
bin\GridGuardServer.exe
```

Open **`http://localhost:8081`**. The C++ server will serve both the React web application and all `/api/*` endpoints.

---

### Option D — Standalone C++ Terminal Simulation

To run the interactive CLI version without the web dashboard:

```powershell
make console
# OR compile manually:
g++ -std=c++17 -O2 -Wall -Iinclude main.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/GridGuard.exe
bin\GridGuard.exe
```

---

## 8. Verification & Tests

GridGuard includes an automated verification harness covering all 14 core operational and algorithmic requirements:

```powershell
make test
# OR compile and run manually:
g++ -std=c++17 -O2 -Wall -Iinclude tests/test_suite.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/test_suite.exe
bin\test_suite.exe
```

### Verified Test Cases (14/14 Passing):
1. **Karachi Baseline:** All 44 locations connected and energized.
2. **Non-Critical Outage:** Line #48 trip accurately isolates Malir.
3. **Grid Redundancy:** Aga Khan Hospital remains powered via redundant feed after primary failure.
4. **Critical Facility Isolation:** South City Hospital outage elevates triage priority.
5. **Multi-Fault Heap Priority:** Binary Max-Heap extracts highest priority work orders first.
6. **Restoration Verification:** Re-energizing power lines recovers BFS reachability.
7. **Stack Undo Operation:** Reverses line failures and restores pre-incident state.
8. **Dijkstra Route Navigation:** Computes optimal travel time along Karachi road corridors.
9. **Hash Table Collision Handling:** Separate chaining resolves collisions with $O(1)$ average retrieval.
10. **Doubly Linked List Traversal:** Bidirectional inspection of chronological incident history.
11. **Circular Queue Wraparound:** Array-based circular queue verifies FIFO order across boundary wrap.
12. **Array Stack Operations:** Dynamic contiguous LIFO stack executes push, pop, and auto-doubling.
13. **Manual Sorting & Searching:** Verifies 5 sorting algorithms and binary/linear search.
14. **Hash Table Node Deletion:** Validates node unlinking in collision chains without orphan creation.

---

## 9. API Reference

The C++ backend exposes the following RESTful endpoints on port `8081`:

| Method | Endpoint | Description |
|:---:|:---|:---|
| `GET` | `/api/health` | Service health status check |
| `GET` | `/api/network-state` | Full grid state (locations, lines, faults, incident history, stats) |
| `POST` | `/api/simulate-failure` | Injects line failure (`{"lineId": 5, "severity": 3, "durationMinutes": 60}`) |
| `POST` | `/api/process-repair` | Dispatches highest priority repair from Max-Heap (`{"faultId": -1}`) |
| `POST` | `/api/undo` | Pops operation stack and reverses most recent reversible action |

---

## 10. License

This project was developed for educational and academic demonstration purposes.
