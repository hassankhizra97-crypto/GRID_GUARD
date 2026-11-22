# GridGuard Data Structures Comparative Analysis

## Concise source-verified summary

**Exact count: 9 distinct implemented data-structure types.** Seven are active in GridGuard's runtime workflows; two are legacy/manual reference implementations that are compiled but not called by the current application.

| # | Structure | Runtime status | Primary role |
| --- | --- | --- | --- |
| 1 | Adjacency-list graph | Active, two instances | Electricity topology and separate repair-road topology |
| 2 | `CustomVector<T>` dynamic array | Active | Expandable edge, traversal and path result storage |
| 3 | `CircularQueue<T>` | Active | FIFO frontier for electricity-grid BFS |
| 4 | `ArrayStack<T>` | Active | LIFO undo log |
| 5 | `MaxHeap` | Active | Highest-priority fault/repair scheduling |
| 6 | `HashTable<K,V>` with separate chaining | Active | Location and fault lookup/update |
| 7 | `DoublyLinkedList<T>` | Active | Chronological and reverse incident history |
| 8 | `Queue<T>` linked-list queue | Implemented, not called | Legacy/reference queue; BFS no longer uses it |
| 9 | `Stack<T>` linked-list stack | Implemented, not called as a stack | Legacy/reference stack; its `UndoAction` type is still included |

