import React, { useState } from 'react';
import {
  Settings,
  Server,
  RotateCcw,
  Download,
  ShieldCheck,
  CheckCircle,
  AlertTriangle,
  Zap,
  Code,
  BookOpen,
} from 'lucide-react';
import { api } from '../services/api';
import { SimulationStats, LocationNode, PowerLineEdge, Fault, IncidentEvent } from '../types';

interface SettingsPageProps {
  stats: SimulationStats;
  locations: LocationNode[];
  powerLines: PowerLineEdge[];
  faults: Fault[];
  incidentHistory: IncidentEvent[];
  onResetSimulation: () => void;
}

export const SettingsPage: React.FC<SettingsPageProps> = ({
  stats,
  locations,
  powerLines,
  faults,
  incidentHistory,
  onResetSimulation,
}) => {
  const [apiUrl, setApiUrl] = useState('http://localhost:8080/api');
  const [testResult, setTestResult] = useState<string | null>(null);
  const [isTesting, setIsTesting] = useState(false);

  const handleTestConnection = async () => {
    setIsTesting(true);
    setTestResult(null);
    api.setBackendUrl(apiUrl);
    const connected = await api.checkBackendHealth();
    setIsTesting(false);
    if (connected) {
      setTestResult('CONNECTED: Successfully pinged live C++ HTTP backend server.');
    } else {
      setTestResult(
        'LOCAL MODE: C++ live server not running at endpoint. Synchronized local simulation engine active with 100% C++ algorithmic fidelity.'
      );
    }
  };

  const handleExportData = () => {
    const data = {
      timestamp: new Date().toISOString(),
      stats,
      locations,
      powerLines,
      faults,
      incidentHistory,
    };
    const blob = new Blob([JSON.stringify(data, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `gridguard-audit-${Date.now()}.json`;
    a.click();
    URL.revokeObjectURL(url);
  };

  return (
    <div className="space-y-6 max-w-5xl mx-auto animate-in fade-in duration-300">
      {/* Settings Header */}
      <div className="bg-[#151F32] border border-[#26344A] rounded-2xl p-6 shadow-panel">
        <div className="flex items-center gap-3">
          <div className="p-2.5 rounded-xl bg-blue-600/20 border border-blue-500/40 text-blue-400">
            <Settings className="w-5 h-5" />
          </div>
          <div>
            <h2 className="text-xl font-bold text-[#F1F5F9]">
              System Parameters & Engine Configuration
            </h2>
            <p className="text-xs text-[#94A3B8] mt-0.5">
              Manage live C++ backend endpoint, verify algorithmic synchronization, and reset grid states.
            </p>
          </div>
        </div>
      </div>

      {/* Backend Integration Settings */}
      <div className="bg-[#151F32] border border-[#26344A] rounded-xl p-6 shadow-panel space-y-4">
        <div className="flex items-center justify-between border-b border-[#26344A] pb-3">
          <div className="flex items-center gap-2.5">
            <Server className="w-4 h-4 text-blue-400" />
            <h3 className="text-sm font-bold text-[#F1F5F9]">C++ Backend Service Integration</h3>
          </div>
          <span className="text-[10px] font-bold px-2 py-0.5 rounded bg-emerald-500/10 text-emerald-400 border border-emerald-500/30">
            Engine Synchronized
          </span>
        </div>

        <p className="text-xs text-[#94A3B8] leading-relaxed">
          GridGuard connects to an optional live C++ REST/HTTP server via standard JSON endpoints.
          When running offline or in standalone mode, the centralized API service maintains exact
          parity with the C++ data structures (Adjacency List Graph, Queue BFS, Max-Heap triage,
          Dijkstra roads, Doubly Linked List logs, and Undo Stack).
        </p>

        <div className="flex flex-col sm:flex-row items-center gap-3 pt-2">
          <div className="relative flex-1 w-full">
            <input
              type="text"
              value={apiUrl}
              onChange={(e) => setApiUrl(e.target.value)}
              placeholder="http://localhost:8080/api"
              className="w-full px-3.5 py-2 bg-[#0B1120] border border-[#26344A] focus:border-blue-500 rounded-lg text-xs font-mono text-[#F1F5F9] focus:outline-none focus:ring-1 focus:ring-blue-500"
            />
          </div>
          <button
            onClick={handleTestConnection}
            disabled={isTesting}
            className="w-full sm:w-auto px-4 py-2 bg-blue-600 hover:bg-blue-500 text-white rounded-lg text-xs font-bold transition-all shadow-glow-blue whitespace-nowrap"
          >
            {isTesting ? 'Pinging...' : 'Test Connection'}
          </button>
        </div>

        {testResult && (
          <div
            className={`p-3 rounded-lg text-xs font-medium border ${
              testResult.includes('CONNECTED')
                ? 'bg-emerald-500/10 text-emerald-300 border-emerald-500/30'
                : 'bg-blue-500/10 text-blue-300 border-blue-500/30'
            }`}
          >
            {testResult}
          </div>
        )}
      </div>

      {/* Simulation Reset & Maintenance */}
      <div className="bg-[#151F32] border border-[#26344A] rounded-xl p-6 shadow-panel space-y-4">
        <div className="flex items-center gap-2.5 border-b border-[#26344A] pb-3">
          <RotateCcw className="w-4 h-4 text-amber-400" />
          <h3 className="text-sm font-bold text-[#F1F5F9]">Simulation State Reset</h3>
        </div>

        <p className="text-xs text-[#94A3B8]">
          Restore all 14 locations and 18 power lines to full baseline operational status.
          Clears active work orders and resets the incident timeline.
        </p>

        <button
          onClick={onResetSimulation}
          className="flex items-center gap-2 px-4 py-2 bg-amber-600/20 hover:bg-amber-600/30 text-amber-300 border border-amber-500/40 rounded-lg text-xs font-bold transition-all"
        >
          <RotateCcw className="w-4 h-4" />
          <span>Reset Grid to Baseline State</span>
        </button>
      </div>

      {/* Export Audit Log */}
      <div className="bg-[#151F32] border border-[#26344A] rounded-xl p-6 shadow-panel space-y-4">
        <div className="flex items-center gap-2.5 border-b border-[#26344A] pb-3">
          <Download className="w-4 h-4 text-emerald-400" />
          <h3 className="text-sm font-bold text-[#F1F5F9]">Data Export & Regulatory Audit</h3>
        </div>

        <p className="text-xs text-[#94A3B8]">
          Download a complete snapshot of all location statuses, transmission line impedances,
          active triage records, and chronological incident logs.
        </p>

        <button
          onClick={handleExportData}
          className="flex items-center gap-2 px-4 py-2 bg-emerald-600 hover:bg-emerald-500 text-white rounded-lg text-xs font-bold shadow-lg shadow-emerald-900/30 transition-all"
        >
          <Download className="w-4 h-4" />
          <span>Export Audit Snapshot (JSON)</span>
        </button>
      </div>

      {/* Academic DSA Engine Specs Summary */}
      <div className="bg-[#101A2B] border border-[#26344A] rounded-xl p-6 text-xs text-[#94A3B8] space-y-2">
        <div className="flex items-center gap-2 text-slate-200 font-bold mb-2">
          <Code className="w-4 h-4 text-blue-400" />
          <span>Underlying C++ Data Structures & Algorithms Mapping</span>
        </div>
        <div className="grid grid-cols-1 sm:grid-cols-2 gap-2 text-[11px]">
          <div>&bull; Graph Topology: Custom Adjacency List (`AdjEdgeNode*`)</div>
          <div>&bull; Reachability Analysis: Custom Queue FIFO BFS</div>
          <div>&bull; Repair Triage Queue: Custom Binary Max-Heap</div>
          <div>&bull; Transit Route Optimization: Dijkstra Shortest Path</div>
          <div>&bull; Incident Audit Log: Doubly Linked List (`prev`/`next`)</div>
          <div>&bull; Reversible State Machine: Custom LIFO Stack</div>
        </div>
      </div>
    </div>
  );
};
