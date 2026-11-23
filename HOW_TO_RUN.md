# GridGuard — Full-Stack Application: How to Run

> **Branch:** `fullstack-integrated`  
> **Stack:** C++ HTTP Backend (port 8081) + React/Vite Frontend (port 5173 in dev)

---

## Prerequisites

| Tool | Version | Download |
|------|---------|----------|
| **g++ (MinGW-w64)** | ≥ 11 | https://www.mingw-w64.org/ or via [MSYS2](https://www.msys2.org/) |
| **Node.js** | ≥ 18 | https://nodejs.org/ |
| **npm** | ≥ 9 (bundled with Node) | — |

Verify your installations:

```powershell
g++ --version
node --version
npm --version
```

---

## Option A — Development Mode (Recommended for demos)

> Two servers run simultaneously. Vite **proxies** all `/api` calls to the C++ backend — no CORS issues.

### One-command launch (double-click or run in terminal):

```bat
start-dev.bat
```

**What it does:**
1. Compiles the C++ backend → `bin/GridGuardServer.exe`
2. Installs npm packages (first run only)
3. Starts the **C++ backend** in a new window on `http://localhost:8081`
4. Starts the **Vite dev server** on `http://localhost:5173`

### Open in browser:

```
http://localhost:5173
```

The dashboard will show **"Backend Connected"** in the sidebar when the C++ server is reachable.

---

## Option B — Manual Step-by-Step

### Step 1 — Compile the C++ backend

```powershell
cd "c:\Users\HP\OneDrive\Desktop\dsa-project"

g++ -std=c++17 -O2 -Wall -Iinclude `
    src/Graph.cpp `
    src/RoadNetwork.cpp `
    src/Simulation.cpp `
    src/Sorting.cpp `
    src/GridGuardServer.cpp `
    src/server_main.cpp `
    -o bin/GridGuardServer.exe `
    -lws2_32
```

### Step 2 — Start the backend

```powershell
bin\GridGuardServer.exe
```

You should see:
```
[main] Creating Simulation...
[main] Simulation created.
[GridGuard Server] Listening on port 8081
GridGuard Backend Server running at http://localhost:8081
```

### Step 3 — Start the frontend (in a second terminal)

```powershell
cd frontend
npm install      # only needed the first time
npm run dev
```

Vite starts at `http://localhost:5173`.

### Step 4 — Open the dashboard

Navigate to **http://localhost:5173** in your browser.

---

## Option C — Production Build (Single server, single port)

Build the React app into static files and let the C++ server serve everything on **port 8081** only.

```bat
build-production.bat
```

Then run just the backend:

```powershell
bin\GridGuardServer.exe
```

Open **http://localhost:8081** — the C++ server now serves the React app AND the API.

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────┐
│                   Browser                           │
│         http://localhost:5173  (dev)                │
│         http://localhost:8081  (prod)               │
└──────────────┬──────────────────────────────────────┘
               │
     ┌─────────▼──────────┐
     │   Vite Dev Server  │  (dev mode only)
     │   localhost:5173   │
     │                    │
     │  /api/* → proxy ───┼──────────────────────────┐
     └────────────────────┘                           │
                                                      ▼
                                         ┌────────────────────────┐
                                         │  GridGuardServer.exe   │
                                         │  C++ HTTP Server       │
                                         │  localhost:8081        │
                                         │                        │
                                         │  GET  /api/health      │
                                         │  GET  /api/network-state│
                                         │  POST /api/simulate-failure│
                                         │  POST /api/process-repair  │
                                         │  POST /api/undo        │
                                         │  GET  /*  → frontend/dist/ │
                                         │            (prod only) │
                                         │                        │
                                         │  ┌──────────────────┐  │
                                         │  │  Simulation.cpp  │  │
                                         │  │  Graph.cpp (BFS) │  │
                                         │  │  RoadNetwork.cpp │  │
                                         │  │  (Dijkstra)      │  │
                                         │  │  Sorting.cpp     │  │
                                         │  └──────────────────┘  │
                                         └────────────────────────┘
```

---

## Available API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/health` | Backend health check |
| GET | `/api/network-state` | Full grid state (locations, lines, faults, incidents) |
| POST | `/api/simulate-failure` | Simulate power line failure |
| POST | `/api/process-repair` | Process next repair from heap |
| POST | `/api/undo` | Undo last operation (uses C++ Stack) |

### Example — health check:
```powershell
Invoke-RestMethod http://localhost:8081/api/health
```

### Example — simulate failure:
```powershell
Invoke-RestMethod -Method POST -Uri http://localhost:8081/api/simulate-failure `
    -ContentType "application/json" `
    -Body '{"lineId": 5, "severity": 3, "durationMinutes": 60}'
```

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| `g++ not found` | Install MinGW-w64 and add `C:\mingw64\bin` to your PATH |
| `node not found` | Install Node.js from https://nodejs.org |
| Backend shows "FAILED: WSAStartup" | Run as normal user (not admin) or restart |
| Frontend shows "Backend Offline" | Make sure `bin\GridGuardServer.exe` is running first |
| Port 8081 already in use | Kill the previous server: `netstat -ano \| findstr 8081` then `taskkill /PID <pid> /F` |
| Port 5173 already in use | Vite will auto-select the next available port — check the terminal output |
| CORS errors in browser | Make sure you're using the Vite dev server (port 5173), not opening index.html directly |

---

## Build Outputs

| File | Description |
|------|-------------|
| `bin/GridGuard.exe` | Console DSA simulation (original CLI) |
| `bin/GridGuardServer.exe` | HTTP API server + static file server |
| `bin/test_suite.exe` | Automated DSA test suite |
| `frontend/dist/` | Bundled React dashboard (production) |

---

## Running the DSA Console App

The original console-based DSA application is still fully functional:

```powershell
g++ -std=c++17 -Wall -Iinclude main.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/GridGuard.exe
bin\GridGuard.exe
```

---

## Running Tests

```powershell
g++ -std=c++17 -Wall -Iinclude tests/test_suite.cpp src/Graph.cpp src/RoadNetwork.cpp src/Simulation.cpp src/Sorting.cpp -o bin/test_suite.exe
bin\test_suite.exe
```

Expected: **10/10 tests PASS**
