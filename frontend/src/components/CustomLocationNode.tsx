import React, { memo } from 'react';
import { Handle, Position, NodeProps } from '@xyflow/react';
import {
  Zap,
  Radio,
  Building2,
  Home,
  GraduationCap,
  Activity,
  AlertOctagon,
} from 'lucide-react';
import { LocationNode } from '../types';

export const CustomLocationNode = memo(({ data, selected }: NodeProps) => {
  const loc = data as unknown as LocationNode;

  const getIcon = () => {
    switch (loc.type) {
      case 'POWER_STATION':
        return <Zap className="w-5 h-5 text-amber-400" />;
      case 'SUBSTATION':
        return <Radio className="w-5 h-5 text-cyan-400" />;
      case 'HOSPITAL':
        return <Activity className="w-5 h-5 text-red-400" />;
      case 'UNIVERSITY':
        return <GraduationCap className="w-5 h-5 text-purple-400" />;
      case 'RESIDENTIAL':
        return <Home className="w-5 h-5 text-emerald-400" />;
      case 'COMMERCIAL':
        return <Building2 className="w-5 h-5 text-blue-400" />;
      default:
        return <Building2 className="w-5 h-5 text-slate-400" />;
    }
  };

  const getBorderColor = () => {
    if (selected) return 'border-blue-400 ring-2 ring-blue-500/40 shadow-glow-blue';
    if (!loc.isPowered) return 'border-red-500/80 ring-2 ring-red-500/30 animate-pulse';
    if (loc.type === 'POWER_STATION') return 'border-amber-500/40';
    if (loc.type === 'HOSPITAL') return 'border-red-400/30';
    return 'border-[#26344A]';
  };

  const getCriticalityBadge = () => {
    switch (loc.criticality) {
      case 'CRITICAL':
        return 'text-red-400 bg-red-500/10 border-red-500/30';
      case 'HIGH':
        return 'text-amber-400 bg-amber-500/10 border-amber-500/30';
      case 'MEDIUM':
        return 'text-blue-400 bg-blue-500/10 border-blue-500/30';
      default:
        return 'text-slate-400 bg-slate-800/40 border-slate-700/40';
    }
  };

  return (
    <div
      className={`relative min-w-[210px] bg-[#151F32] border rounded-xl p-3.5 shadow-panel transition-all duration-200 select-none ${getBorderColor()}`}
    >
      {/* Handles for connections */}
      <Handle
        type="target"
        position={Position.Left}
        className="!w-2.5 !h-2.5 !bg-[#26344A] !border-2 !border-slate-400"
      />
      <Handle
        type="source"
        position={Position.Right}
        className="!w-2.5 !h-2.5 !bg-[#26344A] !border-2 !border-slate-400"
      />
      <Handle
        type="target"
        position={Position.Top}
        className="!w-2.5 !h-2.5 !bg-[#26344A] !border-2 !border-slate-400"
      />
      <Handle
        type="source"
        position={Position.Bottom}
        className="!w-2.5 !h-2.5 !bg-[#26344A] !border-2 !border-slate-400"
      />

      {/* Node Header */}
      <div className="flex items-start justify-between gap-2">
        <div className="flex items-center gap-2">
          <div className="p-1.5 rounded-lg bg-[#0B1120] border border-[#26344A]">
            {getIcon()}
          </div>
          <div>
            <span className="text-[10px] font-bold text-[#94A3B8] tracking-wider uppercase">
              NODE #{loc.id}
            </span>
            <h4 className="text-xs font-bold text-[#F1F5F9] max-w-[130px] truncate leading-tight">
              {loc.name}
            </h4>
          </div>
        </div>

        {/* Live Power Indicator */}
        <span className="relative flex h-2 w-2 mt-1">
          {loc.isPowered ? (
            <>
              <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-emerald-400 opacity-75" />
              <span className="relative inline-flex rounded-full h-2 w-2 bg-emerald-500" />
            </>
          ) : (
            <>
              <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-red-400 opacity-75" />
              <span className="relative inline-flex rounded-full h-2 w-2 bg-red-500" />
            </>
          )}
        </span>
      </div>

      {/* Node Details Footer */}
      <div className="mt-3 pt-2.5 border-t border-[#26344A]/60 flex items-center justify-between text-[10px]">
        <span
          className={`font-semibold px-1.5 py-0.5 rounded border uppercase ${getCriticalityBadge()}`}
        >
          {loc.criticality}
        </span>

        <span className="text-[#94A3B8] font-medium">
          {loc.population > 0 ? `${loc.population.toLocaleString()} pop` : 'Infrastructure'}
        </span>

        <span
          className={`font-bold ${
            loc.isPowered ? 'text-emerald-400' : 'text-red-400 font-extrabold flex items-center gap-1'
          }`}
        >
          {!loc.isPowered && <AlertOctagon className="w-2.5 h-2.5" />}
          {loc.isPowered ? 'ONLINE' : 'OUTAGE'}
        </span>
      </div>
    </div>
  );
});
