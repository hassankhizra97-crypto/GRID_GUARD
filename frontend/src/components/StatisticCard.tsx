import React from 'react';
import { LucideIcon } from 'lucide-react';

interface StatisticCardProps {
  title: string;
  value: string | number;
  subtitle?: string;
  icon: LucideIcon;
  colorScheme?: 'blue' | 'green' | 'red' | 'amber' | 'purple' | 'cyan';
  badgeText?: string;
  badgeType?: 'up' | 'down' | 'neutral' | 'alert';
}

export const StatisticCard: React.FC<StatisticCardProps> = ({
  title,
  value,
  subtitle,
  icon: Icon,
  colorScheme = 'blue',
  badgeText,
  badgeType = 'neutral',
}) => {
  const getIconStyles = () => {
    switch (colorScheme) {
      case 'green':
        return 'bg-emerald-500/10 text-emerald-400 border-emerald-500/20 group-hover:border-emerald-500/40 group-hover:bg-emerald-500/20';
      case 'red':
        return 'bg-red-500/10 text-red-400 border-red-500/20 group-hover:border-red-500/40 group-hover:bg-red-500/20';
      case 'amber':
        return 'bg-amber-500/10 text-amber-400 border-amber-500/20 group-hover:border-amber-500/40 group-hover:bg-amber-500/20';
      case 'purple':
        return 'bg-purple-500/10 text-purple-400 border-purple-500/20 group-hover:border-purple-500/40 group-hover:bg-purple-500/20';
      case 'cyan':
        return 'bg-cyan-500/10 text-cyan-400 border-cyan-500/20 group-hover:border-cyan-500/40 group-hover:bg-cyan-500/20';
      default:
        return 'bg-blue-500/10 text-blue-400 border-blue-500/20 group-hover:border-blue-500/40 group-hover:bg-blue-500/20';
    }
  };

  const getBadgeStyles = () => {
    switch (badgeType) {
      case 'up':
        return 'text-emerald-400 bg-emerald-500/10 border-emerald-500/20';
      case 'down':
        return 'text-slate-400 bg-slate-500/10 border-slate-500/20';
      case 'alert':
        return 'text-red-400 bg-red-500/10 border-red-500/20 animate-pulse';
      default:
        return 'text-slate-400 bg-slate-800/60 border-slate-700/50';
    }
  };

  return (
    <div className="group relative bg-[#151F32] border border-[#26344A] rounded-xl p-5 hover:border-slate-600 transition-all duration-300 shadow-panel hover:shadow-2xl">
      <div className="flex items-start justify-between">
        <div>
          <p className="text-xs font-medium uppercase tracking-wider text-[#94A3B8]">
            {title}
          </p>
          <div className="mt-2 flex items-baseline gap-2">
            <span className="text-2xl sm:text-3xl font-bold tracking-tight text-[#F1F5F9]">
              {value}
            </span>
          </div>
        </div>

        <div
          className={`p-3 rounded-lg border transition-all duration-300 ${getIconStyles()}`}
        >
          <Icon className="w-5 h-5" />
        </div>
      </div>

      <div className="mt-4 flex items-center justify-between pt-3 border-t border-[#26344A]/60">
        <span className="text-xs text-[#94A3B8] truncate">{subtitle}</span>
        {badgeText && (
          <span
            className={`text-[11px] font-medium px-2 py-0.5 rounded-md border ${getBadgeStyles()}`}
          >
            {badgeText}
          </span>
        )}
      </div>
    </div>
  );
};
