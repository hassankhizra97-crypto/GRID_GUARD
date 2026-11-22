import React, { useState, useEffect, useCallback } from 'react';
import { Sidebar } from './components/Sidebar';
import { Header } from './components/Header';
import { SimulateFailureModal } from './components/SimulateFailureModal';
import { OverviewPage } from './pages/OverviewPage';
import { PowerNetworkPage } from './pages/PowerNetworkPage';
import { FaultManagementPage } from './pages/FaultManagementPage';
import { RepairSchedulingPage } from './pages/RepairSchedulingPage';
import { IncidentHistoryPage } from './pages/IncidentHistoryPage';
import { SettingsPage } from './pages/SettingsPage';
import { KarachiMap } from './components/KarachiMap';
import { api } from './services/api';
import {
  ActivePage,
  LocationNode,
  PowerLineEdge,
  Fault,
  IncidentEvent,
  SimulationStats,
  DijkstraRoute,
} from './types';
import { CheckCircle2, AlertTriangle, Info, X } from 'lucide-react';

export const App: React.FC = () => {
  const [activePage, setActivePage] = useState<ActivePage>('overview');
  const [locations, setLocations] = useState<LocationNode[]>([]);
  const [powerLines, setPowerLines] = useState<PowerLineEdge[]>([]);
  const [faults, setFaults] = useState<Fault[]>([]);
  const [incidentHistory, setIncidentHistory] = useState<IncidentEvent[]>([]);
  const [stats, setStats] = useState<SimulationStats>({
    totalLocations: 44,
    operationalLines: 61,
    failedLines: 0,
    totalLines: 61,
    activeFaults: 0,
    affectedPopulation: 0,
    criticalFacilitiesAffected: 0,
    repairsCompleted: 0,
    networkHealthPercentage: 100,
  });

  const [activeDijkstraRoute, setActiveDijkstraRoute] = useState<DijkstraRoute | null>(null);
  const [isSimulateModalOpen, setIsSimulateModalOpen] = useState<boolean>(false);
  const [searchValue, setSearchValue] = useState<string>('');
  const [toast, setToast] = useState<{ message: string; type: 'success' | 'alert' | 'info' } | null>(null);
  const [isBackendConnected, setIsBackendConnected] = useState<boolean>(false);

  // Show auto-dismissing toast notification
  const showToast = (message: string, type: 'success' | 'alert' | 'info' = 'info') => {
    setToast({ message, type });
    setTimeout(() => {
      setToast((prev) => (prev?.message === message ? null : prev));
    }, 4000);
  };

  // Fetch state from centralized API
  const refreshState = useCallback(async () => {
    const data = await api.getNetworkState();
    setLocations(data.locations);
    setPowerLines(data.powerLines);
    setFaults(data.faults);
    setIncidentHistory(data.incidentHistory);
    setStats(data.stats);

    const backendStatus = api.getBackendStatus();
    setIsBackendConnected(backendStatus.isConnected);
  }, []);

  useEffect(() => {
    refreshState();
    const interval = setInterval(refreshState, 3000);
    return () => clearInterval(interval);
  }, [refreshState]);

  // Handle failure simulation
  const handleSimulateFailure = async (lineId: number, severity: number, durationMinutes: number) => {
    const res = await api.simulatePowerLineFailure(lineId, severity, durationMinutes);
    if (res.success) {
      showToast(res.message, 'alert');
      await refreshState();
    } else {
      showToast(res.message, 'info');
    }
  };

  // Handle repair processing
  const handleProcessRepair = async (specificFaultId?: number) => {
    const res = await api.processRepair(specificFaultId);
    if (res.success) {
      showToast(res.message, 'success');
      await refreshState();
    } else {
      showToast(res.message, 'info');
    }
  };

  // Handle undo operation
  const handleUndo = async () => {
    const res = await api.undoLastOperation();
    if (res.success) {
      showToast(res.message, 'info');
      await refreshState();
    } else {
      showToast(res.message, 'alert');
    }
  };

  // Handle line restoration
  const handleRestoreLine = async (lineId: number) => {
    const res = await api.restorePowerLineManually(lineId);
    if (res.success) {
      showToast(res.message, 'success');
      await refreshState();
    } else {
      showToast(res.message, 'alert');
    }
  };

  // Handle reset
  const handleResetSimulation = async () => {
    api.resetSimulation();
    showToast('Simulation reset to baseline state.', 'info');
    await refreshState();
  };

  const handleInspectRoute = (destId: number) => {
    const route = api.getDijkstraRoute(destId);
    setActiveDijkstraRoute(route);
    setActivePage('map');
  };

  const activeFaultsCount = faults.filter((f) => f.status !== 'REPAIRED').length;
  const canUndo = incidentHistory.length > 1;

  return (
    <div className="min-h-screen bg-[#0B1120] text-[#F1F5F9] flex overflow-x-hidden selection:bg-blue-600 selection:text-white">
      {/* Fixed Left Sidebar */}
      <Sidebar
        activePage={activePage}
        onNavigate={setActivePage}
        activeFaultsCount={activeFaultsCount}
        isBackendConnected={isBackendConnected}
      />

      {/* Main Content Area */}
      <div className="flex-1 ml-64 flex flex-col min-h-screen bg-[#0B1120]">
        {/* Top Header */}
        <Header
          activePage={activePage}
          onSimulateClick={() => setIsSimulateModalOpen(true)}
          onUndoClick={handleUndo}
          canUndo={canUndo}
          searchValue={searchValue}
          onSearchChange={setSearchValue}
          activeFaultsCount={activeFaultsCount}
        />

        {/* Page Content View */}
        <main className="flex-1 p-8">
          {activePage === 'overview' && (
            <OverviewPage
              stats={stats}
              locations={locations}
              powerLines={powerLines}
              faults={faults}
              incidentHistory={incidentHistory}
              onNavigate={setActivePage}
              onSimulateClick={() => setIsSimulateModalOpen(true)}
            />
          )}

          {activePage === 'map' && (
            <KarachiMap
              locations={locations}
              powerLines={powerLines}
              faults={faults}
              activeDijkstraRoute={activeDijkstraRoute}
              onSimulateLineFailure={(id) => handleSimulateFailure(id, 3, 60)}
              onRestoreLine={handleRestoreLine}
              onInspectRoute={handleInspectRoute}
            />
          )}

          {activePage === 'network' && (
            <PowerNetworkPage
              locations={locations}
              powerLines={powerLines}
              onSimulateLineFailure={(id) => handleSimulateFailure(id, 3, 60)}
              onRestoreLine={handleRestoreLine}
              onInspectRoute={handleInspectRoute}
            />
          )}

          {activePage === 'faults' && (
            <FaultManagementPage
              faults={faults}
              powerLines={powerLines}
              locations={locations}
              onSimulateClick={() => setIsSimulateModalOpen(true)}
              onProcessRepair={handleProcessRepair}
              onInspectRoute={handleInspectRoute}
            />
          )}

          {activePage === 'repairs' && (
            <RepairSchedulingPage
              faults={faults}
              powerLines={powerLines}
              locations={locations}
              onProcessNextRepair={handleProcessRepair}
              getDijkstraRoute={(id) => api.getDijkstraRoute(id)}
            />
          )}

          {activePage === 'history' && (
            <IncidentHistoryPage
              incidentHistory={incidentHistory}
              onUndoClick={handleUndo}
              canUndo={canUndo}
            />
          )}

          {activePage === 'settings' && (
            <SettingsPage
              stats={stats}
              locations={locations}
              powerLines={powerLines}
              faults={faults}
              incidentHistory={incidentHistory}
              onResetSimulation={handleResetSimulation}
            />
          )}
        </main>
      </div>

      {/* Simulate Line Failure Modal */}
      <SimulateFailureModal
        isOpen={isSimulateModalOpen}
        onClose={() => setIsSimulateModalOpen(false)}
        powerLines={powerLines}
        locations={locations}
        onConfirm={handleSimulateFailure}
      />

      {/* Floating Toast Notification */}
      {toast && (
        <div
          className={`fixed bottom-6 right-6 z-50 px-4 py-3 rounded-xl border shadow-2xl flex items-center gap-3 animate-in fade-in slide-in-from-bottom-5 duration-200 ${
            toast.type === 'alert'
              ? 'bg-red-950/90 border-red-500/50 text-red-200'
              : toast.type === 'success'
              ? 'bg-emerald-950/90 border-emerald-500/50 text-emerald-200'
              : 'bg-[#151F32] border-[#26344A] text-slate-200'
          }`}
        >
          {toast.type === 'alert' ? (
            <AlertTriangle className="w-5 h-5 text-red-400" />
          ) : toast.type === 'success' ? (
            <CheckCircle2 className="w-5 h-5 text-emerald-400" />
          ) : (
            <Info className="w-5 h-5 text-blue-400" />
          )}
          <span className="text-xs font-semibold">{toast.message}</span>
          <button
            onClick={() => setToast(null)}
            className="p-1 rounded text-slate-400 hover:text-white transition-all ml-2"
          >
            <X className="w-4 h-4" />
          </button>
        </div>
      )}
    </div>
  );
};

export default App;
