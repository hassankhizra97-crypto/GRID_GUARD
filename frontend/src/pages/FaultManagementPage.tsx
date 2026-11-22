import React, { useState } from 'react';
import {
  AlertTriangle,
  Search,
  Filter,
  Plus,
  Wrench,
  Navigation,
  CheckCircle,
  Clock,
  ShieldAlert,
  ArrowUpDown,
} from 'lucide-react';
import { Fault, PowerLineEdge, LocationNode } from '../types';
import { StatusBadge } from '../components/StatusBadge';

interface FaultManagementPageProps {
  faults: Fault[];
  powerLines: PowerLineEdge[];
  locations: LocationNode[];
  onSimulateClick: () => void;
  onProcessRepair: (faultId: number) => void;
  onInspectRoute: (destinationId: number) => void;
}

export const FaultManagementPage: React.FC<FaultManagementPageProps> = ({
  faults,
  powerLines,
  locations,
  onSimulateClick,
  onProcessRepair,
  onInspectRoute,
}) => {
  const [searchTerm, setSearchTerm] = useState('');
  const [severityFilter, setSeverityFilter] = useState<string>('ALL');
  const [statusFilter, setStatusFilter] = useState<string>('ALL');
  const [sortField, setSortField] = useState<'priorityScore' | 'severity' | 'population' | 'faultId'>('priorityScore');
  const [sortAsc, setSortAsc] = useState<boolean>(false);

  const getLocationName = (id: number) => {
    return locations.find((l) => l.id === id)?.name || `Node ${id}`;
  };

  const getLineDescription = (lineId: number) => {
    const line = powerLines.find((l) => l.id === lineId);
    if (!line) return `Line #${lineId}`;
    return `Line #${lineId} (${getLocationName(line.fromLocationId)} ↔ ${getLocationName(line.toLocationId)})`;
  };

  const getLineDestinationId = (lineId: number) => {
    const line = powerLines.find((l) => l.id === lineId);
    return line ? line.toLocationId : 1;
  };

  // Filter & Sort
  const filteredFaults = faults.filter((f) => {
    const matchesSearch =
      f.faultId.toString().includes(searchTerm) ||
      f.powerLineId.toString().includes(searchTerm) ||
      getLineDescription(f.powerLineId).toLowerCase().includes(searchTerm.toLowerCase());

    const matchesSeverity =
      severityFilter === 'ALL' || f.severity.toString() === severityFilter;

    const matchesStatus =
      statusFilter === 'ALL' || f.status === statusFilter;

    return matchesSearch && matchesSeverity && matchesStatus;
  });

  filteredFaults.sort((a, b) => {
    let diff = 0;
    if (sortField === 'priorityScore') diff = a.priorityScore - b.priorityScore;
    else if (sortField === 'severity') diff = a.severity - b.severity;
    else if (sortField === 'population') diff = a.populationAffected - b.populationAffected;
    else if (sortField === 'faultId') diff = a.faultId - b.faultId;
    return sortAsc ? diff : -diff;
  });

  const toggleSort = (field: 'priorityScore' | 'severity' | 'population' | 'faultId') => {
    if (sortField === field) {
      setSortAsc(!sortAsc);
    } else {
      setSortField(field);
      setSortAsc(false); // default to descending for priority triage
    }
  };

  const activeCount = faults.filter((f) => f.status !== 'REPAIRED').length;
  const repairedCount = faults.filter((f) => f.status === 'REPAIRED').length;
  const maxScore = faults.length > 0 ? Math.max(...faults.map((f) => f.priorityScore)) : 0;

  return (
    <div className="space-y-6 animate-in fade-in duration-300">
      {/* Header & Metric Cards */}
      <div className="grid grid-cols-1 md:grid-cols-4 gap-4">
        <div className="p-4 rounded-xl bg-[#151F32] border border-[#26344A] shadow-panel">
          <span className="text-xs font-semibold text-[#94A3B8] uppercase">Total Registered Faults</span>
          <p className="text-2xl font-bold text-[#F1F5F9] mt-1">{faults.length}</p>
        </div>
        <div className="p-4 rounded-xl bg-[#151F32] border border-[#26344A] shadow-panel">
          <span className="text-xs font-semibold text-[#94A3B8] uppercase">Active Triage Queue</span>
          <p className="text-2xl font-bold text-red-400 mt-1">{activeCount}</p>
        </div>
        <div className="p-4 rounded-xl bg-[#151F32] border border-[#26344A] shadow-panel">
          <span className="text-xs font-semibold text-[#94A3B8] uppercase">Repairs Completed</span>
          <p className="text-2xl font-bold text-emerald-400 mt-1">{repairedCount}</p>
        </div>
        <div className="p-4 rounded-xl bg-[#151F32] border border-[#26344A] shadow-panel">
          <span className="text-xs font-semibold text-[#94A3B8] uppercase">Peak Priority Score</span>
          <p className="text-2xl font-bold text-amber-400 mt-1">{maxScore.toFixed(1)}</p>
        </div>
      </div>

      {/* Control Bar: Search, Filters & Action */}
      <div className="bg-[#151F32] border border-[#26344A] rounded-xl p-4 shadow-panel flex flex-col md:flex-row md:items-center justify-between gap-4">
        <div className="flex flex-wrap items-center gap-3 flex-1">
          {/* Search */}
          <div className="relative min-w-[240px] flex-1 max-w-xs">
            <Search className="w-4 h-4 absolute left-3 top-1/2 -translate-y-1/2 text-[#94A3B8]" />
            <input
              type="text"
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              placeholder="Search by ID, line, or location..."
              className="w-full pl-9 pr-3 py-1.5 bg-[#0B1120] border border-[#26344A] focus:border-blue-500 rounded-lg text-xs text-[#F1F5F9] placeholder-[#94A3B8] focus:outline-none focus:ring-1 focus:ring-blue-500"
            />
          </div>

          {/* Severity Filter */}
          <div className="flex items-center gap-1.5 text-xs">
            <span className="text-[#94A3B8] font-medium">Severity:</span>
            <select
              value={severityFilter}
              onChange={(e) => setSeverityFilter(e.target.value)}
              className="px-2.5 py-1.5 bg-[#0B1120] border border-[#26344A] rounded-lg text-xs text-[#F1F5F9] focus:outline-none focus:border-blue-500"
            >
              <option value="ALL">All Levels</option>
              <option value="1">Level 1 (Minor)</option>
              <option value="2">Level 2</option>
              <option value="3">Level 3 (Moderate)</option>
              <option value="4">Level 4</option>
              <option value="5">Level 5 (Critical)</option>
            </select>
          </div>

          {/* Status Filter */}
          <div className="flex items-center gap-1.5 text-xs">
            <span className="text-[#94A3B8] font-medium">Status:</span>
            <select
              value={statusFilter}
              onChange={(e) => setStatusFilter(e.target.value)}
              className="px-2.5 py-1.5 bg-[#0B1120] border border-[#26344A] rounded-lg text-xs text-[#F1F5F9] focus:outline-none focus:border-blue-500"
            >
              <option value="ALL">All Statuses</option>
              <option value="PENDING">Pending</option>
              <option value="IN_PROGRESS">In Progress</option>
              <option value="REPAIRED">Repaired</option>
            </select>
          </div>
        </div>

        {/* Action Button */}
        <button
          onClick={onSimulateClick}
          className="flex items-center gap-2 px-4 py-2 bg-red-600 hover:bg-red-500 text-white rounded-lg text-xs font-bold shadow-md shadow-red-900/40 transition-all whitespace-nowrap"
        >
          <Plus className="w-4 h-4" />
          <span>Simulate Failure</span>
        </button>
      </div>

      {/* Fault Data Table */}
      <div className="bg-[#151F32] border border-[#26344A] rounded-xl shadow-panel overflow-hidden">
        <div className="overflow-x-auto">
          <table className="w-full text-left text-xs">
            <thead className="bg-[#101A2B] text-[#94A3B8] uppercase text-[10px] tracking-wider border-b border-[#26344A]">
              <tr>
                <th
                  onClick={() => toggleSort('faultId')}
                  className="py-3 px-4 cursor-pointer hover:text-white transition-colors"
                >
                  <div className="flex items-center gap-1">
                    <span>Fault ID</span>
                    <ArrowUpDown className="w-3 h-3" />
                  </div>
                </th>
                <th className="py-3 px-4">Damaged Transmission Line</th>
                <th
                  onClick={() => toggleSort('severity')}
                  className="py-3 px-4 cursor-pointer hover:text-white transition-colors"
                >
                  <div className="flex items-center gap-1">
                    <span>Severity</span>
                    <ArrowUpDown className="w-3 h-3" />
                  </div>
                </th>
                <th
                  onClick={() => toggleSort('population')}
                  className="py-3 px-4 cursor-pointer hover:text-white transition-colors"
                >
                  <div className="flex items-center gap-1">
                    <span>Population Impact</span>
                    <ArrowUpDown className="w-3 h-3" />
                  </div>
                </th>
                <th className="py-3 px-4">Critical Fac.</th>
                <th className="py-3 px-4">Duration</th>
                <th
                  onClick={() => toggleSort('priorityScore')}
                  className="py-3 px-4 cursor-pointer hover:text-white transition-colors text-amber-400"
                >
                  <div className="flex items-center gap-1 font-bold">
                    <span>Priority Score</span>
                    <ArrowUpDown className="w-3 h-3" />
                  </div>
                </th>
                <th className="py-3 px-4">Status</th>
                <th className="py-3 px-4 text-right">Actions</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-[#26344A]/60">
              {filteredFaults.map((fault) => (
                <tr key={fault.faultId} className="hover:bg-[#101A2B]/50 transition-colors">
                  <td className="py-3.5 px-4 font-mono font-bold text-blue-400">
                    #{fault.faultId}
                  </td>
                  <td className="py-3.5 px-4 font-medium text-[#F1F5F9]">
                    {getLineDescription(fault.powerLineId)}
                  </td>
                  <td className="py-3.5 px-4">
                    <span
                      className={`px-2 py-0.5 rounded text-[10px] font-extrabold border ${
                        fault.severity >= 4
                          ? 'bg-red-500/20 text-red-400 border-red-500/30'
                          : fault.severity === 3
                          ? 'bg-amber-500/20 text-amber-400 border-amber-500/30'
                          : 'bg-blue-500/20 text-blue-400 border-blue-500/30'
                      }`}
                    >
                      SEV {fault.severity} / 5
                    </span>
                  </td>
                  <td className="py-3.5 px-4 font-medium text-slate-300">
                    {fault.populationAffected.toLocaleString()} residents
                  </td>
                  <td className="py-3.5 px-4">
                    <span
                      className={`font-semibold ${
                        fault.criticalFacilitiesAffected > 0 ? 'text-red-400 font-bold' : 'text-slate-400'
                      }`}
                    >
                      {fault.criticalFacilitiesAffected}
                    </span>
                  </td>
                  <td className="py-3.5 px-4 font-mono text-[#94A3B8]">
                    {fault.outageDuration}m
                  </td>
                  <td className="py-3.5 px-4 font-mono font-extrabold text-amber-400 text-sm">
                    {fault.priorityScore.toFixed(1)}
                  </td>
                  <td className="py-3.5 px-4">
                    <StatusBadge
                      status={fault.status}
                      type={
                        fault.status === 'REPAIRED'
                          ? 'operational'
                          : fault.status === 'IN_PROGRESS'
                          ? 'info'
                          : 'critical'
                      }
                      size="sm"
                    />
                  </td>
                  <td className="py-3.5 px-4 text-right">
                    <div className="flex items-center justify-end gap-2">
                      <button
                        onClick={() => onInspectRoute(getLineDestinationId(fault.powerLineId))}
                        title="Calculate Crew Route"
                        className="p-1.5 rounded-lg bg-[#0B1120] border border-[#26344A] text-[#94A3B8] hover:text-blue-400 hover:border-blue-500/40 transition-all"
                      >
                        <Navigation className="w-3.5 h-3.5" />
                      </button>

                      {fault.status !== 'REPAIRED' && (
                        <button
                          onClick={() => onProcessRepair(fault.faultId)}
                          className="flex items-center gap-1 px-2.5 py-1 bg-emerald-600 hover:bg-emerald-500 text-white rounded text-[11px] font-bold shadow transition-all"
                        >
                          <Wrench className="w-3 h-3" />
                          <span>Dispatch</span>
                        </button>
                      )}
                    </div>
                  </td>
                </tr>
              ))}

              {filteredFaults.length === 0 && (
                <tr>
                  <td colSpan={9} className="py-12 text-center text-[#94A3B8]">
                    <ShieldAlert className="w-8 h-8 mx-auto text-emerald-400/40 mb-2" />
                    <p className="font-semibold text-sm text-[#F1F5F9]">No faults found</p>
                    <p className="text-xs text-[#94A3B8] mt-0.5">
                      All monitored transmission lines operational or filter matched zero items.
                    </p>
                  </td>
                </tr>
              )}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
};
