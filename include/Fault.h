#ifndef FAULT_H
#define FAULT_H

#include <iostream>
#include <iomanip>
#include <string>

enum FaultStatus {
    PENDING,
    IN_PROGRESS,
    REPAIRED
};

class Fault {
public:
    int faultId;
    int powerLineId;
    int severity;                    // 1 (minor) to 5 (catastrophic)
    int criticalFacilitiesAffected;  // Count of critical facilities without power
    int populationAffected;          // Total population disconnected
    int outageDuration;              // In minutes
    double priorityScore;            // Computed priority
    FaultStatus status;

    Fault()
        : faultId(-1), powerLineId(-1), severity(1),
          criticalFacilitiesAffected(0), populationAffected(0),
          outageDuration(0), priorityScore(0.0), status(PENDING) {}

    Fault(int fId, int lineId, int sev, int critFac, int popAff, int duration)
        : faultId(fId), powerLineId(lineId), severity(sev),
          criticalFacilitiesAffected(critFac), populationAffected(popAff),
          outageDuration(duration), status(PENDING) {
        calculatePriority();
    }

    // Formula specified in requirement:
    // Priority Score = (Severity * 10) + (Critical Facilities Affected * 20)
    //                + (Population Affected / 100) + (Outage Duration * 2)
    void calculatePriority() {
        priorityScore = (severity * 10.0)
                      + (criticalFacilitiesAffected * 20.0)
                      + (static_cast<double>(populationAffected) / 100.0)
                      + (outageDuration * 2.0);
    }

    std::string getStatusString() const {
        switch (status) {
            case PENDING:     return "PENDING";
            case IN_PROGRESS: return "IN PROGRESS";
            case REPAIRED:    return "REPAIRED";
            default:          return "UNKNOWN";
        }
    }

    void display() const {
        std::cout << "Fault ID: " << std::setw(3) << faultId
                  << " | Line: #" << std::setw(2) << powerLineId
                  << " | Severity: " << severity << "/5"
                  << " | Crit Fac: " << std::setw(2) << criticalFacilitiesAffected
                  << " | Pop Affected: " << std::setw(6) << populationAffected
                  << " | Duration: " << std::setw(4) << outageDuration << "m"
                  << " | Priority Score: " << std::fixed << std::setprecision(1) << std::setw(7) << priorityScore
                  << " | Status: " << getStatusString()
                  << "\n";
    }
};

#endif // FAULT_H
