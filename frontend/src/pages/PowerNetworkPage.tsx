import React, { useState, useMemo, useCallback } from 'react';
import {
  ReactFlow,
  Controls,
  Background,
  Node,
  Edge,
} from '@xyflow/react';
import {
  Zap,
  Radio,
  Activity,
  GraduationCap,
  Home,
  Building2,
  AlertTriangle,
  RotateCcw,
  Search,
  Maximize2,
  HelpCircle,
} from 'lucide-react';
import { LocationNode, PowerLineEdge } from '../types';
import { CustomLocationNode } from '../components/CustomLocationNode';
import { NetworkInspectorPanel } from '../components/NetworkInspectorPanel';

interface PowerNetworkPageProps {
  locations: LocationNode[];
  powerLines: PowerLineEdge[];
  onSimulateLineFailure: (lineId: number) => void;
  onRestoreLine: (lineId: number) => void;
  onInspectRoute: (destinationId: number) => void;
}

export const PowerNetworkPage: React.FC<PowerNetworkPageProps> = ({
  locations,
  powerLines,
  onSimulateLineFailure,
  onRestoreLine,
  onInspectRoute,
}) => {
  const [selectedNodeId, setSelectedNodeId] = useState<number | null>(null);
  const [selectedEdgeId, setSelectedEdgeId] = useState<number | null>(null);

  // Define node types for React Flow
  const nodeTypes = useMemo(() => ({ customLocation: CustomLocationNode }), []);

  // Convert locations to React Flow Nodes
  const initialNodes: Node[] = useMemo(() => {
    return locations.map((loc) => ({
      id: loc.id.toString(),
      type: 'customLocation',
      position: { x: loc.x, y: loc.y },
      data: loc,
      selected: selectedNodeId === loc.id,
    }));
  }, [locations, selectedNodeId]);

  // Convert powerLines to React Flow Edges
  const initialEdges: Edge[] = useMemo(() => {
    return powerLines.map((line) => {
      const isFailed = !line.isOperational;
      const isSelected = selectedEdgeId === line.id;
      return {
        id: line.id.toString(),
        source: line.fromLocationId.toString(),
        target: line.toLocationId.toString(),
        animated: line.isOperational,
        style: {
          stroke: isFailed ? '#EF4444' : isSelected ? '#3B82F6' : '#22C55E',
          strokeWidth: isSelected ? 4 : isFailed ? 3 : 2.5,
          strokeDasharray: isFailed ? '6, 6' : undefined,
          filter: isFailed
            ? 'drop-shadow(0 0 8px rgba(239, 68, 68, 0.8))'
            : isSelected
            ? 'drop-shadow(0 0 8px rgba(59, 130, 246, 0.8))'
            : 'drop-shadow(0 0 3px rgba(34, 197, 94, 0.5))',
        },
        label: `#${line.id}`,
        labelStyle: {
          fill: isFailed ? '#EF4444' : '#94A3B8',
          fontWeight: 700,
          fontSize: 10,
        },
        labelBgStyle: {
          fill: '#101A2B',
          stroke: isFailed ? '#EF4444' : '#26344A',
          strokeWidth: 1,
        },
      };
    });
  }, [powerLines, selectedEdgeId]);

  // Selection handlers
  const onNodeClick = useCallback((_: React.MouseEvent, node: Node) => {
    setSelectedNodeId(Number(node.id));
    setSelectedEdgeId(null);
  }, []);

  const onEdgeClick = useCallback((_: React.MouseEvent, edge: Edge) => {
    setSelectedEdgeId(Number(edge.id));
    setSelectedNodeId(null);
  }, []);

  const handleCloseInspector = () => {
    setSelectedNodeId(null);
    setSelectedEdgeId(null);
  };

  const selectedNode = locations.find((l) => l.id === selectedNodeId) || null;
  const selectedEdge = powerLines.find((l) => l.id === selectedEdgeId) || null;

  return (
    <div className="relative h-[calc(100vh-100px)] w-full flex overflow-hidden rounded-xl border border-[#26344A] bg-[#0B1120] shadow-panel">
      {/* Main Graph Area */}
      <div className="flex-1 h-full relative flex flex-col">
        {/* Top Control Bar */}
        <div className="h-14 px-6 bg-[#101A2B]/95 backdrop-blur-md border-b border-[#26344A] flex items-center justify-between z-10">
          {/* Facility Type Legend Chips */}
          <div className="flex items-center gap-2 overflow-x-auto py-1">
            <span className="text-[11px] font-bold text-[#94A3B8] uppercase mr-1">
              Legend:
            </span>
            <span className="flex items-center gap-1 text-[11px] font-semibold text-amber-400 bg-amber-500/10 px-2 py-0.5 rounded border border-amber-500/20">
              <Zap className="w-3 h-3" /> Power Plant
            </span>
            <span className="flex items-center gap-1 text-[11px] font-semibold text-cyan-400 bg-cyan-500/10 px-2 py-0.5 rounded border border-cyan-500/20">
              <Radio className="w-3 h-3" /> Substation
            </span>
            <span className="flex items-center gap-1 text-[11px] font-semibold text-red-400 bg-red-500/10 px-2 py-0.5 rounded border border-red-500/20">
              <Activity className="w-3 h-3" /> Hospital
            </span>
            <span className="flex items-center gap-1 text-[11px] font-semibold text-purple-400 bg-purple-500/10 px-2 py-0.5 rounded border border-purple-500/20">
              <GraduationCap className="w-3 h-3" /> University
            </span>
            <span className="flex items-center gap-1 text-[11px] font-semibold text-emerald-400 bg-emerald-500/10 px-2 py-0.5 rounded border border-emerald-500/20">
              <Home className="w-3 h-3" /> Residential
            </span>
            <span className="flex items-center gap-1 text-[11px] font-semibold text-blue-400 bg-blue-500/10 px-2 py-0.5 rounded border border-blue-500/20">
              <Building2 className="w-3 h-3" /> Commercial
            </span>
          </div>

          {/* Line Status Legend */}
          <div className="flex items-center gap-3 text-xs">
            <div className="flex items-center gap-1.5 font-medium text-emerald-400">
              <span className="w-3 h-0.5 bg-emerald-500 rounded" />
              <span>Operational Line</span>
            </div>
            <div className="flex items-center gap-1.5 font-medium text-red-400">
              <span className="w-3 h-0.5 bg-red-500 border-dashed rounded" />
              <span>Failed / Severed Line</span>
            </div>
          </div>
        </div>

        {/* React Flow Canvas */}
        <div className="flex-1 w-full h-full">
          <ReactFlow
            nodes={initialNodes}
            edges={initialEdges}
            onNodeClick={onNodeClick}
            onEdgeClick={onEdgeClick}
            nodeTypes={nodeTypes}
            fitView
            minZoom={0.3}
            maxZoom={1.8}
            proOptions={{ hideAttribution: true }}
          >
            <Background color="#26344A" gap={20} size={1.2} />
            <Controls />
          </ReactFlow>
        </div>

        {/* Bottom Floating Hint */}
        <div className="absolute bottom-4 left-6 z-10 bg-[#151F32]/90 backdrop-blur-md border border-[#26344A] px-3.5 py-1.5 rounded-lg text-xs text-[#94A3B8] shadow-lg flex items-center gap-2">
          <HelpCircle className="w-3.5 h-3.5 text-blue-400" />
          <span>Click any facility node or transmission line to inspect live metrics.</span>
        </div>
      </div>

      {/* Right-Side Inspector Panel */}
      <NetworkInspectorPanel
        selectedNode={selectedNode}
        selectedEdge={selectedEdge}
        locations={locations}
        powerLines={powerLines}
        onClose={handleCloseInspector}
        onSimulateLineFailure={onSimulateLineFailure}
        onRestoreLine={onRestoreLine}
        onInspectRoute={onInspectRoute}
      />
    </div>
  );
};
