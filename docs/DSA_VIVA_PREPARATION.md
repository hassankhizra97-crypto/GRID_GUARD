# GridGuard DSA Viva Preparation

## Fast opening answer

“GridGuard implements **9 data-structure types** in source. Seven are active: adjacency-list graphs, a custom dynamic array, circular queue, array stack, max-heap, chained hash table, and doubly linked list. Two older linked implementations, `Queue<T>` and `Stack<T>`, are compiled reference code but are not used by current BFS or undo. We use two graph instances: the electrical graph for reachability and a separate road graph for Dijkstra routes.”

## Professor-style questions and answers

1. **Why did you choose an adjacency list rather than an adjacency matrix?**  
   Karachi's network is sparse: each location has a few real connections, not a connection to every other location. `Graph` and `RoadNetwork` store only real edges, so traversal is O(V+E) and neighbour inspection is O(degree). A matrix would reserve O(V²) cells.

2. **Why do you have two graphs?**  
   `powerGrid` represents electricity flow and line operational state. `roadNetwork` represents crew travel minutes. A line that transmits power is not automatically a drivable route, so mixing them would give incorrect simulations.

3. **What is stored in an electricity adjacency node?**  
   `AdjEdgeNode` stores neighbour location ID, power-line ID, repair cost, travel cost, and `next`. The line ID lets BFS ask whether that power line is operational.

4. **How does BFS use the circular queue?**  
   `Graph::runBFS` marks the source, enqueues it, then repeatedly dequeues one location and enqueues each newly discovered operational neighbour. FIFO ordering explores by distance in number of edges.

5. **Why circular queue instead of a normal array queue?**  
   A normal array queue often shifts every remaining item after dequeue, costing O(n). `CircularQueue` advances `front_` and `rear_` with modulo arithmetic, so both actions are O(1) and freed slots are reused.

6. **Why not use the linked `Queue<T>` in the codebase?**  
   It is a valid manual reference implementation, but production BFS uses `CircularQueue<int> q(MAX_VERTICES)`. The graph is capped at 128 vertices, so an array queue has known sufficient capacity and avoids per-node allocation.

7. **What happens if the circular queue becomes full?**  
   `enqueue` throws. In current BFS it cannot happen because a vertex is marked visited before enqueue and there are at most `MAX_VERTICES` vertices, exactly the queue capacity.

8. **How does DFS differ from BFS here?**  
   DFS in `Graph::dfsHelper` recursively follows one branch before returning. BFS uses a queue and visits by layers. Both can report reachability, but BFS is the actual method used by `analyzeConnectivity`.

9. **Why use a custom dynamic array?**  
   `CustomVector<T>` gives variable-size contiguous storage without `std::vector`. It is used for graph edge records, traversal outputs, and reconstructed routes, where O(1) index access matters.

10. **What is the cost of growing `CustomVector`?**  
   One resize is O(n), because it allocates a larger array and copies elements. Since capacity doubles, repeated appends are O(1) amortized.

11. **Why not use a linked list for paths and traversal results?**  
   GridGuard indexes and prints results in order. Arrays give O(1) index access and contiguous memory. A linked list would make indexing O(n) and allocate a node for each result.

12. **Why is undo a stack?**  
   Undo must reverse the most recent change first. `ArrayStack<UndoAction>` provides that LIFO rule. A queue would undo the oldest event, which is incorrect for ordinary undo behavior.

13. **What exactly is stored for undo?**  
   `UndoAction` records the action type, affected power-line ID, related fault ID where applicable, and a description. `undoLastOperation` uses it to restore a disabled line or disable a restored line.

14. **Why array stack instead of linked stack?**  
   The array stack is compact and grows automatically. The linked `Stack<T>` exists but is not active. A linked stack avoids an occasional O(n) resize but allocates one node per action.

15. **Why use a max-heap for repair scheduling?**  
   `processNextRepair` must repeatedly choose the greatest priority score, while failures can continue arriving. `MaxHeap` gives O(1) peek and O(log n) extraction/insertion.

16. **Why not use a sorted array as the repair queue?**  
   It makes reading the maximum easy but insertion O(n), because items must be shifted. A heap gives a better balance when faults arrive and repairs are processed interleaved.

17. **Why not use `std::priority_queue`?**  
   The lab requirement is to demonstrate the implementation. `MaxHeap` manually owns `Fault* heap`, calculates parent/child indexes, and performs heapify-up/down.

18. **What is the difference between a heap and a BST?**  
   A heap guarantees only parent-versus-child priority and makes the maximum available at the root. A balanced BST keeps full key ordering and supports range queries. GridGuard needs next-highest repair, not sorted/range traversal.