`Graph` and `RoadNetwork` are two *uses* of the same adjacency-list graph idea, not two distinct kinds of data structure. Fixed C-style arrays (`locations[128]`, `adjList[128]`, and Dijkstra's `dist`, `visited`, `parent`) are internal storage primitives, not additional standalone classes in this count. Sorting and searching are algorithms, so they are deliberately excluded.

## Evidence and actual usage map

`Simulation` owns the active structures in [include/Simulation.h](../include/Simulation.h): `powerGrid`, `roadNetwork`, `repairQueue`, `locationTable`, `faultTable`, `incidentHistory`, and `arrayUndoStack`. Its `loadSampleNetwork()` method in [src/Simulation.cpp](../src/Simulation.cpp) constructs both networks and inserts locations into the hash table. There is no C++ HTTP/REST API server in this repository snapshot; this analysis therefore makes no claim that these structures directly serialize frontend data.

## 1. Adjacency-list graph

**What it is.** A graph models things and their connections. GridGuard models a location as a vertex and a power line or road as an edge. An adjacency list stores, for each vertex, a linked chain of only its neighbours.

**Implementation and usage.** `Graph` in [include/Graph.h](../include/Graph.h) and [src/Graph.cpp](../src/Graph.cpp) stores `Location locations[128]`, `bool locationExists[128]`, `AdjEdgeNode* adjList[128]`, and `CustomVector<PowerLine> powerLines`. It is `Simulation::powerGrid`. `RoadNetwork` in [include/RoadNetwork.h](../include/RoadNetwork.h) and [src/RoadNetwork.cpp](../src/RoadNetwork.cpp) has the same adjacency-list pattern with `RoadEdgeNode* adjList[128]`; it is `Simulation::roadNetwork`. `loadSampleNetwork()` adds 44 locations/power edges and road edges.

**How it works.** `addPowerLine()` first appends the line record, then allocates two `AdjEdgeNode`s because a power line is bidirectional. `runBFS()` follows each node's `next` pointer, ignores disabled power lines via `isPowerLineOperational()`, and records reachable vertices. `runDFS()` does the same recursively. `RoadNetwork::findShortestPath()` walks road adjacency nodes while Dijkstra relaxes their travel weights.

**Problem solved and selection.** The electricity graph answers “what remains connected to the main station?” after a failure. The road graph answers “how should a crew reach the damaged line?” Keeping them separate prevents electrical connectivity from being confused with travel time.

| Comparison | Selected adjacency list | Adjacency matrix | Edge list |
| --- | --- | --- | --- |
| Storage | O(V + E); stores existing links only | O(V²), including absent links | O(E) |
| Find neighbours of one vertex | O(degree) | O(V) scan of a row | O(E) scan |
| Test/disable a known line | Current code scans `powerLines`: O(E) | O(1) cell if line ID mapping exists | O(E) |
| Best fit | Sparse Karachi networks and traversals | Very dense, bounded graph with frequent pair-existence queries | Simple edge iteration or tiny graphs |

The selected form costs pointer allocations and does not give O(1) “is u connected to v?” lookup. An adjacency matrix would be reasonable if almost every location connected directly to most others, or if millions of edge-existence checks dominated. An edge list would be easier to load but would make BFS/DFS repeatedly scan unrelated edges. Here BFS/DFS and Dijkstra repeatedly need neighbours, so adjacency lists fit the actual calls.

## 2. `CustomVector<T>` dynamic array

**What it is.** A growable contiguous array. It is a manual replacement for `std::vector`, not an STL container.

**Implementation and usage.** [include/CustomVector.h](../include/CustomVector.h) owns `T* data`, `capacity_`, and `size_`. It stores `Graph::powerLines`; it receives BFS/DFS results in `Graph::runBFS`/`runDFS`; it stores Dijkstra output and `reversePath` in `RoadNetwork::findShortestPath`; and it is used repeatedly for connected/disconnected/route lists in `Simulation`.

**How it works.** `push_back` checks `size_ >= capacity_`; on overflow `resize` allocates a larger array, copies all live elements, deletes the old array, then appends. Indexing is direct pointer arithmetic. `clear` resets logical size but retains capacity.

| Comparison | Dynamic array | Fixed static array | Linked list |
| --- | --- | --- | --- |
| Indexed access | O(1) | O(1) | O(n) |
| Append | O(1) amortized | O(1) until fixed capacity, then impossible | O(1) with tail pointer |
| Memory/layout | Contiguous; occasional resize copy | Contiguous; may waste fixed slots | One allocation and pointer per node |
| Better when | Size is variable and indexed iteration matters | Hard maximum is known and small | Frequent middle insertion/deletion dominates |

GridGuard needs arrays that grow as paths, traversal results and power-line records grow, then iterates by index. A static array would require a chosen hard limit everywhere. A linked list would avoid resize copies but makes indexed access and cache-friendly traversal worse. The trade-off is that a growth event is O(n), and pointers/references can become invalid after resize.

## 3. `CircularQueue<T>`

**What it is.** A fixed-capacity FIFO queue stored in an array whose rear and front wrap using modulo arithmetic.

**Implementation and usage.** [include/CircularQueue.h](../include/CircularQueue.h) has `data`, `front_`, `rear_`, `size_`, and `cap`. The actual production call is `CircularQueue<int> q(MAX_VERTICES)` in `Graph::runBFS()` in [src/Graph.cpp](../src/Graph.cpp). It is therefore the BFS worklist for connectivity analysis, outage reporting and BFS menu traversal.

**How it works.** Mark and enqueue the source. Repeatedly dequeue the oldest location, inspect adjacency nodes, mark each newly discovered reachable neighbour, and enqueue it. `rear_ = (rear_ + 1) % cap` reuses slots vacated by dequeues; `front_` advances the same way.

| Comparison | Circular array queue | Ordinary shifting array queue | Linked-list queue |
| --- | --- | --- | --- |
| Enqueue/dequeue | O(1) / O(1) | O(1) / O(n) if dequeue shifts items | O(1) / O(1) |
| Memory | One fixed array | One fixed/growing array | One allocation per item plus pointers |
| Limitation | Full at `cap` | Wasted front space or shift cost | Allocation overhead and poorer locality |

The capacity is deliberately `MAX_VERTICES` (128), and the graph cannot have more vertices than that, so BFS cannot overflow it under the present graph invariant. A linked queue would also be correct but allocates a node for every discovered location. A resizable circular queue would be needed if the graph maximum were removed.

## 4. `ArrayStack<T>`

**What it is.** A LIFO collection backed by a resizable array; the last pushed action is the first action undone.

**Implementation and usage.** [include/ArrayStack.h](../include/ArrayStack.h) maintains `data`, `topIdx`, and `cap`. `Simulation::arrayUndoStack` in [include/Simulation.h](../include/Simulation.h) receives actions in `simulatePowerLineFailure`, `processNextRepair`, and `restorePowerLineDirectly`; `undoLastOperation` pops one action in [src/Simulation.cpp](../src/Simulation.cpp).

**How it works.** Push increments `topIdx` after growing if necessary. Pop returns `data[topIdx--]`. Each `UndoAction` records whether a line was disabled/restored, its line ID, optional fault ID, and description. On pop, `undoLastOperation` performs the inverse grid operation, updates a fault where necessary, and reruns connectivity analysis.

| Comparison | Array stack | Linked-list stack | Queue |
| --- | --- | --- | --- |
| Push/pop | O(1) amortized / O(1) | O(1) / O(1) | FIFO, so wrong undo order |
| Memory | Contiguous; resize copy occasionally | Node/pointer per entry | Same possible storage but different semantics |
| Better when | Typical undo log with compact storage | Size is unpredictable and allocations are acceptable | Processing in original arrival order |

Undo must reverse the **most recent** change, so a stack is semantically necessary; replacing it with a queue would undo the oldest change instead. The array version is compact and grows from an initial 32 slots. A linked stack would avoid resize copying, but brings one allocation and pointer per operation.

## 5. `MaxHeap`

**What it is.** A complete binary tree represented in an array, where every parent's priority score is at least that of its children.

**Implementation and usage.** [include/MaxHeap.h](../include/MaxHeap.h) owns `Fault* heap`, `heapSize`, and `capacity`. `Simulation::repairQueue` receives fault records through `insert` in `simulatePowerLineFailure`; `processNextRepair` uses `peekMax`/`extractMax`; `displayRepairQueue` calls `displayQueue`.

**How it works.** Child and parent indexes are calculated as `(i - 1) / 2`, `2i + 1`, and `2i + 2`. Insertion appends, then `heapifyUp` swaps while the parent has a lower `priorityScore`. Extraction saves root zero, moves the last item there, then `heapifyDown` swaps it with the larger child.

| Comparison | Binary max-heap | Sorted array | Unsorted array |
| --- | --- | --- | --- |
| Insert | O(log n) | O(n) to keep order | O(1) |
| Peek highest | O(1) | O(1) | O(n) |
| Extract highest | O(log n) | O(1) then shift O(n) in this layout | O(n) to find it |
| Better when | Interleaved arrivals and next-repair extraction | Mostly read ordered reports, few inserts | Many inserts and rare extraction |

Faults can arrive while repairs are being processed, so GridGuard needs both insertion and “next highest priority” to stay efficient. A heap does not produce a fully sorted array—only the root is guaranteed maximum—so `displayQueue` makes a temporary heap copy and extracts from it. A balanced BST could also work and support ordered traversal, but is not implemented here and adds pointer/rotation complexity.

## 6. `HashTable<K,V>` with separate chaining

**What it is.** An array of buckets; each bucket heads a singly linked chain of entries whose keys hash to that index.

**Implementation and usage.** [include/HashTable.h](../include/HashTable.h) implements `HashNode`, `HashNode** buckets`, `capacity`, element/collision counts, `insert`, `search`, `searchPtr`, `update`, and `remove`. `Simulation` constructs `locationTable(101)` and `faultTable(53)`. Locations are inserted in `loadSampleNetwork`; faults are inserted/updated in failure/repair/undo workflows; lookup menu calls `searchLocationById` and `searchFaultById`.

**How it works.** Integer key modulo capacity chooses a bucket. `insert` first walks that bucket to update an existing key; otherwise it allocates a new node at that chain's head. `search` walks only that chain. Collisions are counted when a new key arrives in a non-empty bucket. `remove` repairs either the bucket head or the previous node's `next` pointer.

| Comparison | Chained hash table | Linear search in array/list | Binary-search tree |
| --- | --- | --- | --- |
| Exact lookup average | O(1) | O(n) | O(log n) if balanced |
| Worst lookup | O(n), if one long chain | O(n) | O(n) if unbalanced |
| Ordered traversal | No | Only storage order | Yes, in-order |
| Better when | Fast ID lookup/update is primary | Very few records or no extra index | Need sorted/range queries |

Location and fault IDs are exact-key lookups, not range queries, which favours hashing. Separate chaining is simple to explain and deletion does not require tombstones. It does allocate one node per entry and needs sensible capacities/load factors. If GridGuard needed “all fault IDs between 200 and 300,” a balanced BST would be a stronger choice.

## 7. `DoublyLinkedList<T>`

**What it is.** A sequence of nodes where each node points to both its predecessor and successor.

**Implementation and usage.** [include/DoublyLinkedList.h](../include/DoublyLinkedList.h) defines `DNode`, `head`, `tail`, and `count`; it offers `append`, `prepend`, `removeFirst`, `removeLast`, forward/backward display and keyword scan. `Simulation::logEvent` appends an `IncidentEvent`; the history menu calls `displayIncidentHistoryForward` and `displayIncidentHistoryBackward`.

**How it works.** `append` links its new node after `tail`, changing both the old tail's `next` and the new node's `prev`. Reverse display starts at `tail` and repeatedly follows `prev`. End removals reconnect the new endpoint and delete the old node.

| Comparison | Doubly linked list | Singly linked list | Dynamic array |
| --- | --- | --- | --- |
| Append with tail | O(1) | O(1) with tail | O(1) amortized |
| Reverse traversal | O(n), direct `prev` links | Not directly possible | O(n) by reverse indexing |
| Delete known end | O(1) both ends | O(1) head, O(n) tail | O(1) last, O(n) first/middle shifting |
| Cost | Two pointers per node | One pointer per node | Contiguous resize/shifting costs |

GridGuard visibly offers reverse incident history, so `prev` is not theoretical: it is used by `displayBackward`. The cost is extra pointer memory and careful link maintenance. A dynamic array would be better for indexed history pages; a singly linked list would be enough if reverse history were removed.

## 8–9. Implemented but inactive linked queue and linked stack

[include/Queue.h](../include/Queue.h) implements `Queue<T>` using `frontNode`, `rearNode`, and singly linked nodes. [include/Stack.h](../include/Stack.h) implements `Stack<T>` using `topNode` and singly linked nodes. Source search finds no `Queue<...>` or `Stack<...>` object in active production code. `Simulation.h` includes `Stack.h` because `UndoAction` is declared there, but it declares `ArrayStack<UndoAction>`, not `Stack<UndoAction>`.

They count as implemented structures but must not be presented as actively powering BFS or undo. Their relevant operations are O(1) enqueue/dequeue or push/pop, with O(n) space and node allocation overhead. They would be reasonable alternatives if fixed capacities/resizing were unacceptable, but the current source intentionally chose array variants.

## How the active structures work together

### Failure → outage → repair → undo

1. `Simulation::simulatePowerLineFailure` obtains a `PowerLine` from `Graph`, disables it, and pushes an `UndoAction` to `ArrayStack`.
2. It calls `Graph::analyzeConnectivity`, which calls `Graph::runBFS`. BFS uses the adjacency list plus `CircularQueue<int>` to form connected and disconnected `CustomVector<int>` results.
3. The method updates affected locations in `locationTable`, creates a `Fault`, inserts it into `faultTable`, and inserts the same fault into the `MaxHeap` repair queue.
4. `processNextRepair` extracts the max-priority fault from the heap. It calls `RoadNetwork::findShortestPath`, which runs array-based Dijkstra: fixed `dist[128]`, `visited[128]`, and `parent[128]` arrays plus adjacency-list road edges. **It does not use a min-heap or any priority queue.**
5. After restoration, connectivity BFS runs again and the result updates the hash table. An incident event is appended to the doubly linked history and a reverse action is pushed to the array stack.
6. `undoLastOperation` pops the stack, applies the inverse line operation, and reruns connectivity. This is why LIFO is essential.

## Consolidated complexity table

| Structure | Main GridGuard operations | Time | Space | Suitability |
| --- | --- | --- | --- | --- |
| Adjacency-list graph | add edge, BFS/DFS, Dijkstra neighbour scan | add O(1); traversal O(V+E) | O(V+E) | Sparse city connections and neighbour traversal |
| `CustomVector` | append lines/results/path; index | append O(1) amortized, resize O(n), index O(1) | O(n) | Expandable indexed results |
| Circular queue | BFS enqueue/dequeue | O(1) each | O(V) fixed here | Bounded graph frontier |
| Array stack | undo push/pop/peek | O(1) amortized/O(1)/O(1) | O(u) | Latest action is undone first |
| Max heap | insert, peek, extract repair | O(log n), O(1), O(log n) | O(n) | Interleaved fault arrivals and priority repairs |
| Chained hash table | insert/search/update/remove | average O(1), worst O(n) | O(n + buckets) | Exact ID lookup/update |
| Doubly linked list | append, reverse traverse, end removal | O(1), O(n), O(1) | O(n) | Chronological and reverse incidents |
| Linked `Queue` (inactive) | enqueue/dequeue | O(1)/O(1) | O(n) | Reference only |
| Linked `Stack` (inactive) | push/pop | O(1)/O(1) | O(n) | Reference only |

For this implementation, Dijkstra is O(V² + E), because each iteration linearly selects the unvisited vertex with minimum distance. The fixed `MAX_VERTICES = 128` makes this acceptable for the supplied network. For millions of nodes, replace fixed arrays with dynamically sized storage and use a manual min-heap to obtain approximately O((V+E) log V) Dijkstra time; also use a resizable graph representation and capacity/load-factor management for hash tables.

## Documentation/source consistency notes

- `docs/DSA_AUDIT.md` historically describes the linked `Queue` and `Stack` as the queue/stack implementations, then adds a final integration note. The source is authoritative: `Graph.cpp` uses `CircularQueue`; `Simulation.cpp` uses `ArrayStack`.
- `docs/MANUAL_DSA_IMPLEMENTATIONS.md` describes a linked `Queue` in its table of contents. That class exists, but it is not the queue used by current BFS.
- The user-facing labels “priority queue” refer to `MaxHeap`, not `std::priority_queue`.
- `RoadNetwork::findShortestPath` uses manual array minimum selection, not a priority queue/min-heap. Do not claim otherwise in a viva.
