# GridGuard — Data Structures & Algorithms (DSA) Technical Specification

This document provides an academic, in-depth explanation of the six core data structures and algorithmic procedures implemented in the **GridGuard** project.

---

## 1. Graph (Adjacency List Representation)

### A. What It Is
A graph $G = (V, E)$ consists of a set of vertices $V$ and edges $E$. In GridGuard, the power grid is modeled as an undirected, weighted graph where vertices represent city facilities and edges represent power transmission lines. An independent road network is similarly modeled as a graph of ground transit corridors.

### B. Why It Is Used in GridGuard
Electricity flow depends on physical connectivity. Power lines can be disrupted without affecting the physical city topology. By modeling transmission lines as edges with boolean operational states, we can simulate line breaks, redundant feeds, and topological partitions.

### C. Implementation Details
- Handcrafted adjacency list without STL:
  ```cpp
  struct AdjEdgeNode {
      int neighborLocationId;
      int powerLineId;
      double repairCost;
      double travelCost;
      AdjEdgeNode* next;
  };
  ```
- An array `AdjEdgeNode* adjList[MAX_VERTICES]` anchors the linked list of incident edges for each vertex.
- Bidirectional insertion ensures undirected power transmission.

### D. Primary Operations & Time Complexity
- `addLocation(Location loc)`: $O(1)$
- `addPowerLine(PowerLine line)`: $O(1)$ amortized insertion at head of linked list
- `disablePowerLine(int lineId)`: $O(E)$ linear search in line registry
- `restorePowerLine(int lineId)`: $O(E)$
- `displayNeighbors(int u)`: $O(\text{deg}(u))$ where $\text{deg}(u)$ is the degree of vertex $u$

---

## 2. Queue (Manual Singly Linked List FIFO)

### A. What It Is
A Queue is a linear First-In, First-Out (FIFO) data structure where elements are inserted at the rear and removed from the front.

### B. Why It Is Used in GridGuard
Breadth-First Search (BFS) is the fundamental algorithm used to verify reachability from the Main Power Generation Station. A queue is mandatory for BFS to explore the network level-by-level (outward by hop count).

### C. Implementation Details
- Built with dynamic `Node` structures containing `data` and `next` pointers.
- Maintains `frontNode` and `rearNode` pointers:
  ```cpp
  template <typename T>
  class Queue {
      struct Node { T data; Node* next; };
      Node* frontNode;
      Node* rearNode;
      int count;
  };
  ```
- Enqueue attaches to `rearNode->next` and advances `rearNode`.
- Dequeue captures `frontNode->data`, advances `frontNode`, and frees the old head.

### D. Primary Operations & Time Complexity
- `enqueue(T item)`: $O(1)$
- `dequeue()`: $O(1)$
- `peek()`: $O(1)$
- `isEmpty()`: $O(1)$
- `size()`: $O(1)$

---

## 3. Priority Queue / Binary Max-Heap

### A. What It Is
A complete binary tree stored compactly in a contiguous dynamic array, satisfying the Max-Heap property: for every node $i$ (except the root), $\text{value}(\text{parent}(i)) \ge \text{value}(i)$.

### B. Why It Is Used in GridGuard
During power emergencies, multiple transmission lines may fail simultaneously. Emergency management demands triaging repair work orders by impact rather than arrival order. The Max-Heap ensures that the most severe, high-consequence failure is always accessible at the root in $O(1)$ time and extractable in $O(\log n)$ time.

### C. Priority Scoring Formula
$$\text{Priority Score} = (\text{Severity} \times 10) + (\text{Critical Facilities} \times 20) + \left(\frac{\text{Population}}{100}\right) + (\text{Outage Duration} \times 2)$$
- Severity (1 to 5) scales base impact.
- Outages affecting hospitals or emergency facilities are heavily weighted ($+20$ points each).
- Population affected scales gradually to prevent integer overflow while giving weight to large urban centers.
- Outage duration escalates priority as customers sit without power over time.

### D. Implementation Details
- Array indexing:
  - $\text{Parent}(i) = \lfloor (i - 1) / 2 \rfloor$
  - $\text{LeftChild}(i) = 2i + 1$
  - $\text{RightChild}(i) = 2i + 2$
- `heapifyUp(int i)`: Swaps node $i$ with its parent until the heap property is restored.
- `heapifyDown(int i)`: Compares node $i$ with its left and right children, swapping with the larger child iteratively.

### E. Primary Operations & Time Complexity
- `insert(Fault f)`: $O(\log n)$
- `extractMax()`: $O(\log n)$
- `peekMax()`: $O(1)$
- `displayQueue()`: $O(n \log n)$ non-destructive sorted print using copy extraction

---

## 4. Hash Table (Separate Chaining with Linked Lists)

### A. What It Is
An associative data structure mapping unique keys to values by evaluating a hash function to compute an index into an array of buckets. Collisions are handled via linked-list chains.

