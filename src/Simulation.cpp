#include "../include/Simulation.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Simulation::Simulation()
    : locationTable(101), faultTable(53),
      nextFaultId(101), nextEventId(1),
      simulationTimeMinutes(0), mainPowerStationId(1),
      repairDepotLocationId(1) {
    loadSampleNetwork();
}

Simulation::~Simulation() {
}

std::string Simulation::getCurrentTimeStr() const {
    int hours = (simulationTimeMinutes / 60) % 24;
    int mins = simulationTimeMinutes % 60;
    std::ostringstream oss;
    oss << "T+" << std::setfill('0') << std::setw(2) << hours
        << ":" << std::setfill('0') << std::setw(2) << mins;
    return oss.str();
}

void Simulation::logEvent(const std::string& desc, int faultId, const std::string& eventType) {
    IncidentEvent ev(nextEventId++, desc, faultId, eventType, getCurrentTimeStr());
    incidentHistory.append(ev);
}

void Simulation::loadSampleNetwork() {
    // 44 Verified Geographical Karachi Locations
    // Real public hospitals, universities, residential areas, commercial/industrial zones,
    // and clearly labelled SIMULATED supply and distribution nodes.
    Location locs[] = {
        // Simulated Supply Nodes (Primary Power Hubs)
        Location(1,  "Simulated Karachi Supply Hub - Central", POWER_STATION, 0, CRITICAL, 24.8716, 67.0599, "PECHS / Shahrah-e-Faisal"),
        Location(2,  "Simulated Karachi Supply Hub - West",    POWER_STATION, 0, HIGH,     24.8988, 66.9850, "SITE / West"),
        Location(3,  "Simulated Karachi Supply Hub - East",    POWER_STATION, 0, HIGH,     24.9180, 67.1250, "Malir / East"),
        Location(4,  "Simulated Karachi Supply Hub - South",   POWER_STATION, 0, HIGH,     24.8150, 67.0300, "Clifton / South"),

        // Simulated Regional Distribution Nodes (Grid Substations)
        Location(5,  "Simulated Dist Node - Saddar",          SUBSTATION,    0, HIGH,     24.8580, 67.0120, "Saddar"),
        Location(6,  "Simulated Dist Node - Gulshan",         SUBSTATION,    0, HIGH,     24.9200, 67.0900, "Gulshan-e-Iqbal"),
        Location(7,  "Simulated Dist Node - Korangi",         SUBSTATION,    0, HIGH,     24.8350, 67.1350, "Korangi"),
        Location(8,  "Simulated Dist Node - North Nazimabad", SUBSTATION,    0, HIGH,     24.9450, 67.0350, "North Nazimabad"),

        // Real Karachi Hospitals
        Location(9,  "Jinnah Postgrad Medical Centre (JPMC)", HOSPITAL, 3500, CRITICAL, 24.8522, 67.0511, "Cantt / Saddar"),
        Location(10, "Aga Khan University Hospital",          HOSPITAL, 4200, CRITICAL, 24.8924, 67.0747, "Stadium Road"),
        Location(11, "Indus Hospital, Korangi",               HOSPITAL, 2800, CRITICAL, 24.8290, 67.1255, "Korangi"),
        Location(12, "Civil Hospital Karachi (Dr. Ruth Pfau)",HOSPITAL, 3900, CRITICAL, 24.8585, 67.0105, "Saddar / Old Town"),
        Location(13, "Liaquat National Hospital",             HOSPITAL, 2500, CRITICAL, 24.8945, 67.0705, "Stadium Road"),
        Location(14, "South City Hospital",                   HOSPITAL, 1400, CRITICAL, 24.8210, 67.0270, "Clifton Block 3"),
        Location(15, "SIUT (Sindh Institute of Urology)",     HOSPITAL, 2900, CRITICAL, 24.8570, 67.0090, "Ranchore Line"),
        Location(16, "Abbasi Shaheed Hospital",               HOSPITAL, 2600, CRITICAL, 24.9230, 67.0315, "Nazimabad"),

        // Real Karachi Universities
        Location(17, "FAST-NUCES Karachi",                    UNIVERSITY, 6500,  HIGH,   24.8569, 67.2647, "Shah Latif Town / Hwy"),
        Location(18, "NED Univ of Engineering & Tech",        UNIVERSITY, 14000, HIGH,   24.9338, 67.1114, "University Road"),
        Location(19, "University of Karachi (KU)",            UNIVERSITY, 42000, HIGH,   24.9416, 67.1141, "University Road"),
        Location(20, "IBA Karachi (Main Campus)",             UNIVERSITY, 5000,  HIGH,   24.9405, 67.1130, "University Road"),
        Location(21, "Habib University",                      UNIVERSITY, 3200,  MEDIUM, 24.8988, 67.1350, "Gulistan-e-Jauhar"),
        Location(22, "Dow University of Health Sciences",     UNIVERSITY, 8000,  HIGH,   24.9470, 67.1180, "Gulzar-e-Hijri / Ojha"),
        Location(23, "Bahria University Karachi Campus",      UNIVERSITY, 7500,  MEDIUM, 24.8870, 67.0890, "Stadium Road"),

        // Real Karachi Residential Neighborhood Feeders
        Location(24, "Gulshan-e-Iqbal",                       RESIDENTIAL, 9500,   LOW,    24.9180, 67.0970, "District East"),
        Location(25, "Gulistan-e-Jauhar",                     RESIDENTIAL, 8500,   LOW,    24.9120, 67.1320, "District East"),
        Location(26, "North Nazimabad",                       RESIDENTIAL, 7200,   LOW,    24.9400, 67.0350, "District Central"),
        Location(27, "Federal B Area",                        RESIDENTIAL, 6800,   LOW,    24.9300, 67.0650, "District Central"),
        Location(28, "Clifton Residential",                   RESIDENTIAL, 5500,   MEDIUM, 24.8250, 67.0320, "District South"),
        Location(29, "DHA (Defence Housing Authority)",       RESIDENTIAL, 8000,   MEDIUM, 24.8050, 67.0650, "DHA / South"),
        Location(30, "Saddar Neighborhood",                   RESIDENTIAL, 6200,   LOW,    24.8550, 67.0150, "District South"),
        Location(31, "Malir",                                 RESIDENTIAL, 9000,   LOW,    24.8900, 67.1900, "District Malir"),
        Location(32, "Korangi Residential",                   RESIDENTIAL, 9800,   LOW,    24.8300, 67.1150, "District Korangi"),
        Location(33, "Landhi Residential",                    RESIDENTIAL, 8200,   LOW,    24.8450, 67.1850, "District Malir/Korangi"),
        Location(34, "PECHS",                                 RESIDENTIAL, 4500,   LOW,    24.8680, 67.0600, "Jamshed Town"),
        Location(35, "Nazimabad",                             RESIDENTIAL, 5800,   LOW,    24.9150, 67.0250, "District Central"),
        Location(36, "Lyari",                                 RESIDENTIAL, 11000,  LOW,    24.8650, 66.9950, "District South / Lyari"),
        Location(37, "Shah Faisal Colony",                    RESIDENTIAL, 6400,   LOW,    24.8780, 67.1550, "Shah Faisal Town"),

        // Real Karachi Commercial and Industrial Areas
        Location(38, "Saddar Commercial & Electronics",       COMMERCIAL,  4500,   HIGH,   24.8595, 67.0180, "Saddar Downtown"),
        Location(39, "Tariq Road Commercial District",        COMMERCIAL,  3800,   HIGH,   24.8720, 67.0650, "PECHS Block 2"),
        Location(40, "Clifton Commercial / Marine Drive",     COMMERCIAL,  3200,   MEDIUM, 24.8180, 67.0250, "Clifton Waterfront"),
        Location(41, "Korangi Industrial Area (KIA)",         INDUSTRIAL,  5200,   HIGH,   24.8220, 67.1450, "Korangi Sector 7-A"),
        Location(42, "SITE Industrial Area",                  INDUSTRIAL,  6500,   HIGH,   24.9050, 66.9800, "Manghopir / SITE"),
        Location(43, "Landhi Industrial Area (LIA)",          INDUSTRIAL,  4800,   HIGH,   24.8400, 67.2050, "Landhi Sector 3"),
        Location(44, "Port Qasim Industrial Area",            INDUSTRIAL,  3500,   HIGH,   24.7850, 67.3350, "Bin Qasim Town")
    };

    for (int i = 0; i < 44; ++i) {
        powerGrid.addLocation(locs[i]);
        locationTable.insert(locs[i].id, locs[i]);
    }

    // 61 Simulated Power Transmission Lines (Combining Redundant & Radial Paths)
    PowerLine lines[] = {
        // Primary Transmission Backbone Ties between Supply Hubs
        PowerLine(1,  1, 2,  18000.0, 15.0), // Central Hub <-> West Hub (SITE)
        PowerLine(2,  1, 3,  19500.0, 18.0), // Central Hub <-> East Hub (Malir)
        PowerLine(3,  1, 4,  16000.0, 14.0), // Central Hub <-> South Hub (Clifton)
        PowerLine(4,  2, 4,  17500.0, 16.0), // West Hub <-> South Hub
        PowerLine(5,  3, 4,  21000.0, 22.0), // East Hub <-> South Hub

        // Supply Hub to Regional Distribution Nodes
        PowerLine(6,  1, 5,  12000.0, 10.0), // Central Hub -> Saddar Dist Node
        PowerLine(7,  1, 6,  14000.0, 12.0), // Central Hub -> Gulshan Dist Node
        PowerLine(8,  1, 7,  15000.0, 14.0), // Central Hub -> Korangi Dist Node
        PowerLine(9,  2, 8,  13000.0, 11.0), // West Hub -> North Nazimabad Dist Node
        PowerLine(10, 3, 7,  14500.0, 13.0), // East Hub -> Korangi Dist Node
        PowerLine(11, 4, 5,  11500.0,  9.0), // South Hub -> Saddar Dist Node

        // Regional Distribution Interties (Redundant Loop)
        PowerLine(12, 6, 8,   9000.0,  8.0), // Gulshan Dist <-> North Nazimabad Dist
        PowerLine(13, 5, 6,  10500.0, 10.0), // Saddar Dist <-> Gulshan Dist
        PowerLine(14, 7, 6,  11000.0, 11.0), // Korangi Dist <-> Gulshan Dist

        // Critical Hospital Power Feeds (Redundant & Radial)
        PowerLine(15, 1, 9,   7500.0,  6.0), // Central Hub -> JPMC
        PowerLine(16, 5, 9,   6500.0,  5.0), // Saddar Dist -> JPMC (Redundant feed to JPMC)
        PowerLine(17, 1, 10,  8000.0,  7.0), // Central Hub -> Aga Khan Hospital
        PowerLine(18, 6, 10,  7000.0,  6.0), // Gulshan Dist -> Aga Khan Hospital (Redundant feed to AKU)
        PowerLine(19, 7, 11,  8500.0,  6.0), // Korangi Dist -> Indus Hospital, Korangi
        PowerLine(20, 3, 11,  9000.0,  8.0), // East Hub -> Indus Hospital (Redundant feed to Indus)
        PowerLine(21, 5, 12,  6000.0,  4.0), // Saddar Dist -> Civil Hospital Karachi
        PowerLine(22, 2, 12,  7200.0,  6.0), // West Hub -> Civil Hospital (Redundant feed to Civil)
        PowerLine(23, 6, 13,  6800.0,  5.0), // Gulshan Dist -> Liaquat National Hospital
        PowerLine(24, 1, 13,  7200.0,  6.0), // Central Hub -> Liaquat National Hospital (Redundant feed to LNH)
        PowerLine(25, 4, 14, 12500.0, 10.0), // South Hub -> South City Hospital (Single radial feed to South City for critical outage test!)
        PowerLine(26, 5, 15,  6200.0,  4.0), // Saddar Dist -> SIUT
        PowerLine(27, 2, 15,  7100.0,  6.0), // West Hub -> SIUT (Redundant feed to SIUT)
        PowerLine(28, 8, 16, 11500.0,  9.0), // North Nazimabad Dist -> Abbasi Shaheed Hospital (Single feed to Abbasi Shaheed!)

        // Universities
        PowerLine(29, 3, 17, 13000.0, 16.0), // East Hub -> FAST-NUCES (Radial feed for university outage test!)
        PowerLine(30, 6, 18,  6500.0,  5.0), // Gulshan Dist -> NED University
        PowerLine(31, 3, 18,  7000.0,  7.0), // East Hub -> NED University (Redundant feed to NED)
        PowerLine(32, 6, 19,  6800.0,  6.0), // Gulshan Dist -> University of Karachi (KU)
        PowerLine(33, 3, 19,  7200.0,  7.0), // East Hub -> University of Karachi (Redundant feed to KU)
        PowerLine(34, 6, 20,  5500.0,  4.0), // Gulshan Dist -> IBA Karachi (Main Campus)
        PowerLine(35, 3, 21,  7500.0,  8.0), // East Hub -> Habib University (Jauhar)
        PowerLine(36, 6, 22,  6900.0,  6.0), // Gulshan Dist -> Dow University (Ojha)
        PowerLine(37, 1, 23,  6200.0,  5.0), // Central Hub -> Bahria University

        // Residential Neighborhoods
        PowerLine(38, 6, 24,  5500.0,  5.0), // Gulshan Dist -> Gulshan-e-Iqbal
        PowerLine(39, 3, 25,  6500.0,  7.0), // East Hub -> Gulistan-e-Jauhar
        PowerLine(40, 24, 25, 4500.0,  4.0), // Gulshan <-> Jauhar (Neighborhood tie)
        PowerLine(41, 8, 26,  5800.0,  5.0), // North Nazimabad Dist -> North Nazimabad
        PowerLine(42, 8, 27,  5600.0,  5.0), // North Nazimabad Dist -> Federal B Area
        PowerLine(43, 26, 27, 4200.0,  4.0), // North Nazimabad <-> FB Area (Neighborhood tie)
        PowerLine(44, 4, 28,  6200.0,  5.0), // South Hub -> Clifton Residential
        PowerLine(45, 4, 29,  6800.0,  6.0), // South Hub -> DHA
        PowerLine(46, 28, 29, 4800.0,  4.0), // Clifton <-> DHA (Neighborhood tie)
        PowerLine(47, 5, 30,  5200.0,  4.0), // Saddar Dist -> Saddar Neighborhood
        PowerLine(48, 3, 31, 10500.0, 12.0), // East Hub -> Malir (Single radial feed to Malir for non-critical outage test!)
        PowerLine(49, 7, 32,  6000.0,  5.0), // Korangi Dist -> Korangi Residential
        PowerLine(50, 7, 33,  6800.0,  6.0), // Korangi Dist -> Landhi Residential
        PowerLine(51, 1, 34,  4800.0,  4.0), // Central Hub -> PECHS
        PowerLine(52, 8, 35,  5400.0,  4.0), // North Nazimabad Dist -> Nazimabad
        PowerLine(53, 5, 36, 11000.0, 10.0), // Saddar Dist -> Lyari (Single radial feed to Lyari for outage test!)
        PowerLine(54, 7, 37,  5800.0,  5.0), // Korangi Dist -> Shah Faisal Colony

        // Commercial and Industrial Areas
        PowerLine(55, 5, 38,  5500.0,  4.0), // Saddar Dist -> Saddar Commercial
        PowerLine(56, 1, 39,  5000.0,  4.0), // Central Hub -> Tariq Road Commercial
        PowerLine(57, 4, 40,  5800.0,  5.0), // South Hub -> Clifton Commercial
        PowerLine(58, 7, 41,  8000.0,  8.0), // Korangi Dist -> Korangi Industrial Area (KIA)
        PowerLine(59, 2, 42,  8500.0,  8.0), // West Hub -> SITE Industrial Area
        PowerLine(60, 7, 43,  8200.0,  8.0), // Korangi Dist -> Landhi Industrial Area (LIA)
        PowerLine(61, 3, 44, 14000.0, 18.0)  // East Hub -> Port Qasim Industrial (Radial feed to Port Qasim)
    };

    for (int i = 0; i < 61; ++i) {
        powerGrid.addPowerLine(lines[i]);
    }

    // Realistic Simulated Road Network across Karachi Corridors
    // Travel times (minutes) connecting sectors along Shahrah-e-Faisal, University Road,
    // Korangi Road, MA Jinnah Road, National Highway, Lyari Expressway, and Ring Corridors.
    roadNetwork.addRoad(1, 34, 4.0);  // Central Hub <-> PECHS
    roadNetwork.addRoad(34, 39, 4.0); // PECHS <-> Tariq Road
    roadNetwork.addRoad(1, 9, 8.0);   // Central Hub <-> JPMC (Cantt)
    roadNetwork.addRoad(9, 5, 6.0);   // JPMC <-> Saddar Dist Node
    roadNetwork.addRoad(5, 30, 4.0);  // Saddar Dist <-> Saddar Neighborhood
    roadNetwork.addRoad(5, 38, 4.0);  // Saddar Dist <-> Saddar Commercial
    roadNetwork.addRoad(5, 12, 5.0);  // Saddar Dist <-> Civil Hospital
    roadNetwork.addRoad(12, 15, 3.0); // Civil Hospital <-> SIUT
    roadNetwork.addRoad(5, 36, 8.0);  // Saddar Dist <-> Lyari
    roadNetwork.addRoad(36, 2, 10.0); // Lyari <-> West Hub (SITE)
    roadNetwork.addRoad(2, 42, 6.0);  // West Hub <-> SITE Industrial Area
    roadNetwork.addRoad(2, 35, 7.0);  // West Hub <-> Nazimabad
    roadNetwork.addRoad(35, 16, 5.0); // Nazimabad <-> Abbasi Shaheed Hospital
    roadNetwork.addRoad(35, 8, 7.0);  // Nazimabad <-> North Nazimabad Dist
    roadNetwork.addRoad(8, 26, 4.0);  // North Nazimabad Dist <-> North Nazimabad
    roadNetwork.addRoad(8, 27, 5.0);  // North Nazimabad Dist <-> FB Area
    roadNetwork.addRoad(27, 6, 7.0);  // FB Area <-> Gulshan Dist Node
    roadNetwork.addRoad(6, 24, 4.0);  // Gulshan Dist <-> Gulshan-e-Iqbal
    roadNetwork.addRoad(6, 18, 5.0);  // Gulshan Dist <-> NED University
    roadNetwork.addRoad(18, 19, 3.0); // NED <-> University of Karachi (KU)
    roadNetwork.addRoad(19, 20, 2.0); // KU <-> IBA Karachi
    roadNetwork.addRoad(19, 22, 4.0); // KU <-> Dow University (Ojha)
    roadNetwork.addRoad(6, 10, 6.0);  // Gulshan Dist <-> Aga Khan University Hospital
    roadNetwork.addRoad(10, 13, 2.0); // Aga Khan Hospital <-> Liaquat National Hospital
    roadNetwork.addRoad(10, 23, 4.0); // Aga Khan Hospital <-> Bahria University
    roadNetwork.addRoad(23, 1, 8.0);  // Bahria Univ <-> Central Hub
    roadNetwork.addRoad(24, 25, 6.0); // Gulshan <-> Gulistan-e-Jauhar
    roadNetwork.addRoad(25, 21, 5.0); // Gulistan-e-Jauhar <-> Habib University
    roadNetwork.addRoad(21, 3, 7.0);  // Habib Univ <-> East Hub (Malir)
    roadNetwork.addRoad(3, 31, 8.0);  // East Hub <-> Malir
    roadNetwork.addRoad(31, 37, 7.0); // Malir <-> Shah Faisal Colony
    roadNetwork.addRoad(37, 7, 8.0);  // Shah Faisal <-> Korangi Dist Node
    roadNetwork.addRoad(7, 32, 5.0);  // Korangi Dist <-> Korangi Residential
    roadNetwork.addRoad(7, 11, 4.0);  // Korangi Dist <-> Indus Hospital
    roadNetwork.addRoad(7, 41, 6.0);  // Korangi Dist <-> Korangi Industrial Area
    roadNetwork.addRoad(41, 33, 7.0); // KIA <-> Landhi Residential
    roadNetwork.addRoad(33, 43, 5.0); // Landhi Res <-> Landhi Industrial Area
    roadNetwork.addRoad(43, 17, 9.0); // Landhi Ind <-> FAST-NUCES Karachi
    roadNetwork.addRoad(17, 44, 12.0);// FAST-NUCES <-> Port Qasim Industrial
    roadNetwork.addRoad(41, 4, 14.0); // KIA <-> South Hub (Clifton)
    roadNetwork.addRoad(4, 28, 5.0);  // South Hub <-> Clifton Residential
    roadNetwork.addRoad(28, 40, 3.0); // Clifton Res <-> Clifton Commercial
    roadNetwork.addRoad(40, 14, 4.0); // Clifton Commercial <-> South City Hospital
    roadNetwork.addRoad(4, 29, 6.0);  // South Hub <-> DHA
    roadNetwork.addRoad(29, 9, 9.0);  // DHA <-> JPMC (Cantt)
    roadNetwork.addRoad(1, 4, 12.0);  // Central Hub <-> South Hub
    roadNetwork.addRoad(1, 6, 11.0);  // Central Hub <-> Gulshan Dist
    roadNetwork.addRoad(1, 7, 13.0);  // Central Hub <-> Korangi Dist

    logEvent("GridGuard Karachi Power Grid Simulator initialized with 44 municipal locations and 61 simulated lines.", -1, "SYSTEM_INIT");
}

