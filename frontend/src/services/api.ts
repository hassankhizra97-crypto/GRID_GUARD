import {
  LocationNode,
  PowerLineEdge,
  Fault,
  IncidentEvent,
  DijkstraRoute,
  SimulationStats,
} from '../types';
import {
  INITIAL_LOCATIONS,
  INITIAL_POWER_LINES,
  INITIAL_ROADS,
} from '../data/initialData';

// Centralized API Service for GridGuard
// Connects to live C++ HTTP endpoint if active, with full high-fidelity fallback
// matching the exact data model and behavior of GridGuard C++ DSA engine.

class GridGuardApiService {
  private apiBaseUrl: string = 'http://localhost:8080/api';
  private isLiveBackend: boolean = false;

  // Local synchronized state mirror
  private locations: LocationNode[] = JSON.parse(JSON.stringify(INITIAL_LOCATIONS));
  private powerLines: PowerLineEdge[] = JSON.parse(JSON.stringify(INITIAL_POWER_LINES));
  private faults: Fault[] = [];
  private incidentHistory: IncidentEvent[] = [
    {
      eventId: 1,
      description: 'GridGuard Simulation initialized with 14 locations and 18 power lines.',
      relatedFaultId: -1,
      eventType: 'SYSTEM_INIT',
      timestamp: 'T+00:00',
    },
  ];
  private undoStack: { type: 'FAILURE' | 'REPAIR'; lineId: number; faultId: number; desc: string }[] = [];
  private nextFaultId: number = 101;
  private nextEventId: number = 2;
  private simulationClockMinutes: number = 0;
  private repairsCompletedCount: number = 0;

  constructor() {
    this.checkBackendHealth();
  }

  public async checkBackendHealth(): Promise<boolean> {
    try {
      const controller = new AbortController();
      const timeoutId = setTimeout(() => controller.abort(), 1200);
      const res = await fetch(`${this.apiBaseUrl}/health`, { signal: controller.signal });
      clearTimeout(timeoutId);
      this.isLiveBackend = res.ok;
    } catch {
      this.isLiveBackend = false;
    }
    return this.isLiveBackend;
  }

  public getBackendStatus(): { isConnected: boolean; mode: 'LIVE_CPP' | 'SYNCED_LOCAL'; url: string } {
    return {
      isConnected: this.isLiveBackend,
      mode: this.isLiveBackend ? 'LIVE_CPP' : 'SYNCED_LOCAL',
      url: this.apiBaseUrl,
    };
  }

  public setBackendUrl(url: string) {
    this.apiBaseUrl = url;
    this.checkBackendHealth();
  }

  private getTimeString(): string {
    const hours = Math.floor(this.simulationClockMinutes / 60) % 24;
    const mins = this.simulationClockMinutes % 60;
    return `T+${hours.toString().padStart(2, '0')}:${mins.toString().padStart(2, '0')}`;
  }

  // Exact BFS implementation mirroring C++ Graph::analyzeConnectivity
  private recomputePowerReachability(): { connected: number[]; disconnected: number[] } {
    const visited = new Set<number>();
    const queue: number[] = [];

    // Start BFS at Main Power Station (ID 1)
    queue.push(1);
    visited.add(1);

    while (queue.length > 0) {
      const u = queue.shift()!;
      // Find operational edges incident to u
      for (const line of this.powerLines) {
        if (!line.isOperational) continue;

        if (line.fromLocationId === u && !visited.has(line.toLocationId)) {
          visited.add(line.toLocationId);
          queue.push(line.toLocationId);
        } else if (line.toLocationId === u && !visited.has(line.fromLocationId)) {
          visited.add(line.fromLocationId);
          queue.push(line.fromLocationId);
        }
      }
    }

    const connected: number[] = [];
    const disconnected: number[] = [];

    for (const loc of this.locations) {
      if (visited.has(loc.id)) {
        loc.isPowered = true;
        connected.push(loc.id);
      } else {
        loc.isPowered = false;
        disconnected.push(loc.id);
      }
    }

    return { connected, disconnected };
  }

