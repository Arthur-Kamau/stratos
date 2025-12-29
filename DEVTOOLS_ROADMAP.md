# Stratos DevTools Roadmap

Inspired by [Dart DevTools](https://dart.dev/tools/dart-devtools), this roadmap outlines the implementation of comprehensive developer tools for Stratos.

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    Stratos DevTools UI                       │
│                   (Web-based Interface)                      │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐  │
│  │ Debugger │  Memory  │ Network  │  Logging │Performance│  │
│  │   View   │   View   │   View   │   View   │   View    │  │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ▲
                            │ WebSocket / HTTP
                            │ (JSON-RPC Protocol)
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                  DevTools Server (C++)                       │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Debug Engine  │  Memory Profiler  │  Logger  │  Net │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ▲
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│              Stratos Interpreter/Runtime                     │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Instrumented Execution  │  GC  │  Native Registry  │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Phase 1: Foundation (P0 - Critical)

### 1.1 Core Infrastructure
- [ ] **DevTools Protocol Design**
  - Define JSON-RPC 2.0 message format
  - Specify event types and command structure
  - Version protocol for backward compatibility

- [ ] **DevTools Server Implementation**
  - WebSocket server using libwebsockets or similar
  - Message routing and handler registration
  - Connection management (multiple clients)
  - File: `interpreter/C++/include/stratos/DevToolsServer.h`
  - File: `interpreter/C++/src/devtools/DevToolsServer.cpp`

- [ ] **DevTools UI Scaffold**
  - HTML/CSS/JavaScript single-page app
  - Tab-based interface (Debugger, Memory, Network, Logging, Performance)
  - WebSocket client connection
  - Directory: `devtools/ui/`

- [ ] **Launch Integration**
  - Add `--devtools` flag to stratos command
  - Auto-open browser when DevTools enabled
  - Connection status indicator

### 1.2 Logging Framework (Simplest to implement first)
- [ ] **Logging Infrastructure**
  - Log levels: DEBUG, INFO, WARN, ERROR, FATAL
  - Structured log entries (timestamp, level, message, source location)
  - Thread-safe logging queue
  - File: `interpreter/C++/include/stratos/Logger.h`

- [ ] **Logging View UI**
  - Real-time log stream display
  - Filter by level, search by text
  - Color-coded by severity
  - Timestamp formatting
  - Source location links

- [ ] **Native Logging API**
  - Add `log()` function to standard library
  - Support for log levels: `log.debug()`, `log.info()`, etc.
  - Automatic source location capture

## Phase 2: Debugger (P1 - High Priority)

### 2.1 Debug Engine Core
- [ ] **Breakpoint System**
  - Line-based breakpoints
  - Conditional breakpoints
  - Breakpoint hit counts
  - File: `interpreter/C++/include/stratos/DebugEngine.h`

- [ ] **Execution Control**
  - Pause/Resume execution
  - Step Over (execute next statement)
  - Step Into (enter function calls)
  - Step Out (exit current function)
  - Continue (run until next breakpoint)

- [ ] **Debug State**
  - Current execution position (file, line, column)
  - Breakpoint management
  - Debug session lifecycle

### 2.2 Runtime Inspection
- [ ] **Call Stack Tracking**
  - Function call stack with source locations
  - Stack frame navigation
  - Local variables per frame

- [ ] **Variable Inspection**
  - View all variables in current scope
  - Nested object exploration
  - Type information display
  - Value formatting (primitives, objects, arrays)

- [ ] **Watch Expressions**
  - User-defined expressions
  - Auto-evaluation on each pause
  - Expression history

- [ ] **Expression Evaluation**
  - REPL in debug context
  - Evaluate arbitrary expressions
  - Modify variables during debugging

### 2.3 Debugger UI
- [ ] **Source Code View**
  - Syntax highlighting
  - Current line indicator
  - Breakpoint gutter
  - Click-to-add breakpoints

- [ ] **Call Stack Panel**
  - Stack frames list
  - Click to navigate frames
  - Show function name, file, line

- [ ] **Variables Panel**
  - Tree view of variables
  - Expandable objects
  - Value editing

- [ ] **Watch Panel**
  - Add/remove watch expressions
  - Show current values
  - Error indication for invalid expressions

- [ ] **Debug Controls**
  - Pause/Resume button
  - Step Over/Into/Out buttons
  - Restart button
  - Keyboard shortcuts

## Phase 3: Memory View (P1 - High Priority)

### 3.1 Memory Profiling Infrastructure
- [ ] **Memory Statistics Collection**
  - Track total allocations/deallocations
  - Monitor heap size over time
  - Object count by type
  - File: `interpreter/C++/include/stratos/MemoryProfiler.h`

- [ ] **GC Statistics**
  - Collection count and timing
  - Cycles broken
  - Objects freed per collection
  - Pause time metrics

- [ ] **Heap Snapshots**
  - Capture complete heap state
  - Object graph with references
  - Retained size calculation
  - Snapshot comparison (diff)

- [ ] **Allocation Tracking**
  - Record allocation stack traces
  - Group allocations by type/location
  - Allocation timeline

### 3.2 Memory View UI
- [ ] **Overview Dashboard**
  - Total memory usage graph
  - Heap size over time
  - Object count by type (pie chart)
  - GC activity timeline

- [ ] **Heap Snapshot View**
  - Object tree/list
  - Retained size visualization
  - Reference path to GC roots
  - Snapshot comparison tool

- [ ] **Allocation Profile**
  - Top allocating functions
  - Allocation call trees
  - Allocation timeline

- [ ] **GC Stats Panel**
  - Collection frequency
  - Pause times (min/max/avg)
  - Cycles broken history
  - Memory freed per collection

## Phase 4: Network View (P2 - Medium Priority)

### 4.1 Network Instrumentation
- [ ] **HTTP Client Wrapper**
  - Intercept HTTP requests/responses
  - Record timing (DNS, connect, send, wait, receive)
  - Capture headers and body
  - File: `interpreter/C++/include/stratos/NetworkMonitor.h`

- [ ] **WebSocket Monitoring**
  - Track connections/disconnections
  - Message frame capture
  - Handshake details

- [ ] **Request Recording**
  - Request/response pairs
  - Status codes
  - Content types
  - Payload sizes

### 4.2 Network View UI
- [ ] **Request List**
  - Table of all requests
  - Filter by status, type, URL
  - Search functionality
  - Timeline view

- [ ] **Request Details**
  - Headers (request/response)
  - Body preview (JSON, text, HTML)
  - Timing breakdown waterfall
  - cURL command export

- [ ] **WebSocket Panel**
  - Connection list
  - Message stream
  - Message filtering
  - Send test messages

## Phase 5: Performance View (P2 - Medium Priority)

### 5.1 Performance Profiling
- [ ] **CPU Profiler**
  - Sample-based profiling
  - Function execution time
  - Call count tracking
  - File: `interpreter/C++/include/stratos/Profiler.h`

- [ ] **Timeline Recording**
  - Event markers
  - Function enter/exit events
  - GC events
  - User-defined markers

- [ ] **Hot Path Detection**
  - Identify performance bottlenecks
  - Top time-consuming functions
  - Call graph analysis

### 5.2 Performance View UI
- [ ] **Flame Graph**
  - Interactive flame chart
  - Zoom and filter
  - Function details on hover
  - Time percentage display

- [ ] **Timeline View**
  - Horizontal timeline
  - Event tracks (CPU, GC, User)
  - Zoom controls
  - Event details panel

- [ ] **Call Tree View**
  - Hierarchical call structure
  - Self time vs total time
  - Call count
  - Sort by various metrics

- [ ] **Function Table**
  - List of all functions
  - Self/total time
  - Call count
  - Sort and filter

## Phase 6: Advanced Features (P3 - Nice to Have)

### 6.1 Advanced Debugging
- [ ] **Exception Breakpoints**
  - Break on all exceptions
  - Break on specific exception types
  - Uncaught exception tracking

- [ ] **Data Breakpoints**
  - Break on variable modification
  - Break on object field changes

- [ ] **Hot Reload**
  - Reload code without restarting
  - Preserve application state
  - Show reload status

### 6.2 Testing Integration
- [ ] **Test Runner View**
  - Run tests from UI
  - Test results visualization
  - Coverage reporting

### 6.3 Code Metrics
- [ ] **Code Coverage**
  - Line coverage
  - Branch coverage
  - Coverage visualization in source

- [ ] **Code Complexity**
  - Cyclomatic complexity
  - Function size metrics
  - Dependency analysis

## Protocol Specification

### Message Format (JSON-RPC 2.0)

**Request:**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "Debugger.setBreakpoint",
  "params": {
    "file": "main.st",
    "line": 42
  }
}
```

**Response:**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "breakpointId": "bp-1",
    "actualLine": 42
  }
}
```