bool Simulation::simulatePowerLineFailure(int lineId, int severity, int durationMinutes) {
    // STEP 1: Select power line
    PowerLine* line = powerGrid.getPowerLine(lineId);
    if (!line) {
        std::cout << "[ERROR] Power line #" << lineId << " does not exist in the network.\n";
        return false;
    }
    if (!line->isOperational) {
        std::cout << "[WARNING] Power line #" << lineId << " has ALREADY failed.\n";
        return false;
    }

    if (severity < 1 || severity > 5) {
        severity = 3; // Clamp to reasonable default
    }

    simulationTimeMinutes += 15;

    // Analyze reachable locations BEFORE disabling to isolate impact of this specific line
    CustomVector<int> beforeConnected, beforeDisconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, beforeConnected, beforeDisconnected);

    // STEP 2: Mark selected power line as FAILED
    powerGrid.disablePowerLine(lineId);

    // STEP 3: Create fault record and assign unique fault ID
    int fId = nextFaultId++;

    // STEP 4: Record operation in undo stack
    undoStack.push(UndoAction(ACTION_LINE_DISABLED, lineId, fId,
                              "Simulated failure on Power Line #" + std::to_string(lineId)));

    // STEP 5: Run BFS from main power station
    CustomVector<int> reachable;
    CustomVector<int> disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, reachable, disconnected);

    // Update location powered statuses
    for (size_t i = 0; i < reachable.size(); ++i) {
        Location* loc = powerGrid.getLocation(reachable[i]);
        if (loc) {
            loc->isPowered = true;
            locationTable.update(loc->id, *loc);
        }
    }
    for (size_t i = 0; i < disconnected.size(); ++i) {
        Location* loc = powerGrid.getLocation(disconnected[i]);
        if (loc) {
            loc->isPowered = false;
            locationTable.update(loc->id, *loc);
        }
    }

    // Identify locations specifically disconnected by THIS line failure (beforeConnected \ reachable)
    CustomVector<int> lineImpactLocations;
    bool isStillReachable[128] = {false};
    for (size_t i = 0; i < reachable.size(); ++i) {
        if (reachable[i] >= 0 && reachable[i] < 128) {
            isStillReachable[reachable[i]] = true;
        }
    }

    int critCount = 0;
    int popCount = 0;
    for (size_t i = 0; i < beforeConnected.size(); ++i) {
        int u = beforeConnected[i];
        if (!isStillReachable[u]) {
            lineImpactLocations.push_back(u);
            const Location* loc = powerGrid.getLocation(u);
            if (loc) {
                popCount += loc->population;
                if (loc->isCriticalFacility()) {
                    critCount++;
                }
            }
        }
    }

    // STEP 8: Calculate fault priority score
    Fault faultRecord(fId, lineId, severity, critCount, popCount, durationMinutes);

    // STEP 9: Insert fault into max-heap
    repairQueue.insert(faultRecord);

    // STEP 10: Insert fault record into hash table
    faultTable.insert(fId, faultRecord);

    // STEP 11: Add failure event to incident history
    std::string desc = "Power Line #" + std::to_string(lineId) + " (" +
                       powerGrid.getLocation(line->fromLocationId)->name + " <-> " +
                       powerGrid.getLocation(line->toLocationId)->name + ") FAILED. " +
                       std::to_string(lineImpactLocations.size()) + " location(s) newly isolated.";
    logEvent(desc, fId, "FAILURE_DETECTED");

    // STEP 12: Display outage report
    std::cout << "\n========================================================================================\n";
    std::cout << "                        GRIDGUARD KARACHI INCIDENT OUTAGE REPORT                        \n";
    std::cout << "========================================================================================\n";
    std::cout << "Fault ID Registered : " << fId << "\n";
    std::cout << "Failed Power Line   : Line #" << lineId << " ["
              << powerGrid.getLocation(line->fromLocationId)->name << " <---> "
              << powerGrid.getLocation(line->toLocationId)->name << "]\n";
    std::cout << "Fault Severity      : " << severity << " / 5\n";
    std::cout << "Outage Duration Est : " << durationMinutes << " minutes\n";
    std::cout << "Priority Score      : " << std::fixed << std::setprecision(1) << faultRecord.priorityScore << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "DIRECTLY ISOLATED LOCATIONS BY THIS LINE (" << lineImpactLocations.size() << "):\n";
    if (lineImpactLocations.empty()) {
        std::cout << "   None! Grid redundancy preserved power flow through alternate transmission lines.\n";
    } else {
        for (size_t i = 0; i < lineImpactLocations.size(); ++i) {
            const Location* loc = powerGrid.getLocation(lineImpactLocations[i]);
            if (loc) {
                std::cout << "   * [" << std::setw(2) << loc->id << "] "
                          << std::setw(32) << std::left << loc->name
                          << " | Area: " << std::setw(22) << loc->area
                          << " | Type: " << std::setw(18) << loc->getTypeString()
                          << " | Pop: " << std::setw(6) << loc->population
                          << " | Crit: " << loc->getCriticalityString() << "\n";
            }
        }
    }
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "TOTAL GRID STATUS            : " << reachable.size() << " Online / "
              << disconnected.size() << " Offline\n";
    std::cout << "POPULATION IMPACTED BY LINE  : " << popCount << " residents\n";
    std::cout << "CRITICAL FACILITIES IMPACTED : " << critCount << "\n";
    std::cout << "========================================================================================\n";

    return true;
}

