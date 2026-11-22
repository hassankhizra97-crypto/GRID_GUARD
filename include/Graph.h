#ifndef GRAPH_H
#define GRAPH_H

#include "Location.h"
#include "PowerLine.h"
#include "Queue.h"
#include "CustomVector.h"
#include <iostream>
#include <iomanip>

// Adjacency List Node for Graph representation
struct AdjEdgeNode {
    int neighborLocationId;
    int powerLineId;
    double repairCost;
    double travelCost;
    AdjEdgeNode* next;

    AdjEdgeNode(int neighborId, int lineId, double rCost, double tCost, AdjEdgeNode* nxt = nullptr)
        : neighborLocationId(neighborId), powerLineId(lineId),
          repairCost(rCost), travelCost(tCost), next(nxt) {}
};

// Power Grid Graph Representation
class Graph {
private:
    static const int MAX_VERTICES = 64;

    Location locations[MAX_VERTICES];
    bool locationExists[MAX_VERTICES];
    AdjEdgeNode* adjList[MAX_VERTICES];

    CustomVector<PowerLine> powerLines;
    int locationCount;

    // Helper for DFS traversal
    void dfsHelper(int u, bool visited[], CustomVector<int>& traversalOrder, bool onlyOperational) const;

public:
    Graph();
    ~Graph();

    // Prevent direct copying or handle deep copies properly
    Graph(const Graph& other);
    Graph& operator=(const Graph& other);

    void clear();

    // Graph construction
    bool addLocation(const Location& loc);
    bool addPowerLine(const PowerLine& line);

    // Operational state manipulation
    bool disablePowerLine(int lineId);
    bool restorePowerLine(int lineId);
    bool isPowerLineOperational(int lineId) const;

    // Accessors
    Location* getLocation(int id);
    const Location* getLocation(int id) const;
    PowerLine* getPowerLine(int id);
    const PowerLine* getPowerLine(int id) const;

    int getLocationCount() const { return locationCount; }
    int getPowerLineCount() const { return static_cast<int>(powerLines.size()); }
    const CustomVector<PowerLine>& getAllPowerLines() const { return powerLines; }
    void getAllLocationIds(CustomVector<int>& outIds) const;

    // Network inspection and display
    void displayNetwork() const;
    void displayNeighbors(int locationId) const;
    void displayAllLocations() const;

    // Traversal and Connectivity Algorithms
    // BFS uses custom Queue to find all reachable vertices from startVertexId
    void runBFS(int startVertexId, CustomVector<int>& reachableLocations, bool onlyOperational = true) const;

    // DFS demonstrates recursive exploration of graph
    void runDFS(int startVertexId, CustomVector<int>& traversalOrder, bool onlyOperational = true) const;

    // Connectivity analysis: partitions locations into reachable vs disconnected
    void analyzeConnectivity(int mainStationId, CustomVector<int>& connected, CustomVector<int>& disconnected) const;
};

#endif // GRAPH_H
