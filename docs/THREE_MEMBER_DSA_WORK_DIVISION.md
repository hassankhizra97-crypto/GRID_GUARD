# GridGuard Three-Member DSA Work Division

## 1. Project DSA scope and audit methodology

This report audits only C++ headers and source files under `include/`, `src/`, `tests/`, and `main.cpp`. It ignores the React frontend and all non-C++ work. The assignment below is a **recommended viva ownership division**, not a claim about historical authorship: the repository has no per-component authorship evidence, so it would be inaccurate to say that any named member originally implemented a particular class.

The audit used declarations plus active call sites. A header is not treated as active merely because it exists. In particular, `Graph::runBFS()` constructs `CircularQueue<int>` and `Simulation` owns `ArrayStack<UndoAction>`; neither active workflow constructs the legacy linked `Queue<T>` or `Stack<T>` class.

## 2. Complete C++ DSA inventory

### Data structures

| Structure | Implementation files | Active source use | Status |
| --- | --- | --- | --- |
| Adjacency-list electricity graph | `include/Graph.h`, `src/Graph.cpp` | `Simulation::powerGrid` | Active |
| Adjacency-list road graph | `include/RoadNetwork.h`, `src/RoadNetwork.cpp` | `Simulation::roadNetwork` | Active |
| `CustomVector<T>` dynamic array | `include/CustomVector.h` | power lines, traversal results, paths | Active |
| `CircularQueue<T>` | `include/CircularQueue.h` | `Graph::runBFS` | Active |
| `ArrayStack<T>` | `include/ArrayStack.h` | `Simulation::arrayUndoStack` | Active |
| `MaxHeap` | `include/MaxHeap.h` | `Simulation::repairQueue` | Active |
| `HashTable<K,V>` with separate chaining | `include/HashTable.h` | location/fault records | Active |
| `DoublyLinkedList<T>` | `include/DoublyLinkedList.h` | incident history | Active |
| Linked `Queue<T>` | `include/Queue.h` | no production object/call site | Legacy/reference only |
| Linked `Stack<T>` | `include/Stack.h` | no production stack object; `UndoAction` type is included | Legacy/reference only |

The two graph classes are separate implementations with different edge-node types and meanings, but they use the same adjacency-list representation. Fixed arrays inside those classes (`locations[128]`, adjacency-head arrays, and Dijkstra `dist`, `visited`, `parent`) are internal storage, not separate standalone structures. `AdjEdgeNode`, `RoadEdgeNode`, `HashNode`, and `DNode` are node types internal to their owners.

### Algorithms

| Algorithm | Implemented in | Current use |
| --- | --- | --- |
| BFS | `Graph::runBFS`, `Graph::analyzeConnectivity` in `src/Graph.cpp` | connectivity, outage calculation, BFS menu |
| Recursive DFS | `Graph::dfsHelper`, `Graph::runDFS` in `src/Graph.cpp` | DFS menu/demo |
| Dijkstra, linear minimum selection | `RoadNetwork::findShortestPath` in `src/RoadNetwork.cpp` | repair route and route menu |
| Heapify up/down | `MaxHeap::heapifyUp`, `heapifyDown` | repair scheduling |
| Bubble, selection, insertion sort | `Sorting::*Sort` in `src/Sorting.cpp` | selected fault-sort display |
| Quick sort and merge sort | `Sorting::quickSort`, `partition`, `mergeSort`, `merge` | selected fault-sort display |
| Linear/binary search | `SearchAlgorithms` in `include/SearchAlgorithms.h` | fault-search demonstrations |
| Manual ID ordering before binary search | `Simulation::demonstrateBinarySearch` | binary-search precondition |

`main.cpp` calls the simulation menu methods; it does not implement a DSA algorithm itself. `tests/test_suite.cpp` verifies structures/algorithms but is test code, not production implementation.

## 3. Active versus inactive/legacy components

**Active:** all graph, vector, circular queue, array stack, heap, hash-table and doubly-linked-list classes in the table above. Sorting and searching are also implemented and reachable from the menu after the current refactor.

**Legacy/reference only:** `Queue<T>` in `include/Queue.h` and `Stack<T>` in `include/Stack.h`. They are valid manual linked implementations, but a source search finds no active `Queue<...>` or `Stack<...>` instance. `Stack.h` remains included because it declares `UndoAction`, which the active `ArrayStack<UndoAction>` stores. These should be discussed honestly but not presented as the live BFS/undo implementation.

## 4. Member 1 — Network model, graph traversal, and route finding

### Assigned components

- Electricity `Graph` adjacency list and its manual node/array storage.
- Road `RoadNetwork` adjacency list and Dijkstra shortest path.
- BFS and recursive DFS.
- The active circular queue specifically as the BFS dependency.

