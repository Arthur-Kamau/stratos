# Stratos DevTools

Web-based development tools for debugging, profiling, and monitoring Stratos applications. Inspired by Chrome DevTools and Dart DevTools, Stratos DevTools provides a browser-based UI with five integrated views.

## Quick Start

```bash
# Build the interpreter
cd src && ./build.sh

# Run any Stratos program with DevTools enabled
./build/stratos run --devtools path/to/program.st

# Open the DevTools UI in your browser
# http://localhost:9222
```

The DevTools server starts on port 9222 and serves the built-in web UI directly — no separate web server needed.

## Features

### Logging View
Real-time structured log monitoring with filtering and search.

- Live log streaming from your Stratos program
- Filter by level: DEBUG, INFO, WARN, ERROR, FATAL
- Full-text search across log messages
- Click any log entry to see full details (timestamp, source location, data)
- Export logs to text file
- Color-coded severity levels

### Memory View
Monitor memory usage and garbage collection activity.

- Dashboard with current memory usage, object count, GC collections, and average pause time
- Visual memory usage bar
- GC history table with per-collection stats (freed objects, cycles broken, pause time)
- Color-coded pause times (green < 5ms, yellow < 20ms, red > 20ms)
- Auto-refresh every 2 seconds when enabled

### Debugger View
Full step-through debugger with breakpoints, call stack, and variable inspection.

- **Source display** with line numbers and syntax highlighting (keywords, strings, numbers, comments)
- **Breakpoints**: Click the gutter to toggle breakpoints on any line
- **Stepping**: Step Over (F10), Step Into (F11), Step Out (Shift+F11)
- **Resume/Pause**: Continue execution (F5) or pause at current line (F6)
- **Call Stack panel**: View the full call stack, click frames to navigate
- **Variables panel**: Inspect local and global variables at the current frame
- **Watch panel**: Add custom expressions to evaluate at each pause
- **Breakpoints panel**: Manage all breakpoints with enable/disable toggles
- **Keyboard shortcuts**: F5 (Resume), F6 (Pause), F9 (Toggle Breakpoint), F10 (Step Over), F11 (Step Into)

### Network View
Monitor HTTP requests made by your Stratos programs.

- Request table showing Method, URL, Status, Size, and Duration
- Filter by HTTP method or search by URL
- Detail panel with Headers, Response body, and Timing breakdown
- Color-coded status codes (green for 2xx, yellow for 3xx, red for 4xx/5xx)
- Timing waterfall visualization
- Auto-refresh every 3 seconds

### Performance View
CPU profiling with flame graph visualization and function-level timing.

- **Recording controls**: Start/Stop profiling sessions
- **Flame Graph**: Visual representation of call stacks and time spent per function
- **Functions Table**: Sortable table with self time, total time, percentage, and call count
- **Call Tree**: Hierarchical view of function calls with timing
- Click column headers to sort by self time, total time, or call count

## Using DevTools in Your Code

### Logging

```stratos
package main;

use log;

fn main() {
    log.info("Application starting");
    log.debug("Debug details here");
    log.warn("Something might be wrong");
    log.error("An error occurred");
}
```

### Debugger

Run your program with `--devtools`, open http://localhost:9222, switch to the Debugger tab, and:

1. Select a source file from the dropdown
2. Click the gutter (left of line numbers) to set breakpoints
3. The program pauses when it hits a breakpoint
4. Use Step Over/Into/Out to walk through code
5. Inspect variables in the right panel

### Memory Profiling

The Memory view activates automatically. Create objects and trigger garbage collection to see stats:

```stratos
package main;

use log;

class Node {
    var value: int;
    var next: Optional<Node>;

    fn init(v: int) {
        val this.value = v;
        val this.next = None;
    }
}

fn main() {
    log.info("Creating objects...");
    var i = 0;
    while i < 200 {
        val node = Node(i);
        i = i + 1;
    }
    log.info("Check Memory tab for GC stats");
}
```

### Network Monitoring

HTTP requests made through the `http` module are automatically captured:

```stratos
package main;

use http;

fn main() {
    val router = http.newRouter();

    router.get("/api/status", (req, res) => {
        res.json({"status": "ok"});
    });

    val server = http.newServer(router);
    server.listen(3000);
}
```

### Performance Profiling

1. Open the Performance tab in DevTools
2. Click **Record**
3. Let your program run (or trigger the code you want to profile)
4. Click **Stop**
5. Examine the flame graph and function table

```stratos
package main;

fn fibonacci(n: int) int {
    if n <= 1 { return n; }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

fn main() {
    // This will show up clearly in the profiler
    val result = fibonacci(20);
    println("Result: " + result);
}
```

## Architecture

