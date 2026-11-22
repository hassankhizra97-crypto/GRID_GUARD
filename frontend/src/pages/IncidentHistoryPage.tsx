import React, { useState } from 'react';
import {
  Clock,
  RotateCcw,
  Search,
  ArrowDownUp,
  ArrowDown,
  ArrowUp,
  AlertTriangle,
  Wrench,
  CheckCircle,
  Zap,
  RotateCw,
  FileText,
  ShieldAlert,
} from 'lucide-react';
import { IncidentEvent } from '../types';

interface IncidentHistoryPageProps {
  incidentHistory: IncidentEvent[];
  onUndoClick: () => void;
  canUndo: boolean;
}

export const IncidentHistoryPage: React.FC<IncidentHistoryPageProps> = ({
  incidentHistory,
  onUndoClick,
  canUndo,
}) => {
  const [searchTerm, setSearchTerm] = useState('');
  const [isReverseOrder, setIsReverseOrder] = useState<boolean>(false);

  // Filter events by keyword
  const filteredEvents = incidentHistory.filter((ev) => {
    return (
      ev.description.toLowerCase().includes(searchTerm.toLowerCase()) ||
      ev.eventType.toLowerCase().includes(searchTerm.toLowerCase()) ||
      (ev.relatedFaultId !== -1 && ev.relatedFaultId.toString().includes(searchTerm)) ||
      ev.eventId.toString().includes(searchTerm)
    );
  });

  // Doubly linked list traversal demonstration:
  // Forward (oldest to newest) vs Backward (newest to oldest)
  const displayEvents = isReverseOrder ? [...filteredEvents].reverse() : filteredEvents;

  const getEventBadge = (type: string) => {
    if (type.includes('FAILURE')) {
      return {
        icon: AlertTriangle,
        color: 'text-red-400 bg-red-500/10 border-red-500/30',
        dot: 'bg-red-400',
      };
    }
    if (type.includes('STARTED')) {
      return {
        icon: Wrench,
        color: 'text-blue-400 bg-blue-500/10 border-blue-500/30',
        dot: 'bg-blue-400',
      };
    }
    if (type.includes('COMPLETED') || type.includes('RESTORED')) {
      return {
        icon: CheckCircle,
        color: 'text-emerald-400 bg-emerald-500/10 border-emerald-500/30',
        dot: 'bg-emerald-400',
      };
    }
    if (type.includes('UNDONE')) {
      return {
        icon: RotateCw,
        color: 'text-amber-400 bg-amber-500/10 border-amber-500/30',
        dot: 'bg-amber-400',
      };
    }
    return {
      icon: Zap,
      color: 'text-slate-300 bg-slate-800 border-slate-700',
      dot: 'bg-slate-400',
    };
  };

  return (
    <div className="space-y-6 animate-in fade-in duration-300">
      {/* Top Header & Undo Action Banner */}
      <div className="bg-[#151F32] border border-[#26344A] rounded-2xl p-6 shadow-panel flex flex-col sm:flex-row sm:items-center justify-between gap-4">
        <div>
          <div className="flex items-center gap-2 mb-1">
            <span className="text-xs font-semibold uppercase tracking-wider text-blue-400">
              Doubly Linked List Audit Ledger
            </span>
            <span className="text-[10px] font-mono px-2 py-0.5 rounded bg-blue-500/10 text-blue-300 border border-blue-500/20">
              {incidentHistory.length} Total Events
            </span>
          </div>
          <h2 className="text-xl font-bold text-[#F1F5F9]">
            Incident History & Operations Timeline
          </h2>
          <p className="text-xs text-[#94A3B8] mt-0.5">
            Demonstrates forward (oldest &rarr; newest) and backward (newest &rarr; oldest) traversal over custom Doubly Linked List nodes.
          </p>
        </div>

        <div className="flex items-center gap-3">
          <button
            onClick={() => setIsReverseOrder(!isReverseOrder)}
            className="flex items-center gap-2 px-3.5 py-2 rounded-lg bg-[#0B1120] border border-[#26344A] hover:border-slate-500 text-xs font-semibold text-slate-200 transition-all"
          >
            <ArrowDownUp className="w-4 h-4 text-blue-400" />
            <span>{isReverseOrder ? 'Direction: Oldest → Newest' : 'Direction: Newest → Oldest'}</span>
          </button>

          <button
            onClick={onUndoClick}
            disabled={!canUndo}
            className={`flex items-center gap-2 px-4 py-2 rounded-lg text-xs font-bold border transition-all ${
              canUndo
                ? 'bg-amber-500/20 text-amber-300 border-amber-500/40 hover:bg-amber-500/30 shadow-lg shadow-amber-900/20'
                : 'bg-[#0B1120] text-slate-600 border-[#26344A] cursor-not-allowed'
            }`}
          >
            <RotateCcw className="w-4 h-4" />
            <span>Undo Last Operation</span>
          </button>
        </div>
      </div>

      {/* Search & Filter Bar */}
      <div className="bg-[#151F32] border border-[#26344A] rounded-xl p-4 shadow-panel flex items-center justify-between gap-4">
        <div className="relative flex-1 max-w-md">
          <Search className="w-4 h-4 absolute left-3 top-1/2 -translate-y-1/2 text-[#94A3B8]" />
          <input
            type="text"
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
            placeholder="Search incident descriptions, fault IDs, or event types..."
            className="w-full pl-9 pr-4 py-1.5 bg-[#0B1120] border border-[#26344A] focus:border-blue-500 rounded-lg text-xs text-[#F1F5F9] placeholder-[#94A3B8] focus:outline-none focus:ring-1 focus:ring-blue-500"
          />
        </div>

        <div className="text-xs text-[#94A3B8]">
          Showing <span className="text-[#F1F5F9] font-bold">{displayEvents.length}</span> matching events
        </div>
      </div>

      {/* Chronological Incident Timeline */}
      <div className="bg-[#151F32] border border-[#26344A] rounded-xl p-6 shadow-panel">
        <div className="relative pl-6 sm:pl-8 space-y-6 before:absolute before:left-3 sm:before:left-4 before:top-2 before:bottom-2 before:w-0.5 before:bg-[#26344A]">
          {displayEvents.map((ev) => {
            const badge = getEventBadge(ev.eventType);
            const Icon = badge.icon;
            return (
              <div key={ev.eventId} className="relative group">
                {/* Node Dot on Timeline */}
                <div
                  className={`absolute -left-6 sm:-left-8 top-1.5 w-3.5 h-3.5 rounded-full border-2 border-[#151F32] ${badge.dot} shadow-md transition-transform group-hover:scale-125`}
                />

                {/* Event Card */}
                <div className="p-4 rounded-xl bg-[#0B1120] border border-[#26344A] group-hover:border-slate-500 transition-all">
                  <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-2 mb-2">
                    <div className="flex items-center gap-2">
                      <span className="text-xs font-mono font-bold text-blue-400">
                        Event #{ev.eventId}
                      </span>
                      <span
                        className={`text-[10px] font-extrabold px-2 py-0.5 rounded border uppercase flex items-center gap-1 ${badge.color}`}
                      >
                        <Icon className="w-3 h-3" />
                        <span>{ev.eventType.replace('_', ' ')}</span>
                      </span>

                      {ev.relatedFaultId !== -1 && (
                        <span className="text-[10px] font-mono font-semibold px-2 py-0.5 rounded bg-slate-800 text-slate-300 border border-slate-700">
                          Related Fault #{ev.relatedFaultId}
                        </span>
                      )}
                    </div>

                    <div className="flex items-center gap-1.5 text-xs text-[#94A3B8] font-mono">
                      <Clock className="w-3.5 h-3.5" />
                      <span>{ev.timestamp}</span>
                    </div>
                  </div>

                  <p className="text-xs text-[#F1F5F9] leading-relaxed">
                    {ev.description}
                  </p>
                </div>
              </div>
            );
          })}

          {displayEvents.length === 0 && (
            <div className="py-12 text-center text-[#94A3B8]">
              <FileText className="w-8 h-8 mx-auto text-slate-600 mb-2" />
              <p className="text-sm font-semibold text-[#F1F5F9]">No events match search</p>
              <p className="text-xs text-[#94A3B8] mt-0.5">Try searching with a different keyword.</p>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};
