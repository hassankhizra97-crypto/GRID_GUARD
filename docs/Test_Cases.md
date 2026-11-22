# GridGuard — Academic Test Cases & Verification Suite

This document outlines the formal test cases, execution steps, expected outcomes, and actual verification results for **GridGuard: Intelligent Power Grid Fault Detection & Restoration Simulator**.

All 10 test cases are programmatically implemented in `tests/test_suite.cpp` and can be verified via `./bin/test_suite.exe`.

---

## Test Scenario Matrix

| Test ID | Scenario Description | Primary Data Structure Tested | Core Algorithm Tested | Status |
| :---: | :--- | :--- | :--- | :---: |
| **TEST 1** | Baseline Normal Grid Operation | Graph, Adjacency List | Breadth-First Search (BFS) | **PASS** |
| **TEST 2** | Non-Critical Power Line Failure | Graph, Queue | BFS Outage Isolation | **PASS** |
| **TEST 3** | Redundant Transmission Line Failure | Graph, Custom Vector | Alternate Flow Reachability | **PASS** |
| **TEST 4** | Critical Hospital Outage & Priority Elevation | Max-Heap, Hash Table | Priority Score Calculation | **PASS** |
| **TEST 5** | Multiple Simultaneous Faults & Heap Order | Binary Max-Heap | HeapifyUp / Heap Order | **PASS** |
| **TEST 6** | End-to-End Repair & Re-energization | Graph, Max-Heap, Queue | Heap ExtractMax + BFS | **PASS** |
| **TEST 7** | Undo Mechanism via LIFO Stack | Stack (LIFO) | Reversible State Machine | **PASS** |
| **TEST 8** | Dijkstra Shortest Route Calculation | Road Network Graph | Dijkstra Shortest Path | **PASS** |
| **TEST 9** | Hash Table Separate Chaining & Collisions | Hash Table (Separate Chaining) | Modular Hashing & Traversal | **PASS** |
| **TEST 10** | Incident History Bidirectional Traversal | Doubly Linked List | Forward & Backward Traversal | **PASS** |

---

## Detailed Test Case Specifications

### TEST 1: Baseline Normal Grid Operation
- **Objective:** Verify that in the unperturbed initial state, all 14 city locations are connected to the Main Generation Station and receive power.
- **Initial State:** 14 locations registered, 18 power lines operational.
- **Execution:**
  1. Call `analyzeConnectivity(mainPowerStationId, connected, disconnected)`.
- **Expected Outcome:** `connected.size() == 14`, `disconnected.size() == 0`. Every location has `isPowered == true`.
- **Result:** **PASSED**. Complete grid connectivity verified.

---

### TEST 2: Non-Critical Power Line Failure
- **Objective:** Verify that disabling a non-redundant branch isolates only the dependent sub-network.
- **Action:** Simulate failure on Line #17 (South Substation [4] <---> Oakridge Suburb [12]).
- **Parameters:** Severity = 2, Duration = 45 mins.
- **Expected Outcome:**
  - Oakridge Suburb [12] transitions to `isPowered = false`.
  - Disconnected locations: exactly 1 (ID 12).
  - Unaffected locations: 13.
  - Population impacted: 11,000 residents.
  - Critical facilities offline: 0.
- **Result:** **PASSED**. Location 12 correctly partitioned and flagged offline.

---

### TEST 3: Redundant Transmission Line Failure
- **Objective:** Demonstrate that loops in the graph maintain electrical service through alternative paths when a redundant line breaks.
- **Action:** Simulate failure on Line #6 (North Substation [2] <---> City General Hospital [5]).
- **Network Context:** City General Hospital also receives power from Central Substation [3] via Line #7.
- **Expected Outcome:**
  - Hospital [5] remains `isPowered == true` via Line #7.
  - Total disconnected locations: 0.
  - Outage report acknowledges grid redundancy.
- **Result:** **PASSED**. Zero locations disconnected; redundancy successfully demonstrated.

---

### TEST 4: Critical Hospital Outage & Priority Elevation
- **Objective:** Validate that a non-redundant line failure cutting power to a trauma center triggers critical facility detection and boosts the triage priority score.
- **Action:** Simulate failure on Line #14 (South Substation [4] <---> St. Jude Trauma Center [6]).
- **Parameters:** Severity = 5, Duration = 120 mins.
- **Scoring Formula:**
  $$\text{Priority Score} = (5 \times 10) + (1 \times 20) + (850 / 100) + (120 \times 2) = 50 + 20 + 8.5 + 240 = 318.5$$