bool Simulation::processNextRepair(int specificFaultId) {
    if (repairQueue.isEmpty()) {
        std::cout << "[INFO] No pending faults in the repair queue. All lines operational!\n";
        return false;
    }

    Fault faultToRepair;
    if (specificFaultId != -1) {
        Fault* fPtr = faultTable.searchPtr(specificFaultId);
        if (!fPtr) {
            std::cout << "[ERROR] Fault ID #" << specificFaultId << " not found in records.\n";
            return false;
        }
        if (fPtr->status == REPAIRED) {
            std::cout << "[WARNING] Fault ID #" << specificFaultId << " has ALREADY been repaired.\n";
            return false;
        }
        faultToRepair = *fPtr;
        // Also extract from heap if it matches top or re-heapify
        if (!repairQueue.isEmpty() && repairQueue.peekMax().faultId == specificFaultId) {
            repairQueue.extractMax();
        }
    } else {
        // STEP 1: Select highest-priority fault from Max-Heap
        faultToRepair = repairQueue.extractMax();
    }

    int fId = faultToRepair.faultId;
    int lineId = faultToRepair.powerLineId;
    PowerLine* line = powerGrid.getPowerLine(lineId);
    if (!line) {
        std::cout << "[ERROR] Corrupted power line ID in fault record.\n";
        return false;
    }

    simulationTimeMinutes += 30;

    std::cout << "\n========================================================================================\n";
    std::cout << "                      KARACHI EMERGENCY REPAIR OPERATION INITIATED                      \n";
    std::cout << "========================================================================================\n";
    std::cout << "Processing Fault ID: #" << fId << " (Priority Score: "
              << std::fixed << std::setprecision(1) << faultToRepair.priorityScore << ")\n";
    std::cout << "Target Damaged Line: Line #" << lineId << " ["
              << powerGrid.getLocation(line->fromLocationId)->name << " <---> "
              << powerGrid.getLocation(line->toLocationId)->name << "]\n";

    // STEP 2: Use Dijkstra's algorithm to calculate repair crew route
    int destination = line->toLocationId;
    CustomVector<int> route;
    double travelMinutes = 0.0;
    bool routeFound = roadNetwork.findShortestPath(repairDepotLocationId, destination, route, travelMinutes);

    // STEP 3: Display route and simulated travel cost
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "REPAIR CREW DISPATCH ROUTE (DIJKSTRA'S ALGORITHM ACROSS KARACHI ARTERIES):\n";
    std::cout << "   Origin (Depot): [" << repairDepotLocationId << "] "
              << powerGrid.getLocation(repairDepotLocationId)->name << "\n";
    std::cout << "   Destination   : [" << destination << "] "
              << powerGrid.getLocation(destination)->name << "\n";
    if (routeFound && !route.empty()) {
        std::cout << "   Optimal Path  : ";
        for (size_t i = 0; i < route.size(); ++i) {
            const Location* loc = powerGrid.getLocation(route[i]);
            std::cout << "[" << route[i] << "] " << (loc ? loc->name : "Loc");
            if (i + 1 < route.size()) std::cout << "  ==>  ";
        }
        std::cout << "\n   Travel Time   : " << std::fixed << std::setprecision(1)
                  << travelMinutes << " minutes (Simulated Karachi Road Network)\n";
    } else {
        std::cout << "   [Notice] Destination reached directly at location depot.\n";
    }
    std::cout << "----------------------------------------------------------------------------------------\n";

    // STEP 4: Mark repair as started
    faultToRepair.status = IN_PROGRESS;
    faultTable.update(fId, faultToRepair);
    logEvent("Repair crew dispatched to Line #" + std::to_string(lineId), fId, "REPAIR_STARTED");

    // STEP 5: Simulate repair operation
    std::cout << "Executing repairs on Line #" << lineId << "... [Simulated Cost: $"
              << std::fixed << std::setprecision(0) << line->repairCost << "]\n";

    // STEP 6: Restore failed power line
    powerGrid.restorePowerLine(lineId);

    // Push to undo stack
    undoStack.push(UndoAction(ACTION_LINE_RESTORED, lineId, fId,
                              "Completed repair of Line #" + std::to_string(lineId)));

    // STEP 7: Run BFS again
    CustomVector<int> connected;
    CustomVector<int> disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);

    // Update power status
    CustomVector<int> newlyRestored;
    for (size_t i = 0; i < connected.size(); ++i) {
        Location* loc = powerGrid.getLocation(connected[i]);
        if (loc) {
            if (!loc->isPowered) {
                newlyRestored.push_back(loc->id);
            }
            loc->isPowered = true;
            locationTable.update(loc->id, *loc);
        }
    }
    for (size_t i = 0; i < disconnected.size(); ++i) {
        Location* loc = powerGrid.getLocation(disconnected[i]);
        if (loc) {
            loc->isPowered = false;
            locationTable.update(loc->id, *loc);
        }
    }

    // STEP 8: Display newly restored locations
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "ELECTRICITY RESTORATION RESULTS (POST-REPAIR BFS ANALYSIS):\n";
    if (newlyRestored.empty()) {
        std::cout << "   Line #" << lineId << " restored. Network redundancy and stability reinforced.\n";
    } else {
        std::cout << "   " << newlyRestored.size() << " Location(s) successfully re-energized:\n";
        for (size_t i = 0; i < newlyRestored.size(); ++i) {
            const Location* loc = powerGrid.getLocation(newlyRestored[i]);
            if (loc) {
                std::cout << "   + [" << std::setw(2) << loc->id << "] "
                          << std::setw(32) << std::left << loc->name
                          << " (Pop: " << loc->population << " restored in " << loc->area << ")\n";
            }
        }
    }

    // STEP 9: Update fault status
    faultToRepair.status = REPAIRED;
    faultTable.update(fId, faultToRepair);

    // STEP 10: Record repair and restoration events in incident history
    logEvent("Line #" + std::to_string(lineId) + " successfully repaired and re-energized.", fId, "REPAIR_COMPLETED");
    logEvent("Electricity restored to " + std::to_string(newlyRestored.size()) + " location(s).", fId, "ELECTRICITY_RESTORED");

    std::cout << "========================================================================================\n";
    std::cout << "Repair complete. Fault #" << fId << " marked as REPAIRED.\n";
    return true;
}

