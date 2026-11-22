# DSA Lab Compliance Audit — GridGuard Karachi Power Grid Simulator

**Audit Date:** 2026-11-22  
**Branch:** `dsa-lab-compliance`  
**Auditor:** GridGuard DSA Team

---

## Audit Methodology

Every C++ source file was inspected for STL container and algorithm usage.
Each occurrence was classified as:
- **CORE DSA** — part of the primary data structure demonstration (must be replaced)
- **SUPPORT** — infrastructure/serialization/I/O (acceptable to retain)
- **ALREADY MANUAL** — already manually implemented (no action needed)

---

## STL Usage Inventory

### `include/CustomVector.h`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| `new T[]` / `delete[]` | Dynamic array allocation | ALREADY MANUAL | RETAIN — this IS the manual implementation |
| `std::out_of_range` | Bounds checking exception | SUPPORT | RETAIN — standard exception |

**Verdict:** `CustomVector<T>` is a **fully manual dynamic array** implementation. No STL `std::vector` is used. This satisfies the requirement.

---

### `include/CircularQueue.h` / `include/Queue.h`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| Linked-list nodes with `new`/`delete` | Queue storage | ALREADY MANUAL | RETAIN |
| `std::runtime_error` | Exception on empty dequeue | SUPPORT | RETAIN |

**Verdict:** Queue is a **manual singly linked-list Queue**. No `std::queue` or `std::deque` used. ✅  
**Enhancement Added:** `CircularQueue.h` — array-based circular queue added as an additional variant for academic demonstration.

---

### `include/ArrayStack.h` / `include/Stack.h`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| Linked-list nodes with `new`/`delete` | Stack storage | ALREADY MANUAL | RETAIN |
| `std::runtime_error` | Exception on empty pop | SUPPORT | RETAIN |
| `std::string` | Description field in `UndoAction` | SUPPORT | RETAIN |

**Verdict:** Stack is a **manual singly linked-list Stack**. No `std::stack` used. ✅  
**Enhancement Added:** `ArrayStack.h` — array-based stack added as additional variant.

---

### `include/MaxHeap.h`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| `Fault* heap = new Fault[]` | Dynamic heap array | ALREADY MANUAL | RETAIN |
| `parent(i)`, `leftChild(i)`, `rightChild(i)` | Manual index calculations | ALREADY MANUAL | RETAIN |
| `std::runtime_error` | Exception on empty extract | SUPPORT | RETAIN |

**Verdict:** MaxHeap is a **fully manual binary max-heap** using a dynamically allocated array. No `std::priority_queue` used. ✅

---

### `include/HashTable.h`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| `HashNode** buckets = new HashNode*[]` | Bucket array | ALREADY MANUAL | RETAIN |
| Manual linked-list nodes per bucket | Separate chaining | ALREADY MANUAL | RETAIN |
| `std::string` for keys | Key type | SUPPORT | RETAIN |

**Verdict:** HashTable is a **fully manual separate-chaining hash table**. No `std::unordered_map` or `std::map` used. ✅

---

### `include/DoublyLinkedList.h`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| `DNode* head`, `DNode* tail` | Doubly linked nodes | ALREADY MANUAL | RETAIN |
| `prev` and `next` pointers | Bidirectional traversal | ALREADY MANUAL | RETAIN |

**Verdict:** DoublyLinkedList is a **fully manual doubly linked list**. No `std::list` used. ✅

---

### `include/Graph.h` / `src/Graph.cpp`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| `Location locations[MAX_VERTICES]` | Vertex storage array | ALREADY MANUAL | RETAIN — static array |
| `AdjEdgeNode* adjList[MAX_VERTICES]` | Adjacency list heads | ALREADY MANUAL | RETAIN — manual pointer array |
| `CustomVector<PowerLine> powerLines` | Edge storage | ALREADY MANUAL | RETAIN — uses our custom vector |
| `bool visited[MAX_VERTICES]` | BFS/DFS visited array | ALREADY MANUAL | RETAIN — stack-allocated array |
| `Queue<int> q` | BFS queue | ALREADY MANUAL | RETAIN — uses our manual Queue |

**Verdict:** Graph uses **manual adjacency list** representation with pointer-based nodes. No Boost, no STL graph libraries. BFS uses our custom Queue. DFS is recursive. ✅

---

