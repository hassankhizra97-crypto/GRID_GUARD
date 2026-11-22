# GridGuard DSA Viva Guide

## Quick questions and answers

**Why use a graph?** The power grid is naturally a graph: locations are vertices and transmission lines are edges. A line can be disabled without deleting the physical connection.

**Why an adjacency list instead of an adjacency matrix?** Karachi's 44 locations have far fewer than 44 x 44 possible lines. Linked adjacency nodes store only real connections, so memory is O(V + E), rather than O(V^2).

**How does BFS use a queue?** It enqueues the source, removes the oldest location, then enqueues each unvisited operational neighbour. The `CircularQueue<int>` makes this FIFO order O(1) per enqueue/dequeue. BFS identifies locations still powered from the main station.

**How does DFS differ from BFS?** DFS follows one neighbour chain recursively before returning; BFS visits level by level. DFS is useful for demonstrating graph traversal; BFS is used for reachability/outage analysis.

**How does the binary heap maintain priority?** In `MaxHeap`, each parent has a priority at least as high as its children. Insert heapifies upward; extraction moves the final element to the root and heapifies downward. Both are O(log n).

**How does hashing handle collisions?** `HashTable` maps a key to a bucket. If two keys select the same bucket, separate chaining links their nodes. Search and deletion walk only that bucket's chain.

**Why a doubly linked incident list?** Every event has `prev` and `next` pointers. This makes forward and backward history traversal direct, and allows O(1) deletion at either end.

**How does undo work?** Every reversible operation pushes an `UndoAction` onto the manual array stack. Popping the most recent action restores a disabled line or re-disables a restored line, so it obeys LIFO.

**How does Dijkstra reconstruct a path?** It stores the predecessor of each improved vertex in a `parent[]` array. Starting at the destination and following parents backwards gives the route, which is reversed into the output array.

**Why must binary search use sorted data?** It discards one half after comparing the middle. That decision is only valid when all smaller values are on one side and larger values on the other. GridGuard explicitly insertion-sorts by fault ID before its binary-search demonstration.

## Complexity recap

| Feature | Time complexity |
| --- | --- |
| BFS / DFS | O(V + E) |
| Dijkstra (array min-selection) | O(V^2 + E) |
| Hash lookup | O(1) average, O(chain length) worst |
| Heap insert/extract | O(log n) |
| Array stack push/pop | O(1) amortized / O(1) |
| Circular queue enqueue/dequeue | O(1) |
| Bubble/selection/insertion sort | O(n^2) |
| Quick sort average / merge sort | O(n log n) |
| Linear / binary search | O(n) / O(log n) |
