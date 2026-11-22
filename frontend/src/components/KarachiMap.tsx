import React, { useState, useMemo, useEffect } from 'react';
import {
  MapContainer,
  TileLayer,
  Marker,
  Popup,
  Polyline,
  useMap,
} from 'react-leaflet';
import L from 'leaflet';
import {
  Zap,
  Radio,
  Activity,
  GraduationCap,
  Home,
  Building2,
  Factory,
  Search,
  Filter,
  Eye,
  EyeOff,
  Navigation,
  AlertTriangle,
  Layers,
  MapPin,
  CheckCircle,
} from 'lucide-react';
import { LocationNode, PowerLineEdge, Fault, DijkstraRoute } from '../types';

interface KarachiMapProps {
  locations: LocationNode[];
  powerLines: PowerLineEdge[];
  faults: Fault[];
  activeDijkstraRoute: DijkstraRoute | null;
  onSimulateLineFailure: (lineId: number) => void;
  onRestoreLine: (lineId: number) => void;
  onInspectRoute: (destinationId: number) => void;
  onOpenSimulateModal?: (lineId?: number) => void;
}

// Helper component to smoothly pan/zoom map when searching or selecting location
const MapController: React.FC<{ center: [number, number]; zoom?: number }> = ({
  center,
  zoom,
}) => {
  const map = useMap();
  useEffect(() => {
    map.flyTo(center, zoom || map.getZoom(), { duration: 1.2 });
  }, [center, zoom, map]);
  return null;
};

// Create custom SVG Leaflet divIcon
const createCustomMarkerIcon = (
  type: string,
  isPowered: boolean,
  criticality: string
) => {
  let bgColor = '#3B82F6';
  let borderColor = '#60A5FA';
  let svgPath = '';

  switch (type) {
    case 'POWER_STATION':
      bgColor = '#EAB308'; // Yellow
      borderColor = '#FACC15';
      svgPath = '<polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2" fill="white" stroke="none" transform="scale(0.8) translate(3,3)"/>';
      break;
    case 'SUBSTATION':
      bgColor = '#F97316'; // Orange
      borderColor = '#FB923C';
      svgPath = '<circle cx="12" cy="12" r="2" fill="white"/><path d="M4.93 4.93a10 10 0 0 1 14.14 0" stroke="white" stroke-width="2" fill="none"/><path d="M7.76 7.76a6 6 0 0 1 8.48 0" stroke="white" stroke-width="2" fill="none"/>';
      break;
    case 'HOSPITAL':
      bgColor = '#EF4444'; // Red
      borderColor = '#F87171';
      svgPath = '<path d="M12 6v12M6 12h12" stroke="white" stroke-width="3" stroke-linecap="round"/>';
      break;
    case 'UNIVERSITY':
      bgColor = '#8B5CF6'; // Purple/Blue
      borderColor = '#A78BFA';
      svgPath = '<path d="M22 10v6M2 10l10-5 10 5-10 5z" stroke="white" stroke-width="2" fill="none"/><path d="M6 12v5c0 2 3 3 6 3s6-1 6-3v-5" stroke="white" stroke-width="2" fill="none"/>';
      break;
    case 'RESIDENTIAL':
      bgColor = '#10B981'; // Green
      borderColor = '#34D399';
      svgPath = '<path d="m3 9 9-7 9 7v11a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2z" stroke="white" stroke-width="2" fill="none"/><polyline points="9 22 9 12 15 12 15 22" stroke="white" stroke-width="2" fill="none"/>';
      break;
    case 'COMMERCIAL':
      bgColor = '#3B82F6'; // Blue
      borderColor = '#60A5FA';
      svgPath = '<rect x="4" y="2" width="16" height="20" rx="2" stroke="white" stroke-width="2" fill="none"/><path d="M9 22v-4h6v4" stroke="white" stroke-width="2"/>';
      break;
    case 'INDUSTRIAL':
      bgColor = '#64748B'; // Slate Grey
      borderColor = '#94A3B8';
      svgPath = '<path d="M2 20a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2V8l-7 5V8l-7 5V4a2 2 0 0 0-2-2H4a2 2 0 0 0-2 2Z" stroke="white" stroke-width="2" fill="none"/>';
      break;
    default:
      bgColor = '#3B82F6';
      borderColor = '#60A5FA';
      svgPath = '<circle cx="12" cy="12" r="6" fill="white"/>';
  }

  // If node is currently de-energized (outage)
  if (!isPowered) {
    bgColor = '#DC2626';
    borderColor = '#EF4444';
  }

  const pulseClass = !isPowered ? 'marker-offline-pulse' : '';
  const size = type === 'POWER_STATION' ? 36 : type === 'HOSPITAL' ? 32 : 28;

  const html = `
    <div class="${pulseClass}" style="
      width: ${size}px;
      height: ${size}px;
      background: ${bgColor};
      border: 2px solid ${borderColor};
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: 0 0 12px ${bgColor}88;
      cursor: pointer;
    ">
      <svg width="${size * 0.6}" height="${size * 0.6}" viewBox="0 0 24 24" fill="none">
        ${svgPath}
      </svg>
    </div>
  `;

  return L.divIcon({
    html,
    className: 'custom-leaflet-marker',
    iconSize: [size, size],
    iconAnchor: [size / 2, size / 2],
    popupAnchor: [0, -size / 2],
  });
};

