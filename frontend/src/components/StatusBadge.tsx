import React from 'react';

interface StatusBadgeProps {
  status: string;
  type?: 'operational' | 'fault' | 'warning' | 'info' | 'critical' | 'neutral';
  pulse?: boolean;
  size?: 'sm' | 'md';
}

export const StatusBadge: React.FC<StatusBadgeProps> = ({
  status,
  type = 'neutral',
  pulse = false,
  size = 'md',
}) => {
  const getColors = () => {
    switch (type) {
      case 'operational':
        return 'bg-emerald-500/10 text-emerald-400 border-emerald-500/30';
      case 'fault':
      case 'critical':
        return 'bg-red-500/10 text-red-400 border-red-500/30';
      case 'warning':
        return 'bg-amber-500/10 text-amber-400 border-amber-500/30';
      case 'info':
        return 'bg-blue-500/10 text-blue-400 border-blue-500/30';
      default:
        return 'bg-slate-500/10 text-slate-300 border-slate-700';
    }
  };

  const getDotColor = () => {
    switch (type) {
      case 'operational':
        return 'bg-emerald-400';
      case 'fault':
      case 'critical':
        return 'bg-red-400';
      case 'warning':
        return 'bg-amber-400';
      case 'info':
        return 'bg-blue-400';
      default:
        return 'bg-slate-400';
    }
  };

  const sizeClass = size === 'sm' ? 'px-2 py-0.5 text-xs' : 'px-2.5 py-1 text-xs';

  return (
    <span
      className={`inline-flex items-center gap-1.5 font-medium rounded-full border ${getColors()} ${sizeClass} tracking-wide uppercase transition-all duration-200`}
    >
      <span className="relative flex h-2 w-2">
        {pulse && (
          <span
            className={`animate-ping absolute inline-flex h-full w-full rounded-full opacity-75 ${getDotColor()}`}
          />
        )}
        <span className={`relative inline-flex rounded-full h-2 w-2 ${getDotColor()}`} />
      </span>
      {status}
    </span>
  );
};
