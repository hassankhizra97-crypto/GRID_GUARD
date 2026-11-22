#include "../include/Graph.h"
#include <iostream>
#include <iomanip>

Graph::Graph() : locationCount(0) {
    for (int i = 0; i < MAX_VERTICES; ++i) {
        locationExists[i] = false;
        adjList[i] = nullptr;
    }
}

Graph::~Graph() {
    clear();
}

void Graph::clear() {
    for (int i = 0; i < MAX_VERTICES; ++i) {
        AdjEdgeNode* curr = adjList[i];
        while (curr != nullptr) {
            AdjEdgeNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        adjList[i] = nullptr;
        locationExists[i] = false;
    }
    powerLines.clear();
    locationCount = 0;
}

Graph::Graph(const Graph& other) : locationCount(other.locationCount) {
    for (int i = 0; i < MAX_VERTICES; ++i) {
        locationExists[i] = other.locationExists[i];
        locations[i] = other.locations[i];
        adjList[i] = nullptr;
    }
    powerLines = other.powerLines;

    // Deep copy adjacency list
    for (int i = 0; i < MAX_VERTICES; ++i) {
        if (other.locationExists[i]) {
            AdjEdgeNode* curr = other.adjList[i];
            AdjEdgeNode** target = &adjList[i];
            while (curr != nullptr) {
                *target = new AdjEdgeNode(curr->neighborLocationId, curr->powerLineId,
                                          curr->repairCost, curr->travelCost);
                target = &((*target)->next);
                curr = curr->next;
            }
        }
    }
}

Graph& Graph::operator=(const Graph& other) {
    if (this != &other) {
        clear();
        locationCount = other.locationCount;
        for (int i = 0; i < MAX_VERTICES; ++i) {
            locationExists[i] = other.locationExists[i];
            locations[i] = other.locations[i];
        }
        powerLines = other.powerLines;

        for (int i = 0; i < MAX_VERTICES; ++i) {
            if (other.locationExists[i]) {
                AdjEdgeNode* curr = other.adjList[i];
                AdjEdgeNode** target = &adjList[i];
                while (curr != nullptr) {
                    *target = new AdjEdgeNode(curr->neighborLocationId, curr->powerLineId,
                                              curr->repairCost, curr->travelCost);
                    target = &((*target)->next);
                    curr = curr->next;
                }
            }
        }
    }
    return *this;
}

bool Graph::addLocation(const Location& loc) {
    if (loc.id < 0 || loc.id >= MAX_VERTICES) {
        return false;
    }
    if (!locationExists[loc.id]) {
        locationCount++;
    }
    locations[loc.id] = loc;
    locationExists[loc.id] = true;
    return true;
}

bool Graph::addPowerLine(const PowerLine& line) {
    if (line.fromLocationId < 0 || line.fromLocationId >= MAX_VERTICES ||
        line.toLocationId < 0 || line.toLocationId >= MAX_VERTICES) {
        return false;
    }
    if (!locationExists[line.fromLocationId] || !locationExists[line.toLocationId]) {
        return false;
    }

    // Check if line with same ID already registered
    for (size_t i = 0; i < powerLines.size(); ++i) {
        if (powerLines[i].id == line.id) {
            return false;
        }
    }

    powerLines.push_back(line);

    // Add bidirectional edge in adjacency list
    adjList[line.fromLocationId] = new AdjEdgeNode(line.toLocationId, line.id,
                                                   line.repairCost, line.travelCost,
                                                   adjList[line.fromLocationId]);

    adjList[line.toLocationId] = new AdjEdgeNode(line.fromLocationId, line.id,
                                                 line.repairCost, line.travelCost,
                                                 adjList[line.toLocationId]);
    return true;
}

bool Graph::disablePowerLine(int lineId) {
    for (size_t i = 0; i < powerLines.size(); ++i) {
        if (powerLines[i].id == lineId) {
            if (!powerLines[i].isOperational) {
                return false; // Already disabled
            }
            powerLines[i].isOperational = false;
            return true;
        }
    }
    return false;
}

bool Graph::restorePowerLine(int lineId) {
    for (size_t i = 0; i < powerLines.size(); ++i) {
        if (powerLines[i].id == lineId) {
            if (powerLines[i].isOperational) {
                return false; // Already operational
            }
            powerLines[i].isOperational = true;
            return true;
        }
    }
    return false;
}

bool Graph::isPowerLineOperational(int lineId) const {
    for (size_t i = 0; i < powerLines.size(); ++i) {
        if (powerLines[i].id == lineId) {
            return powerLines[i].isOperational;
        }
    }
    return false;
}

Location* Graph::getLocation(int id) {
    if (id >= 0 && id < MAX_VERTICES && locationExists[id]) {
        return &locations[id];
    }
    return nullptr;
}

const Location* Graph::getLocation(int id) const {
    if (id >= 0 && id < MAX_VERTICES && locationExists[id]) {
        return &locations[id];
    }
    return nullptr;
}

PowerLine* Graph::getPowerLine(int id) {
    for (size_t i = 0; i < powerLines.size(); ++i) {
        if (powerLines[i].id == id) {
            return &powerLines[i];
        }
    }
    return nullptr;
}

const PowerLine* Graph::getPowerLine(int id) const {
    for (size_t i = 0; i < powerLines.size(); ++i) {
        if (powerLines[i].id == id) {
            return &powerLines[i];
        }
    }
    return nullptr;
}

void Graph::getAllLocationIds(CustomVector<int>& outIds) const {
    outIds.clear();
    for (int i = 0; i < MAX_VERTICES; ++i) {
        if (locationExists[i]) {
            outIds.push_back(i);
        }
    }
}

void Graph::displayNetwork() const {
    std::cout << "========================================================================================\n";
    std::cout << "                         POWER GRID NETWORK TOPOLOGY & STATUS                           \n";
    std::cout << "========================================================================================\n";
    std::cout << "Total Locations Registered: " << locationCount
              << " | Total Power Transmission Lines: " << powerLines.size() << "\n\n";

    std::cout << "POWER TRANSMISSION LINES:\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    for (size_t i = 0; i < powerLines.size(); ++i) {
        const PowerLine& pl = powerLines[i];
        const Location* fromLoc = getLocation(pl.fromLocationId);
        const Location* toLoc = getLocation(pl.toLocationId);

        std::string fromName = fromLoc ? fromLoc->name : "Unknown";
        std::string toName = toLoc ? toLoc->name : "Unknown";

        std::cout << "Line #" << std::setw(2) << pl.id << ": "
                  << "[" << std::setw(2) << pl.fromLocationId << "] "
                  << std::setw(25) << std::left << fromName
                  << " <====> "
                  << "[" << std::setw(2) << pl.toLocationId << "] "
                  << std::setw(25) << std::left << toName
                  << " | " << (pl.isOperational ? "[OPERATIONAL]" : "[*** FAILED ***]")
                  << "\n";
    }
    std::cout << "========================================================================================\n";
}