### `include/RoadNetwork.h` / `src/RoadNetwork.cpp`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| `RoadEdgeNode* adjList[MAX_VERTICES]` | Road adjacency list | ALREADY MANUAL | RETAIN |
| `double dist[MAX_VERTICES]` | Dijkstra distance array | ALREADY MANUAL | RETAIN — stack array |
| `bool visited[MAX_VERTICES]` | Dijkstra visited set | ALREADY MANUAL | RETAIN |
| `int parent[MAX_VERTICES]` | Path reconstruction | ALREADY MANUAL | RETAIN |
| `CustomVector<int>` for path output | Path storage | ALREADY MANUAL | RETAIN |

**Verdict:** Dijkstra uses **manual arrays and manual min-selection loop**. No `std::priority_queue` for Dijkstra. ✅

---

### `include/Sorting.h` / `src/Sorting.cpp`
| Algorithm | Before Audit | After Audit |
|-----------|-------------|-------------|
| `std::sort` | NOT USED ✅ | NOT USED ✅ |
| QuickSort | Manual ✅ | Manual ✅ |
| MergeSort | Manual ✅ | Manual ✅ |
| BubbleSort | ❌ MISSING | ✅ ADDED |
| SelectionSort | ❌ MISSING | ✅ ADDED |
| InsertionSort | ❌ MISSING | ✅ ADDED |

---

### `src/Simulation.cpp`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| `std::ostringstream` | Timestamp formatting | SUPPORT | RETAIN — I/O utility |
| `std::string` | Text storage | SUPPORT | RETAIN — allowed by spec |
| `std::cout` / `std::cin` | Console I/O | SUPPORT | RETAIN — allowed by spec |
| `Fault* arr = new Fault[]` | Sorting array | ALREADY MANUAL | RETAIN — manual allocation |

**No `std::sort`, `std::find`, `std::map`, `std::unordered_map`, or other STL containers found in simulation logic.** ✅

---

### `main.cpp`
| Usage | Purpose | Classification | Action |
|-------|---------|---------------|--------|
| `std::cin`, `std::cout` | Console I/O | SUPPORT | RETAIN |
| `std::string` | Menu text | SUPPORT | RETAIN |
| `std::numeric_limits` | Input buffer clearing | SUPPORT | RETAIN |

---

## Summary Table

| Container / Algorithm | Used? | Manual Replacement | Status |
|----------------------|-------|-------------------|--------|
| `std::vector` | ❌ No | `CustomVector<T>` | ✅ Manual |
| `std::list` | ❌ No | `DoublyLinkedList<T>` | ✅ Manual |
| `std::stack` | ❌ No | `Stack<T>` (linked) + `ArrayStack<T>` (array) | ✅ Manual |
| `std::queue` | ❌ No | `Queue<T>` (linked) + `CircularQueue<T>` (array) | ✅ Manual |
| `std::priority_queue` | ❌ No | `MaxHeap` (binary heap array) | ✅ Manual |
| `std::map` | ❌ No | `HashTable<K,V>` (separate chaining) | ✅ Manual |
| `std::unordered_map` | ❌ No | `HashTable<K,V>` | ✅ Manual |
| `std::sort` | ❌ No | QuickSort, MergeSort, BubbleSort, SelectionSort, InsertionSort | ✅ Manual |
| `std::find` | ❌ No | LinearSearch, BinarySearch | ✅ Manual |
| `std::binary_search` | ❌ No | `SearchAlgorithms::binarySearchFaultById` | ✅ Manual |
| `std::string` | ✅ Yes | — | ✅ Retained (allowed) |
| `std::cout`/`std::cin` | ✅ Yes | — | ✅ Retained (allowed) |
| JSON library | ✅ Yes (HTTP API) | — | ✅ Retained (infrastructure) |

---

## Conclusion

The GridGuard codebase demonstrates **exemplary DSA compliance**. All core data structures and algorithms are manually implemented. The STL usage is restricted exclusively to I/O and string utilities, which are explicitly permitted by the project specification.

## Final Integration Addendum

The audit was updated after implementation. `Graph::runBFS()` now uses the manual array-based `CircularQueue<int>` (capacity `MAX_VERTICES`) in production. `Simulation` now stores every undo action in the dynamically resizing manual `ArrayStack<UndoAction>`. The older linked queue and stack headers remain as manual reference implementations; neither delegates work to STL containers.
