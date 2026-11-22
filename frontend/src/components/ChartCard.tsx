import React, { ReactNode } from 'react';

interface ChartCardProps {
  title: string;
  subtitle?: string;
  action?: ReactNode;
  children: ReactNode;
}

export const ChartCard: React.FC<ChartCardProps> = ({
  title,
  subtitle,
  action,
  children,
}) => {
  return (
    <div className="bg-[#151F32] border border-[#26344A] rounded-xl p-5 shadow-panel flex flex-col justify-between">
      <div className="flex items-start justify-between mb-4">
        <div>
          <h3 className="text-sm font-bold text-[#F1F5F9] tracking-tight">{title}</h3>
          {subtitle && <p className="text-xs text-[#94A3B8] mt-0.5">{subtitle}</p>}
        </div>
        {action && <div>{action}</div>}
      </div>

      <div className="w-full flex-1 min-h-[240px] flex items-center justify-center">
        {children}
      </div>
    </div>
  );
};
