#include "../include/RoadNetwork.h"
#include <iostream>
#include <iomanip>

RoadNetwork::RoadNetwork() : vertexCount(0) {
    for (int i = 0; i < MAX_VERTICES; ++i) {
        adjList[i] = nullptr;
        vertexExists[i] = false;
    }
}

RoadNetwork::~RoadNetwork() {
    clear();
}

void RoadNetwork::clear() {
    for (int i = 0; i < MAX_VERTICES; ++i) {
        RoadEdgeNode* curr = adjList[i];
        while (curr != nullptr) {
            RoadEdgeNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        adjList[i] = nullptr;
        vertexExists[i] = false;
    }
    vertexCount = 0;
}

RoadNetwork::RoadNetwork(const RoadNetwork& other) : vertexCount(other.vertexCount) {
    for (int i = 0; i < MAX_VERTICES; ++i) {
        vertexExists[i] = other.vertexExists[i];
        adjList[i] = nullptr;
        if (other.vertexExists[i]) {
            RoadEdgeNode* curr = other.adjList[i];
            RoadEdgeNode** target = &adjList[i];
            while (curr != nullptr) {
                *target = new RoadEdgeNode(curr->toLocationId, curr->travelMinutes);
                target = &((*target)->next);
                curr = curr->next;
            }
        }
    }
}

RoadNetwork& RoadNetwork::operator=(const RoadNetwork& other) {
    if (this != &other) {
        clear();
        vertexCount = other.vertexCount;
        for (int i = 0; i < MAX_VERTICES; ++i) {
            vertexExists[i] = other.vertexExists[i];
            if (other.vertexExists[i]) {
                RoadEdgeNode* curr = other.adjList[i];
                RoadEdgeNode** target = &adjList[i];
                while (curr != nullptr) {
                    *target = new RoadEdgeNode(curr->toLocationId, curr->travelMinutes);
                    target = &((*target)->next);
                    curr = curr->next;
                }
            }
        }
    }
    return *this;
}

void RoadNetwork::addVertex(int id) {
    if (id >= 0 && id < MAX_VERTICES && !vertexExists[id]) {
        vertexExists[id] = true;
        vertexCount++;
    }
}

void RoadNetwork::addRoad(int u, int v, double travelMinutes) {
    if (u < 0 || u >= MAX_VERTICES || v < 0 || v >= MAX_VERTICES) {
        return;
    }
    addVertex(u);
    addVertex(v);

    // Bidirectional road link
    adjList[u] = new RoadEdgeNode(v, travelMinutes, adjList[u]);
    adjList[v] = new RoadEdgeNode(u, travelMinutes, adjList[v]);
}

// Dijkstra's Algorithm implementation
bool RoadNetwork::findShortestPath(int startId, int targetId,
                                   CustomVector<int>& pathOut, double& totalCostOut) const {
    pathOut.clear();
    totalCostOut = 0.0;

    if (startId < 0 || startId >= MAX_VERTICES || targetId < 0 || targetId >= MAX_VERTICES) {
        return false;
    }
    if (!vertexExists[startId] || !vertexExists[targetId]) {
        return false;
    }

    const double INF = 1e9;
    double dist[MAX_VERTICES];
    bool visited[MAX_VERTICES];
    int parent[MAX_VERTICES];

    for (int i = 0; i < MAX_VERTICES; ++i) {
        dist[i] = INF;
        visited[i] = false;
        parent[i] = -1;
    }

    dist[startId] = 0.0;

    for (int count = 0; count < MAX_VERTICES; ++count) {
        // Find unvisited vertex with minimum distance
        double minVal = INF;
        int u = -1;

        for (int v = 0; v < MAX_VERTICES; ++v) {
            if (vertexExists[v] && !visited[v] && dist[v] < minVal) {
                minVal = dist[v];
                u = v;
            }
        }

        if (u == -1 || minVal >= INF) {
            break; // All reachable vertices have been processed
        }

        visited[u] = true;

        if (u == targetId) {
            break; // Reached target
        }

        // Relax neighbors
        RoadEdgeNode* edge = adjList[u];
        while (edge != nullptr) {
            int v = edge->toLocationId;
            if (!visited[v]) {
                double newDist = dist[u] + edge->travelMinutes;
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    parent[v] = u;
                }
            }
            edge = edge->next;
        }
    }

    if (dist[targetId] >= INF) {
        return false; // Target is unreachable
    }

    totalCostOut = dist[targetId];

    // Reconstruct path by backtracking parent pointers
    CustomVector<int> reversePath;
    int curr = targetId;
    while (curr != -1) {
        reversePath.push_back(curr);
        curr = parent[curr];
    }

    // Reverse to get path from startId to targetId
    for (int i = static_cast<int>(reversePath.size()) - 1; i >= 0; --i) {
        pathOut.push_back(reversePath[i]);
    }

    return true;
}

void RoadNetwork::displayRoads() const {
    std::cout << "Road Network Connections (Travel weights in minutes):\n";
    for (int i = 0; i < MAX_VERTICES; ++i) {
        if (vertexExists[i]) {
            RoadEdgeNode* curr = adjList[i];
            while (curr != nullptr) {
                if (i < curr->toLocationId) { // Print each undirected road once
                    std::cout << "   Road [" << std::setw(2) << i << " <---> "
                              << std::setw(2) << curr->toLocationId << "] : "
                              << std::fixed << std::setprecision(1) << curr->travelMinutes << " mins\n";
                }
                curr = curr->next;
            }
        }
    }
}