bool Simulation::restorePowerLineDirectly(int lineId) {
    PowerLine* line = powerGrid.getPowerLine(lineId);
    if (!line) {
        std::cout << "[ERROR] Power line #" << lineId << " does not exist.\n";
        return false;
    }
    if (line->isOperational) {
        std::cout << "[INFO] Power line #" << lineId << " is ALREADY operational.\n";
        return false;
    }

    powerGrid.restorePowerLine(lineId);
    undoStack.push(UndoAction(ACTION_LINE_RESTORED, lineId, -1,
                              "Manual direct restoration of Line #" + std::to_string(lineId)));

    // Re-run BFS to restore status
    CustomVector<int> connected, disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);

    for (size_t i = 0; i < connected.size(); ++i) {
        Location* loc = powerGrid.getLocation(connected[i]);
        if (loc) {
            loc->isPowered = true;
            locationTable.update(loc->id, *loc);
        }
    }

    logEvent("Line #" + std::to_string(lineId) + " manually restored.", -1, "MANUAL_RESTORATION");
    std::cout << "[SUCCESS] Power line #" << lineId << " restored to operational status.\n";
    return true;
}

bool Simulation::undoLastOperation() {
    if (undoStack.isEmpty()) {
        std::cout << "[INFO] Undo Stack is empty. No reversible operations to undo.\n";
        return false;
    }

    UndoAction action = undoStack.pop();
    simulationTimeMinutes += 5;

    std::cout << "\n========================================================================================\n";
    std::cout << "                              REVERSING LAST OPERATION                                  \n";
    std::cout << "========================================================================================\n";
    std::cout << "Reversing: " << action.description << " (Target ID: " << action.targetId << ")\n";

    if (action.type == ACTION_LINE_DISABLED) {
        // Line was disabled: Undo by restoring the line!
        powerGrid.restorePowerLine(action.targetId);

        // Update fault record if it was generated
        if (action.secondaryId != -1) {
            Fault* f = faultTable.searchPtr(action.secondaryId);
            if (f) {
                f->status = REPAIRED;
            }
        }

        // Re-analyze power grid
        CustomVector<int> connected, disconnected;
        powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);
        for (size_t i = 0; i < connected.size(); ++i) {
            Location* loc = powerGrid.getLocation(connected[i]);
            if (loc) {
                loc->isPowered = true;
                locationTable.update(loc->id, *loc);
            }
        }

        logEvent("Reversed failure of Line #" + std::to_string(action.targetId) + "; line re-enabled.",
                 action.secondaryId, "OPERATION_UNDONE");

        std::cout << "[STATUS] Line #" << action.targetId << " re-enabled. Grid connectivity restored.\n";
    } else if (action.type == ACTION_LINE_RESTORED) {
        // Line was restored: Undo by disabling the line!
        powerGrid.disablePowerLine(action.targetId);

        if (action.secondaryId != -1) {
            Fault* f = faultTable.searchPtr(action.secondaryId);
            if (f) {
                f->status = PENDING;
                repairQueue.insert(*f);
            }
        }

        // Re-analyze power grid
        CustomVector<int> connected, disconnected;
        powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);
        for (size_t i = 0; i < disconnected.size(); ++i) {
            Location* loc = powerGrid.getLocation(disconnected[i]);
            if (loc) {
                loc->isPowered = false;
                locationTable.update(loc->id, *loc);
            }
        }

        logEvent("Reversed repair of Line #" + std::to_string(action.targetId) + "; line re-disabled.",
                 action.secondaryId, "OPERATION_UNDONE");

        std::cout << "[STATUS] Line #" << action.targetId << " re-disabled. Grid state returned to outage.\n";
    }

    std::cout << "========================================================================================\n";
    return true;
}

