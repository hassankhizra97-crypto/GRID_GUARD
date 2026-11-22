# Manual DSA Implementations — GridGuard Karachi Power Grid Simulator

> [!NOTE]
> This document explains every manually implemented data structure and algorithm in GridGuard. All implementations are written from scratch in C++ without using any STL containers for core DSA logic.

---

## Table of Contents

**Data Structures**
1. [CustomVector\<T\>](#1-customvectort--dynamic-array)
2. [Graph — Power Grid](#2-graph--power-grid-adjacency-list)
3. [Queue\<T\>](#3-queuet--singly-linked-list-queue)
4. [CircularQueue\<T\>](#4-circularqueuet--array-based-circular-queue)
5. [Stack\<T\>](#5-stackt--singly-linked-list-stack)
6. [ArrayStack\<T\>](#6-arraystackt--array-based-stack)
7. [MaxHeap](#7-maxheap--binary-max-heap)
8. [HashTable\<K,V\>](#8-hashtablekv--separate-chaining-hash-table)
9. [DoublyLinkedList\<T\>](#9-doublylinkedlistt--incident-history)

**Algorithms**
1. [BFS — Breadth-First Search](#1-bfs--breadth-first-search)
2. [DFS — Depth-First Search](#2-dfs--depth-first-search)
3. [Dijkstra — Shortest Path](#3-dijkstra--shortest-path-algorithm)
4. [BubbleSort](#4-bubblesort)
5. [SelectionSort](#5-selectionsort)
6. [InsertionSort](#6-insertionsort)
7. [QuickSort](#7-quicksort)
8. [MergeSort](#8-mergesort)
9. [LinearSearch](#9-linearsearch)
10. [BinarySearch](#10-binarysearch)

---

# DATA STRUCTURES

---

## 1. CustomVector\<T\> — Dynamic Array

### Purpose
`CustomVector<T>` is a generic dynamically-resizing array that replaces `std::vector`. It is used throughout the project wherever a growable list is needed — storing power lines, BFS/DFS traversal results, and path outputs.

### Internal Representation
```
+---------+-----------+---------+
| data*   | capacity_ | size_   |
+---------+-----------+---------+
     |
     v  Heap-allocated T[] array
     [e0][e1][e2][e3][--][--][--][--]
      <--- size_ --->
      <----------- capacity_ ------->
```

### Important Variables
| Variable | Type | Description |
|----------|------|-------------|
| `data` | `T*` | Pointer to the heap-allocated array |
| `capacity_` | `size_t` | Total allocated slots |
| `size_` | `size_t` | Number of elements currently stored |

### All Operations

| Operation | Description |
|-----------|-------------|
| `push_back(e)` | Appends element; triggers `resize(capacity * 2)` if full |
| `pop_back()` | Decrements `size_` (no deallocation) |
| `operator[](i)` | Direct index access, no bounds check |
| `at(i)` | Bounds-checked access; throws `std::out_of_range` |
| `size()` | Returns current number of elements |
| `capacity()` | Returns currently allocated capacity |
| `empty()` | Returns `true` if `size_ == 0` |
| `clear()` | Resets `size_` to 0 (data remains allocated) |
| `resize(n)` | Internal: allocates new array, copies elements, frees old array |
| `begin()` / `end()` | Iterator pointers for range-based loops |

### Time Complexities
| Operation | Time Complexity |
|-----------|----------------|
| `push_back` | O(1) amortized (O(n) during resize) |
| `pop_back` | O(1) |
| `operator[]` / `at` | O(1) |
| `clear` | O(1) |
| `resize` | O(n) |

### Where Used in GridGuard
- `Graph.h` — `CustomVector<PowerLine> powerLines` stores all 61 power line edges
- `Graph.cpp` — BFS and DFS store reachable location IDs in `CustomVector<int>`
- `RoadNetwork.cpp` — Dijkstra path reconstruction output is `CustomVector<int>`
- `Simulation.cpp` — Fault arrays for sorting demos

---

## 2. Graph — Power Grid Adjacency List

### Purpose
The `Graph` class models the Karachi power grid as an **undirected weighted graph**. Each vertex (node) is a power grid `Location` (substation, hospital, industrial zone, etc.). Each edge is a `PowerLine` connecting two locations with associated repair cost and travel cost.

### Internal Representation
```
Vertex Array (static):
locations[0..127]  — Location structs indexed by location ID

Adjacency List (array of linked-list heads):
adjList[0] -> AdjEdgeNode(neighbor=5, lineId=3, ...) -> AdjEdgeNode(neighbor=12, ...) -> nullptr
adjList[1] -> nullptr
adjList[5] -> AdjEdgeNode(neighbor=0, lineId=3, ...) -> AdjEdgeNode(neighbor=8, ...) -> nullptr
...
adjList[127] -> nullptr

Edge Storage:
powerLines  — CustomVector<PowerLine> (all 61 power line structs)
```

### Important Variables
| Variable | Type | Description |
|----------|------|-------------|
| `locations[MAX_VERTICES]` | `Location[]` | Vertex data indexed by location ID |
| `locationExists[MAX_VERTICES]` | `bool[]` | Marks which vertex IDs are in use |
| `adjList[MAX_VERTICES]` | `AdjEdgeNode*[]` | Array of linked-list heads (one per vertex) |
| `powerLines` | `CustomVector<PowerLine>` | All power line edge objects |
| `locationCount` | `int` | Number of registered locations |

### AdjEdgeNode Structure
```cpp
struct AdjEdgeNode {
    int neighborLocationId;
    int powerLineId;
    double repairCost;
    double travelCost;
    AdjEdgeNode* next;   // next edge in adjacency linked list
};
```

### All Operations
| Operation | Description |
|-----------|-------------|
| `addLocation(loc)` | Inserts a vertex into `locations[]` and sets `locationExists` flag |
| `addPowerLine(line)` | Pushes edge to `powerLines`; inserts `AdjEdgeNode` at head of both endpoints' adjacency lists |
| `disablePowerLine(id)` | Marks power line as non-operational (simulates fault) |
| `restorePowerLine(id)` | Marks power line as operational again |
| `getLocation(id)` | O(1) direct lookup in `locations[]` |
| `getPowerLine(id)` | O(n) scan of `powerLines` vector |
| `runBFS(start, ...)` | Breadth-first traversal using custom `Queue<int>` |
| `runDFS(start, ...)` | Depth-first traversal using recursive helper |
| `analyzeConnectivity(...)` | BFS from main station; classifies each location as connected or disconnected |

### Time Complexities
| Operation | Complexity |
|-----------|-----------|
| `addLocation` | O(1) |
| `addPowerLine` | O(1) amortized |
| `getLocation` | O(1) |
| BFS / DFS | O(V + E) where V = locations, E = power lines |
| `analyzeConnectivity` | O(V + E) |

### Where Used in GridGuard
- `Simulation` — `Graph powerGrid` is the primary simulation object
- Fault simulation: when a line is disabled, BFS detects which locations lose power
- Connectivity analysis shows powered vs. unpowered areas of Karachi

---

## 3. Queue\<T\> — Singly Linked-List Queue

### Purpose
A generic FIFO (First-In, First-Out) queue backed by a manually managed singly linked list. Used directly by the BFS algorithm to process vertices level by level.

### Internal Representation
```
frontNode                      rearNode
   |                               |
   v                               v
[Node: A] -> [Node: B] -> [Node: C] -> nullptr
 (dequeue from front)        (enqueue at rear)
```

### Important Variables
| Variable | Type | Description |
|----------|------|-------------|
| `frontNode` | `Node*` | Points to the front of the queue (next to be dequeued) |
| `rearNode` | `Node*` | Points to the rear (where new elements are added) |
| `count` | `int` | Current number of elements |

### Node Structure
```cpp
struct Node {
    T data;
    Node* next;
};
```

### All Operations
| Operation | Description |
|-----------|-------------|
| `enqueue(val)` | Allocates new node at rear; updates `rearNode->next` and `rearNode` |
| `dequeue()` | Removes front node, returns its data; updates `frontNode`; throws if empty |
| `peek()` | Returns reference to front data without removing |
| `isEmpty()` | Returns `true` when `frontNode == nullptr` |
| `size()` | Returns `count` |
| `clear()` | Calls `dequeue()` until empty |
| `display()` | Prints all elements front-to-rear |

### Time Complexities
| Operation | Complexity |
|-----------|-----------|
| `enqueue` | O(1) |
| `dequeue` | O(1) |
| `peek` | O(1) |
| `clear` | O(n) |

### Where Used in GridGuard
- `Graph::runBFS()` — `Queue<int>` holds location IDs to visit next during BFS traversal
- Instantiated inside BFS function scope; no persistent queue stored in Simulation

---

## 4. CircularQueue\<T\> — Array-Based Circular Queue

### Purpose
An alternative queue implementation using a **fixed-size circular array** instead of linked nodes. Demonstrates modulo-based index wraparound. Added for academic variety alongside the linked-list Queue.

### Internal Representation
```
Underlying array (capacity = 8):
Index:  0    1    2    3    4    5    6    7
       [--] [B ] [C ] [D ] [--] [--] [--] [A ]
              ^                            ^
            front (next dequeue)        rear (last enqueued)

After enqueue 'E':
       [--] [B ] [C ] [D ] [E ] [--] [--] [A ]
                                ^
                              rear = (rear+1) % capacity
```

### Important Variables
| Variable | Type | Description |
|----------|------|-------------|
| `buffer` | `T*` | Dynamically allocated array of fixed capacity |
| `front` | `int` | Index of next element to dequeue |
| `rear` | `int` | Index of most recently enqueued element |
| `count` | `int` | Number of valid elements |
| `capacity` | `int` | Total array size |

### All Operations
| Operation | Description |
|-----------|-------------|
| `enqueue(val)` | Places element at `(rear+1) % capacity`; throws if full |
| `dequeue()` | Reads element at `front`; advances `front = (front+1) % capacity` |
| `peek()` | Returns element at `front` without removing |
| `isFull()` | Returns `true` when `count == capacity` |
| `isEmpty()` | Returns `true` when `count == 0` |

### Time Complexities
| Operation | Complexity |
|-----------|-----------|
| `enqueue` | O(1) |
| `dequeue` | O(1) |
| `peek` | O(1) |

### Key Difference from Linear Queue
In a **linear array queue**, once the rear reaches the end, you cannot add more elements even if the front has advanced (wasted space). A circular queue reuses those vacated slots by wrapping the index with modulo arithmetic: `rear = (rear + 1) % capacity`.

### Where Used in GridGuard
- Included as a supplementary academic demonstration (`CircularQueue.h`)
- Demonstrable via simulation menu as an alternative BFS backing structure

---

## 5. Stack\<T\> — Singly Linked-List Stack

### Purpose
A generic LIFO (Last-In, First-Out) stack backed by a manually managed singly linked list. Used to record and reverse user operations (undo functionality).

### Internal Representation
```
topNode
   |
   v
[Node: Action3] -> [Node: Action2] -> [Node: Action1] -> nullptr
   (most recent)                        (oldest)
   <- push/pop here
```

### Important Variables
| Variable | Type | Description |
|----------|------|-------------|
| `topNode` | `Node*` | Points to the most recently pushed element |
| `count` | `int` | Current number of elements |

### UndoAction Struct (what Stack stores in GridGuard)
```cpp
struct UndoAction {
    UndoActionType type;   // ACTION_LINE_DISABLED, ACTION_LINE_RESTORED, ...
    int targetId;          // e.g., power line ID
    int secondaryId;       // auxiliary info
    std::string description;
};
```

### All Operations
| Operation | Description |
|-----------|-------------|
| `push(val)` | Allocates new node at top; sets `topNode = new Node(val, topNode)` |
| `pop()` | Removes and returns top element; deletes node; throws if empty |
| `peek()` | Returns const reference to top data without removing |
| `isEmpty()` | Returns `true` when `topNode == nullptr` |
| `size()` | Returns `count` |
| `clear()` | Calls `pop()` until empty |
| `displayHistory()` | Iterates top-to-bottom printing each `UndoAction` |

### Time Complexities
| Operation | Complexity |
|-----------|-----------|
| `push` | O(1) |
| `pop` | O(1) |
| `peek` | O(1) |
| `clear` | O(n) |

### Where Used in GridGuard
- `Simulation` — `Stack<UndoAction> undoStack` records every power line disable/restore action
- `undoLastOperation()` pops from this stack and reverses the recorded action

---

## 6. ArrayStack\<T\> — Array-Based Stack

### Purpose
An alternative stack implementation using a **dynamically resizing array** instead of linked nodes. Demonstrates that a stack ADT can be implemented with either a linked list (O(1) push/pop with pointer overhead) or an array (O(1) amortized with potential resize cost but better cache locality).

### Internal Representation
```
data array (heap-allocated):
[e0][e1][e2][e3][--][--][--][--]
                ^
              topIdx = 3  (index of current top)
```

### Important Variables
| Variable | Type | Description |
|----------|------|-------------|
| `data` | `T*` | Heap-allocated array |
| `topIdx` | `int` | Index of the topmost element (-1 when empty) |
| `capacity` | `int` | Allocated array size |

### All Operations
| Operation | Description |
|-----------|-------------|
| `push(val)` | Increments `topIdx`, stores at `data[topIdx]`; resizes if needed |
| `pop()` | Returns `data[topIdx--]`; throws if empty |
| `peek()` | Returns `data[topIdx]` without changing `topIdx` |
| `isEmpty()` | Returns `true` when `topIdx == -1` |
| `isFull()` | Returns `true` when `topIdx == capacity - 1` |

### Time Complexities
| Operation | Complexity |
|-----------|-----------|
| `push` | O(1) amortized |
| `pop` | O(1) |
| `peek` | O(1) |

### Key Difference from Linked-List Stack
- **Array stack**: Better cache locality (contiguous memory); `topIdx` arithmetic; no pointer overhead per element; O(1) amortized `push` (occasionally O(n) for resize)
- **Linked-list stack**: No capacity limit without resize; each node carries a `next` pointer; O(1) guaranteed per operation

### Where Used in GridGuard
- Included as `ArrayStack.h` for academic demonstration and comparison

---

## 7. MaxHeap — Binary Max-Heap

### Purpose
A **binary max-heap** implemented using a dynamically allocated array. It is the core **priority queue** for the fault/repair scheduling system: whichever power line fault has the highest `priorityScore` is always extracted first for repair.

### Internal Representation
```
Conceptual tree:
              [0: score=95]
             /             \
    [1: score=72]       [2: score=88]
    /         \
[3: score=50] [4: score=60]

Physical array:
Index:   0     1     2     3     4
Score: [95]  [72]  [88]  [50]  [60]

Parent-child index math (0-indexed):
  parent(i)      = (i - 1) / 2
  leftChild(i)   = 2*i + 1
  rightChild(i)  = 2*i + 2
```

### Important Variables
| Variable | Type | Description |
|----------|------|-------------|
| `heap` | `Fault*` | Dynamically allocated array of Fault objects |
| `capacity` | `int` | Allocated slots |
| `heapSize` | `int` | Number of valid elements in the heap |

### Heap Property
Every node's `priorityScore` is **greater than or equal to** both its children. The root (`heap[0]`) always holds the fault with the **highest priority**.

### Priority Score Formula (from Simulation)
```
priorityScore = severity * 20
              + criticalFacilitiesAffected * 15
              + (populationAffected / 1000)
              + (outageDuration / 10)
```
Hospitals and critical infrastructure push faults to the top of the queue.

### All Operations

#### `insert(fault)` — O(log n)
1. Place new element at `heap[heapSize]`
2. Increment `heapSize`
3. Call `heapifyUp(heapSize - 1)`

#### `heapifyUp(i)` — O(log n)
```
while i > 0 AND heap[i].score > heap[parent(i)].score:
    swap(heap[i], heap[parent(i)])
    i = parent(i)
```
Bubbles the new element up until it is smaller than its parent (or reaches root).

#### `extractMax()` — O(log n)
1. Save `heap[0]` (the max)
2. Move `heap[heapSize-1]` to `heap[0]`
3. Decrement `heapSize`
4. Call `heapifyDown(0)`
5. Return saved max

#### `heapifyDown(i)` — O(log n)
```
largest = i
if leftChild(i) < heapSize AND heap[left].score > heap[largest].score: largest = left
if rightChild(i) < heapSize AND heap[right].score > heap[largest].score: largest = right
if largest != i:
    swap(heap[i], heap[largest])
    heapifyDown(largest)
```
Pushes the displaced root down until it is larger than both children.

#### Other Operations
| Operation | Description | Complexity |
|-----------|-------------|-----------|
| `peekMax()` | Returns `heap[0]` by reference | O(1) |
| `isEmpty()` | Returns `heapSize == 0` | O(1) |
| `size()` | Returns `heapSize` | O(1) |
| `clear()` | Sets `heapSize = 0` | O(1) |
| `updateFaultStatus(id, status)` | Linear scan to find and update a fault | O(n) |
| `findFault(id)` | Linear scan; returns pointer or `nullptr` | O(n) |
| `resize(n)` | Internal; doubles capacity; copies elements | O(n) |
| `displayQueue()` | Copies heap, extracts all in sorted order for display | O(n log n) |

### Time Complexities
| Operation | Complexity |
|-----------|-----------|
| `insert` | O(log n) |
| `extractMax` | O(log n) |
| `peekMax` | O(1) |
| Build heap from n elements | O(n) |

### Where Used in GridGuard
- `Simulation` — `MaxHeap repairQueue` holds all active fault objects
- `simulatePowerLineFailure()` — inserts new `Fault` into the heap
- `processNextRepair()` — calls `extractMax()` to get the highest-priority fault to fix first

---

## 8. HashTable\<K,V\> — Separate Chaining Hash Table

### Purpose
A generic hash table using **separate chaining** for collision resolution. Provides O(1) average-case lookup for Locations and Faults by their integer IDs. Also uses a string-keyed variant for name lookups.

### Internal Representation
```
buckets array (capacity = 17):
 [0] -> nullptr
 [1] -> [HashNode: key=18, val=...] -> [HashNode: key=35, val=...] -> nullptr  (collision chain)
 [2] -> [HashNode: key=2,  val=...] -> nullptr
 ...
[16] -> nullptr

Each HashNode:
  key | value | next*
```

### Hash Functions

**Integer keys:**
```cpp
int hashFunction(int key) const {
    int hash = key % capacity;
    return hash < 0 ? hash + capacity : hash;
}
```

**String keys (djb2 algorithm):**
```cpp
int hashFunction(const std::string& key) const {
    unsigned long hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + (unsigned char)c;
        // equivalent to: hash = hash * 33 + c
    }
    return (int)(hash % capacity);
}
```

### Important Variables
| Variable | Type | Description |
|----------|------|-------------|
| `buckets` | `HashNode**` | Array of linked-list head pointers |
| `capacity` | `int` | Number of buckets (default 17, a prime) |
| `totalElements` | `int` | Number of key-value pairs stored |
| `collisionCount` | `int` | Tracks detected collisions for diagnostics |

### All Operations
| Operation | Description | Complexity |
|-----------|-------------|-----------|
| `insert(k, v)` | Hashes key → index; traverses chain to check for existing key (update) or prepends new node | O(1) avg, O(n) worst |
| `search(k, outVal)` | Hashes key → index; traverses chain comparing keys | O(1) avg, O(n) worst |
| `searchPtr(k)` | Returns `V*` pointer for in-place modification | O(1) avg |
| `update(k, v)` | Uses `searchPtr` to update existing value | O(1) avg |
| `remove(k)` | Traverses chain with prev pointer; unlinks and deletes node | O(1) avg |
| `contains(k)` | Wraps `search` | O(1) avg |
| `clear()` | Deletes all nodes in all chains | O(n) |
| `displayBucketDiagnostics()` | Prints chain lengths per bucket | O(n) |

### Why Capacity is a Prime Number
Using a prime number (like 17) as the table capacity reduces clustering because fewer keys will share the same modulo result, leading to more uniform bucket distribution.

### Where Used in GridGuard
- `Simulation` — `HashTable<int, Location> locationTable` — fast O(1) lookup by location ID
- `Simulation` — `HashTable<int, Fault> faultTable` — fast O(1) lookup by fault ID
- `searchLocationById()` and `searchFaultById()` use these tables
- `displayHashTableDiagnostics()` prints collision statistics for academic inspection

---

## 9. DoublyLinkedList\<T\> — Incident History

### Purpose
A generic doubly linked list where each node has both a `next` and `prev` pointer. This enables bidirectional traversal — viewing incident history from oldest-to-newest OR newest-to-oldest without reversing the list.

### Internal Representation
```
head                                            tail
 |                                               |
 v                                               v
[Event1] <-> [Event2] <-> [Event3] <-> [Event4]
prev=null     prev→E1      prev→E2      prev→E3
next→E2       next→E3      next→E4      next=null
```

### Important Variables
| Variable | Type | Description |
|----------|------|-------------|
| `head` | `DNode*` | Points to oldest (first) event |
| `tail` | `DNode*` | Points to newest (last) event |
| `count` | `int` | Total number of events |

### DNode Structure
```cpp
struct DNode {
    T data;
    DNode* prev;
    DNode* next;
};
```

### All Operations
| Operation | Description | Complexity |
|-----------|-------------|-----------|
| `append(val)` | Inserts new node at tail; updates `tail->next` and old tail's `next`; sets new node's `prev` to old tail | O(1) |
| `prepend(val)` | Inserts new node at head; updates `head->prev` and old head's `prev` | O(1) |
| `isEmpty()` | Returns `head == nullptr` | O(1) |
| `size()` | Returns `count` | O(1) |
| `getLatest()` | Returns pointer to `tail->data` (most recent event) | O(1) |
| `clear()` | Traverses from head, deleting each node | O(n) |
| `displayForward()` | Traverses `head → tail` via `next` pointers | O(n) |
| `displayBackward()` | Traverses `tail → head` via `prev` pointers | O(n) |
| `searchEvents(keyword)` | Forward traversal; checks `description` and `eventType` for substring | O(n) |

### Where Used in GridGuard
- `Simulation` — `DoublyLinkedList<IncidentEvent> incidentHistory`
- Every fault simulation and repair logs an `IncidentEvent` via `append()`
- `displayIncidentHistoryForward()` — chronological log view
- `displayIncidentHistoryBackward()` — reverse chronological (most recent first)
- `searchIncidentHistory(keyword)` — keyword search through all events

---

# ALGORITHMS

---

## 1. BFS — Breadth-First Search

### Purpose
Explores a graph level by level from a starting vertex. In GridGuard, BFS identifies which locations are **reachable** (powered) vs. **unreachable** (suffering an outage) from the main power station.

### How It Uses the Custom Queue
BFS is impossible without a FIFO structure — the queue ensures nodes are visited level by level, not depth by depth.

### Algorithm (Step by Step)
```
Input: startVertexId, Graph G (adjacency list), onlyOperational flag

1. Initialize: visited[MAX_VERTICES] = all false
2. Mark visited[startVertexId] = true
3. Enqueue startVertexId into Queue<int> q
4. While q is not empty:
   a. u = q.dequeue()
   b. Record u in traversalOrder
   c. For each AdjEdgeNode* edge in adjList[u]:
      - neighborId = edge->neighborLocationId
      - If edge's power line is operational (or onlyOperational is false)
        AND visited[neighborId] == false:
          i.  Mark visited[neighborId] = true
          ii. q.enqueue(neighborId)
5. All vertices in traversalOrder are reachable from start
```

### Trace Example (small grid)
```
Graph: 1--2--4
       |
       3

Start BFS from 1:
  Queue: [1]
  Visit 1 → enqueue neighbors 2, 3 → Queue: [2, 3]
  Visit 2 → enqueue neighbor 4     → Queue: [3, 4]
  Visit 3 → no unvisited neighbors → Queue: [4]
  Visit 4 → no unvisited neighbors → Queue: []
  Result: [1, 2, 3, 4]
```

### Time Complexity
- **O(V + E)** — each vertex and each edge is processed exactly once

### Where Used
- `Graph::runBFS()` — called with main station as start
- `Graph::analyzeConnectivity()` — BFS result partitions locations into connected/disconnected

---

## 2. DFS — Depth-First Search

### Purpose
Explores a graph by going as deep as possible along each branch before backtracking. Implemented **recursively** in GridGuard. DFS is useful for detecting all connected components and for topological analysis.

### Algorithm (Recursive)
```
dfsHelper(u, visited[], traversalOrder):
  1. Mark visited[u] = true
  2. Append u to traversalOrder
  3. For each AdjEdgeNode* edge in adjList[u]:
     - neighborId = edge->neighborLocationId
     - If edge is operational AND visited[neighborId] == false:
         dfsHelper(neighborId, visited, traversalOrder)

runDFS(startVertexId):
  1. Initialize visited[] = all false
  2. Call dfsHelper(startVertexId, visited, traversalOrder)
```

### DFS vs BFS Comparison
| Aspect | BFS | DFS |
|--------|-----|-----|
| Order | Level-by-level | Deep-first |
| Data Structure | Queue (FIFO) | Call stack (recursion) or explicit stack |
| Finds shortest path? | Yes (unweighted) | No |
| Memory | O(V) for queue | O(V) for recursion depth |
| Use in GridGuard | Outage detection | Full graph exploration |

### Time Complexity
- **O(V + E)**

---

## 3. Dijkstra — Shortest Path Algorithm

### Purpose
Finds the **minimum-cost path** from a repair depot to a target location through the road network. GridGuard uses a **pure array-based** implementation — no `std::priority_queue`.

### Internal Data Structures (all manual arrays)
```cpp
double dist[MAX_VERTICES];   // dist[v] = best known cost to reach v
bool   visited[MAX_VERTICES]; // visited[v] = true once finalized
int    parent[MAX_VERTICES]; // parent[v] = which vertex we came from to reach v
```

### Algorithm (Step by Step)
```
Input: startId, targetId, road graph adjacency list

1. Initialize:
   dist[v] = INF (1e18) for all v
   visited[v] = false for all v
   parent[v] = -1 for all v
   dist[startId] = 0.0

2. Repeat V times:
   a. Find unvisited vertex u with minimum dist[u]  ← manual O(V) scan
   b. If dist[u] == INF, break (remaining vertices unreachable)
   c. Mark visited[u] = true
   d. For each road edge (u → neighbor, weight w):
      - If NOT visited[neighbor] AND dist[u] + w < dist[neighbor]:
          dist[neighbor] = dist[u] + w
          parent[neighbor] = u

3. Path reconstruction (working backwards from targetId):
   path = []
   v = targetId
   while parent[v] != -1:
       path.prepend(v)
       v = parent[v]
   path.prepend(startId)

4. Return path and dist[targetId] as total cost
```

### Why O(V²)?
The manual min-selection loop in step 2a is O(V), and it runs V times → **O(V²)** total. A heap-based Dijkstra would be O((V+E) log V), but array-based is simpler to implement and correct for small graphs (V ≤ 128 in GridGuard).

### Time Complexity
- **O(V²)** with array min-selection (GridGuard implementation)
- O((V+E) log V) with binary heap (not used here, by design)

### Where Used
- `RoadNetwork::findShortestPath()` — called by `Simulation::findRepairCrewRoute()`
- Output: optimal sequence of location IDs + total travel minutes

---

## 4. BubbleSort

### Purpose
The simplest comparison-based sorting algorithm. Repeatedly passes through the array, swapping adjacent elements that are out of order. Each pass "bubbles" the largest unsorted element to the end.

### Algorithm
```
for i from 0 to n-2:
    for j from 0 to n-2-i:
        if arr[j] > arr[j+1]:
            swap(arr[j], arr[j+1])
```

### Properties
- **Stable**: equal elements maintain their original relative order
- **In-place**: O(1) extra memory
- **Adaptive**: with an early-exit flag, terminates early if already sorted

### Time Complexities
| Case | Complexity |
|------|-----------|
| Best (sorted, with early exit) | O(n) |
| Average | O(n²) |
| Worst (reverse sorted) | O(n²) |

### Where Used
- `Sorting::bubbleSort()` in `Sorting.cpp`
- Menu option in Simulation for sorting faults by priority score / severity / population / duration

---

## 5. SelectionSort

### Purpose
Divides the array into sorted and unsorted halves. On each pass, finds the **minimum** (or maximum) element in the unsorted portion and swaps it to the correct position.

### Algorithm
```
for i from 0 to n-2:
    minIdx = i
    for j from i+1 to n-1:
        if arr[j] < arr[minIdx]:
            minIdx = j
    swap(arr[i], arr[minIdx])
```

### Properties
- **Not stable** in its basic form
- **In-place**: O(1) extra memory
- Always performs exactly n(n-1)/2 comparisons regardless of input order
- Makes at most n-1 swaps (useful when writes are expensive)

### Time Complexities
| Case | Complexity |
|------|-----------|
| Best | O(n²) |
| Average | O(n²) |
| Worst | O(n²) |

### Where Used
- `Sorting::selectionSort()` in `Sorting.cpp`

---

## 6. InsertionSort

### Purpose
Builds the sorted array one element at a time by picking each element and inserting it into its correct position among the already-sorted elements.

### Algorithm
```
for i from 1 to n-1:
    key = arr[i]
    j = i - 1
    while j >= 0 AND arr[j] > key:
        arr[j+1] = arr[j]
        j--
    arr[j+1] = key
```

### Properties
- **Stable**: equal elements maintain relative order
- **Adaptive**: O(n) for nearly-sorted input
- **In-place**: O(1) extra memory
- Efficient for small arrays (< 20 elements)

### Time Complexities
| Case | Complexity |
|------|-----------|
| Best (sorted) | O(n) |
| Average | O(n²) |
| Worst (reverse sorted) | O(n²) |

### Where Used
- `Sorting::insertionSort()` in `Sorting.cpp`

---

## 7. QuickSort

### Purpose
A divide-and-conquer sorting algorithm that selects a **pivot** element and partitions the array so all elements less than the pivot are to its left and all greater elements are to its right. Then recursively sorts both halves.

### Algorithm
```
quickSort(arr, low, high):
    if low < high:
        pivotIdx = partition(arr, low, high)
        quickSort(arr, low, pivotIdx - 1)
        quickSort(arr, pivotIdx + 1, high)

partition(arr, low, high):
    pivot = arr[high]  // choose last element as pivot
    i = low - 1
    for j from low to high-1:
        if arr[j] <= pivot:
            i++
            swap(arr[i], arr[j])
    swap(arr[i+1], arr[high])
    return i + 1
```

### Properties
- **Not stable** in its basic Lomuto/Hoare partition form
- **In-place** (O(log n) stack space for recursion)
- Average case O(n log n) but degrades to O(n²) on already-sorted input with naive pivot selection

### Time Complexities
| Case | Complexity |
|------|-----------|
| Best | O(n log n) |
| Average | O(n log n) |
| Worst (sorted input, bad pivot) | O(n²) |
| Space | O(log n) avg recursion stack |

### Where Used
- `Sorting::quickSort()` in `Sorting.cpp` — primary fault-sorting algorithm
- Used when sorting faults by priority score, severity, population, or outage duration

---

## 8. MergeSort

### Purpose
A divide-and-conquer algorithm that splits the array in half, recursively sorts each half, then **merges** the two sorted halves into one. Unlike QuickSort, it guarantees O(n log n) even in the worst case.

### Algorithm
```
mergeSort(arr, left, right):
    if left < right:
        mid = (left + right) / 2
        mergeSort(arr, left, mid)
        mergeSort(arr, mid+1, right)
        merge(arr, left, mid, right)

merge(arr, left, mid, right):
    // Copy left half to temp array L[], right half to R[]
    // Merge L and R back into arr in sorted order
    i = 0, j = 0, k = left
    while i < lenL AND j < lenR:
        if L[i] <= R[j]: arr[k++] = L[i++]
        else:            arr[k++] = R[j++]
    copy remaining elements from L or R
```

### Properties
- **Stable**: equal elements maintain relative order ← key advantage
- **Not in-place**: requires O(n) temporary arrays
- Predictable O(n log n) always (no worst-case degradation)
- Preferred when stability matters (e.g., sorting by severity then by duration, maintaining order)

### Time Complexities
| Case | Complexity |
|------|-----------|
| Best | O(n log n) |
| Average | O(n log n) |
| Worst | O(n log n) |
| Space | O(n) |

### Where Used
- `Sorting::mergeSort()` in `Sorting.cpp` — stable fault-sorting algorithm

---

## 9. LinearSearch

### Purpose
Searches for a target element by examining every element in sequence until found. Works on **unsorted** data. The simplest search algorithm.

### Algorithm
```
linearSearch(arr, n, target):
    for i from 0 to n-1:
        if arr[i] == target:
            return i  // found at index i
    return -1  // not found
```

### Properties
- Works on any data, sorted or unsorted
- Returns first occurrence
- O(n) — must check every element in worst case

### Time Complexities
| Case | Complexity |
|------|-----------|
| Best (target at index 0) | O(1) |
| Average | O(n/2) ≈ O(n) |
| Worst (target at end or missing) | O(n) |

### Where Used
- `SearchAlgorithms::linearSearchFaultById()` — scan fault array for a specific fault ID
- `SearchAlgorithms::linearSearchFaultByLineId()` — find faults affecting a specific power line

---

## 10. BinarySearch

### Purpose
Searches a **sorted** array by repeatedly halving the search space. At each step, compares the target to the middle element and eliminates the half that cannot contain the target.

### Algorithm
```
binarySearch(arr, n, target):
    low = 0, high = n - 1
    while low <= high:
        mid = (low + high) / 2
        if arr[mid] == target:
            return mid
        else if arr[mid] < target:
            low = mid + 1
        else:
            high = mid - 1
    return -1  // not found
```

### Critical Prerequisite
**The array must be sorted before binary search is applied.** GridGuard calls `quickSort` or `mergeSort` on the fault array before invoking binary search.

### Properties
- Logarithmic time — eliminates half the search space each iteration
- In-place: O(1) extra space
- Only valid on sorted data

### Time Complexities
| Case | Complexity |
|------|-----------|
| Best (target at middle) | O(1) |
| Average | O(log n) |
| Worst (target at edge or missing) | O(log n) |

### Comparison: Linear vs Binary
| Aspect | Linear | Binary |
|--------|--------|--------|
| Data requirement | None (any order) | Must be sorted |
| Time complexity | O(n) | O(log n) |
| For 1,000 faults | Up to 1,000 checks | Up to 10 checks |
| For 1,000,000 faults | 1,000,000 checks | 20 checks |

### Where Used
- `SearchAlgorithms::binarySearchFaultById()` — fast lookup after sorting by fault ID
- Demonstrated via simulation menu alongside linear search for academic comparison
