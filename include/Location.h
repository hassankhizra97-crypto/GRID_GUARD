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
    COMMERCIAL,
    INDUSTRIAL
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
    double latitude;
    double longitude;
    std::string area;

    Location()
        : id(-1), name(""), type(RESIDENTIAL), population(0),
          criticality(LOW), isPowered(true),
          latitude(24.8607), longitude(67.0011), area("Karachi") {}

    Location(int id, const std::string& name, FacilityType type,
             int population, CriticalityLevel criticality,
             double lat = 24.8607, double lon = 67.0011,
             const std::string& areaName = "Karachi")
        : id(id), name(name), type(type), population(population),
          criticality(criticality), isPowered(true),
          latitude(lat), longitude(lon), area(areaName) {}

    std::string getTypeString() const {
        switch (type) {
            case POWER_STATION: return "Simulated Supply Hub";
            case SUBSTATION:    return "Simulated Dist Node";
            case HOSPITAL:      return "Hospital";
            case UNIVERSITY:    return "University";
            case RESIDENTIAL:   return "Residential Area";
            case COMMERCIAL:    return "Commercial Area";
            case INDUSTRIAL:    return "Industrial Zone";
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
                  << std::setw(32) << std::left << name
                  << " | " << std::setw(18) << area
                  << " | " << std::setw(18) << getTypeString()
                  << " | Pop: " << std::setw(6) << population
                  << " | Crit: " << std::setw(8) << getCriticalityString()
                  << " | " << (isPowered ? "ONLINE" : "OFFLINE")
                  << "\n";
    }
};

#endif // LOCATION_H
