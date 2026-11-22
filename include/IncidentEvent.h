#ifndef INCIDENT_EVENT_H
#define INCIDENT_EVENT_H

#include <string>
#include <iostream>
#include <iomanip>

class IncidentEvent {
public:
    int eventId;
    std::string description;
    int relatedFaultId;       // -1 if not applicable
    std::string eventType;    // FAILURE_DETECTED, FAULT_REGISTERED, REPAIR_PRIORITIZED,
                              // REPAIR_STARTED, REPAIR_COMPLETED, ELECTRICITY_RESTORED, OPERATION_UNDONE
    std::string timestamp;    // Simulation timestamp or clock time

    IncidentEvent()
        : eventId(-1), description(""), relatedFaultId(-1),
          eventType(""), timestamp("") {}

    IncidentEvent(int id, const std::string& desc, int faultId,
                  const std::string& type, const std::string& timeStr)
        : eventId(id), description(desc), relatedFaultId(faultId),
          eventType(type), timestamp(timeStr) {}

    void display() const {
        std::cout << "[Event #" << std::setw(3) << eventId << " | "
                  << std::setw(8) << timestamp << "] "
                  << std::setw(22) << std::left << eventType << " | "
                  << description;
        if (relatedFaultId != -1) {
            std::cout << " (Fault #" << relatedFaultId << ")";
        }
        std::cout << "\n";
    }
};

#endif // INCIDENT_EVENT_H