**Event (Notification):**
```json
{
  "jsonrpc": "2.0",
  "method": "Debugger.paused",
  "params": {
    "reason": "breakpoint",
    "file": "main.st",
    "line": 42,
    "callStack": [...]
  }
}
```

### API Domains

1. **Debugger Domain**
   - `Debugger.enable` - Enable debugging
   - `Debugger.disable` - Disable debugging
   - `Debugger.setBreakpoint` - Set a breakpoint
   - `Debugger.removeBreakpoint` - Remove a breakpoint
   - `Debugger.pause` - Pause execution
   - `Debugger.resume` - Resume execution
   - `Debugger.stepOver` - Step over
   - `Debugger.stepInto` - Step into
   - `Debugger.stepOut` - Step out
   - `Debugger.evaluateExpression` - Evaluate expression
   - `Debugger.getVariables` - Get variables in scope
   - Event: `Debugger.paused` - Execution paused
   - Event: `Debugger.resumed` - Execution resumed

2. **Memory Domain**
   - `Memory.enable` - Enable memory tracking
   - `Memory.disable` - Disable memory tracking
   - `Memory.getStats` - Get current memory stats
   - `Memory.takeHeapSnapshot` - Capture heap snapshot
   - `Memory.getObjectById` - Get object details
   - Event: `Memory.statsUpdated` - Periodic stats update
   - Event: `Memory.gcPerformed` - GC occurred

