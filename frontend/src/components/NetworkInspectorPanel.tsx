import React from 'react';
import {
  X,
  Zap,
  Activity,
  AlertTriangle,
  Wrench,
  Navigation,
  Shield,
  Layers,
  Power,
  RotateCw,
} from 'lucide-react';
import { LocationNode, PowerLineEdge } from '../types';

interface NetworkInspectorPanelProps {
  selectedNode: LocationNode | null;
  selectedEdge: PowerLineEdge | null;
  locations: LocationNode[];
  powerLines: PowerLineEdge[];
  onClose: () => void;
  onSimulateLineFailure: (lineId: number) => void;
  onRestoreLine: (lineId: number) => void;
  onInspectRoute: (destinationId: number) => void;
}

export const NetworkInspectorPanel: React.FC<NetworkInspectorPanelProps> = ({
  selectedNode,
  selectedEdge,
  locations,
  powerLines,
  onClose,
  onSimulateLineFailure,
  onRestoreLine,
  onInspectRoute,
}) => {
  if (!selectedNode && !selectedEdge) return null;

  const getLocationName = (id: number) => {
    return locations.find((l) => l.id === id)?.name || `Location #${id}`;
  };

  return (
    <aside className="w-80 sm:w-96 bg-[#101A2B] border-l border-[#26344A] h-full flex flex-col justify-between shadow-2xl z-20 animate-in slide-in-from-right duration-200">
      <div>
        {/* Panel Header */}
        <div className="p-4 border-b border-[#26344A] flex items-center justify-between bg-[#151F32]">
          <div className="flex items-center gap-2">
            <Layers className="w-4 h-4 text-blue-400" />
            <h3 className="text-sm font-bold text-[#F1F5F9]">
              {selectedNode ? 'Facility Node Inspector' : 'Transmission Line Inspector'}
            </h3>
          </div>
          <button
            onClick={onClose}
            className="p-1 rounded-lg text-[#94A3B8] hover:text-[#F1F5F9] hover:bg-[#26344A] transition-all"
          >
            <X className="w-4 h-4" />
          </button>
        </div>

        {/* Panel Content */}
        <div className="p-5 space-y-5 overflow-y-auto max-h-[calc(100vh-140px)]">
          {/* Node Selected */}
          {selectedNode && (
            <>
              {/* Header Card */}
              <div className="p-4 rounded-xl bg-[#151F32] border border-[#26344A]">
                <div className="flex items-center justify-between mb-2">
                  <span className="text-[11px] font-bold text-[#94A3B8] uppercase tracking-wider">
                    Location ID #{selectedNode.id}
                  </span>
                  <span
                    className={`text-[10px] font-bold px-2 py-0.5 rounded-full border ${
                      selectedNode.isPowered
                        ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/30'
                        : 'bg-red-500/10 text-red-400 border-red-500/30 animate-pulse'
                    }`}
                  >
                    {selectedNode.isPowered ? 'POWERED (ONLINE)' : 'DE-ENERGIZED (OUTAGE)'}
                  </span>
                </div>
                <h2 className="text-base font-bold text-[#F1F5F9]">{selectedNode.name}</h2>
                <p className="text-xs text-[#94A3B8] mt-0.5">{selectedNode.type.replace('_', ' ')}</p>
              </div>

              {/* Detailed Metrics */}
              <div className="grid grid-cols-2 gap-3">
                <div className="p-3 rounded-lg bg-[#151F32] border border-[#26344A]">
                  <span className="text-[10px] font-semibold text-[#94A3B8] uppercase">
                    Population Served
                  </span>
                  <p className="text-sm font-bold text-[#F1F5F9] mt-1">
                    {selectedNode.population.toLocaleString()} residents
                  </p>
                </div>

                <div className="p-3 rounded-lg bg-[#151F32] border border-[#26344A]">
                  <span className="text-[10px] font-semibold text-[#94A3B8] uppercase">
                    Criticality Rating
                  </span>
                  <p className="text-sm font-bold text-amber-400 mt-1">
                    {selectedNode.criticality} Priority
                  </p>
                </div>
              </div>

              {/* Connected Lines in Adjacency List */}
              <div>
                <h4 className="text-xs font-bold text-[#94A3B8] uppercase tracking-wider mb-2">
                  Connected Power Lines (Adjacency List)
                </h4>
                <div className="space-y-2">
                  {powerLines
                    .filter(
                      (l) =>
                        l.fromLocationId === selectedNode.id ||
                        l.toLocationId === selectedNode.id
                    )
                    .map((line) => {
                      const otherId =
                        line.fromLocationId === selectedNode.id
                          ? line.toLocationId
                          : line.fromLocationId;
                      return (
                        <div
                          key={line.id}
                          className="p-2.5 rounded-lg bg-[#151F32] border border-[#26344A] flex items-center justify-between text-xs"
                        >
                          <div>
                            <span className="font-semibold text-slate-200">
                              Line #{line.id} &rarr; {getLocationName(otherId)}
                            </span>
                            <div className="text-[10px] text-[#94A3B8] mt-0.5">
                              Repair Est: ${line.repairCost.toLocaleString()} | Road Weight:{' '}
                              {line.travelCost}m
                            </div>
                          </div>
                          <div>
                            {line.isOperational ? (
                              <button
                                onClick={() => onSimulateLineFailure(line.id)}
                                title="Fail Line"
                                className="px-2 py-1 bg-red-600/20 hover:bg-red-600/40 text-red-400 border border-red-500/30 rounded text-[10px] font-bold transition-all"
                              >
                                Trigger Break
                              </button>
                            ) : (
                              <button
                                onClick={() => onRestoreLine(line.id)}
                                title="Restore Line"
                                className="px-2 py-1 bg-emerald-600/20 hover:bg-emerald-600/40 text-emerald-400 border border-emerald-500/30 rounded text-[10px] font-bold transition-all"
                              >
                                Restore
                              </button>
                            )}
                          </div>
                        </div>
                      );
                    })}
                </div>
              </div>

              {/* Action Buttons */}
              <div className="pt-2">
                <button
                  onClick={() => onInspectRoute(selectedNode.id)}
                  className="w-full flex items-center justify-center gap-2 px-4 py-2.5 bg-blue-600 hover:bg-blue-500 text-white rounded-lg text-xs font-bold shadow-glow-blue transition-all"
                >
                  <Navigation className="w-4 h-4" />
                  <span>Calculate Dijkstra Route from Depot</span>
                </button>
              </div>
            </>
          )}

          {/* Edge Selected */}
          {selectedEdge && (
            <>
              {/* Header Card */}
              <div className="p-4 rounded-xl bg-[#151F32] border border-[#26344A]">
                <div className="flex items-center justify-between mb-2">
                  <span className="text-[11px] font-bold text-[#94A3B8] uppercase tracking-wider">
                    Transmission Line #{selectedEdge.id}
                  </span>
                  <span
                    className={`text-[10px] font-bold px-2 py-0.5 rounded-full border ${
                      selectedEdge.isOperational
                        ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/30'
                        : 'bg-red-500/10 text-red-400 border-red-500/30 animate-pulse'
                    }`}
                  >
                    {selectedEdge.isOperational ? 'OPERATIONAL' : 'FAILED / OFF'}
                  </span>
                </div>
                <h3 className="text-sm font-bold text-[#F1F5F9]">
                  {getLocationName(selectedEdge.fromLocationId)} &harr;{' '}
                  {getLocationName(selectedEdge.toLocationId)}
                </h3>
              </div>

              {/* Edge Metrics */}
              <div className="space-y-2.5">
                <div className="p-3 rounded-lg bg-[#151F32] border border-[#26344A] flex justify-between items-center text-xs">
                  <span className="text-[#94A3B8]">Origin Facility:</span>
                  <span className="font-semibold text-slate-200">
                    [{selectedEdge.fromLocationId}] {getLocationName(selectedEdge.fromLocationId)}
                  </span>
                </div>

                <div className="p-3 rounded-lg bg-[#151F32] border border-[#26344A] flex justify-between items-center text-xs">
                  <span className="text-[#94A3B8]">Destination Facility:</span>
                  <span className="font-semibold text-slate-200">
                    [{selectedEdge.toLocationId}] {getLocationName(selectedEdge.toLocationId)}
                  </span>
                </div>

                <div className="p-3 rounded-lg bg-[#151F32] border border-[#26344A] flex justify-between items-center text-xs">
                  <span className="text-[#94A3B8]">Simulated Repair Cost:</span>
                  <span className="font-bold text-amber-400">
                    ${selectedEdge.repairCost.toLocaleString()}
                  </span>
                </div>

                <div className="p-3 rounded-lg bg-[#151F32] border border-[#26344A] flex justify-between items-center text-xs">
                  <span className="text-[#94A3B8]">Travel Distance Weight:</span>
                  <span className="font-bold text-blue-400">
                    {selectedEdge.travelCost} minutes
                  </span>
                </div>
              </div>

              {/* Edge Actions */}
              <div className="pt-3">
                {selectedEdge.isOperational ? (
                  <button
                    onClick={() => onSimulateLineFailure(selectedEdge.id)}
                    className="w-full flex items-center justify-center gap-2 px-4 py-2.5 bg-red-600 hover:bg-red-500 text-white rounded-lg text-xs font-bold shadow-lg shadow-red-900/30 transition-all"
                  >
                    <AlertTriangle className="w-4 h-4" />
                    <span>Simulate Sever / Failure on Line #{selectedEdge.id}</span>
                  </button>
                ) : (
                  <button
                    onClick={() => onRestoreLine(selectedEdge.id)}
                    className="w-full flex items-center justify-center gap-2 px-4 py-2.5 bg-emerald-600 hover:bg-emerald-500 text-white rounded-lg text-xs font-bold shadow-lg shadow-emerald-900/30 transition-all"
                  >
                    <Power className="w-4 h-4" />
                    <span>Restore & Re-energize Line #{selectedEdge.id}</span>
                  </button>
                )}
              </div>
            </>
          )}
        </div>
      </div>
    </aside>
  );
};