  // Exact Dijkstra Algorithm mirroring C++ RoadNetwork::findShortestPath
  public getDijkstraRoute(destinationId: number): DijkstraRoute {
    const originId = 1; // Depot is at Main Power Station
    const originLoc = this.locations.find((l) => l.id === originId)!;
    const destLoc = this.locations.find((l) => l.id === destinationId) || originLoc;

    if (originId === destinationId) {
      return {
        originId,
        originName: originLoc.name,
        destinationId,
        destinationName: destLoc.name,
        path: [originId],
        pathNames: [originLoc.name],
        totalTravelMinutes: 0,
        steps: [],
      };
    }

    const INF = 1e9;
    const dist: { [key: number]: number } = {};
    const parent: { [key: number]: number } = {};
    const visited = new Set<number>();

    for (const loc of this.locations) {
      dist[loc.id] = INF;
      parent[loc.id] = -1;
    }
    dist[originId] = 0;

    for (let i = 0; i < this.locations.length; i++) {
      let u = -1;
      let minVal = INF;

      for (const loc of this.locations) {
        if (!visited.has(loc.id) && dist[loc.id] < minVal) {
          minVal = dist[loc.id];
          u = loc.id;
        }
      }

      if (u === -1 || u === destinationId) break;
      visited.add(u);

      // Relax road edges
      for (const road of INITIAL_ROADS) {
        let v = -1;
        if (road.u === u) v = road.v;
        else if (road.v === u) v = road.u;

        if (v !== -1 && !visited.has(v)) {
          const newDist = dist[u] + road.travelMinutes;
          if (newDist < dist[v]) {
            dist[v] = newDist;
            parent[v] = u;
          }
        }
      }
    }

    const path: number[] = [];
    let curr = destinationId;
    while (curr !== -1) {
      path.unshift(curr);
      curr = parent[curr];
    }

    const pathNames = path.map((id) => this.locations.find((l) => l.id === id)?.name || `Loc ${id}`);
    const steps = [];
    for (let i = 0; i < path.length - 1; i++) {
      const u = path[i];
      const v = path[i + 1];
      const road = INITIAL_ROADS.find((r) => (r.u === u && r.v === v) || (r.u === v && r.v === u));
      steps.push({
        fromId: u,
        toId: v,
        fromName: this.locations.find((l) => l.id === u)?.name || `Loc ${u}`,
        toName: this.locations.find((l) => l.id === v)?.name || `Loc ${v}`,
        segmentMinutes: road ? road.travelMinutes : 5.0,
      });
    }

    return {
      originId,
      originName: originLoc.name,
      destinationId,
      destinationName: destLoc.name,
      path,
      pathNames,
      totalTravelMinutes: dist[destinationId] < INF ? dist[destinationId] : 0,
      steps,
    };
  }

  // Get full state of network
  public async getNetworkState(): Promise<{
    locations: LocationNode[];
    powerLines: PowerLineEdge[];
    faults: Fault[];
    incidentHistory: IncidentEvent[];
    stats: SimulationStats;
  }> {
    if (this.isLiveBackend) {
      try {
        const res = await fetch(`${this.apiBaseUrl}/network-state`);
        if (res.ok) {
          return await res.json();
        }
      } catch {
        this.isLiveBackend = false;
      }
    }

    const operationalLines = this.powerLines.filter((l) => l.isOperational).length;
    const failedLines = this.powerLines.length - operationalLines;
    const activeFaults = this.faults.filter((f) => f.status !== 'REPAIRED').length;
    const disconnectedLocs = this.locations.filter((l) => !l.isPowered);
    const affectedPopulation = disconnectedLocs.reduce((acc, l) => acc + l.population, 0);
    const criticalFacilitiesAffected = disconnectedLocs.filter(
      (l) => l.type === 'HOSPITAL' || l.criticality === 'CRITICAL' || l.criticality === 'HIGH'
    ).length;

    const networkHealthPercentage = Math.round(
      (operationalLines / this.powerLines.length) * 0.7 * 100 +
        (this.locations.filter((l) => l.isPowered).length / this.locations.length) * 0.3 * 100
    );

    return {
      locations: [...this.locations],
      powerLines: [...this.powerLines],
      faults: [...this.faults],
      incidentHistory: [...this.incidentHistory],
      stats: {
        totalLocations: this.locations.length,
        operationalLines,
        failedLines,
        totalLines: this.powerLines.length,
        activeFaults,
        affectedPopulation,
        criticalFacilitiesAffected,
        repairsCompleted: this.repairsCompletedCount,
        networkHealthPercentage,
      },
    };
  }

