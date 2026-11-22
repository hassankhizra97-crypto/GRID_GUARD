#ifndef ROAD_NETWORK_H
#define ROAD_NETWORK_H

#include "CustomVector.h"
#include <iostream>
#include <iomanip>

// Adjacency node for Road Graph
struct RoadEdgeNode {
    int toLocationId;
    double travelMinutes;
    RoadEdgeNode* next;

    RoadEdgeNode(int to, double weight, RoadEdgeNode* nxt = nullptr)
        : toLocationId(to), travelMinutes(weight), next(nxt) {}
};

// Independent Road Network for Repair Crew Dispatch using Dijkstra's Algorithm
class RoadNetwork {
private:
    static const int MAX_VERTICES = 64;
    RoadEdgeNode* adjList[MAX_VERTICES];
    bool vertexExists[MAX_VERTICES];
    int vertexCount;

public:
    RoadNetwork();
    ~RoadNetwork();

    RoadNetwork(const RoadNetwork& other);
    RoadNetwork& operator=(const RoadNetwork& other);

    void clear();
    void addVertex(int id);
    void addRoad(int u, int v, double travelMinutes);

    // Dijkstra's Shortest Path Algorithm
    // Computes the minimum-cost route between startId and targetId
    bool findShortestPath(int startId, int targetId,
                          CustomVector<int>& pathOut, double& totalCostOut) const;

    void displayRoads() const;
};

#endif // ROAD_NETWORK_H