void Simulation::displayPowerNetwork() const {
    powerGrid.displayNetwork();
}

void Simulation::displayAllLocations() const {
    powerGrid.displayAllLocations();
}

void Simulation::displayLocationNeighbors(int locationId) const {
    powerGrid.displayNeighbors(locationId);
}

void Simulation::displayRepairQueue() const {
    repairQueue.displayQueue();
}

void Simulation::displayAllFaults() const {
    std::cout << "========================================================================================\n";
    std::cout << "                                REGISTERED FAULT DATABASE                               \n";
    std::cout << "========================================================================================\n";
    if (faultTable.size() == 0) {
        std::cout << "   [No faults registered in database]\n";
    } else {
        for (int id = 101; id < nextFaultId; ++id) {
            Fault f;
            if (faultTable.search(id, f)) {
                f.display();
            }
        }
    }
    std::cout << "========================================================================================\n";
}

void Simulation::displayIncidentHistoryForward() const {
    incidentHistory.displayForward();
}

void Simulation::displayIncidentHistoryBackward() const {
    incidentHistory.displayBackward();
}

void Simulation::searchIncidentHistory(const std::string& keyword) const {
    incidentHistory.searchEvents(keyword);
}

void Simulation::displayUndoHistory() const {
    undoStack.displayHistory();
}