19. **Does the heap keep every fault fully sorted?**  
   No. Only the root is guaranteed maximum. `displayQueue` copies the heap and repeatedly extracts from the copy to print strict priority order without changing the real queue.

20. **Why use a hash table for locations and faults?**  
   The application asks for records by exact integer ID. `locationTable` and `faultTable` provide average O(1) lookup and update, faster than scanning all records as the dataset grows.

21. **What happens in a hash collision?**  
   Two keys select the same bucket. `HashTable` allocates a `HashNode` and links it at that bucket's chain head; lookup walks the chain and compares keys.

22. **Why separate chaining rather than open addressing?**  
   Chaining makes insert/delete logic easy to demonstrate with pointers and continues to work when a bucket has multiple records. Open addressing would need probing rules and deletion markers/rehashing care.

23. **What is hash-table worst case?**  
   O(n) if many keys fall into one chain. Average O(1) relies on a sensible hash distribution and controlled load factor. Current capacities are 101 for locations and 53 for faults.

24. **Why use a doubly linked list for history?**  
   The UI exposes chronological and reverse chronological history. `prev` pointers let `displayBackward` start at `tail` and walk backwards directly.

25. **Why not a singly linked list for incidents?**  
   It would save one pointer but cannot traverse backwards without rescanning from the head. It also cannot remove the tail in O(1) without finding its predecessor.

26. **Why not a dynamic array for incidents?**  
   It could support reverse indexing, but deletion near the front/middle shifts records. A doubly linked list keeps stable nodes and offers O(1) endpoint insertion/removal.

27. **How does Dijkstra work in this code exactly?**  
   `RoadNetwork::findShortestPath` initializes fixed `dist`, `visited`, and `parent` arrays. It repeatedly scans all vertices to select the unvisited smallest distance, relaxes adjacent road edges, then follows `parent` backwards into `reversePath`.

28. **Does Dijkstra use a priority queue in GridGuard?**  
   No. It uses manual linear minimum selection, so its time is O(V²+E). The max-heap is only for repair priorities, not road shortest paths.

29. **Why is Dijkstra valid here?**  
   Road travel minutes are non-negative. Dijkstra requires non-negative weights; negative travel time would invalidate its greedy selection.

30. **Which structure is most critical?**  
   The electricity adjacency-list graph is the central model: without it the project cannot represent connections, failures, BFS reachability, or affected areas. The other structures make operations on that model efficient and explainable.

31. **What would you change for millions of nodes?**  
   Remove fixed 128-element limits, use dynamically sized graph storage, add a manual min-heap for Dijkstra, resize/hash by load factor, and consider partitioning or external storage for large incident history. The present fixed arrays are appropriate only for the supplied small simulation.

32. **What is the trade-off of adjacency lists?**  
   They save memory for sparse graphs and give fast neighbour iteration, but testing one arbitrary pair of vertices is not constant-time as it is with a matrix.

33. **What is the trade-off of a circular queue?**  
   Constant-time operations and good locality, but a fixed capacity. GridGuard proves the bound from `MAX_VERTICES`; a general-purpose queue would need resizing.

34. **Which structures are not actively used, and why mention them?**  
   `Queue<T>` and `Stack<T>` are implemented in `include/Queue.h` and `include/Stack.h`, but source usage shows current BFS and undo use `CircularQueue` and `ArrayStack`. They are mentioned honestly as reference implementations, not claimed as runtime features.

35. **How does a power failure propagate through structures?**  
   Disable an edge in `Graph`; push undo action to `ArrayStack`; run BFS with graph + circular queue; update location/fault hash tables; insert the new fault into max-heap; append an incident event to doubly linked history.

36. **Why are algorithms not included in the data-structure count?**  
   BFS, DFS, Dijkstra, sorting, and searching specify procedures. Graphs, queues, heaps, arrays, hash tables and lists specify how data is stored and accessed. The analysis therefore counts structures only.

37. **Can binary search replace the hash table for faults?**  
   Not directly. Binary search needs an array sorted by fault ID and maintaining that order makes insertion/update costly. Hashing suits frequent exact lookup; binary search is demonstrated separately after explicit sorting.

38. **What proves this is not just theoretical code?**  
   `tests/test_suite.cpp` directly tests graph connectivity, Dijkstra, heap priority, hash collisions/deletion, incident-list endpoint deletion, circular-queue wraparound, array-stack growth/undo, and manual sort/search behavior.
