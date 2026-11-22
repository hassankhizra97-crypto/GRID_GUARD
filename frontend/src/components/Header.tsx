import React from 'react';
import {
  Bell,
  Search,
  RotateCcw,
  AlertTriangle,
  User,
  Shield,
  Activity,
} from 'lucide-react';
import { ActivePage } from '../types';

interface HeaderProps {
  activePage: ActivePage;
  onSimulateClick: () => void;
  onUndoClick: () => void;
  canUndo: boolean;
  onSearchChange?: (val: string) => void;
  searchValue?: string;
  activeFaultsCount: number;
}

export const Header: React.FC<HeaderProps> = ({
  activePage,
  onSimulateClick,
  onUndoClick,
  canUndo,
  onSearchChange,
  searchValue = '',
  activeFaultsCount,
}) => {
  const getPageInfo = (): { title: string; breadcrumb: string } => {
    switch (activePage) {
      case 'overview':
        return { title: 'Karachi Grid Operations Overview', breadcrumb: 'Dashboard / Overview' };
      case 'map':
        return { title: 'Karachi Geographical Power Grid Map', breadcrumb: 'GIS / Karachi Interactive Map' };
      case 'network':
        return { title: 'Interactive Power Transmission Network', breadcrumb: 'Topology / Graph Analysis' };
      case 'faults':
        return { title: 'Fault Triage & Diagnostic Ledger', breadcrumb: 'Operations / Fault Management' };
      case 'repairs':
        return { title: 'Emergency Repair Scheduling (Max-Heap)', breadcrumb: 'Logistics / Work Orders' };
      case 'history':
        return { title: 'Incident Audit Log & History (DLL)', breadcrumb: 'Compliance / Timeline' };
      case 'settings':
        return { title: 'System & Engine Configuration', breadcrumb: 'System / Parameters' };
      default:
        return { title: 'Grid Operations', breadcrumb: 'Console' };
    }
  };

  const { title, breadcrumb } = getPageInfo();

  return (
    <header className="h-16 bg-[#101A2B]/90 backdrop-blur-md border-b border-[#26344A] sticky top-0 z-[10001] px-8 flex items-center justify-between">
      {/* Title & Breadcrumbs */}
      <div>
        <div className="text-[11px] font-medium text-[#94A3B8] tracking-wider uppercase flex items-center gap-1.5">
          <span>GridGuard</span>
          <span>/</span>
          <span className="text-blue-400 font-semibold">{breadcrumb}</span>
        </div>
        <h1 className="text-lg font-bold text-[#F1F5F9] tracking-tight">{title}</h1>
      </div>

      {/* Center Search & Actions */}
      <div className="flex items-center gap-4">
        {/* Global Search Bar */}
        <div className="relative w-64 md:w-80">
          <Search className="w-4 h-4 absolute left-3 top-1/2 -translate-y-1/2 text-[#94A3B8]" />
          <input
            type="text"
            value={searchValue}
            onChange={(e) => onSearchChange && onSearchChange(e.target.value)}
            placeholder="Search facility, line #, or fault..."
            className="w-full pl-9 pr-4 py-1.5 bg-[#0B1120] border border-[#26344A] focus:border-blue-500 rounded-lg text-xs text-[#F1F5F9] placeholder-[#94A3B8]/60 focus:outline-none focus:ring-1 focus:ring-blue-500 transition-all"
          />
        </div>

        {/* Quick Undo Action */}
        <button
          onClick={onUndoClick}
          disabled={!canUndo}
          title="Undo last reversible operation (C++ Stack)"
          className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-medium border transition-all duration-200 ${
            canUndo
              ? 'bg-[#151F32] text-amber-400 border-amber-500/30 hover:bg-amber-500/10 hover:border-amber-500/50'
              : 'bg-[#151F32]/50 text-slate-500 border-[#26344A] cursor-not-allowed'
          }`}
        >
          <RotateCcw className="w-3.5 h-3.5" />
          <span>Undo</span>
        </button>

        {/* Simulate Failure Button */}
        <button
          onClick={onSimulateClick}
          className="flex items-center gap-1.5 px-3.5 py-1.5 bg-red-600 hover:bg-red-500 text-white rounded-lg text-xs font-semibold shadow-md shadow-red-900/30 border border-red-500/40 transition-all duration-200"
        >
          <AlertTriangle className="w-3.5 h-3.5" />
          <span>Simulate Failure</span>
        </button>

        {/* Notification Bell */}
        <div className="relative">
          <button
            className="p-2 rounded-lg bg-[#151F32] border border-[#26344A] text-[#94A3B8] hover:text-[#F1F5F9] transition-all"
            title="Notifications"
          >
            <Bell className="w-4 h-4" />
          </button>
          {activeFaultsCount > 0 && (
            <span className="absolute -top-1 -right-1 w-4 h-4 rounded-full bg-red-500 text-[10px] font-bold text-white flex items-center justify-center animate-pulse">
              {activeFaultsCount}
            </span>
          )}
        </div>

        {/* Operator Profile Section */}
        <div className="flex items-center gap-3 pl-4 border-l border-[#26344A]">
          <div className="w-8 h-8 rounded-full bg-blue-600/20 border border-blue-500/40 flex items-center justify-center text-blue-400 font-semibold text-xs">
            OP
          </div>
          <div className="hidden lg:block text-left">
            <p className="text-xs font-semibold text-[#F1F5F9] leading-tight">Dispatch Ops</p>
            <p className="text-[10px] text-emerald-400 font-medium leading-tight">Chief Controller</p>
          </div>
        </div>
      </div>
    </header>
  );
};
