import React from 'react';
import {
  Activity,
  MapPin,
  Zap,
  AlertTriangle,
  Wrench,
  Clock,
  Settings,
  ShieldCheck,
  Server,
} from 'lucide-react';
import { ActivePage } from '../types';

interface SidebarProps {
  activePage: ActivePage;
  onNavigate: (page: ActivePage) => void;
  activeFaultsCount: number;
  isBackendConnected: boolean;
}

export const Sidebar: React.FC<SidebarProps> = ({
  activePage,
  onNavigate,
  activeFaultsCount,
  isBackendConnected,
}) => {
  const navItems = [
    { id: 'overview' as ActivePage, label: 'Overview', icon: Activity },
    { id: 'map' as ActivePage, label: 'Karachi Map', icon: MapPin },
    { id: 'network' as ActivePage, label: 'Power Network', icon: Zap },
    {
      id: 'faults' as ActivePage,
      label: 'Fault Management',
      icon: AlertTriangle,
      badge: activeFaultsCount > 0 ? activeFaultsCount : undefined,
    },
    { id: 'repairs' as ActivePage, label: 'Repair Scheduling', icon: Wrench },
    { id: 'history' as ActivePage, label: 'Incident History', icon: Clock },
    { id: 'settings' as ActivePage, label: 'Settings', icon: Settings },
  ];

  return (
    <aside className="w-64 bg-[#101A2B] border-r border-[#26344A] flex flex-col justify-between h-screen fixed left-0 top-0 z-[10002] select-none">
      <div>
        {/* Brand / Logo */}
        <div className="h-16 px-6 flex items-center gap-3 border-b border-[#26344A]">
          <div className="w-9 h-9 rounded-lg bg-emerald-600/20 border border-emerald-500/40 flex items-center justify-center text-emerald-400 shadow-glow-blue">
            <Zap className="w-5 h-5 fill-emerald-400/30 text-emerald-400" />
          </div>
          <div>
            <div className="flex items-center gap-1.5">
              <span className="font-extrabold tracking-wider text-base text-[#F1F5F9]">
                GRIDGUARD
              </span>
              <span className="text-[10px] font-bold px-1.5 py-0.2 bg-emerald-500/20 text-emerald-400 border border-emerald-500/30 rounded">
                KARACHI
              </span>
            </div>
            <p className="text-[10px] text-[#94A3B8] tracking-tight uppercase">
              Karachi Power Grid Simulator
            </p>
          </div>
        </div>

        {/* Navigation Section */}
        <div className="px-3 py-6">
          <p className="px-3 text-[11px] font-semibold text-[#94A3B8]/70 uppercase tracking-wider mb-2">
            Operations Console
          </p>
          <nav className="space-y-1">
            {navItems.map((item) => {
              const Icon = item.icon;
              const isActive = activePage === item.id;
              return (
                <button
                  key={item.id}
                  onClick={() => onNavigate(item.id)}
                  className={`w-full flex items-center justify-between px-3.5 py-2.5 rounded-lg text-sm font-medium transition-all duration-200 group ${
                    isActive
                      ? 'bg-blue-600/15 text-blue-400 border border-blue-500/30 shadow-sm'
                      : 'text-[#94A3B8] hover:text-[#F1F5F9] hover:bg-[#151F32] border border-transparent'
                  }`}
                >
                  <div className="flex items-center gap-3">
                    <Icon
                      className={`w-4 h-4 transition-transform duration-200 group-hover:scale-110 ${
                        isActive ? 'text-blue-400' : 'text-[#94A3B8]'
                      }`}
                    />
                    <span>{item.label}</span>
                  </div>

                  {item.badge !== undefined && (
                    <span
                      className={`text-xs px-2 py-0.5 rounded-full font-bold ${
                        isActive
                          ? 'bg-red-500 text-white'
                          : 'bg-red-500/20 text-red-400 border border-red-500/30 animate-pulse'
                      }`}
                    >
                      {item.badge}
                    </span>
                  )}
                </button>
              );
            })}
          </nav>
        </div>
      </div>

      {/* Footer System Status Panel */}
      <div className="p-4 border-t border-[#26344A] bg-[#0E1726]/60">
        <div className="space-y-2.5">
          {/* Main System Status */}
          <div className="flex items-center justify-between text-xs">
            <div className="flex items-center gap-2">
              <span className="relative flex h-2.5 w-2.5">
                <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-emerald-400 opacity-75" />
                <span className="relative inline-flex rounded-full h-2.5 w-2.5 bg-emerald-500" />
              </span>
              <span className="text-[#94A3B8] font-medium">System Status</span>
            </div>
            <span className="font-semibold text-emerald-400 uppercase text-[11px] tracking-wide">
              Online
            </span>
          </div>

          {/* C++ DSA Backend Status */}
          <div className="flex items-center justify-between text-xs pt-2 border-t border-[#26344A]/50">
            <div className="flex items-center gap-1.5 text-[#94A3B8]">
              <Server className="w-3.5 h-3.5" />
              <span className="text-[11px]">C++ DSA Engine</span>
            </div>
            <div className="flex items-center gap-1 text-[11px]">
              <span
                className={`w-1.5 h-1.5 rounded-full ${
                  isBackendConnected ? 'bg-blue-400' : 'bg-emerald-400'
                }`}
              />
              <span className="text-slate-300 font-medium">
                {isBackendConnected ? 'C++ Live' : 'Synced'}
              </span>
            </div>
          </div>
        </div>
      </div>
    </aside>
  );
};
