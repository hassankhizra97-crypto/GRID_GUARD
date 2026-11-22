# GridGuard Karachi — Academic Test Cases & Verification Suite

This document outlines the formal test cases, execution steps, expected outcomes, and actual verification results for **GridGuard: Intelligent Power Grid Fault Detection & Restoration Simulator (Karachi Edition)**.

All 10 test cases are programmatically implemented in `tests/test_suite.cpp` and can be verified via `./bin/test_suite.exe`.

---

## Test Scenario Matrix

| Test ID | Scenario Description | Primary Data Structure Tested | Core Algorithm Tested | Status |
| :---: | :--- | :--- | :--- | :---: |
| **TEST 1** | Karachi Baseline Normal Grid Operation (44 Locations) | Graph, Adjacency List | Breadth-First Search (BFS) | **PASS** |
| **TEST 2** | Non-Critical Power Line Failure (Malir Outage) | Graph, Queue | BFS Outage Isolation | **PASS** |
| **TEST 3** | Redundant Transmission Line Failure (Aga Khan Hospital Dual Feed) | Graph, Custom Vector | Alternate Flow Reachability | **PASS** |
| **TEST 4** | Critical Hospital Outage & Priority Elevation (South City Hospital) | Max-Heap, Hash Table | Priority Score Calculation | **PASS** |
| **TEST 5** | Multiple Simultaneous Faults & Heap Order | Binary Max-Heap | HeapifyUp / Heap Order | **PASS** |
| **TEST 6** | End-to-End Repair & Re-energization (Malir Restored) | Graph, Max-Heap, Queue | Heap ExtractMax + BFS | **PASS** |
| **TEST 7** | Undo Mechanism via LIFO Stack | Stack (LIFO) | Reversible State Machine | **PASS** |
| **TEST 8** | Dijkstra Shortest Route Calculation (Central Hub to Indus Hospital) | Road Network Graph | Dijkstra Shortest Path | **PASS** |
| **TEST 9** | Hash Table Separate Chaining & Collisions | Hash Table (Separate Chaining) | Modular Hashing & Traversal | **PASS** |
| **TEST 10** | Incident History Bidirectional Traversal | Doubly Linked List | Forward & Backward Traversal | **PASS** |

---

## Detailed Test Case Specifications

### TEST 1: Baseline Normal Grid Operation
- **Objective:** Verify that in the unperturbed initial state, all 44 Karachi municipal locations are connected to the Main Generation Station (Central Hub) and receive power.
- **Initial State:** 44 locations registered across Karachi, 61 power lines operational.
- **Execution:**
  1. Call `analyzeConnectivity(mainPowerStationId, connected, disconnected)`.
- **Expected Outcome:** `connected.size() == 44`, `disconnected.size() == 0`. Every location has `isPowered == true`.
- **Result:** **PASSED**. Complete grid connectivity verified.

---

### TEST 2: Non-Critical Power Line Failure (Malir Outage)
- **Objective:** Verify that disabling a non-redundant branch isolates only the dependent sub-network.
- **Action:** Simulate failure on Line #48 (East Hub [3] <---> Malir [31]).
- **Parameters:** Severity = 2, Duration = 45 mins.
- **Expected Outcome:**
  - Malir [31] transitions to `isPowered = false`.
  - Disconnected locations: exactly 1 (ID 31).
  - Unaffected locations: 43.
  - Population impacted: 9,000 residents on local sub-feeder.
  - Critical facilities offline: 0.
- **Result:** **PASSED**. Location 31 correctly partitioned and flagged offline.

---

### TEST 3: Redundant Transmission Line Failure (Aga Khan University Hospital)
- **Objective:** Demonstrate that loops in the graph maintain electrical service through alternative paths when a redundant line breaks.
- **Action:** Simulate failure on Line #17 (Central Hub [1] <---> Aga Khan University Hospital [10]).
- **Network Context:** Aga Khan University Hospital also receives power from Gulshan Distribution Substation [6] via Line #18.
- **Expected Outcome:**
  - Aga Khan Hospital [10] remains `isPowered == true` via Line #18.
  - Total disconnected locations: 0.
  - Outage report acknowledges grid redundancy.
- **Result:** **PASSED**. Zero locations disconnected; redundancy successfully demonstrated.

---