3. **Log Domain**
   - `Log.enable` - Enable log capture
   - `Log.disable` - Disable log capture
   - `Log.clear` - Clear log history
   - Event: `Log.entryAdded` - New log entry

4. **Network Domain**
   - `Network.enable` - Enable network monitoring
   - `Network.disable` - Disable network monitoring
   - `Network.getRequest` - Get request details
   - Event: `Network.requestStarted` - Request initiated
   - Event: `Network.responseReceived` - Response received
   - Event: `Network.requestFinished` - Request completed

5. **Profiler Domain**
   - `Profiler.enable` - Enable CPU profiling
   - `Profiler.disable` - Disable CPU profiling
   - `Profiler.start` - Start profiling session
   - `Profiler.stop` - Stop profiling session
   - `Profiler.getProfile` - Get profile data

## File Structure

```
stratos/
├── devtools/
│   ├── ui/
│   │   ├── index.html
│   │   ├── css/
│   │   │   ├── main.css
│   │   │   ├── debugger.css
│   │   │   ├── memory.css
│   │   │   ├── network.css
│   │   │   └── logging.css
│   │   ├── js/
│   │   │   ├── main.js
│   │   │   ├── connection.js
│   │   │   ├── debugger.js
│   │   │   ├── memory.js
│   │   │   ├── network.js
│   │   │   ├── logging.js
│   │   │   └── profiler.js
│   │   └── lib/
│   │       ├── chart.js
│   │       ├── codemirror/
│   │       └── d3.js
│   └── protocol.md
├── interpreter/C++/
│   ├── include/stratos/
│   │   ├── DevToolsServer.h
│   │   ├── DebugEngine.h
│   │   ├── Logger.h
│   │   ├── MemoryProfiler.h
│   │   ├── NetworkMonitor.h
│   │   └── Profiler.h
│   └── src/devtools/
│       ├── DevToolsServer.cpp
│       ├── DebugEngine.cpp
│       ├── Logger.cpp
│       ├── MemoryProfiler.cpp
│       ├── NetworkMonitor.cpp
│       └── Profiler.cpp
└── docs/
    └── DEVTOOLS.md
```

## Dependencies

### C++ Side
- **libwebsockets** - WebSocket server
- **nlohmann/json** - JSON parsing
- **spdlog** - Logging infrastructure (optional)

### UI Side
- **CodeMirror** - Code editor with syntax highlighting
- **Chart.js** or **D3.js** - Data visualization
- **Bootstrap** or **Tailwind CSS** - UI styling
- Plain JavaScript or **Vue.js** / **React** - UI framework

## Development Phases

### Milestone 1: Foundation + Logging (2-3 weeks)
- DevTools server infrastructure
- Basic UI scaffold
- Logging framework
- Logging View

### Milestone 2: Debugger (3-4 weeks)
- Breakpoint system
- Step debugging
- Call stack and variables
- Debugger UI

### Milestone 3: Memory View (2-3 weeks)
- Memory profiler
- GC statistics
- Heap snapshots
- Memory View UI

### Milestone 4: Network + Performance (2-3 weeks)
- Network monitoring
- Network View UI
- CPU profiler
- Performance View UI

### Milestone 5: Polish + Documentation (1-2 weeks)
- Bug fixes
- Performance optimization
- User documentation
- Example projects

## Success Metrics

1. **Debugger**: Can set breakpoints, step through code, inspect variables
2. **Memory View**: Can detect memory leaks, track GC performance
3. **Network View**: Can monitor HTTP requests, view timing
4. **Logging View**: Can filter and search logs efficiently
5. **Performance**: DevTools overhead < 10% when enabled
6. **Usability**: Clear documentation, intuitive UI

## Notes

- Start with Logging View (simplest) to establish patterns
- Debugger is most complex - build incrementally
- Memory View can leverage existing GC infrastructure
- Network View requires HTTP client library integration
- Consider TypeScript for UI if complexity grows
- Protocol should be versioned for backward compatibility
- Support remote debugging (connect to running process)
- Consider embedding UI in binary (using resource compiler)