bool Simulation::searchLocationById(int locId) const {
    Location loc;
    if (locationTable.search(locId, loc)) {
        std::cout << "\n--- Location Found (Hash Table Lookup) ---\n";
        loc.display();
        return true;
    }
    std::cout << "[NOT FOUND] Location ID #" << locId << " does not exist in Hash Table.\n";
    return false;
}

bool Simulation::searchFaultById(int faultId) const {
    Fault f;
    if (faultTable.search(faultId, f)) {
        std::cout << "\n--- Fault Record Found (Hash Table Lookup) ---\n";
        f.display();
        return true;
    }
    std::cout << "[NOT FOUND] Fault ID #" << faultId << " does not exist in Hash Table.\n";
    return false;
}

void Simulation::displayHashTableDiagnostics() const {
    std::cout << "\n========================================================================================\n";
    std::cout << "                          HASH TABLE SEPARATE CHAINING DIAGNOSTICS                      \n";
    std::cout << "========================================================================================\n";
    std::cout << "--- Location Hash Table ---\n";
    locationTable.displayBucketDiagnostics();
    std::cout << "\n--- Fault Hash Table ---\n";
    faultTable.displayBucketDiagnostics();
    std::cout << "========================================================================================\n";
}

void Simulation::runBFSTraversal(int startId) const {
    if (startId == -1) startId = mainPowerStationId;
    CustomVector<int> reachable;
    powerGrid.runBFS(startId, reachable, true);

    std::cout << "\n--- Breadth-First Search (BFS) Traversal using Custom Queue ---\n";
    std::cout << "Starting from Location [" << startId << "] "
              << powerGrid.getLocation(startId)->name << "\n";
    std::cout << "Reachable Locations Traversal Order (" << reachable.size() << " nodes):\n   ";
    for (size_t i = 0; i < reachable.size(); ++i) {
        std::cout << reachable[i];
        if (i + 1 < reachable.size()) std::cout << " -> ";
    }
    std::cout << "\n";
}