- **Expected Outcome:**
  - St. Jude Trauma Center [6] marked offline.
  - Critical facilities offline: 1.
  - Priority score $\ge 318.0$.
  - Enqueued at root of Max-Heap.
- **Result:** **PASSED**. Criticality weighting verified.

---

### TEST 5: Multiple Simultaneous Faults & Heap Order
- **Objective:** Test simultaneous registration of three distinct faults and confirm the Max-Heap keeps the highest-impact emergency at the root.
- **Actions:**
  1. Fail Line #17 (Oakridge): Severity 1, Duration 30 mins $\rightarrow$ Score: $10 + 0 + 110 + 60 = 180.0$.
  2. Fail Line #14 (St. Jude Hospital): Severity 5, Duration 120 mins $\rightarrow$ Score: $50 + 20 + 8.5 + 240 = 318.5$.
  3. Fail Line #8 (Metro Tech Univ): Severity 2, Duration 30 mins $\rightarrow$ Score: $20 + 0 + 185 + 60 = 265.0$.
- **Expected Outcome:**
  - Repair queue contains 3 faults.
  - `repairQueue.peekMax().powerLineId == 14` (Hospital fault #14 sits at the root with Score 318.5).
- **Result:** **PASSED**. Heap order verified under concurrent emergency conditions.

---

### TEST 6: End-to-End Repair Operation
- **Objective:** Test the complete 10-step repair procedure, including heap extraction, Dijkstra transit calculation, line restoration, post-repair BFS, and incident logging.
- **Action:** Execute `processNextRepair()` on the simulated Line #17 failure.
- **Expected Outcome:**
  - Fault extracted from Max-Heap.
  - Line #17 restored to `isOperational = true`.
  - BFS confirms Oakridge Suburb [12] re-energized.
  - Fault status updated to `REPAIRED` in Hash Table.
  - Restoration event recorded in Doubly Linked List.
- **Result:** **PASSED**. Grid state and all data structures successfully synchronized.

---

### TEST 7: Undo Mechanism via LIFO Stack
- **Objective:** Confirm that the custom Stack correctly tracks state changes and can roll back an accidental line disruption.
- **Actions:**
  1. Fail Line #17 (Oakridge isolated). Undo stack size becomes 1.
  2. Call `undoLastOperation()`.
- **Expected Outcome:**
  - `undoStack.pop()` retrieves `ACTION_LINE_DISABLED` for Line #17.
  - Line #17 re-enabled in Graph.
  - Location #12 restored to powered state.
  - Event `OPERATION_UNDONE` appended to incident history.
  - Undo stack empty.
- **Result:** **PASSED**. Reversible state machine verified.

---

### TEST 8: Dijkstra Shortest Route Calculation
- **Objective:** Verify that Dijkstra's algorithm computes the minimum travel time across the independent ground road network.
- **Query:** Origin = Main Station Depot [1], Destination = Oakridge Suburb [12].
- **Expected Outcome:**
  - Computes path: $[1] \rightarrow [4] \rightarrow [12]$ (or alternate road link).
  - Total travel time correctly accumulated ($15.0 + 14.0 = 29.0$ mins or optimal road shortcut).
  - Positive travel cost guaranteed.
- **Result:** **PASSED**. Shortest path and travel weights verified.

---

### TEST 9: Hash Table Separate Chaining & Collisions
- **Objective:** Force hash collisions in the custom Hash Table and verify that separate chaining preserves all records.
- **Setup:** Create `HashTable<int, string>` with small capacity $C = 5$.
- **Actions:**
  - Insert key 5 ("Hospital Alpha"), key 10 ("Substation Beta"), key 15 ("Power Plant Gamma").
  - Note: $5 \pmod 5 = 10 \pmod 5 = 15 \pmod 5 = 0$.
- **Expected Outcome:**
  - `getCollisionCount() >= 2`.
  - All three distinct keys retrieved successfully with exact values.
- **Result:** **PASSED**. Collision resolution via bucket linked-lists verified.

---

### TEST 10: Doubly Linked List Forward & Backward Traversal
- **Objective:** Verify that the Doubly Linked List maintains bidirectional pointers and can traverse chronologically and in reverse.
- **Actions:**
  - Append 3 `IncidentEvent` instances.
  - Inspect `size()`, `getLatest()`, `displayForward()`, and `displayBackward()`.
- **Expected Outcome:**
  - Size is 3.
  - Latest event matches the last appended node.
  - Backward traversal correctly walks `tail -> prev -> prev`.
- **Result:** **PASSED**. Bidirectional integrity verified.