```
┌──────────────────────────────────────────────┐
│          DevTools UI (Browser)               │
│  ┌─────────┬────────┬─────────┬───────────┐ │
│  │Debugger │Memory  │Network  │Logging    │ │
│  │         │        │         │           │ │
│  │Source   │Stats   │Requests │Live Logs  │ │
│  │Breaks  │GC Hist │Headers  │Filtering  │ │
│  │Stack   │Usage   │Timing   │Search     │ │
│  │Vars    │        │         │           │ │
│  ├─────────┴────────┴─────────┴───────────┤ │
│  │            Performance                  │ │
│  │  Flame Graph │ Functions │ Call Tree    │ │
│  └─────────────────────────────────────────┘ │
└──────────────────────────────────────────────┘
                    │
                    │ HTTP Polling (JSON-RPC 2.0)
                    │ http://localhost:9222
                    ▼
┌──────────────────────────────────────────────┐
│         DevTools Server (C++)                │
│  ┌──────────────────────────────────────┐   │
│  │ JSON-RPC Handler + Static File Server│   │
│  ├──────────────────────────────────────┤   │
│  │  Domains:                            │   │
│  │  - Log.*        (enable, disable)    │   │
│  │  - Memory.*     (stats, GC history)  │   │
│  │  - Debugger.*   (breakpoints, step)  │   │
│  │  - Network.*    (requests, detail)   │   │
│  │  - Profiler.*   (start, stop, data)  │   │
│  └──────────────────────────────────────┘   │
└──────────────────────────────────────────────┘
                    │
                    ▼
┌──────────────────────────────────────────────┐
│         Stratos Interpreter                  │
│  ┌────────────┐ ┌──────────────────────────┐│
│  │ Logger     │ │ DebugEngine              ││
│  │ (sinks)    │ │ (breakpoints, stepping)  ││
│  ├────────────┤ ├──────────────────────────┤│
│  │ MemoryProf │ │ NetworkMonitor           ││
│  │ (GC stats) │ │ (HTTP interception)      ││
│  ├────────────┤ ├──────────────────────────┤│
│  │ GC         │ │ Profiler                 ││
│  │ (mark&sweep│ │ (function timing)        ││
│  └────────────┘ └──────────────────────────┘│
└──────────────────────────────────────────────┘
```

## Protocol

Communication uses JSON-RPC 2.0 over HTTP:

- **Requests**: `POST http://localhost:9222/` with JSON-RPC body
- **Events**: `GET http://localhost:9222/events` (polling at 100ms)
- **UI Files**: `GET http://localhost:9222/{path}` (static file serving)

### Domains

| Domain | Methods |
|--------|---------|
| **Log** | `enable`, `disable`, `clear` |
| **Memory** | `enable`, `disable`, `getStats`, `getGCHistory` |
| **Debugger** | `enable`, `disable`, `setBreakpoint`, `removeBreakpoint`, `getBreakpoints`, `pause`, `resume`, `stepOver`, `stepInto`, `stepOut`, `getCallStack`, `getVariables`, `evaluateOnFrame`, `getSource`, `getSourceFiles`, `getState` |
| **Network** | `enable`, `disable`, `getRequests`, `getRequestDetail`, `clear` |
| **Profiler** | `enable`, `disable`, `start`, `stop`, `getProfile` |

### Events

| Event | Description |
|-------|-------------|
| `Log.entryAdded` | New log entry from the program |
| `Memory.gcPerformed` | Garbage collection completed |
| `Debugger.paused` | Execution paused (breakpoint/step) |
| `Debugger.resumed` | Execution resumed |
| `Network.requestStarted` | HTTP request initiated |
| `Network.requestCompleted` | HTTP response received |

## File Structure

```
devtools/
└── ui/
    ├── index.html             # Main SPA (5 tabs)
    ├── css/
    │   ├── main.css           # Core layout, themes, shared styles
    │   ├── logging.css        # Logging view styles
    │   ├── memory.css         # Memory dashboard styles
    │   ├── debugger.css       # Debugger split-pane layout
    │   ├── network.css        # Network request table
    │   └── performance.css    # Flame graph, profiler tables
    └── js/
        ├── main.js            # Connection manager, HTTP polling, tabs
        ├── logging.js         # Log display, filtering, export
        ├── memory.js          # Memory stats, GC history
        ├── debugger.js        # Source viewer, breakpoints, stepping
        ├── network.js         # Request table, detail panel
        └── performance.js     # Flame graph, recording controls

src/
├── include/stratos/
│   ├── DevToolsServer.h       # HTTP server + JSON-RPC
│   ├── Logger.h               # Logging framework
│   ├── MemoryProfiler.h       # Memory/GC profiling
│   ├── DebugEngine.h          # Debugger engine
│   ├── NetworkMonitor.h       # HTTP request monitoring
│   └── Profiler.h             # CPU profiler
└── src/devtools/
    ├── DevToolsServer.cpp     # Server + all domain handlers
    ├── Logger.cpp             # Logger sinks
    ├── MemoryProfiler.cpp     # GC event tracking
    ├── DebugEngine.cpp        # Breakpoints, stepping, variables
    ├── NetworkMonitor.cpp     # Request interception
    └── Profiler.cpp           # Function timing, flame graph
```

## Demo Programs

Test programs are provided in `tests/devtools/`:

```bash
# Logging demo - produces logs at various levels
./build/stratos run --devtools tests/devtools/logging_demo.st

# Memory demo - creates objects to trigger GC
./build/stratos run --devtools tests/devtools/memory_demo.st

# Debugger demo - set breakpoints and step through functions
./build/stratos run --devtools tests/devtools/debugger_demo.st

# Network demo - HTTP server with monitorable endpoints
./build/stratos run --devtools tests/devtools/network_demo.st

# Profiler demo - CPU-intensive work for flame graph capture
./build/stratos run --devtools tests/devtools/profiler_demo.st
```

## Theming

The UI supports light and dark themes. Click the theme toggle button in the top-right corner. The theme preference is saved to localStorage.

- **Dark theme** (default): VS Code-inspired dark colors
- **Light theme**: Clean light background with high contrast

## Browser Support

- Chrome/Edge 90+
- Firefox 88+
- Safari 14+