export const KarachiMap: React.FC<KarachiMapProps> = ({
  locations,
  powerLines,
  faults,
  activeDijkstraRoute,
  onSimulateLineFailure,
  onRestoreLine,
  onInspectRoute,
  onOpenSimulateModal,
}) => {
  const [filterCategory, setFilterCategory] = useState<string>('ALL');
  const [searchQuery, setSearchQuery] = useState<string>('');
  const [showPowerLines, setShowPowerLines] = useState<boolean>(true);
  const [showRoadRoute, setShowRoadRoute] = useState<boolean>(true);
  const [mapCenter, setMapCenter] = useState<[number, number]>([24.8607, 67.0500]);
  const [mapZoom, setMapZoom] = useState<number>(12);

  // Filter locations
  const filteredLocations = useMemo(() => {
    return locations.filter((loc) => {
      // Search
      const matchesSearch =
        loc.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
        loc.area.toLowerCase().includes(searchQuery.toLowerCase()) ||
        loc.id.toString() === searchQuery;

      if (!matchesSearch) return false;

      // Category filter
      if (filterCategory === 'ALL') return true;
      if (filterCategory === 'HOSPITAL') return loc.type === 'HOSPITAL';
      if (filterCategory === 'UNIVERSITY') return loc.type === 'UNIVERSITY';
      if (filterCategory === 'RESIDENTIAL') return loc.type === 'RESIDENTIAL';
      if (filterCategory === 'COMMERCIAL') return loc.type === 'COMMERCIAL';
      if (filterCategory === 'INDUSTRIAL') return loc.type === 'INDUSTRIAL';
      if (filterCategory === 'SUPPLY') return loc.type === 'POWER_STATION' || loc.type === 'SUBSTATION';
      if (filterCategory === 'OUTAGE') return !loc.isPowered;
      if (filterCategory === 'POWERED') return loc.isPowered;

      return true;
    });
  }, [locations, filterCategory, searchQuery]);

  // Handle Search Selection
  const handleSelectSearchResult = (loc: LocationNode) => {
    setMapCenter([loc.latitude, loc.longitude]);
    setMapZoom(14);
  };

  const getLocationById = (id: number) => locations.find((l) => l.id === id);

  return (
    <div className="relative w-full h-[calc(100vh-100px)] flex flex-col rounded-2xl overflow-hidden border border-[#26344A] bg-[#0B1120] shadow-2xl">
      {/* Top Filter and Search Control Bar */}
      <div className="px-5 py-3 bg-[#101A2B]/95 backdrop-blur-md border-b border-[#26344A] z-[1000] flex flex-wrap items-center justify-between gap-3">
        {/* Left: Search Bar */}
        <div className="relative min-w-[260px] flex-1 max-w-sm">
          <Search className="w-4 h-4 absolute left-3 top-1/2 -translate-y-1/2 text-[#94A3B8]" />
          <input
            type="text"
            value={searchQuery}
            onChange={(e) => setSearchQuery(e.target.value)}
            placeholder="Search Karachi hospital, university, or neighborhood..."
            className="w-full pl-9 pr-3 py-1.5 bg-[#0B1120] border border-[#26344A] focus:border-blue-500 rounded-lg text-xs text-[#F1F5F9] placeholder-[#94A3B8] focus:outline-none focus:ring-1 focus:ring-blue-500"
          />
        </div>

        {/* Center: Category Filters */}
        <div className="flex items-center gap-1.5 overflow-x-auto py-1 text-xs">
          {[
            { id: 'ALL', label: 'All Mapped Nodes' },
            { id: 'HOSPITAL', label: 'Hospitals' },
            { id: 'UNIVERSITY', label: 'Universities' },
            { id: 'RESIDENTIAL', label: 'Residential' },
            { id: 'COMMERCIAL', label: 'Commercial' },
            { id: 'INDUSTRIAL', label: 'Industrial' },
            { id: 'SUPPLY', label: 'Supply Hubs' },
            { id: 'OUTAGE', label: 'Active Outages' },
          ].map((cat) => (
            <button
              key={cat.id}
              onClick={() => setFilterCategory(cat.id)}
              className={`px-2.5 py-1 rounded-lg font-medium whitespace-nowrap transition-all ${
                filterCategory === cat.id
                  ? 'bg-blue-600 text-white shadow-glow-blue font-bold'
                  : 'bg-[#151F32] text-[#94A3B8] hover:text-[#F1F5F9] hover:bg-[#26344A]/60 border border-[#26344A]'
              }`}
            >
              {cat.label}
            </button>
          ))}
        </div>

        {/* Right: Layer Toggles & Action Controls */}
        <div className="flex items-center gap-2">
          {/* Direct Map Failure Simulation Trigger */}
          <button
            onClick={() => {
              if (onOpenSimulateModal) {
                onOpenSimulateModal();
              } else {
                onSimulateLineFailure(17);
              }
            }}
            className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-bold bg-red-600 hover:bg-red-500 text-white shadow-md shadow-red-900/40 border border-red-500/50 transition-all cursor-pointer"
            title="Simulate Power Line Failure on Karachi Grid"
          >
            <AlertTriangle className="w-3.5 h-3.5" />
            <span>Simulate Failure</span>
          </button>

          <button
            onClick={() => setShowPowerLines(!showPowerLines)}
            className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-semibold border transition-all ${
              showPowerLines
                ? 'bg-emerald-500/15 text-emerald-400 border-emerald-500/30'
                : 'bg-[#151F32] text-slate-500 border-[#26344A]'
            }`}
          >
            {showPowerLines ? <Eye className="w-3.5 h-3.5" /> : <EyeOff className="w-3.5 h-3.5" />}
            <span>Power Lines</span>
          </button>

          <button
            onClick={() => {
              setMapCenter([24.8607, 67.0500]);
              setMapZoom(12);
              setFilterCategory('ALL');
              setSearchQuery('');
            }}
            title="Reset to City View"
            className="px-2.5 py-1.5 rounded-lg bg-[#151F32] border border-[#26344A] text-xs text-[#94A3B8] hover:text-[#F1F5F9] transition-all"
          >
            Reset View
          </button>
        </div>
      </div>

      {/* Main Map Canvas */}
      <div className="flex-1 w-full h-full relative dark-tiles">
        <MapContainer
          center={mapCenter}
          zoom={mapZoom}
          className="w-full h-full"
          scrollWheelZoom={true}
        >
          <MapController center={mapCenter} zoom={mapZoom} />

          {/* OpenStreetMap Tile Layer with Custom Dark Navy Styling */}
          <TileLayer
            attribution='&copy; <a href="https://www.openstreetmap.org/copyright" target="_blank">OpenStreetMap</a> contributors'
            url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            maxZoom={18}
          />

          {/* 1. Simulated Power Transmission Lines Overlay */}
          {showPowerLines &&
            powerLines.map((line) => {
              const fromLoc = getLocationById(line.fromLocationId);
              const toLoc = getLocationById(line.toLocationId);
              if (!fromLoc || !toLoc) return null;

              const isFailed = !line.isOperational;
              const positions: [number, number][] = [
                [fromLoc.latitude, fromLoc.longitude],
                [toLoc.latitude, toLoc.longitude],
              ];

              return (
                <Polyline
                  key={`line-${line.id}-${line.isOperational ? 'op' : 'fail'}`}
                  positions={positions}
                  pathOptions={{
                    color: isFailed ? '#EF4444' : '#22C55E',
                    weight: isFailed ? 4.5 : 3.0,
                    dashArray: isFailed ? '6, 8' : undefined,
                    opacity: isFailed ? 0.95 : 0.75,
                  }}
                >
                  <Popup>
                    <div className="p-1 min-w-[220px] text-xs space-y-2">
                      <div className="border-b border-[#26344A] pb-1.5">
                        <div className="flex items-center justify-between">
                          <span className="text-[10px] font-mono font-bold text-amber-400">
                            TRANSMISSION LINE #{line.id}
                          </span>
                          <span
                            className={`text-[9px] font-bold px-1.5 py-0.2 rounded border uppercase ${
                              line.isOperational
                                ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/30'
                                : 'bg-red-500/10 text-red-400 border-red-500/30 animate-pulse'
                            }`}
                          >
                            {line.isOperational ? 'OPERATIONAL' : 'LINE SEVERED'}
                          </span>
                        </div>
                        <h4 className="text-xs font-bold text-[#F1F5F9] mt-1">
                          {fromLoc.name} &harr; {toLoc.name}
                        </h4>
                      </div>

                      <div className="space-y-1 text-[11px] text-[#94A3B8]">
                        <div className="flex justify-between">
                          <span>Capacity / Voltage:</span>
                          <span className="text-slate-200 font-semibold">{line.capacityMW || 25} MW</span>
                        </div>
                        <div className="flex justify-between">
                          <span>Repair Cost:</span>
                          <span className="text-slate-200 font-semibold">${line.repairCost.toLocaleString()}</span>
                        </div>
                        <div className="flex justify-between">
                          <span>Crew Travel:</span>
                          <span className="text-slate-200 font-semibold">{line.travelCost} mins</span>
                        </div>
                      </div>

                      <div className="pt-2 border-t border-[#26344A] flex gap-2">
                        {line.isOperational ? (
                          <button
                            onClick={() => {
                              if (onOpenSimulateModal) {
                                onOpenSimulateModal(line.id);
                              } else {
                                onSimulateLineFailure(line.id);
                              }
                            }}
                            className="w-full py-1.5 px-2 bg-red-600 hover:bg-red-500 text-white rounded font-bold text-[10px] flex items-center justify-center gap-1.5 transition-all shadow-md"
                          >
                            <AlertTriangle className="w-3.5 h-3.5" />
                            <span>Simulate Line Break</span>
                          </button>
                        ) : (
                          <button
                            onClick={() => onRestoreLine(line.id)}
                            className="w-full py-1.5 px-2 bg-emerald-600 hover:bg-emerald-500 text-white rounded font-bold text-[10px] flex items-center justify-center gap-1.5 transition-all shadow-md"
                          >
                            <Zap className="w-3.5 h-3.5" />
                            <span>Restore Power Line</span>
                          </button>
                        )}
                      </div>
                    </div>
                  </Popup>
                </Polyline>
              );
            })}

          {/* 2. Active Dijkstra Repair Crew Route Overlay */}
          {showRoadRoute && activeDijkstraRoute && activeDijkstraRoute.coordinates.length > 1 && (
            <Polyline
              positions={activeDijkstraRoute.coordinates}
              pathOptions={{
                color: '#3B82F6',
                weight: 5,
                opacity: 0.9,
                dashArray: '8, 8',
              }}
            />
          )}

          {/* 3. Geographical Location Markers */}
          {filteredLocations.map((loc) => {
            const markerIcon = createCustomMarkerIcon(loc.type, loc.isPowered, loc.criticality);
            const connectedLines = powerLines.filter(
              (l) => l.fromLocationId === loc.id || l.toLocationId === loc.id
            );
            const operationalConnected = connectedLines.filter((l) => l.isOperational);

            return (
              <Marker
                key={`marker-${loc.id}-${loc.isPowered ? 'pwr' : 'off'}`}
                position={[loc.latitude, loc.longitude]}
                icon={markerIcon}
              >
                <Popup>
                  <div className="p-1 min-w-[250px] text-xs space-y-2">
                    {/* Header */}
                    <div className="border-b border-[#26344A] pb-2">
                      <div className="flex items-center justify-between">
                        <span className="text-[10px] font-mono font-bold text-blue-400">
                          NODE #{loc.id}
                        </span>
                        <span
                          className={`text-[9px] font-bold px-1.5 py-0.2 rounded border uppercase ${
                            loc.isPowered
                              ? 'bg-emerald-500/10 text-emerald-400 border-emerald-500/30'
                              : 'bg-red-500/10 text-red-400 border-red-500/30 animate-pulse'
                          }`}
                        >
                          {loc.isPowered ? 'ENERGIZED' : 'OUTAGE'}
                        </span>
                      </div>
                      <h4 className="text-sm font-bold text-[#F1F5F9] mt-0.5">{loc.name}</h4>
                      <p className="text-[11px] text-[#94A3B8]">{loc.area}</p>
                    </div>

                    {/* Details */}
                    <div className="space-y-1 text-[11px]">
                      <div className="flex justify-between">
                        <span className="text-[#94A3B8]">Category:</span>
                        <span className="font-semibold text-slate-200">
                          {loc.type.replace('_', ' ')}
                        </span>
                      </div>
                      <div className="flex justify-between">
                        <span className="text-[#94A3B8]">Criticality:</span>
                        <span
                          className={`font-bold ${
                            loc.criticality === 'CRITICAL'
                              ? 'text-red-400'
                              : loc.criticality === 'HIGH'
                              ? 'text-amber-400'
                              : 'text-slate-300'
                          }`}
                        >
                          {loc.criticality}
                        </span>
                      </div>
                      <div className="flex justify-between">
                        <span className="text-[#94A3B8]">Population Served:</span>
                        <span className="font-semibold text-slate-200">
                          {loc.population > 0 ? `${loc.population.toLocaleString()} residents` : 'Infrastructure Hub'}
                        </span>
                      </div>
                      <div className="flex justify-between">
                        <span className="text-[#94A3B8]">Connected Feeders:</span>
                        <span className="font-semibold text-slate-200">
                          {operationalConnected.length}/{connectedLines.length} Operational
                        </span>
                      </div>
                      <div className="flex justify-between font-mono text-[10px] text-slate-400 pt-1 border-t border-[#26344A]/40">
                        <span>Coordinates:</span>
                        <span>
                          {loc.latitude.toFixed(4)}° N, {loc.longitude.toFixed(4)}° E
                        </span>
                      </div>
                    </div>

                    {/* Actions */}
                    <div className="pt-2 border-t border-[#26344A] flex flex-col gap-1.5">
                      <div className="flex gap-2">
                        <button
                          onClick={() => onInspectRoute(loc.id)}
                          className="flex-1 py-1.5 px-2 bg-blue-600 hover:bg-blue-500 text-white rounded font-bold text-[10px] flex items-center justify-center gap-1 transition-all"
                        >
                          <Navigation className="w-3 h-3" />
                          <span>Dijkstra Route</span>
                        </button>
                        {operationalConnected.length > 0 && (
                          <button
                            onClick={() => {
                              if (onOpenSimulateModal) {
                                onOpenSimulateModal(operationalConnected[0].id);
                              } else {
                                onSimulateLineFailure(operationalConnected[0].id);
                              }
                            }}
                            className="flex-1 py-1.5 px-2 bg-red-600/90 hover:bg-red-600 text-white rounded font-bold text-[10px] flex items-center justify-center gap-1 transition-all"
                          >
                            <AlertTriangle className="w-3 h-3" />
                            <span>Simulate Line Break</span>
                          </button>
                        )}
                      </div>
                    </div>
                  </div>
                </Popup>
              </Marker>
            );
          })}
        </MapContainer>

        {/* Bottom Floating Map Info Card */}
        <div className="absolute bottom-4 left-4 z-[1000] p-3 rounded-xl bg-[#101A2B]/90 backdrop-blur-md border border-[#26344A] text-xs text-[#94A3B8] shadow-2xl max-w-sm pointer-events-auto">
          <div className="flex items-center justify-between mb-1">
            <span className="font-bold text-[#F1F5F9] text-xs">Karachi Geographical Power Grid</span>
            <span className="text-[10px] text-emerald-400 font-semibold">Live Simulation</span>
          </div>
          <p className="text-[11px] leading-relaxed">
            Click any municipal marker to inspect feeder reachability, population, and criticality.
            Failed lines appear in dashed red; operational links in neon green.
          </p>
        </div>

        {/* Legend Panel on Bottom Right */}
        <div className="absolute bottom-4 right-4 z-[1000] p-3 rounded-xl bg-[#101A2B]/90 backdrop-blur-md border border-[#26344A] text-xs shadow-2xl pointer-events-auto hidden md:block">
          <span className="font-bold text-slate-300 text-[10px] uppercase tracking-wider block mb-2">
            Karachi Map Legend
          </span>
          <div className="grid grid-cols-2 gap-x-4 gap-y-1.5 text-[11px]">
            <span className="flex items-center gap-1.5 text-amber-400">
              <span className="w-2.5 h-2.5 rounded-full bg-amber-400" /> Supply Hub
            </span>
            <span className="flex items-center gap-1.5 text-orange-400">
              <span className="w-2.5 h-2.5 rounded-full bg-orange-400" /> Dist Substation
            </span>
            <span className="flex items-center gap-1.5 text-red-400">
              <span className="w-2.5 h-2.5 rounded-full bg-red-400" /> Hospital
            </span>
            <span className="flex items-center gap-1.5 text-purple-400">
              <span className="w-2.5 h-2.5 rounded-full bg-purple-400" /> University
            </span>
            <span className="flex items-center gap-1.5 text-emerald-400">
              <span className="w-2.5 h-2.5 rounded-full bg-emerald-400" /> Residential
            </span>
            <span className="flex items-center gap-1.5 text-blue-400">
              <span className="w-2.5 h-2.5 rounded-full bg-blue-400" /> Commercial
            </span>
            <span className="flex items-center gap-1.5 text-slate-400">
              <span className="w-2.5 h-2.5 rounded-full bg-slate-400" /> Industrial
            </span>
            <span className="flex items-center gap-1.5 text-red-500 font-bold">
              <span className="w-2.5 h-2.5 rounded-full bg-red-500 animate-pulse" /> Outage (Power Cut)
            </span>
          </div>
        </div>
      </div>
    </div>
  );
};