### Exact files and study targets

- `include/Graph.h`, `src/Graph.cpp`: `AdjEdgeNode`, `Graph`, `addLocation`, `addPowerLine`, `disablePowerLine`, `restorePowerLine`, `runBFS`, `dfsHelper`, `runDFS`, `analyzeConnectivity`.
- `include/RoadNetwork.h`, `src/RoadNetwork.cpp`: `RoadEdgeNode`, `RoadNetwork`, `addVertex`, `addRoad`, `findShortestPath`, `clear`, copy/assignment handling.
- `include/CircularQueue.h`: `CircularQueue<T>::enqueue`, `dequeue`, `peek`, `isEmpty`, `isFull`.
- Integration calls in `src/Simulation.cpp`: `loadSampleNetwork`, `simulatePowerLineFailure`, `processNextRepair`, `findRepairCrewRoute`, `runBFSTraversal`, `runDFSTraversal`, `analyzeAndDisplayConnectivity`.

### Implementation responsibility and internal explanation

`Graph` uses a location array plus an array of adjacency-list heads. `addPowerLine` allocates two `AdjEdgeNode`s for an undirected line. BFS marks a vertex before enqueueing it, then uses `CircularQueue<int>` to process vertices FIFO; it follows only operational lines. DFS instead recursively follows each unvisited operational neighbour. `analyzeConnectivity` runs BFS from the main station, marks reachable IDs in a local fixed array, then puts all non-reachable locations into `disconnected`.

`RoadNetwork` is intentionally separate. Its edges contain `travelMinutes`, while power edges represent electrical connections. Its Dijkstra implementation keeps fixed `dist[128]`, `visited[128]`, and `parent[128]` arrays. It chooses the smallest unvisited distance by scanning vertices—not by using a min-heap—relaxes neighbours, then reconstructs the target route by following `parent` backward.

### Why these components belong together

They are the network/intelligence layer: one member can explain the domain representation, graph traversal after line failure, and physical repair routing without owning repair priority or record storage. The circular queue belongs here because it is directly constructed only inside `Graph::runBFS`.

### Complexity

| Operation | Complexity |
| --- | --- |
| Add adjacency node/edge | O(1), excluding duplicate scan in `Graph::addPowerLine` which is O(E) |
| BFS / DFS | O(V + E) time, O(V) auxiliary space |
| Connectivity partition after BFS | O(V + E) time, O(V) space |
| Circular enqueue/dequeue | O(1) each |
| Current Dijkstra | O(V² + E) time, O(V) auxiliary space |

### Integration responsibility

Member 1 supplies connected/disconnected and road-path results. Member 2 consumes connectivity results to calculate a fault's impact and uses a road route before restoration. Member 3 consumes/creates dynamic arrays used to return traversal and path IDs.

### Likely viva questions

- Why do electricity and road networks need separate graph instances?
- Why adjacency lists rather than an adjacency matrix?
- How does a disabled power line affect BFS?
- Why does BFS use FIFO order and DFS use recursion here?
- Why is the circular queue capacity safe at 128?
- Why is Dijkstra O(V² + E) in this code rather than O((V+E) log V)?

## 5. Member 2 — Fault priority, fast lookup, history, and undo

### Assigned components

- `MaxHeap` emergency repair scheduler.
- Separate-chaining location and fault hash tables.
- Doubly linked incident history.
- Array-stack undo mechanism and the simulation workflows that coordinate these structures.

### Exact files and study targets

- `include/MaxHeap.h`: `resize`, `parent`, `leftChild`, `rightChild`, `heapifyUp`, `heapifyDown`, `insert`, `extractMax`, `peekMax`, `displayQueue`.
- `include/HashTable.h`: `HashNode`, both `hashFunction` overloads, `insert`, `search`, `searchPtr`, `update`, `remove`, `displayBucketDiagnostics`.
- `include/DoublyLinkedList.h`: `DNode`, `append`, `prepend`, `removeFirst`, `removeLast`, `displayForward`, `displayBackward`, `searchEvents`.
- `include/ArrayStack.h`: `resize`, `push`, `pop`, `peek`, `forEachTopDown`.
- `include/Stack.h`: `UndoAction` and `UndoActionType` only; explain that `Stack<T>` itself is legacy.
- `include/Simulation.h`, `src/Simulation.cpp`: ownership fields; `logEvent`, `simulatePowerLineFailure`, `processNextRepair`, `restorePowerLineDirectly`, `undoLastOperation`, lookup/display methods.

### Implementation responsibility and internal explanation

