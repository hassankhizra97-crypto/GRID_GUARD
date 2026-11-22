#ifndef POWER_LINE_H
#define POWER_LINE_H

#include <iostream>
#include <iomanip>

class PowerLine {
public:
    int id;
    int fromLocationId;
    int toLocationId;
    bool isOperational;
    double repairCost;   // In dollars ($)
    double travelCost;   // Simulated crew travel cost / distance weight

    PowerLine()
        : id(-1), fromLocationId(-1), toLocationId(-1),
          isOperational(true), repairCost(0.0), travelCost(0.0) {}

    PowerLine(int id, int from, int to, double repairCost, double travelCost)
        : id(id), fromLocationId(from), toLocationId(to),
          isOperational(true), repairCost(repairCost), travelCost(travelCost) {}

    void display() const {
        std::cout << "Line #" << std::setw(2) << id
                  << " [" << std::setw(2) << fromLocationId
                  << " <---> " << std::setw(2) << toLocationId << "]"
                  << " | Status: " << (isOperational ? "OPERATIONAL" : "FAILED/OFF ")
                  << " | Repair Cost: $" << std::fixed << std::setprecision(0) << repairCost
                  << " | Travel Weight: " << std::fixed << std::setprecision(1) << travelCost
                  << "\n";
    }
};

#endif // POWER_LINE_H
