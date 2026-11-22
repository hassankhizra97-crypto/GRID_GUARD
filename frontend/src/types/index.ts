export type FacilityType =
  | 'POWER_STATION'
  | 'SUBSTATION'
  | 'HOSPITAL'
  | 'UNIVERSITY'
  | 'RESIDENTIAL'
  | 'COMMERCIAL'
  | 'INDUSTRIAL';

export type CriticalityLevel = 'LOW' | 'MEDIUM' | 'HIGH' | 'CRITICAL';

export type FaultStatus = 'PENDING' | 'IN_PROGRESS' | 'REPAIRED';

export type EventType =
  | 'FAILURE_DETECTED'
  | 'FAULT_REGISTERED'
  | 'REPAIR_PRIORITIZED'
  | 'REPAIR_STARTED'
  | 'REPAIR_COMPLETED'
  | 'ELECTRICITY_RESTORED'
  | 'OPERATION_UNDONE'
  | 'SYSTEM_INIT'
  | 'MANUAL_RESTORATION';

export interface LocationNode {
  id: number;
  name: string;
  type: FacilityType;
  population: number;
  criticality: CriticalityLevel;
  isPowered: boolean;
  latitude: number;
  longitude: number;
  area: string;
  x?: number;
  y?: number;
  [key: string]: unknown;
}

export interface PowerLineEdge {
  id: number;
  fromLocationId: number;
  toLocationId: number;
  isOperational: boolean;
  repairCost: number;
  travelCost: number;
  capacityMW?: number;
  lengthKM?: number;
}

export interface Fault {
  faultId: number;
  powerLineId: number;
  severity: number; // 1 to 5
  criticalFacilitiesAffected: number;
  populationAffected: number;
  outageDuration: number; // in minutes
  priorityScore: number;
  status: FaultStatus;
}

export interface IncidentEvent {
  eventId: number;
  description: string;
  relatedFaultId: number;
  eventType: EventType;
  timestamp: string;
}

export interface DijkstraStep {
  fromId: number;
  toId: number;
  fromName: string;
  toName: string;
  fromCoord: [number, number];
  toCoord: [number, number];
  segmentMinutes: number;
}

export interface DijkstraRoute {
  originId: number;
  originName: string;
  destinationId: number;
  destinationName: string;
  path: number[];
  pathNames: string[];
  coordinates: [number, number][];
  totalTravelMinutes: number;
  steps: DijkstraStep[];
}

export interface SimulationStats {
  totalLocations: number;
  operationalLines: number;
  failedLines: number;
  totalLines: number;
  activeFaults: number;
  affectedPopulation: number;
  criticalFacilitiesAffected: number;
  repairsCompleted: number;
  networkHealthPercentage: number;
}

export type ActivePage =
  | 'overview'
  | 'map'
  | 'network'
  | 'faults'
  | 'repairs'
  | 'history'
  | 'settings';