### TEST 4: Critical Hospital Outage & Priority Elevation (South City Hospital)
- **Objective:** Validate that a non-redundant line failure cutting power to a trauma center triggers critical facility detection and boosts the triage priority score.
- **Action:** Simulate failure on Line #25 (South Hub [4] <---> South City Hospital [14]).
- **Parameters:** Severity = 5, Duration = 120 mins.
- **Scoring Formula:**
  $$\text{Priority Score} = (5 \times 10) + (1 \times 20) + (1400 / 100) + (120 \times 2) = 50 + 20 + 14.0 + 240 = 324.0$$
- **Expected Outcome:**
  - South City Hospital [14] marked offline.
  - Critical facilities offline: 1.
  - Priority score $\ge 320.0$.
  - Enqueued at root of Max-Heap.
- **Result:** **PASSED**. Criticality weighting verified.

---

### TEST 5: Multiple Simultaneous Faults & Heap Order
- **Objective:** Test simultaneous registration of three distinct faults across Karachi and confirm the Max-Heap keeps the highest-impact emergency at the root.
- **Actions:**
  1. Fail Line #48 (Malir residential): Severity 1, Duration 30 mins $\rightarrow$ Score: $10 + 0 + 90 + 60 = 160.0$.
  2. Fail Line #25 (South City Hospital): Severity 5, Duration 120 mins $\rightarrow$ Score: $50 + 20 + 14 + 240 = 324.0$.
  3. Fail Line #29 (FAST-NUCES university): Severity 2, Duration 30 mins $\rightarrow$ Score: $20 + 0 + 80 + 60 = 160.0$.
- **Expected Outcome:**
  - Repair queue contains 3 faults.
  - `repairQueue.peekMax().powerLineId == 25` (South City Hospital line #25 sits at the root with Score 324.0).
- **Result:** **PASSED**. Heap order verified under concurrent emergency conditions.

---

### TEST 6: End-to-End Repair Operation
- **Objective:** Test the complete repair procedure, including heap extraction, Dijkstra transit calculation, line restoration, post-repair BFS, and incident logging.
- **Action:** Execute `processNextRepair()` on the simulated Line #48 failure.
- **Expected Outcome:**
  - Fault extracted from Max-Heap.
  - Line #48 restored to `isOperational = true`.
  - BFS confirms Malir [31] re-energized.
  - Fault status updated to `REPAIRED` in Hash Table.
  - Restoration event recorded in Doubly Linked List.
- **Result:** **PASSED**. Complete repair pipeline validated.

---

### TEST 7: Undo Operation via Stack
- **Objective:** Test the LIFO undo stack by simulating a line failure and subsequently executing `undoLastOperation()`.
- **Action:** Fail Line #48, verify stack depth = 1, then execute `undoLastOperation()`.
- **Expected Outcome:**
  - Operation popped from stack.
  - Line #48 operational state restored to `true`.
  - Malir [31] re-energized.
  - Stack depth returned to 0.
- **Result:** **PASSED**. Reversible state verified.

---

### TEST 8: Dijkstra Route Calculation (Karachi Arteries)
- **Objective:** Verify shortest transit route calculation for emergency repair crews over the Karachi road network graph.
- **Action:** Calculate optimal path from Central Supply Hub [1] to Indus Hospital Korangi [11].
- **Expected Outcome:**
  - Shortest path found connecting vertex 1 to vertex 11.
  - Traverses real transit corridors (e.g. via Shahrah-e-Faisal and Korangi Road).
  - Total travel cost $> 0.0$ and strictly minimal.
- **Result:** **PASSED**. Shortest path and non-negative edge relaxation verified.

---

### TEST 9: Hash Table Collision Handling (Separate Chaining)
- **Objective:** Demonstrate robust collision resolution via separate chaining when multiple keys map to the same bucket index.
- **Action:** Insert keys with identical hash indices into a table with small capacity.
- **Expected Outcome:**
  - Collision counter increments properly.
  - All keys remain fully retrievable in $O(L)$ bucket search time.
- **Result:** **PASSED**. Separate chaining collision resolution verified.

---

### TEST 10: Doubly Linked List Traversal
- **Objective:** Verify bidirectional iteration across incident history records.
- **Action:** Append events to the list and verify forward and reverse access.
- **Expected Outcome:**
  - Forward traversal accesses events chronologically.
  - Reverse traversal accesses events reverse-chronologically.
  - Most recent event (`getLatest()`) retrievable in $O(1)$.
- **Result:** **PASSED**. Bidirectional list integrity verified.
