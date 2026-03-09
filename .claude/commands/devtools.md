# Stratos DevTools

Work with the Stratos DevTools system — debugging, profiling, network monitoring, and memory inspection.

## Arguments
- $ARGUMENTS: Action or topic (e.g., "inspect", "debug", "profile", "network", "memory", "setup")

## Instructions

The Stratos DevTools system provides Chrome-style developer tools for debugging Stratos programs.

### Architecture

The DevTools consist of:
1. **C++ Backend** (in `src/`):
   - `src/src/devtools/DevToolsServer.cpp` — WebSocket-based DevTools server
   - `src/src/devtools/Logger.cpp` — Structured logging system
   - `src/src/devtools/MemoryProfiler.cpp` — Memory usage tracking
   - `src/src/devtools/DebugEngine.cpp` — Breakpoints, stepping, variable inspection
   - `src/src/devtools/NetworkMonitor.cpp` — HTTP/WebSocket request tracking
   - `src/src/devtools/Profiler.cpp` — Performance profiling

2. **Frontend UI** (in `devtools/ui/`):
   - Chrome DevTools-style web interface
   - Connects to the DevTools server via WebSocket

3. **Headers** (in `src/include/stratos/`):
   - `DevToolsServer.h`, `Logger.h`, `MemoryProfiler.h`, `DebugEngine.h`, `NetworkMonitor.h`, `Profiler.h`

### Actions

Based on $ARGUMENTS:

- **"setup"**: Explain how to enable DevTools when running a Stratos program
- **"debug"**: Read and explain DebugEngine implementation — breakpoints, stepping, variable watches
- **"profile"**: Read and explain Profiler — function timing, hotspot detection
- **"memory"**: Read and explain MemoryProfiler — allocation tracking, GC stats, heap snapshots
- **"network"**: Read and explain NetworkMonitor — HTTP/WS request logging
- **"inspect"**: Read DevToolsServer to explain the WebSocket protocol and available commands
- **"ui"**: Explore the devtools/ui/ directory and explain the frontend

### For Development

When modifying DevTools:
1. Backend changes: modify files in `src/src/devtools/` and headers in `src/include/stratos/`
2. Frontend changes: modify files in `devtools/ui/`
3. Rebuild the compiler with `/build` after backend changes
4. Test with the devtools test suite in `tests/devtools/`