### B. Why It Is Used in GridGuard
Grid operators need immediate, constant-time retrieval of location records (by Location ID) and incident fault reports (by Fault ID) without traversing graphs or scanning linear lists.

### C. Implementation Details
- Dynamic bucket array of pointer heads: `HashNode<K, V>** buckets`.
- Dual hash functions:
  - Integer keys: $h(k) = k \pmod{\text{capacity}}$
  - String keys: Polynomial rolling hash (djb2 algorithm)
- Insertion prepends to the bucket chain if the key does not already exist, or updates in-place if it exists.
- Tracks `collisionCount` to demonstrate collision resolution.

### D. Primary Operations & Time Complexity
- `insert(key, value)`: $O(1)$ average, $O(n)$ worst-case
- `search(key, outVal)`: $O(1)$ average, $O(n)$ worst-case
- `update(key, newVal)`: $O(1)$ average
- `remove(key)`: $O(1)$ average

---

## 5. Doubly Linked List (Incident History Ledger)

### A. What It Is
A linear collection of nodes where each node contains data and two bidirectional pointers: `next` (pointing to the succeeding node) and `prev` (pointing to the preceding node).

### B. Why It Is Used in GridGuard
Regulatory compliance and operational audits require a tamper-evident, append-only history of grid incidents. Operators need the ability to view incidents chronologically (oldest to newest) or in reverse chronological order (newest to oldest to see immediate recent changes).

### C. Implementation Details
- Node structure:
  ```cpp
  struct DNode {
      IncidentEvent data;
      DNode* prev;
      DNode* next;
  };
  ```
- Maintains `head` (first event logged) and `tail` (latest event logged) pointers.
- Forward traversal starts at `head` and follows `next`.
- Backward traversal starts at `tail` and follows `prev`.

### D. Primary Operations & Time Complexity
- `append(IncidentEvent ev)`: $O(1)$ via `tail` pointer
- `getLatest()`: $O(1)$
- `displayForward()`: $O(n)$
- `displayBackward()`: $O(n)$
- `searchEvents(string keyword)`: $O(n)$ linear scan

---

## 6. Stack (Undo Mechanism)

### A. What It Is
A linear Last-In, First-Out (LIFO) data structure where elements are pushed and popped exclusively from the top.

### B. Why It Is Used in GridGuard
Operators simulating contingencies often need to undo accidental failure triggers or roll back repairs to test alternative mitigation strategies. The stack stores reversible operation deltas (`UndoAction`).

### C. Implementation Details
- Handcrafted singly-linked nodes anchored by `topNode`:
  ```cpp
  struct UndoAction {
      UndoActionType type; // ACTION_LINE_DISABLED, ACTION_LINE_RESTORED
      int targetId;        // lineId
      int secondaryId;     // faultId
      string description;
  };
  ```
- `push`: Creates a node whose `next` points to current `topNode`.
- `pop`: Removes and deletes `topNode`, retreating to `topNode->next`.

### D. Primary Operations & Time Complexity
- `push(UndoAction action)`: $O(1)$
- `pop()`: $O(1)$
- `peek()`: $O(1)$
- `isEmpty()`: $O(1)$

---

## 7. Core Graph & Optimization Algorithms

### A. Breadth-First Search (BFS)
- **Purpose:** Grid connectivity validation and outage scope isolation.
- **Mechanism:** Starts at Main Power Station (Vertex 1). Uses custom `Queue<int>`. Explores adjacent vertices across operational power lines (`isOperational == true`).
- **Time Complexity:** $O(V + E)$ where $V = 14$ and $E = 18$.
- **Space Complexity:** $O(V)$ for queue and boolean visited array.

### B. Depth-First Search (DFS)
- **Purpose:** Structural exploration and cycle/reachability inspection.
- **Mechanism:** Implemented recursively with internal call stack or explicit backtracking.
- **Time Complexity:** $O(V + E)$.

### C. Dijkstra’s Shortest Path Algorithm
- **Purpose:** Finding the fastest road transit route for repair crews dispatched from the central depot to the site of damaged infrastructure.
- **Key Design Insight:** The physical road network is maintained separately from the electrical transmission grid. A severed power line does not block municipal roads, ensuring repair crews can always calculate a valid transit path.
- **Mechanism:** Maintains tentative distances initialized to $\infty$. Greedily relaxes the unvisited node with minimum distance and tracks predecessors via `parent[]` array to reconstruct the exact path.
- **Time Complexity:** $O(V^2)$ with array selection (optimal for dense/moderate city graphs).

### D. Manual Sorting (QuickSort & MergeSort)
- **Purpose:** Sorting fault records dynamically by user-selected criteria:
  1. Priority Score
  2. Severity Level
  3. Population Affected
  4. Outage Duration
- **QuickSort Details:** In-place partitioning using Lomuto scheme with custom comparator. Average time: $O(n \log n)$.
- **MergeSort Details:** Divide-and-conquer splitting with dynamic temporary sub-arrays. Guaranteed worst-case time: $O(n \log n)$.
