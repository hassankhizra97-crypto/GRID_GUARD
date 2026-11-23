import React from 'react';
import {
  MapPin,
  Zap,
  AlertTriangle,
  Users,
  Activity,
  CheckCircle2,
  ShieldCheck,
  TrendingUp,
  Radio,
  ExternalLink,
  ChevronRight,
  ShieldAlert,
  ArrowUpRight,
} from 'lucide-react';
import {
  PieChart,
  Pie,
  Cell,
  ResponsiveContainer,
  Tooltip,
  Legend,
  AreaChart,
  Area,
  XAxis,
  YAxis,
  CartesianGrid,
  BarChart,
  Bar,
} from 'recharts';
import {
  LocationNode,
  PowerLineEdge,
  Fault,
  IncidentEvent,
  SimulationStats,
  ActivePage,
} from '../types';
import { StatisticCard } from '../components/StatisticCard';
import { ChartCard } from '../components/ChartCard';
import { StatusBadge } from '../components/StatusBadge';

interface OverviewPageProps {
  stats: SimulationStats;
  locations: LocationNode[];
  powerLines: PowerLineEdge[];
  faults: Fault[];
  incidentHistory: IncidentEvent[];
  onNavigate: (page: ActivePage) => void;
  onSimulateClick: () => void;
}