void Graph::displayNeighbors(int locationId) const {
    const Location* loc = getLocation(locationId);
    if (!loc) {
        std::cout << "Location ID " << locationId << " does not exist.\n";
        return;
    }

    std::cout << "\nNeighbors of [" << locationId << "] " << loc->name << ":\n";
    AdjEdgeNode* curr = adjList[locationId];
    if (curr == nullptr) {
        std::cout << "   (No connected power lines)\n";
        return;
    }

    while (curr != nullptr) {
        const Location* nLoc = getLocation(curr->neighborLocationId);
        bool op = isPowerLineOperational(curr->powerLineId);
        std::cout << "   --> Connected to [" << std::setw(2) << curr->neighborLocationId << "] "
                  << std::setw(26) << std::left << (nLoc ? nLoc->name : "Unknown")
                  << " via Line #" << std::setw(2) << curr->powerLineId
                  << " (" << (op ? "OPERATIONAL" : "FAILED") << ")"
                  << " | Repair Est: $" << std::fixed << std::setprecision(0) << curr->repairCost
                  << "\n";
        curr = curr->next;
    }
}

void Graph::displayAllLocations() const {
    std::cout << "=================================================================================================\n";
    std::cout << "                                  GRIDGUARD CITY LOCATIONS                                       \n";
    std::cout << "=================================================================================================\n";
    for (int i = 0; i < MAX_VERTICES; ++i) {
        if (locationExists[i]) {
            locations[i].display();
        }
    }
    std::cout << "=================================================================================================\n";
}

// Custom BFS Traversal using our manual circular queue.  The graph holds at
// most MAX_VERTICES vertices, so that capacity is sufficient without resize.
void Graph::runBFS(int startVertexId, CustomVector<int>& reachableLocations, bool onlyOperational) const {
    reachableLocations.clear();
    if (startVertexId < 0 || startVertexId >= MAX_VERTICES || !locationExists[startVertexId]) {
        return;
    }

    bool visited[MAX_VERTICES] = {false};
    CircularQueue<int> q(MAX_VERTICES);

    visited[startVertexId] = true;
    q.enqueue(startVertexId);

    while (!q.isEmpty()) {
        int u = q.dequeue();
        reachableLocations.push_back(u);

        AdjEdgeNode* curr = adjList[u];
        while (curr != nullptr) {
            int v = curr->neighborLocationId;
            bool canTraverse = true;
            if (onlyOperational) {
                canTraverse = isPowerLineOperational(curr->powerLineId);
            }

            if (canTraverse && !visited[v] && locationExists[v]) {
                visited[v] = true;
                q.enqueue(v);
            }
            curr = curr->next;
        }
    }
}

// Recursive helper for DFS
void Graph::dfsHelper(int u, bool visited[], CustomVector<int>& traversalOrder, bool onlyOperational) const {
    visited[u] = true;
    traversalOrder.push_back(u);

    AdjEdgeNode* curr = adjList[u];
    while (curr != nullptr) {
        int v = curr->neighborLocationId;
        bool canTraverse = true;
        if (onlyOperational) {
            canTraverse = isPowerLineOperational(curr->powerLineId);
        }

        if (canTraverse && !visited[v] && locationExists[v]) {
            dfsHelper(v, visited, traversalOrder, onlyOperational);
        }
        curr = curr->next;
    }
}

// Custom DFS Traversal
void Graph::runDFS(int startVertexId, CustomVector<int>& traversalOrder, bool onlyOperational) const {
    traversalOrder.clear();
    if (startVertexId < 0 || startVertexId >= MAX_VERTICES || !locationExists[startVertexId]) {
        return;
    }

    bool visited[MAX_VERTICES] = {false};
    dfsHelper(startVertexId, visited, traversalOrder, onlyOperational);
}

// Connectivity analysis partitioning vertices into reachable vs disconnected
void Graph::analyzeConnectivity(int mainStationId, CustomVector<int>& connected, CustomVector<int>& disconnected) const {
    runBFS(mainStationId, connected, true);

    bool isReachable[MAX_VERTICES] = {false};
    for (size_t i = 0; i < connected.size(); ++i) {
        isReachable[connected[i]] = true;
    }

    disconnected.clear();
    for (int i = 0; i < MAX_VERTICES; ++i) {
        if (locationExists[i]) {
            if (!isReachable[i]) {
                disconnected.push_back(i);
            }
        }
    }
}