void Simulation::runDFSTraversal(int startId) const {
    if (startId == -1) startId = mainPowerStationId;
    CustomVector<int> order;
    powerGrid.runDFS(startId, order, true);

    std::cout << "\n--- Depth-First Search (DFS) Traversal (Recursive) ---\n";
    std::cout << "Starting from Location [" << startId << "] "
              << powerGrid.getLocation(startId)->name << "\n";
    std::cout << "DFS Traversal Order (" << order.size() << " nodes):\n   ";
    for (size_t i = 0; i < order.size(); ++i) {
        std::cout << order[i];
        if (i + 1 < order.size()) std::cout << " -> ";
    }
    std::cout << "\n";
}

void Simulation::analyzeAndDisplayConnectivity() const {
    CustomVector<int> connected, disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);

    std::cout << "\n========================================================================================\n";
    std::cout << "                    KARACHI GRID CONNECTIVITY & POWER FLOW STATUS                       \n";
    std::cout << "========================================================================================\n";
    std::cout << "Main Power Source: [" << mainPowerStationId << "] "
              << powerGrid.getLocation(mainPowerStationId)->name << "\n";
    std::cout << "Total Locations Powered    : " << connected.size() << " / " << powerGrid.getLocationCount() << "\n";
    std::cout << "Total Locations Disconnected : " << disconnected.size() << " / " << powerGrid.getLocationCount() << "\n";

    if (!disconnected.empty()) {
        std::cout << "\nDISCONNECTED CRITICAL FACILITIES & RESIDENTIAL AREAS:\n";
        for (size_t i = 0; i < disconnected.size(); ++i) {
            const Location* loc = powerGrid.getLocation(disconnected[i]);
            if (loc) {
                std::cout << "   ! [" << std::setw(2) << loc->id << "] "
                          << std::setw(32) << std::left << loc->name
                          << " | Area: " << std::setw(20) << loc->area
                          << " | " << loc->getTypeString()
                          << " | Pop: " << loc->population
                          << " | " << loc->getCriticalityString() << "\n";
            }
        }
    } else {
        std::cout << "\n[ALL SYSTEMS NORMAL] Complete Karachi grid connectivity. Zero outages.\n";
    }
    std::cout << "========================================================================================\n";
}