export const OverviewPage: React.FC<OverviewPageProps> = ({
  stats,
  locations,
  powerLines,
  faults,
  incidentHistory,
  onNavigate,
  onSimulateClick,
}) => {
  // Donut chart data for Network Health Distribution
  const networkDistributionData = [
    { name: 'Operational Lines', value: stats.operationalLines, color: '#22C55E' },
    { name: 'Failed Lines', value: stats.failedLines, color: '#EF4444' },
    { name: 'Under Maintenance', value: faults.filter((f) => f.status === 'IN_PROGRESS').length, color: '#3B82F6' },
  ].filter((d) => d.value > 0); // hide zero-value slices so labels don't overlap

  // Custom label renderer — shows the numeric count outside each slice
  const renderPieLabel = ({
    cx, cy, midAngle, outerRadius, value, name,
  }: {
    cx: number; cy: number; midAngle: number; outerRadius: number; value: number; name: string;
  }) => {
    if (value === 0) return null;
    const RADIAN = Math.PI / 180;
    const radius = outerRadius + 22;
    const x = cx + radius * Math.cos(-midAngle * RADIAN);
    const y = cy + radius * Math.sin(-midAngle * RADIAN);
    // Short label text
    const short = name === 'Operational Lines' ? 'OK' : name === 'Failed Lines' ? 'Fail' : 'Maint';
    return (
      <text
        x={x}
        y={y}
        fill="#F1F5F9"
        textAnchor={x > cx ? 'start' : 'end'}
        dominantBaseline="central"
        fontSize={11}
        fontWeight={600}
      >
        {value} <tspan fill="#94A3B8" fontSize={10}>{short}</tspan>
      </text>
    );
  };

  // Activity chart data
  const activityData = [
    { time: 'T-04:00', faults: 0, repairs: 0 },
    { time: 'T-03:00', faults: 1, repairs: 0 },
    { time: 'T-02:00', faults: 1, repairs: 1 },
    { time: 'T-01:00', faults: 2, repairs: 1 },
    { time: 'Current', faults: stats.activeFaults, repairs: stats.repairsCompleted },
  ];

  const criticalFacilities = locations.filter(
    (l) => l.type === 'HOSPITAL' || l.criticality === 'CRITICAL' || l.criticality === 'HIGH'
  );

  return (
    <div className="space-y-6 animate-in fade-in duration-300">
      {/* 1. Welcome Section */}
      <div className="relative overflow-hidden bg-gradient-to-r from-[#151F32] via-[#101A2B] to-[#151F32] border border-[#26344A] rounded-2xl p-6 sm:p-7 shadow-panel">
        <div className="relative z-10 flex flex-col md:flex-row md:items-center justify-between gap-4">
          <div>
            <div className="flex items-center gap-2 mb-2">
              <span className="relative flex h-2.5 w-2.5">
                <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-emerald-400 opacity-75" />
                <span className="relative inline-flex rounded-full h-2.5 w-2.5 bg-emerald-500" />
              </span>
              <span className="text-xs font-semibold uppercase tracking-wider text-emerald-400">
                SCADA Network Online &bull; Karachi Megacity Power Grid Active
              </span>
            </div>
            <h1 className="text-2xl sm:text-3xl font-extrabold text-[#F1F5F9] tracking-tight">
              Karachi Grid Operations Overview
            </h1>
            <p className="text-sm text-[#94A3B8] mt-1 max-w-2xl">
              Monitor real-time network health across Karachi, isolate line breaks using Breadth-First Search,
              triage emergency work orders via Max-Heap, and dispatch restoration crews along
              optimal Dijkstra routes.
            </p>
          </div>

          <div className="flex items-center gap-3">
            <button
              onClick={() => onNavigate('map')}
              className="px-4 py-2.5 bg-emerald-600/20 hover:bg-emerald-600/30 border border-emerald-500/40 text-emerald-300 text-xs font-semibold rounded-lg flex items-center gap-2 transition-all shadow-sm"
            >
              <MapPin className="w-4 h-4 text-emerald-400" />
              <span>Karachi Map</span>
            </button>
            <button
              onClick={() => onNavigate('network')}
              className="px-4 py-2.5 bg-[#101A2B] hover:bg-[#151F32] border border-[#26344A] hover:border-slate-500 text-slate-200 text-xs font-semibold rounded-lg flex items-center gap-2 transition-all"
            >
              <Zap className="w-4 h-4 text-amber-400" />
              <span>Network Graph</span>
            </button>
            <button
              onClick={onSimulateClick}
              className="px-4 py-2.5 bg-red-600 hover:bg-red-500 text-white text-xs font-bold rounded-lg flex items-center gap-2 shadow-lg shadow-red-900/40 transition-all"
            >
              <AlertTriangle className="w-4 h-4" />
              <span>Simulate Line Failure</span>
            </button>
          </div>
        </div>
      </div>

      {/* 2. Six Statistics Cards */}
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-6 gap-4">
        <StatisticCard
          title="Total Locations"
          value={stats.totalLocations}
          subtitle="Monitored city nodes"
          icon={MapPin}
          colorScheme="blue"
          badgeText="100% Mapped"
          badgeType="neutral"
        />
        <StatisticCard
          title="Operational Lines"
          value={`${stats.operationalLines}/${stats.totalLines}`}
          subtitle={`${Math.round((stats.operationalLines / stats.totalLines) * 100)}% energization`}
          icon={Zap}
          colorScheme="green"
          badgeText={stats.failedLines > 0 ? `${stats.failedLines} Offline` : '100% Up'}
          badgeType={stats.failedLines > 0 ? 'alert' : 'up'}
        />
        <StatisticCard
          title="Active Faults"
          value={stats.activeFaults}
          subtitle="Max-Heap repair queue"
          icon={AlertTriangle}
          colorScheme="red"
          badgeText={stats.activeFaults > 0 ? 'Action Req' : 'Zero Faults'}
          badgeType={stats.activeFaults > 0 ? 'alert' : 'up'}
        />
        <StatisticCard
          title="Affected Population"
          value={stats.affectedPopulation.toLocaleString()}
          subtitle="Civic residential impact"
          icon={Users}
          colorScheme="amber"
          badgeText={stats.affectedPopulation > 0 ? 'Outage Active' : 'All Powered'}
          badgeType={stats.affectedPopulation > 0 ? 'alert' : 'up'}
        />
        <StatisticCard
          title="Critical Facilities"
          value={stats.criticalFacilitiesAffected}
          subtitle="Hospitals & Univs down"
          icon={Activity}
          colorScheme="purple"
          badgeText={stats.criticalFacilitiesAffected > 0 ? 'HIGH PRIORITY' : 'Protected'}
          badgeType={stats.criticalFacilitiesAffected > 0 ? 'alert' : 'up'}
        />
        <StatisticCard
          title="Repairs Completed"
          value={stats.repairsCompleted}
          subtitle="Dijkstra dispatches"
          icon={CheckCircle2}
          colorScheme="cyan"
          badgeText="Restored"
          badgeType="up"
        />
      </div>

      {/* 3. Network Overview Panel & Charts */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Network Health Distribution Donut */}
        <ChartCard
          title="Network Health Distribution"
          subtitle="Real-time transmission line status breakdown"
        >
          <div className="relative w-full h-64 flex items-center justify-center">
            <ResponsiveContainer width="100%" height="100%">
              <PieChart>
                <Pie
                  data={networkDistributionData}
                  cx="50%"
                  cy="50%"
                  innerRadius={60}
                  outerRadius={85}
                  paddingAngle={5}
                  dataKey="value"
                  labelLine={false}
                  label={renderPieLabel}
                >
                  {networkDistributionData.map((entry, index) => (
                    <Cell key={`cell-${index}`} fill={entry.color} stroke="#151F32" strokeWidth={2} />
                  ))}
                  {/* Center total */}
                </Pie>
                <Tooltip
                  contentStyle={{
                    backgroundColor: '#101A2B',
                    border: '1px solid #26344A',
                    borderRadius: '8px',
                    color: '#F1F5F9',
                    fontSize: '12px',
                  }}
                  formatter={(value: number, name: string) => [`${value} lines`, name]}
                />
                <Legend
                  verticalAlign="bottom"
                  content={(props) => {
                    const { payload } = props;
                    if (!payload) return null;
                    return (
                      <div className="flex items-center justify-center gap-4 pt-3 flex-wrap">
                        {payload.map((entry: any, index: number) => (
                          <div key={`legend-pie-${index}`} className="flex items-center gap-1.5">
                            <span
                              className="w-2.5 h-2.5 rounded-full inline-block shrink-0 shadow-sm"
                              style={{ backgroundColor: entry.color }}
                            />
                            <span className="text-[#CBD5E1] text-[11px] font-medium leading-none select-none">
                              {entry.value}
                            </span>
                          </div>
                        ))}
                      </div>
                    );
                  }}
                />
              </PieChart>
            </ResponsiveContainer>
            {/* Centred total overlay */}
            <div
              className="absolute flex flex-col items-center justify-center pointer-events-none"
              style={{ width: 120, height: 120 }}
            >
              <span className="text-2xl font-bold text-white">{stats.totalLines}</span>
              <span className="text-[10px] text-slate-400 uppercase tracking-widest">Total Lines</span>
            </div>
          </div>
        </ChartCard>

        {/* Incident Activity Chart */}
        <ChartCard
          title="Incident Activity & Resolution"
          subtitle="Fault occurrences vs repairs completed over timeline"
        >
          <div className="w-full h-64">
            <ResponsiveContainer width="100%" height="100%">
              <BarChart data={activityData} margin={{ top: 10, right: 10, left: -20, bottom: 0 }}>
                <CartesianGrid strokeDasharray="3 3" stroke="#26344A" opacity={0.6} />
                <XAxis dataKey="time" stroke="#94A3B8" fontSize={11} tickLine={false} />
                <YAxis stroke="#94A3B8" fontSize={11} tickLine={false} allowDecimals={false} />
                <Tooltip
                  contentStyle={{
                    backgroundColor: '#101A2B',
                    border: '1px solid #26344A',
                    borderRadius: '8px',
                    color: '#F1F5F9',
                    fontSize: '12px',
                  }}
                />
                <Legend
                  content={(props) => {
                    const { payload } = props;
                    if (!payload) return null;
                    return (
                      <div className="flex items-center justify-center gap-4 pt-2 flex-wrap">
                        {payload.map((entry: any, index: number) => (
                          <div key={`legend-bar-${index}`} className="flex items-center gap-1.5">
                            <span
                              className="w-2.5 h-2.5 rounded-sm inline-block shrink-0 shadow-sm"
                              style={{ backgroundColor: entry.color }}
                            />
                            <span className="text-[#CBD5E1] text-[11px] font-medium leading-none select-none">
                              {entry.value}
                            </span>
                          </div>
                        ))}
                      </div>
                    );
                  }}
                />
                <Bar dataKey="faults" name="Faults Detected" fill="#EF4444" radius={[4, 4, 0, 0]} />
                <Bar dataKey="repairs" name="Repairs Done" fill="#22C55E" radius={[4, 4, 0, 0]} />
              </BarChart>
            </ResponsiveContainer>
          </div>
        </ChartCard>

        {/* Overall Grid Health Status Card */}
        <div className="bg-[#151F32] border border-[#26344A] rounded-xl p-5 shadow-panel flex flex-col justify-between">
          <div>
            <div className="flex items-center justify-between mb-4">
              <h3 className="text-sm font-bold text-[#F1F5F9]">Overall Grid Health</h3>
              <StatusBadge
                status={stats.networkHealthPercentage >= 90 ? 'OPTIMAL' : 'DEGRADED'}
                type={stats.networkHealthPercentage >= 90 ? 'operational' : 'warning'}
                pulse={stats.networkHealthPercentage < 90}
              />
            </div>

            <div className="flex items-baseline gap-3 my-3">
              <span className="text-4xl font-extrabold text-[#F1F5F9]">
                {stats.networkHealthPercentage}%
              </span>
              <span className="text-xs text-[#94A3B8] font-medium">Composite Grid Stability</span>
            </div>

            {/* Health Bar */}
            <div className="w-full bg-[#0B1120] h-3 rounded-full overflow-hidden border border-[#26344A] my-3">
              <div
                className={`h-full transition-all duration-500 rounded-full ${
                  stats.networkHealthPercentage >= 90
                    ? 'bg-gradient-to-r from-emerald-500 to-emerald-400'
                    : 'bg-gradient-to-r from-amber-500 to-red-500'
                }`}
                style={{ width: `${stats.networkHealthPercentage}%` }}
              />
            </div>

            {/* Health Highlights */}
            <div className="space-y-2 mt-4 text-xs">
              <div className="flex justify-between py-1 border-b border-[#26344A]/60">
                <span className="text-[#94A3B8]">Main Plant Connectivity:</span>
                <span className="text-emerald-400 font-bold">100% Operational</span>
              </div>
              <div className="flex justify-between py-1 border-b border-[#26344A]/60">
                <span className="text-[#94A3B8]">Active Substations:</span>
                <span className="text-slate-200 font-semibold">3 / 3 Active</span>
              </div>
              <div className="flex justify-between py-1 border-b border-[#26344A]/60">
                <span className="text-[#94A3B8]">Disconnected Locations:</span>
                <span className={stats.totalLocations - locations.filter((l) => l.isPowered).length > 0 ? 'text-red-400 font-bold' : 'text-slate-200 font-semibold'}>
                  {stats.totalLocations - locations.filter((l) => l.isPowered).length} nodes
                </span>
              </div>
            </div>
          </div>

          <button
            onClick={() => onNavigate('repairs')}
            className="w-full mt-4 py-2 px-3 bg-[#101A2B] hover:bg-blue-600/20 text-blue-400 border border-blue-500/30 rounded-lg text-xs font-bold flex items-center justify-center gap-1.5 transition-all"
          >
            <span>Review Emergency Work Orders</span>
            <ArrowUpRight className="w-3.5 h-3.5" />
          </button>
        </div>
      </div>

      {/* 4. Recent Incidents & Critical Facilities Monitoring */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Recent Incidents Table (2 cols) */}
        <div className="lg:col-span-2 bg-[#151F32] border border-[#26344A] rounded-xl p-5 shadow-panel">
          <div className="flex items-center justify-between mb-4">
            <div>
              <h3 className="text-sm font-bold text-[#F1F5F9]">Recent Incidents & Fault Logs</h3>
              <p className="text-xs text-[#94A3B8]">Audit events recorded in Doubly Linked List</p>
            </div>
            <button
              onClick={() => onNavigate('history')}
              className="text-xs text-blue-400 hover:text-blue-300 font-medium flex items-center gap-1 transition-all"
            >
              <span>View Full History</span>
              <ChevronRight className="w-3.5 h-3.5" />
            </button>
          </div>

          <div className="overflow-x-auto">
            <table className="w-full text-left text-xs">
              <thead className="bg-[#0B1120] text-[#94A3B8] uppercase text-[10px] tracking-wider border-b border-[#26344A]">
                <tr>
                  <th className="py-2.5 px-3">Event ID</th>
                  <th className="py-2.5 px-3">Type</th>
                  <th className="py-2.5 px-3">Description</th>
                  <th className="py-2.5 px-3">Timestamp</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-[#26344A]/50">
                {incidentHistory.slice(0, 5).map((ev) => (
                  <tr key={ev.eventId} className="hover:bg-[#101A2B]/60 transition-colors">
                    <td className="py-2.5 px-3 font-mono font-semibold text-blue-400">
                      #{ev.eventId}
                    </td>
                    <td className="py-2.5 px-3">
                      <span
                        className={`text-[10px] font-bold px-2 py-0.5 rounded border uppercase ${
                          ev.eventType.includes('FAILURE')
                            ? 'text-red-400 bg-red-500/10 border-red-500/30'
                            : ev.eventType.includes('REPAIR')
                            ? 'text-emerald-400 bg-emerald-500/10 border-emerald-500/30'
                            : 'text-slate-300 bg-slate-800 border-slate-700'
                        }`}
                      >
                        {ev.eventType.replace('_', ' ')}
                      </span>
                    </td>
                    <td className="py-2.5 px-3 text-[#F1F5F9] max-w-xs truncate">
                      {ev.description}
                    </td>
                    <td className="py-2.5 px-3 font-mono text-[#94A3B8]">{ev.timestamp}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>

        {/* Critical Facilities Panel (1 col) */}
        <div className="bg-[#151F32] border border-[#26344A] rounded-xl p-5 shadow-panel">
          <div className="flex items-center justify-between mb-4">
            <div>
              <h3 className="text-sm font-bold text-[#F1F5F9]">Critical Facilities Monitoring</h3>
              <p className="text-xs text-[#94A3B8]">Hospitals, Trauma Centers & Universities</p>
            </div>
            <ShieldCheck className="w-4 h-4 text-emerald-400" />
          </div>

          <div className="space-y-3">
            {criticalFacilities.map((fac) => (
              <div
                key={fac.id}
                className="p-3 rounded-lg bg-[#0B1120] border border-[#26344A] flex items-center justify-between transition-all hover:border-slate-600"
              >
                <div>
                  <div className="flex items-center gap-1.5">
                    <span className="text-[10px] font-bold text-slate-400 font-mono">
                      #{fac.id}
                    </span>
                    <h4 className="text-xs font-bold text-[#F1F5F9]">{fac.name}</h4>
                  </div>
                  <div className="flex items-center gap-2 mt-1 text-[11px] text-[#94A3B8]">
                    <span>{fac.type}</span>
                    <span>&bull;</span>
                    <span>{fac.population.toLocaleString()} pop</span>
                  </div>
                </div>

                <div>
                  <StatusBadge
                    status={fac.isPowered ? 'ENERGIZED' : 'OUTAGE'}
                    type={fac.isPowered ? 'operational' : 'critical'}
                    pulse={!fac.isPowered}
                    size="sm"
                  />
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
};
