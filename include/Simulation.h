#ifndef SIMULATION_H
#define SIMULATION_H

#include "Graph.h"
#include "RoadNetwork.h"
#include "MaxHeap.h"
#include "HashTable.h"
#include "DoublyLinkedList.h"
#include "Stack.h"
#include "CustomVector.h"
#include "Sorting.h"
#include "SearchAlgorithms.h"
#include "CircularQueue.h"
#include "ArrayStack.h"
#include <string>
#include <sstream>
#include <vector>

class Simulation {
private:
    Graph powerGrid;
    RoadNetwork roadNetwork;
    MaxHeap repairQueue;
    HashTable<int, Location> locationTable;
    HashTable<int, Fault> faultTable;
    DoublyLinkedList<IncidentEvent> incidentHistory;
    // The undo log is an array-based manual stack. Every state-changing
    // operation pushes enough information for its inverse operation.
    ArrayStack<UndoAction> arrayUndoStack;

    int nextFaultId;
    int nextEventId;
    int simulationTimeMinutes;
    int mainPowerStationId;
    int repairDepotLocationId; // Where repair crews are dispatched from (defaults to Main Station)

    // Helper functions
    std::string getCurrentTimeStr() const;
    void logEvent(const std::string& desc, int faultId, const std::string& eventType);

public:
    Simulation();
    ~Simulation();

    // Initialize sample city network
    void loadSampleNetwork();

    // Core Simulation Workflows
    // Section 5: Simulate Power Line Failure (Steps 1 to 12)
    bool simulatePowerLineFailure(int lineId, int severity, int durationMinutes = 60);

    // Section 5: Process Repair (Steps 1 to 10)
    bool processNextRepair(int specificFaultId = -1);

    // Restore line directly
    bool restorePowerLineDirectly(int lineId);

    // Section 3.F: Undo last reversible operation
    bool undoLastOperation();

    // Inspection & Queries
    void displayPowerNetwork() const;
    void displayAllLocations() const;
    void displayLocationNeighbors(int locationId) const;
    void displayRepairQueue() const;
    void displayAllFaults() const;
    void displayIncidentHistoryForward() const;
    void displayIncidentHistoryBackward() const;
    void searchIncidentHistory(const std::string& keyword) const;
    void displayUndoHistory() const;

    // Fast Lookups (Hash Table)
    bool searchLocationById(int locId) const;
    bool searchFaultById(int faultId) const;
    void displayHashTableDiagnostics() const;

    // Algorithmic Demonstrations
    void runBFSTraversal(int startId = -1) const;
    void runDFSTraversal(int startId = -1) const;
    void analyzeAndDisplayConnectivity() const;
    bool findRepairCrewRoute(int targetLocationId) const;
    void sortAndDisplayFaults(SortCriterion criterion, bool ascending = false);

    // All Sorting Algorithms demonstration
    void sortAndDisplayFaultsAlgo(SortAlgorithm algo, SortCriterion criterion, bool ascending = false);

    // Search demonstrations
    void demonstrateLinearSearch(int targetFaultId) const;
    void demonstrateBinarySearch(int targetFaultId) const;
    void demonstrateSearchComparison(int targetFaultId) const;

    // Status queries
    void displayOutageReportSummary() const;
    void displayCriticalFacilitiesStatus() const;

    // Demonstrate circular queue behavior
    void demonstrateCircularQueue() const;
    // Demonstrate array stack behavior
    void demonstrateArrayStack() const;

    // Getters for automated tests
    int getMainStationId() const { return mainPowerStationId; }
    int getPendingFaultCount() const { return repairQueue.size(); }
    int getIncidentCount() const { return incidentHistory.size(); }
    int getUndoStackSize() const { return arrayUndoStack.size(); }
    const Graph& getPowerGrid() const { return powerGrid; }
    const RoadNetwork& getRoadNetwork() const { return roadNetwork; }
    const MaxHeap& getRepairQueue() const { return repairQueue; }
    const HashTable<int, Fault>& getFaultTable() const { return faultTable; }
    const DoublyLinkedList<IncidentEvent>& getIncidentHistory() const { return incidentHistory; }

    // JSON serialization for HTTP backend integration
    std::string toJsonLocation(const Location& loc) const;
    std::string toJsonPowerLine(const PowerLine& pl) const;
    std::string toJsonFault(const Fault& f) const;
    std::string toJsonIncidentEvent(const IncidentEvent& ev) const;

    std::string exportLocationsJSON() const;
    std::string exportPowerLinesJSON() const;
    std::string exportFaultsJSON() const;
    std::string exportIncidentHistoryJSON() const;
    std::string exportNetworkStateJSON() const;
    std::string exportStatsJSON() const;

    // JSON escaping helper
    static std::string jsonEscape(const std::string& s);
};

#endif // SIMULATION_H
