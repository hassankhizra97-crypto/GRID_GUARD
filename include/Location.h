#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <iostream>
#include <iomanip>

enum FacilityType {
    POWER_STATION,
    SUBSTATION,
    HOSPITAL,
    UNIVERSITY,
    RESIDENTIAL,
    COMMERCIAL
};

enum CriticalityLevel {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4
};

class Location {
public:
    int id;
    std::string name;
    FacilityType type;
    int population;
    CriticalityLevel criticality;
    bool isPowered;

    Location()
        : id(-1), name(""), type(RESIDENTIAL), population(0),
          criticality(LOW), isPowered(true) {}

    Location(int id, const std::string& name, FacilityType type,
             int population, CriticalityLevel criticality)
        : id(id), name(name), type(type), population(population),
          criticality(criticality), isPowered(true) {}

    std::string getTypeString() const {
        switch (type) {
            case POWER_STATION: return "Main Power Station";
            case SUBSTATION:    return "Substation";
            case HOSPITAL:      return "Hospital";
            case UNIVERSITY:    return "University";
            case RESIDENTIAL:   return "Residential Area";
            case COMMERCIAL:    return "Commercial Area";
            default:            return "Unknown";
        }
    }

    std::string getCriticalityString() const {
        switch (criticality) {
            case LOW:      return "LOW";
            case MEDIUM:   return "MEDIUM";
            case HIGH:     return "HIGH";
            case CRITICAL: return "CRITICAL";
            default:       return "UNKNOWN";
        }
    }

    bool isCriticalFacility() const {
        return (type == HOSPITAL || criticality == CRITICAL || criticality == HIGH);
    }

    void display() const {
        std::cout << "[" << std::setw(2) << id << "] "
                  << std::setw(28) << std::left << name
                  << " | Type: " << std::setw(20) << getTypeString()
                  << " | Pop: " << std::setw(6) << population
                  << " | Crit: " << std::setw(8) << getCriticalityString()
                  << " | Power: " << (isPowered ? "ONLINE (ON)" : "OFFLINE (OUT)")
                  << "\n";
    }
};

#endif // LOCATION_H