When a failure occurs, `simulatePowerLineFailure` calculates a `Fault`, inserts it into `repairQueue` and `faultTable`, pushes an `UndoAction` into `arrayUndoStack`, and appends an incident using `logEvent`. The heap keeps the largest `priorityScore` at index zero. On repair, `processNextRepair` extracts that root fault and later records restoration. On undo, `undoLastOperation` pops the last action and performs its inverse.

The two hash tables map integer IDs to `Location` and `Fault` values. A modulo hash picks a bucket; collision nodes are joined with `next` pointers. `searchPtr` allows mutation of a fault in its bucket. The doubly linked list appends events at `tail`; it can display forwards through `next` or backwards through `prev`.

### Why these components belong together

They are all fault-lifecycle state: create/find a fault, decide which fault is repaired next, record what happened, and reverse the latest reversible action. They have meaningful interaction inside a single group of `Simulation` methods, yet are separate from Member 1's graph algorithms.

### Complexity

| Operation | Complexity |
| --- | --- |
| Heap insert/extract / peek | O(log n) / O(log n) / O(1) |
| Hash insert/search/update/remove | O(1) average, O(n) worst chain |
| Incident append / endpoint removal | O(1) |
| Incident forward/backward scan | O(n) |
| Stack push/pop/peek | O(1) amortized / O(1) / O(1) |

### Integration responsibility

Member 2 receives outage impact from Member 1's BFS to calculate and store fault information. It calls Member 1's Dijkstra route before repair. It stores/updates record data for Member 3's manual sort/search demonstrations.

### Likely viva questions

- How do heap parent/child indexes implement a tree in an array?
- Why is a heap more suitable than a sorted array for live repair scheduling?
- What happens to a hash-table collision in this source?
- Why does `searchPtr` exist in addition to `search`?
- Why use a doubly linked list for history instead of a singly linked list?
- Why must undo be LIFO, and what exactly does an `UndoAction` store?

## 6. Member 3 — Manual arrays, sorting, searching, and result presentation

### Assigned components

- `CustomVector<T>` dynamic-array implementation.
- All five manual fault sorting implementations.
- Manual linear and binary fault/location search helpers and search demonstrations.
- Collection/display integration for sorted and searched fault data.

### Exact files and study targets

- `include/CustomVector.h`: constructor/destructor, rule-of-three methods, `resize`, `push_back`, `pop_back`, indexing, `clear`.
- `include/Sorting.h`, `src/Sorting.cpp`: `SortCriterion`, `SortAlgorithm`, `compare`, `partition`, `quickSort`, `merge`, `mergeSort`, `bubbleSort`, `selectionSort`, `insertionSort`.
- `include/SearchAlgorithms.h`: `linearSearchFaultById`, `linearSearchFaultByLineId`, location linear searches, `binarySearchFaultById`, `binarySearchFaultByScore`, `demonstrateSearchComparison`.
- `include/Simulation.h`, `src/Simulation.cpp`: `sortAndDisplayFaults`, `sortAndDisplayFaultsAlgo`, `demonstrateLinearSearch`, `demonstrateBinarySearch`, `demonstrateSearchComparison`.
- `main.cpp`: menu options 12 and 24 dispatch the sorting and search-comparison demonstrations. Option 11 is the hash-table fault lookup and remains Member 2's record-lookup responsibility.

### Implementation responsibility and internal explanation

`CustomVector` grows by allocating a larger `T[]`, copying active values, releasing old storage, and updating capacity. It backs `Graph::powerLines`, BFS/DFS outputs, and Dijkstra path output. Sorting collects faults from `faultTable` into a manually allocated `Fault[]`; the selected sort then orders that temporary array by priority, severity, population, or outage duration. `sortAndDisplayFaultsAlgo` dispatches the five hand-written algorithms.

`SearchAlgorithms` performs linear scans where no ordering is assumed. Binary search repeatedly halves a `Fault[]` ordered by `faultId`; `Simulation::demonstrateBinarySearch` explicitly insertion-sorts its temporary array by fault ID first. Hash lookup is also available in the normal “search fault/location by ID” methods, but that is Member 2's record-storage responsibility; Member 3 should explain the manual comparison algorithms rather than falsely calling the hash-table lookup binary search.

### Why these components belong together

This group owns storage growth plus the algorithms that consume array-like data. It has substantial algorithmic depth—five sort implementations and two search strategies—so it balances the graph/Dijkstra and fault-lifecycle groups without duplicating their ownership.

### Complexity

