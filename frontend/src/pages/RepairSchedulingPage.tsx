import React, { useState } from 'react';
import {
  Wrench,
  Navigation,
  ShieldAlert,
  ArrowRight,
  Clock,
  Users,
  Activity,
  CheckCircle2,
  AlertTriangle,
  MapPin,
  Truck,
  RotateCw,
} from 'lucide-react';
import { Fault, PowerLineEdge, LocationNode, DijkstraRoute } from '../types';
import { StatusBadge } from '../components/StatusBadge';

interface RepairSchedulingPageProps {
  faults: Fault[];
  powerLines: PowerLineEdge[];
  locations: LocationNode[];
  onProcessNextRepair: (specificFaultId?: number) => void;
  getDijkstraRoute: (destinationId: number) => DijkstraRoute;
}

export const RepairSchedulingPage: React.FC<RepairSchedulingPageProps> = ({
  faults,
  powerLines,
  locations,
  onProcessNextRepair,
  getDijkstraRoute,
}) => {
  const pendingFaults = faults
    .filter((f) => f.status !== 'REPAIRED')
    .sort((a, b) => b.priorityScore - a.priorityScore);

  const topFault = pendingFaults.length > 0 ? pendingFaults[0] : null;

  const [selectedFaultId, setSelectedFaultId] = useState<number | null>(
    topFault ? topFault.faultId : null
  );

  const activeFault =
    pendingFaults.find((f) => f.faultId === selectedFaultId) || topFault;

  const getLocationName = (id: number) => {
    return locations.find((l) => l.id === id)?.name || `Location ${id}`;
  };

  const getLine = (lineId: number) => {
    return powerLines.find((l) => l.id === lineId);
  };

  // Compute route for active fault
  const activeLine = activeFault ? getLine(activeFault.powerLineId) : null;
  const currentRoute: DijkstraRoute | null = activeLine
    ? getDijkstraRoute(activeLine.toLocationId)
    : null;

  return (
    <div className="space-y-6 animate-in fade-in duration-300">
      {/* 1. Top Section: Spotlight Highest-Priority Work Order */}
      {topFault ? (
        <div className="relative overflow-hidden bg-gradient-to-r from-[#182338] via-[#151F32] to-[#182338] border-2 border-amber-500/40 rounded-2xl p-6 sm:p-7 shadow-2xl">
          <div className="flex flex-col lg:flex-row lg:items-center justify-between gap-6">
            <div className="space-y-3">
              <div className="flex items-center gap-3">
                <span className="px-2.5 py-1 bg-amber-500/20 text-amber-400 border border-amber-500/30 rounded-lg text-xs font-extrabold uppercase tracking-wider flex items-center gap-1.5">
                  <ShieldAlert className="w-3.5 h-3.5" />
                  <span>Max-Heap Root #1 &bull; Highest Priority Emergency</span>
                </span>
                <span className="text-xs text-[#94A3B8] font-mono">
                  Fault ID #{topFault.faultId}
                </span>
              </div>

              <div>
                <h2 className="text-xl sm:text-2xl font-bold text-[#F1F5F9]">
                  Line #{topFault.powerLineId}:{' '}
                  {activeLine
                    ? `${getLocationName(activeLine.fromLocationId)} ↔ ${getLocationName(activeLine.toLocationId)}`
                    : `Line #${topFault.powerLineId}`}
                </h2>
                <p className="text-xs text-[#94A3B8] mt-1">
                  Severity {topFault.severity}/5 &bull; {topFault.populationAffected.toLocaleString()} residents without power &bull;{' '}
                  {topFault.criticalFacilitiesAffected} critical facilities de-energized
                </p>
              </div>

              {/* Priority Score Breakdown Formula */}
              <div className="inline-flex items-center gap-2 p-2 rounded-lg bg-[#0B1120] border border-[#26344A] text-xs">
                <span className="text-[#94A3B8]">Scoring Formula:</span>
                <span className="font-mono text-amber-400 font-bold">
                  ({topFault.severity} &times; 10) + ({topFault.criticalFacilitiesAffected} &times; 20) + (
                  {topFault.populationAffected} / 100) + ({topFault.outageDuration} &times; 2)
                </span>
                <span className="text-slate-400">&rarr;</span>
                <span className="text-base font-extrabold text-amber-300 font-mono">
                  {topFault.priorityScore.toFixed(1)} PTS
                </span>
              </div>
            </div>

            {/* Big Action Button */}
            <div className="flex flex-col sm:flex-row lg:flex-col gap-3">
              <button
                onClick={() => onProcessNextRepair(topFault.faultId)}
                className="px-6 py-4 bg-emerald-600 hover:bg-emerald-500 text-white rounded-xl text-sm font-extrabold shadow-lg shadow-emerald-900/50 flex items-center justify-center gap-3 transition-all hover:scale-[1.02] active:scale-[0.98]"
              >
                <Wrench className="w-5 h-5" />
                <span>Process Next Repair & Dispatch Crew</span>
              </button>

              <p className="text-[11px] text-[#94A3B8] text-center">
                Uses Dijkstra road routing &bull; Re-analyzes BFS connectivity
              </p>
            </div>
          </div>
        </div>
      ) : (
        <div className="p-8 rounded-2xl bg-[#151F32] border border-[#26344A] text-center">
          <CheckCircle2 className="w-12 h-12 mx-auto text-emerald-400 mb-2" />
          <h2 className="text-lg font-bold text-[#F1F5F9]">All Transmission Lines Operational</h2>
          <p className="text-xs text-[#94A3B8] mt-1">
            The emergency repair max-heap queue is empty. Complete network continuity verified.
          </p>
        </div>
      )}

      {/* 2. Grid with Priority Queue on Left and Dijkstra Route on Right */}
      <div className="grid grid-cols-1 lg:grid-cols-12 gap-6">
        {/* Left Column (5 cols): Max-Heap Priority Queue */}
        <div className="lg:col-span-5 bg-[#151F32] border border-[#26344A] rounded-xl p-5 shadow-panel">
          <div className="flex items-center justify-between mb-4">
            <div>
              <h3 className="text-sm font-bold text-[#F1F5F9]">
                Repair Priority Queue (Binary Max-Heap)
              </h3>
              <p className="text-xs text-[#94A3B8]">
                {pendingFaults.length} work orders ordered by priority score
              </p>
            </div>
            <span className="text-[10px] font-bold px-2 py-0.5 rounded bg-blue-500/10 text-blue-400 border border-blue-500/20">
              O(1) Root Peek
            </span>
          </div>

          <div className="space-y-2.5">
            {pendingFaults.map((fault, index) => {
              const line = getLine(fault.powerLineId);
              const isSelected = activeFault?.faultId === fault.faultId;
              return (
                <div
                  key={fault.faultId}
                  onClick={() => setSelectedFaultId(fault.faultId)}
                  className={`p-3.5 rounded-xl border transition-all cursor-pointer ${
                    isSelected
                      ? 'bg-[#101A2B] border-blue-500 shadow-glow-blue'
                      : 'bg-[#0B1120] border-[#26344A] hover:border-slate-500'
                  }`}
                >
                  <div className="flex items-start justify-between">
                    <div className="flex items-center gap-2">
                      <span className="w-5 h-5 rounded-full bg-[#151F32] border border-[#26344A] text-[10px] font-bold text-[#94A3B8] flex items-center justify-center">
                        {index + 1}
                      </span>
                      <div>
                        <span className="text-xs font-bold text-[#F1F5F9]">
                          Line #{fault.powerLineId} Fault
                        </span>
                        <p className="text-[11px] text-[#94A3B8] mt-0.5">
                          {line
                            ? `${getLocationName(line.fromLocationId)} ↔ ${getLocationName(line.toLocationId)}`
                            : `Line #${fault.powerLineId}`}
                        </p>
                      </div>
                    </div>

                    <div className="text-right">
                      <span className="text-xs font-mono font-extrabold text-amber-400">
                        {fault.priorityScore.toFixed(1)}
                      </span>
                      <p className="text-[9px] text-[#94A3B8] uppercase">Priority</p>
                    </div>
                  </div>

                  <div className="mt-2.5 pt-2 border-t border-[#26344A]/50 flex items-center justify-between text-[10px] text-[#94A3B8]">
                    <span>Sev {fault.severity}/5</span>
                    <span>{fault.populationAffected.toLocaleString()} pop offline</span>
                    <StatusBadge status={fault.status} type="critical" size="sm" />
                  </div>
                </div>
              );
            })}

            {pendingFaults.length === 0 && (
              <p className="text-xs text-center text-[#94A3B8] py-8">
                No faults in queue.
              </p>
            )}
          </div>
        </div>

        {/* Right Column (7 cols): Dijkstra Road Route for Selected Work Order */}
        <div className="lg:col-span-7 bg-[#151F32] border border-[#26344A] rounded-xl p-5 shadow-panel">
          <div className="flex items-center justify-between mb-4">
            <div>
              <h3 className="text-sm font-bold text-[#F1F5F9]">
                Repair Crew Transit Route (Dijkstra's Algorithm)
              </h3>
              <p className="text-xs text-[#94A3B8]">
                Separate municipal road network &bull; Independent of electrical breaks
              </p>
            </div>
            <Navigation className="w-4 h-4 text-blue-400" />
          </div>

          {currentRoute ? (
            <div className="space-y-5">
              {/* Route Summary Banner */}
              <div className="p-4 rounded-xl bg-[#0B1120] border border-[#26344A] flex flex-col sm:flex-row sm:items-center justify-between gap-3">
                <div>
                  <span className="text-[10px] font-bold text-[#94A3B8] uppercase">
                    Calculated Optimal Road Path
                  </span>
                  <div className="flex items-center gap-2 mt-1">
                    <span className="text-sm font-bold text-slate-200">
                      [{currentRoute.originId}] {currentRoute.originName}
                    </span>
                    <ArrowRight className="w-4 h-4 text-blue-400" />
                    <span className="text-sm font-bold text-emerald-400">
                      [{currentRoute.destinationId}] {currentRoute.destinationName}
                    </span>
                  </div>
                </div>

                <div className="sm:text-right">
                  <span className="text-[10px] font-bold text-[#94A3B8] uppercase">
                    Total Estimated Transit Time
                  </span>
                  <p className="text-lg font-extrabold text-blue-400 font-mono mt-0.5">
                    {currentRoute.totalTravelMinutes.toFixed(1)} mins
                  </p>
                </div>
              </div>

              {/* Waypoints Visual Path */}
              <div>
                <h4 className="text-xs font-semibold text-[#94A3B8] uppercase tracking-wider mb-2">
                  Road Waypoint Sequence
                </h4>
                <div className="flex flex-wrap items-center gap-2 p-3 bg-[#0B1120] rounded-xl border border-[#26344A]">
                  {currentRoute.path.map((nodeId, idx) => (
                    <React.Fragment key={nodeId}>
                      <span className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg bg-[#151F32] border border-[#26344A] text-xs font-semibold text-[#F1F5F9]">
                        <MapPin className="w-3 h-3 text-blue-400" />
                        <span>[{nodeId}] {getLocationName(nodeId)}</span>
                      </span>
                      {idx < currentRoute.path.length - 1 && (
                        <ArrowRight className="w-3.5 h-3.5 text-[#94A3B8]" />
                      )}
                    </React.Fragment>
                  ))}
                </div>
              </div>

              {/* Step-by-Step Segments Table */}
              <div>
                <h4 className="text-xs font-semibold text-[#94A3B8] uppercase tracking-wider mb-2">
                  Road Segments & Travel Weights
                </h4>
                <div className="overflow-x-auto rounded-lg border border-[#26344A]">
                  <table className="w-full text-left text-xs">
                    <thead className="bg-[#101A2B] text-[#94A3B8] uppercase text-[10px] tracking-wider border-b border-[#26344A]">
                      <tr>
                        <th className="py-2.5 px-3">Leg</th>
                        <th className="py-2.5 px-3">From Facility</th>
                        <th className="py-2.5 px-3">To Facility</th>
                        <th className="py-2.5 px-3 text-right">Road Travel Weight</th>
                      </tr>
                    </thead>
                    <tbody className="divide-y divide-[#26344A]/60 bg-[#0B1120]">
                      {currentRoute.steps.map((step, idx) => (
                        <tr key={idx} className="hover:bg-[#151F32]/50 transition-colors">
                          <td className="py-2 px-3 font-mono font-bold text-blue-400">
                            #{idx + 1}
                          </td>
                          <td className="py-2 px-3 text-slate-200">{step.fromName}</td>
                          <td className="py-2 px-3 text-slate-200">{step.toName}</td>
                          <td className="py-2 px-3 text-right font-mono font-semibold text-emerald-400">
                            {step.segmentMinutes.toFixed(1)} mins
                          </td>
                        </tr>
                      ))}
                      {currentRoute.steps.length === 0 && (
                        <tr>
                          <td colSpan={4} className="py-4 text-center text-[#94A3B8]">
                            Depot origin is already at destination.
                          </td>
                        </tr>
                      )}
                    </tbody>
                  </table>
                </div>
              </div>

              {/* Single Dispatch Action */}
              {activeFault && activeFault.status !== 'REPAIRED' && (
                <div className="pt-2">
                  <button
                    onClick={() => onProcessNextRepair(activeFault.faultId)}
                    className="w-full py-2.5 bg-emerald-600 hover:bg-emerald-500 text-white rounded-lg text-xs font-bold shadow-lg shadow-emerald-900/30 flex items-center justify-center gap-2 transition-all"
                  >
                    <Truck className="w-4 h-4" />
                    <span>Dispatch Crew on Selected Route & Fix Fault #{activeFault.faultId}</span>
                  </button>
                </div>
              )}
            </div>
          ) : (
            <p className="text-xs text-center text-[#94A3B8] py-12">
              Select an incident from the priority queue to inspect its Dijkstra route.
            </p>
          )}
        </div>
      </div>
    </div>
  );
};