bool Simulation::findRepairCrewRoute(int targetLocationId) const {
    CustomVector<int> path;
    double travelMinutes = 0.0;
    bool found = roadNetwork.findShortestPath(repairDepotLocationId, targetLocationId, path, travelMinutes);

    std::cout << "\n========================================================================================\n";
    std::cout << "                 DIJKSTRA'S SHORTEST ROAD ROUTE CALCULATION (KARACHI)                   \n";
    std::cout << "========================================================================================\n";
    std::cout << "Origin Depot     : [" << repairDepotLocationId << "] "
              << powerGrid.getLocation(repairDepotLocationId)->name << "\n";
    const Location* destLoc = powerGrid.getLocation(targetLocationId);
    std::cout << "Destination Node : [" << targetLocationId << "] "
              << (destLoc ? destLoc->name : "Unknown Location")
              << " (" << (destLoc ? destLoc->area : "Karachi") << ")\n";

    if (!found || path.empty()) {
        std::cout << "[ERROR] No viable road path found to location ID " << targetLocationId << ".\n";
        return false;
    }

    std::cout << "Optimal Path     : ";
    for (size_t i = 0; i < path.size(); ++i) {
        const Location* loc = powerGrid.getLocation(path[i]);
        std::cout << "[" << path[i] << "] " << (loc ? loc->name : "Node");
        if (i + 1 < path.size()) std::cout << " ==> ";
    }
    std::cout << "\nTotal Travel Time: " << std::fixed << std::setprecision(1) << travelMinutes << " minutes.\n";
    std::cout << "========================================================================================\n";
    return true;
}

void Simulation::sortAndDisplayFaults(SortCriterion criterion, bool ascending) {
    int totalFaults = nextFaultId - 101;
    if (totalFaults <= 0) {
        std::cout << "[INFO] No faults recorded yet to sort.\n";
        return;
    }

    // Collect faults into dynamic array for sorting
    Fault* arr = new Fault[totalFaults];
    int count = 0;
    for (int id = 101; id < nextFaultId; ++id) {
        Fault f;
        if (faultTable.search(id, f)) {
            arr[count++] = f;
        }
    }

    if (count == 0) {
        std::cout << "[INFO] No active faults found.\n";
        delete[] arr;
        return;
    }

    // Execute Manual QuickSort
    Sorting::quickSort(arr, 0, count - 1, criterion, ascending);

    std::cout << "\n========================================================================================\n";
    std::cout << "            SORTED FAULT RECORDS BY: "
              << Sorting::getCriterionName(criterion)
              << (ascending ? " (ASCENDING)" : " (DESCENDING)") << "\n";
    std::cout << "========================================================================================\n";
    std::cout << std::left
              << std::setw(10) << "Fault ID"
              << std::setw(10) << "Line ID"
              << std::setw(10) << "Severity"
              << std::setw(12) << "Crit Fac"
              << std::setw(14) << "Pop Affected"
              << std::setw(12) << "Duration"
              << std::setw(16) << "Priority Score"
              << std::setw(12) << "Status"
              << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    for (int i = 0; i < count; ++i) {
        std::cout << std::left
                  << std::setw(10) << arr[i].faultId
                  << std::setw(10) << arr[i].powerLineId
                  << std::setw(10) << arr[i].severity
                  << std::setw(12) << arr[i].criticalFacilitiesAffected
                  << std::setw(14) << arr[i].populationAffected
                  << (std::to_string(arr[i].outageDuration) + "m     ").substr(0, 12)
                  << std::setw(16) << std::fixed << std::setprecision(1) << arr[i].priorityScore
                  << std::setw(12) << arr[i].getStatusString()
                  << "\n";
    }
    std::cout << "========================================================================================\n";

    delete[] arr;
}

void Simulation::displayOutageReportSummary() const {
    CustomVector<int> connected, disconnected;
    powerGrid.analyzeConnectivity(mainPowerStationId, connected, disconnected);

    int critCount = 0;
    int popCount = 0;
    for (size_t i = 0; i < disconnected.size(); ++i) {
        const Location* loc = powerGrid.getLocation(disconnected[i]);
        if (loc) {
            popCount += loc->population;
            if (loc->isCriticalFacility()) {
                critCount++;
            }
        }
    }

    std::cout << "\n========================================================================================\n";
    std::cout << "                   KARACHI CURRENT ACTIVE OUTAGE SUMMARY                                \n";
    std::cout << "========================================================================================\n";
    std::cout << "Total Disconnected Locations : " << disconnected.size() << "\n";
    std::cout << "Total Disconnected Residents : " << popCount << "\n";
    std::cout << "Critical Facilities Affected : " << critCount << "\n";
    std::cout << "Pending Faults in Heap       : " << repairQueue.size() << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    if (disconnected.empty()) {
        std::cout << "All 44 Karachi locations receiving power. No active outages.\n";
    } else {
        for (size_t i = 0; i < disconnected.size(); ++i) {
            const Location* loc = powerGrid.getLocation(disconnected[i]);
            if (loc) {
                std::cout << "   ! [" << std::setw(2) << loc->id << "] "
                          << std::setw(32) << std::left << loc->name
                          << " | " << std::setw(18) << loc->area
                          << " | Pop: " << std::setw(6) << loc->population
                          << " | Crit: " << loc->getCriticalityString() << "\n";
            }
        }
    }
    std::cout << "========================================================================================\n";
}

void Simulation::displayCriticalFacilitiesStatus() const {
    std::cout << "\n========================================================================================\n";
    std::cout << "                 KARACHI CRITICAL FACILITIES MONITORING REPORT                          \n";
    std::cout << "========================================================================================\n";
    for (int i = 1; i <= 44; ++i) {
        const Location* loc = powerGrid.getLocation(i);
        if (loc && loc->isCriticalFacility()) {
            std::cout << "Facility: [" << std::setw(2) << loc->id << "] "
                      << std::setw(34) << std::left << loc->name
                      << " | Area: " << std::setw(22) << loc->area
                      << " | Type: " << std::setw(12) << loc->getTypeString()
                      << " | " << (loc->isPowered ? "[ONLINE - ENERGIZED]" : "[*** OFFLINE - EMERGENCY ***]")
                      << "\n";
        }
    }
    std::cout << "========================================================================================\n";
}