| Operation | Complexity |
| --- | --- |
| Dynamic-array append/index | O(1) amortized / O(1); a resize is O(n) |
| Bubble/selection/insertion sort | O(n²) worst; insertion is O(n) best on sorted input |
| Quick sort | O(n log n) average, O(n²) worst |
| Merge sort | O(n log n) time, O(n) auxiliary space |
| Linear search | O(n) |
| Binary search after sorting | O(log n), but sorting first costs at least O(n log n) or O(n²) here |

### Integration responsibility

Member 3's vector carries results from Member 1's graph operations and route reconstruction. Its sorting/search demonstrations read faults through Member 2's `faultTable`; they do not change the live heap scheduling order.

### Likely viva questions

- What makes `CustomVector` dynamic, and when do existing references become unsafe?
- Why not use `std::vector` or `std::sort` in a DSA lab?
- Which sort is stable in this implementation, and why might stability matter?
- Why must binary search be preceded by sorting?
- Why is binary search not automatically the best fault lookup here?
- Compare QuickSort's average/worst case with MergeSort's guarantees.

## 7. Workload fairness comparison

| Workload factor | Member 1 | Member 2 | Member 3 |
| --- | --- | --- | --- |
| Data structures | Two graph instances, circular queue | Max-heap, hash table, doubly list, array stack | Dynamic array; consumes arrays from other modules |
| Algorithms | BFS, DFS, Dijkstra | Heapify and undo/inverse workflow | Five sorts, linear/binary search |
| Files involved | 5 core + simulation integration | 6 core + simulation integration | 5 core + simulation/menu integration |
| Implementation difficulty | High: pointer graphs, traversal, shortest path | High: several structures with coordinated state transitions | High: memory management plus seven algorithms |
| Integration responsibility | Produces connectivity/routes for lifecycle | Central fault lifecycle; calls graph/route and exposes records | Reads graph/fault results for output/analysis |
| Overall workload | High | High | High |

### Fairness assessment

The counts of classes are intentionally unequal: Member 2 owns more individual structures, while Member 1 owns two graph implementations plus three major graph algorithms, and Member 3 owns five sorting implementations plus two search families. That is a more honest balance than assigning the same number of headers to each person.

Member 2 is the integration-heavy owner and should prepare the most end-to-end explanation. Member 1 has the deepest algorithmic reasoning because Dijkstra and graph traversal are central. Member 3 has the largest algorithm count but less mutation of live simulation state. Each member has both an implementable structure and nontrivial algorithmic work, so no one is assigned only easy utility code.

## 8. Shared components and integration dependencies

The following are shared **interfaces**, not shared ownership of implementation:

1. Member 1 owns graph/route calculations; Member 2 calls them in failure and repair workflows.
2. Member 2 owns live fault records and priority scheduling; Member 3 reads `faultTable` into a temporary `Fault[]` to sort or search.
3. Member 3 owns `CustomVector`; Member 1 uses it as output storage for BFS, DFS and Dijkstra. Member 1 should explain the result semantics; Member 3 should explain resizing/indexing internals.
4. `Simulation` is the orchestration point. No member should claim sole ownership of every `Simulation.cpp` function: its calls cross all three modules.
5. `tests/test_suite.cpp` is shared verification evidence. It does not change responsibility for production code.

## 9. Individual viva preparation topics

### Member 1 focus

Draw the two networks; trace line #48 failure through `disablePowerLine` → `analyzeConnectivity` → `runBFS`; then trace a repair route through `findShortestPath`. Be ready to distinguish power-line IDs from road-edge travel weights and to state that Dijkstra uses array min-selection, not a min-heap.

### Member 2 focus

Trace one fault from creation to heap insertion, table insertion, incident append, repair extraction, restoration, and undo. Be able to draw a hash collision chain, a three-element heap, a doubly linked list in both directions, and a stack top.

### Member 3 focus

Draw dynamic-array resize/copy, explain each sorting algorithm's loop/recursion, and demonstrate why a binary search must sort by the same key first. Be ready to state that normal exact fault lookup uses a hash table, while the search algorithms are explicit academic demonstrations.

## 10. Final recommended division

| Member | Defensible ownership statement |
| --- | --- |
| Member 1 | “I am responsible for both manual adjacency-list networks, circular-queue BFS, recursive DFS, and Dijkstra route computation.” |
| Member 2 | “I am responsible for fault lifecycle structures: max-heap scheduling, chained hash records, doubly linked history, array-stack undo, and their `Simulation` workflow.” |
| Member 3 | “I am responsible for the custom dynamic array plus manual sorting/searching and the temporary fault-array analysis/display flow.” |

All members should state that this is a recommended division based on current source. No source or Git evidence attributes original authorship to a specific person. The linked `Queue<T>` and `Stack<T>` should be mentioned only as inactive reference implementations, not assigned as core live work.