  // Section 5: Simulate Power Line Failure (12-step workflow)
  public async simulatePowerLineFailure(
    lineId: number,
    severity: number = 3,
    durationMinutes: number = 60
  ): Promise<{ success: boolean; fault?: Fault; message: string }> {
    if (this.isLiveBackend) {
      try {
        const res = await fetch(`${this.apiBaseUrl}/simulate-failure`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ lineId, severity, durationMinutes }),
        });
        if (res.ok) return await res.json();
      } catch {
        this.isLiveBackend = false;
      }
    }

    const line = this.powerLines.find((l) => l.id === lineId);
    if (!line) return { success: false, message: `Power Line #${lineId} not found.` };
    if (!line.isOperational) return { success: false, message: `Power Line #${lineId} has already failed.` };

    // Record pre-failure reachability
    const before = this.recomputePowerReachability();

    // STEP 2: Mark failed
    line.isOperational = false;
    this.simulationClockMinutes += 15;

    // STEP 3: Assign unique fault ID
    const fId = this.nextFaultId++;

    // STEP 4: Push to undo stack
    this.undoStack.push({
      type: 'FAILURE',
      lineId,
      faultId: fId,
      desc: `Simulated failure on Power Line #${lineId}`,
    });

    // STEP 5: Run BFS to find new reachability
    const after = this.recomputePowerReachability();

    // Isolated directly by this line (before connected minus after connected)
    const newlyIsolated = before.connected.filter((id) => !after.connected.includes(id));
    const isolatedLocs = this.locations.filter((l) => newlyIsolated.includes(l.id));

    const popAff = isolatedLocs.reduce((acc, l) => acc + l.population, 0);
    const critAff = isolatedLocs.filter(
      (l) => l.type === 'HOSPITAL' || l.criticality === 'CRITICAL' || l.criticality === 'HIGH'
    ).length;

    // STEP 8: Calculate priority score
    // Priority Score = (Severity * 10) + (CritFac * 20) + (Pop / 100) + (Duration * 2)
    const priorityScore =
      severity * 10.0 + critAff * 20.0 + popAff / 100.0 + durationMinutes * 2.0;

    const fault: Fault = {
      faultId: fId,
      powerLineId: lineId,
      severity,
      criticalFacilitiesAffected: critAff,
      populationAffected: popAff,
      outageDuration: durationMinutes,
      priorityScore: parseFloat(priorityScore.toFixed(1)),
      status: 'PENDING',
    };

    // STEP 9 & 10: Insert into Heap & Hash Table
    this.faults.push(fault);
    // Keep sorted by priority score descending (mimicking Max-Heap peek)
    this.faults.sort((a, b) => b.priorityScore - a.priorityScore);

    // STEP 11: Add failure event to incident history
    const fromName = this.locations.find((l) => l.id === line.fromLocationId)?.name || 'Node';
    const toName = this.locations.find((l) => l.id === line.toLocationId)?.name || 'Node';
    this.incidentHistory.unshift({
      eventId: this.nextEventId++,
      description: `Power Line #${lineId} (${fromName} <---> ${toName}) FAILED. ${newlyIsolated.length} location(s) newly isolated.`,
      relatedFaultId: fId,
      eventType: 'FAILURE_DETECTED',
      timestamp: this.getTimeString(),
    });

    return { success: true, fault, message: `Fault #${fId} registered on Line #${lineId}.` };
  }

  // Section 5: Process Repair (10-step workflow)
  public async processRepair(
    specificFaultId?: number
  ): Promise<{ success: boolean; repairedFault?: Fault; message: string; route?: DijkstraRoute }> {
    if (this.isLiveBackend) {
      try {
        const res = await fetch(`${this.apiBaseUrl}/process-repair`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ faultId: specificFaultId }),
        });
        if (res.ok) return await res.json();
      } catch {
        this.isLiveBackend = false;
      }
    }

    const pendingFaults = this.faults.filter((f) => f.status !== 'REPAIRED');
    if (pendingFaults.length === 0) {
      return { success: false, message: 'No pending faults in repair queue.' };
    }

    // Extract highest-priority fault (or specific fault)
    let faultToRepair: Fault | undefined;
    if (specificFaultId) {
      faultToRepair = this.faults.find((f) => f.faultId === specificFaultId);
    } else {
      faultToRepair = pendingFaults[0]; // Already sorted by Max-Heap priority
    }

    if (!faultToRepair) {
      return { success: false, message: 'Fault record not found.' };
    }

    const line = this.powerLines.find((l) => l.id === faultToRepair!.powerLineId);
    if (!line) return { success: false, message: 'Corrupted line ID in fault record.' };

    this.simulationClockMinutes += 30;

    // STEP 2: Calculate Dijkstra route
    const destination = line.toLocationId;
    const route = this.getDijkstraRoute(destination);

    // STEP 4: Mark in progress
    faultToRepair.status = 'IN_PROGRESS';
    this.incidentHistory.unshift({
      eventId: this.nextEventId++,
      description: `Repair crew dispatched to Line #${line.id} via Dijkstra route (${route.totalTravelMinutes.toFixed(1)}m).`,
      relatedFaultId: faultToRepair.faultId,
      eventType: 'REPAIR_STARTED',
      timestamp: this.getTimeString(),
    });

    // STEP 6: Restore line
    line.isOperational = true;
    this.undoStack.push({
      type: 'REPAIR',
      lineId: line.id,
      faultId: faultToRepair.faultId,
      desc: `Completed repair of Line #${line.id}`,
    });

    // STEP 7: Recompute BFS
    this.recomputePowerReachability();

    // STEP 9: Mark repaired
    faultToRepair.status = 'REPAIRED';
    this.repairsCompletedCount++;

    // STEP 10: Record incident events
    this.incidentHistory.unshift({
      eventId: this.nextEventId++,
      description: `Line #${line.id} successfully repaired and re-energized. Power flow normalized.`,
      relatedFaultId: faultToRepair.faultId,
      eventType: 'REPAIR_COMPLETED',
      timestamp: this.getTimeString(),
    });

    return {
      success: true,
      repairedFault: faultToRepair,
      message: `Fault #${faultToRepair.faultId} on Line #${line.id} successfully repaired.`,
      route,
    };
  }

  // Section 3.F: Undo Last Operation
  public async undoLastOperation(): Promise<{ success: boolean; message: string }> {
    if (this.isLiveBackend) {
      try {
        const res = await fetch(`${this.apiBaseUrl}/undo`, { method: 'POST' });
        if (res.ok) return await res.json();
      } catch {
        this.isLiveBackend = false;
      }
    }

    if (this.undoStack.length === 0) {
      return { success: false, message: 'Undo stack is empty. No reversible operations.' };
    }

    const action = this.undoStack.pop()!;
    this.simulationClockMinutes += 5;

    if (action.type === 'FAILURE') {
      // Restore the line
      const line = this.powerLines.find((l) => l.id === action.lineId);
      if (line) line.isOperational = true;

      const fault = this.faults.find((f) => f.faultId === action.faultId);
      if (fault) fault.status = 'REPAIRED';

      this.recomputePowerReachability();

      this.incidentHistory.unshift({
        eventId: this.nextEventId++,
        description: `Operation undone: Restored Line #${action.lineId} to operational state.`,
        relatedFaultId: action.faultId,
        eventType: 'OPERATION_UNDONE',
        timestamp: this.getTimeString(),
      });

      return { success: true, message: `Reversed failure of Line #${action.lineId}. Grid re-energized.` };
    } else if (action.type === 'REPAIR') {
      // Re-disable the line
      const line = this.powerLines.find((l) => l.id === action.lineId);
      if (line) line.isOperational = false;

      const fault = this.faults.find((f) => f.faultId === action.faultId);
      if (fault) fault.status = 'PENDING';
      this.repairsCompletedCount = Math.max(0, this.repairsCompletedCount - 1);

      this.recomputePowerReachability();

      this.incidentHistory.unshift({
        eventId: this.nextEventId++,
        description: `Operation undone: Line #${action.lineId} re-disabled back to fault state.`,
        relatedFaultId: action.faultId,
        eventType: 'OPERATION_UNDONE',
        timestamp: this.getTimeString(),
      });

      return { success: true, message: `Reversed repair of Line #${action.lineId}. Fault re-opened.` };
    }

    return { success: false, message: 'Unknown undo action.' };
  }

  // Manual restoration of a specific line
  public async restorePowerLineManually(lineId: number): Promise<{ success: boolean; message: string }> {
    const line = this.powerLines.find((l) => l.id === lineId);
    if (!line) return { success: false, message: `Power line #${lineId} not found.` };
    if (line.isOperational) return { success: false, message: `Power line #${lineId} is already operational.` };

    line.isOperational = true;
    this.recomputePowerReachability();

    this.incidentHistory.unshift({
      eventId: this.nextEventId++,
      description: `Line #${lineId} manually restored by operator override.`,
      relatedFaultId: -1,
      eventType: 'MANUAL_RESTORATION',
      timestamp: this.getTimeString(),
    });

    return { success: true, message: `Line #${lineId} restored to service.` };
  }

  // Reset to initial baseline
  public resetSimulation() {
    this.locations = JSON.parse(JSON.stringify(INITIAL_LOCATIONS));
    this.powerLines = JSON.parse(JSON.stringify(INITIAL_POWER_LINES));
    this.faults = [];
    this.undoStack = [];
    this.nextFaultId = 101;
    this.nextEventId = 2;
    this.simulationClockMinutes = 0;
    this.repairsCompletedCount = 0;
    this.incidentHistory = [
      {
        eventId: 1,
        description: 'GridGuard Simulation reset to initial baseline network topology.',
        relatedFaultId: -1,
        eventType: 'SYSTEM_INIT',
        timestamp: 'T+00:00',
      },
    ];
    this.recomputePowerReachability();
  }
}

export const api = new GridGuardApiService();
