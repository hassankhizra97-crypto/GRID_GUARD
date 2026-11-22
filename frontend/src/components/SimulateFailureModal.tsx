import React, { useState, useEffect } from 'react';
import { X, AlertTriangle, Zap, Clock, ShieldAlert } from 'lucide-react';
import { PowerLineEdge, LocationNode } from '../types';

interface SimulateFailureModalProps {
  isOpen: boolean;
  onClose: () => void;
  powerLines: PowerLineEdge[];
  locations: LocationNode[];
  initialLineId?: number;
  onConfirm: (lineId: number, severity: number, durationMinutes: number) => void;
}

export const SimulateFailureModal: React.FC<SimulateFailureModalProps> = ({
  isOpen,
  onClose,
  powerLines,
  locations,
  initialLineId,
  onConfirm,
}) => {
  const operationalLines = powerLines.filter((l) => l.isOperational);
  const [selectedLineId, setSelectedLineId] = useState<number>(() => {
    if (initialLineId && operationalLines.some((l) => l.id === initialLineId)) {
      return initialLineId;
    }
    return operationalLines.length > 0 ? operationalLines[0].id : 1;
  });
  const [severity, setSeverity] = useState<number>(3);
  const [duration, setDuration] = useState<number>(60);

  useEffect(() => {
    if (isOpen) {
      if (initialLineId && operationalLines.some((l) => l.id === initialLineId)) {
        setSelectedLineId(initialLineId);
      } else if (operationalLines.length > 0) {
        if (!operationalLines.some((l) => l.id === selectedLineId)) {
          setSelectedLineId(operationalLines[0].id);
        }
      }
    }
  }, [isOpen, initialLineId, operationalLines, selectedLineId]);

  if (!isOpen) return null;

  const getLocationName = (id: number) => {
    return locations.find((l) => l.id === id)?.name || `Location ${id}`;
  };

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (selectedLineId) {
      onConfirm(selectedLineId, severity, duration);
      onClose();
    }
  };

  return (
    <div className="fixed inset-0 z-50 bg-black/75 backdrop-blur-sm flex items-center justify-center p-4">
      <div className="bg-[#151F32] border border-[#26344A] rounded-2xl w-full max-w-lg shadow-2xl overflow-hidden animate-in fade-in zoom-in-95 duration-200">
        {/* Modal Header */}
        <div className="px-6 py-4 bg-[#101A2B] border-b border-[#26344A] flex items-center justify-between">
          <div className="flex items-center gap-3">
            <div className="p-2 rounded-lg bg-red-500/10 border border-red-500/30 text-red-400">
              <AlertTriangle className="w-5 h-5" />
            </div>
            <div>
              <h3 className="text-base font-bold text-[#F1F5F9]">
                Simulate Power Line Failure
              </h3>
              <p className="text-xs text-[#94A3B8]">
                Inject simulated physical line damage into the C++ Grid Model
              </p>
            </div>
          </div>
          <button
            onClick={onClose}
            className="p-1.5 rounded-lg text-[#94A3B8] hover:text-[#F1F5F9] hover:bg-[#26344A]/50 transition-all"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        {/* Modal Body */}
        <form onSubmit={handleSubmit} className="p-6 space-y-5">
          {/* Power Line Selection */}
          <div>
            <label className="block text-xs font-semibold text-[#94A3B8] uppercase tracking-wider mb-2">
              Select Target Transmission Line
            </label>
            <select
              value={selectedLineId}
              onChange={(e) => setSelectedLineId(Number(e.target.value))}
              className="w-full px-3.5 py-2.5 bg-[#0B1120] border border-[#26344A] focus:border-blue-500 rounded-lg text-sm text-[#F1F5F9] focus:outline-none focus:ring-1 focus:ring-blue-500 transition-all"
            >
              {operationalLines.map((line) => (
                <option key={line.id} value={line.id}>
                  Line #{line.id}: {getLocationName(line.fromLocationId)} &harr;{' '}
                  {getLocationName(line.toLocationId)}
                </option>
              ))}
            </select>
            {operationalLines.length === 0 && (
              <p className="text-xs text-red-400 mt-1">
                All lines currently failed! Restore a line first.
              </p>
            )}
          </div>

          {/* Severity Slider */}
          <div>
            <div className="flex items-center justify-between mb-2">
              <label className="text-xs font-semibold text-[#94A3B8] uppercase tracking-wider">
                Fault Severity Level
              </label>
              <span className="text-xs font-bold text-amber-400 px-2 py-0.5 rounded bg-amber-500/10 border border-amber-500/20">
                Level {severity} of 5
              </span>
            </div>
            <input
              type="range"
              min={1}
              max={5}
              value={severity}
              onChange={(e) => setSeverity(Number(e.target.value))}
              className="w-full h-2 bg-[#0B1120] rounded-lg appearance-none cursor-pointer accent-blue-500"
            />
            <div className="flex justify-between text-[10px] text-[#94A3B8] mt-1.5 font-medium">
              <span>1 - Minor Glitch</span>
              <span>3 - Moderate Sag</span>
              <span>5 - Catastrophic Sever</span>
            </div>
          </div>

          {/* Outage Duration */}
          <div>
            <div className="flex items-center justify-between mb-2">
              <label className="text-xs font-semibold text-[#94A3B8] uppercase tracking-wider">
                Estimated Outage Duration
              </label>
              <span className="text-xs font-bold text-blue-400 px-2 py-0.5 rounded bg-blue-500/10 border border-blue-500/20">
                {duration} Minutes
              </span>
            </div>
            <input
              type="range"
              min={15}
              max={360}
              step={15}
              value={duration}
              onChange={(e) => setDuration(Number(e.target.value))}
              className="w-full h-2 bg-[#0B1120] rounded-lg appearance-none cursor-pointer accent-blue-500"
            />
            <div className="flex justify-between text-[10px] text-[#94A3B8] mt-1.5 font-medium">
              <span>15 mins</span>
              <span>60 mins</span>
              <span>360 mins (6h)</span>
            </div>
          </div>

          {/* Academic Scoring Formula Preview Box */}
          <div className="p-3.5 rounded-lg bg-[#0B1120] border border-[#26344A] text-xs">
            <div className="flex items-center gap-2 text-slate-300 font-semibold mb-1">
              <ShieldAlert className="w-4 h-4 text-blue-400" />
              <span>Priority Score Calculation Preview</span>
            </div>
            <p className="text-[11px] text-[#94A3B8] leading-relaxed">
              Formula: (Severity &times; 10) + (CritFac &times; 20) + (Pop / 100) + (Duration &times; 2)
            </p>
          </div>

          {/* Footer Actions */}
          <div className="pt-2 flex items-center justify-end gap-3 border-t border-[#26344A]/60">
            <button
              type="button"
              onClick={onClose}
              className="px-4 py-2 rounded-lg text-xs font-semibold text-[#94A3B8] hover:text-[#F1F5F9] hover:bg-[#26344A]/40 transition-all"
            >
              Cancel
            </button>
            <button
              type="submit"
              disabled={operationalLines.length === 0}
              className="flex items-center gap-2 px-5 py-2 rounded-lg text-xs font-bold bg-red-600 hover:bg-red-500 text-white shadow-lg shadow-red-900/30 transition-all"
            >
              <AlertTriangle className="w-4 h-4" />
              <span>Trigger Fault</span>
            </button>
          </div>
        </form>
      </div>
    </div>
  );
};
