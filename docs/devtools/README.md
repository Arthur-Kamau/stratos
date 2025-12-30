# Stratos DevTools

Web-based development tools for debugging and profiling Stratos applications.

## Current Status

### ✅ Implemented
- **Logging Framework** (C++)
  - Structured logging with log levels (DEBUG, INFO, WARN, ERROR, FATAL)
  - Multiple sinks (Console with colors, Buffer for DevTools)
  - Thread-safe operation
  - Millisecond-precision timestamps
  - Source location tracking

- **Logging View** (UI)
  - Real-time log display
  - Filter by level
  - Search functionality
  - Log details panel
  - Export logs to JSON
  - Color-coded by severity
  - Demo mode (works without backend)

### 🚧 In Progress
- WebSocket server for DevTools protocol
- Log streaming from interpreter to UI

### 📋 Planned
- Debugger View (breakpoints, stepping, variables)
- Memory View (heap analysis, GC stats)
- Network View (HTTP monitoring)
- Performance View (CPU profiling, flame graphs)

## Usage

### Running the UI

1. Open `devtools/ui/index.html` in a web browser
2. The UI will attempt to connect to `ws://localhost:9222`
3. If no backend is available, demo logs will be displayed

### Using Logging in Stratos Code

```stratos
package main;

use log;

fn main() {
    log.info("Application starting");
    log.debug("Debug information");
    log.warn("Warning message");
    log.error("Error occurred");
    log.fatal("Fatal error - will exit");
}
```

### Logging from C++ Code

```cpp
#include "stratos/Logger.h"

// Using macros (automatic source location)
STRATOS_LOG_INFO("Server started");
STRATOS_LOG_DEBUG("Connection established");
STRATOS_LOG_WARN("High memory usage");
STRATOS_LOG_ERROR("Failed to connect");

// Or direct API
Logger::instance().info("Message here");
```

## Architecture

```
┌─────────────────────────────────────┐
│      DevTools UI (Browser)          │
│  ┌────────────────────────────────┐ │
│  │ Logging │ Memory │ Network... │ │
│  └────────────────────────────────┘ │
└─────────────────────────────────────┘
                 │
                 │ WebSocket (JSON-RPC)
                 │ ws://localhost:9222
                 ▼
┌─────────────────────────────────────┐
│    DevTools Server (TODO)           │
│  - WebSocket handler                │
│  - JSON-RPC protocol                │
│  - Event streaming                  │
└─────────────────────────────────────┘
                 │
                 │
                 ▼
┌─────────────────────────────────────┐
│    Stratos Interpreter               │
│  - Logger (BufferSink)              │
│  - GarbageCollector                 │
│  - NativeRegistry                   │
└─────────────────────────────────────┘
```

## File Structure

```
devtools/
├── README.md              # This file
├── PROTOCOL.md            # JSON-RPC protocol specification
├── UI_MOCKUPS.md          # UI design mockups
└── ui/
    ├── index.html         # Main UI page
    ├── css/
    │   ├── main.css       # Core styles
    │   └── logging.css    # Logging view styles
    └── js/
        ├── main.js        # Tab switching, WebSocket connection
        └── logging.js     # Logging view logic
```

## Next Steps

1. **Implement DevTools Server (C++)**
   - WebSocket server (libwebsockets or similar)
   - JSON-RPC message handling
   - Log event streaming

2. **Connect Logger to DevTools**
   - Create DevToolsSink that sends logs via WebSocket
   - Implement Log domain protocol methods

3. **Add More Views**
   - Memory profiling integration
   - Debugger with breakpoints
   - Network monitoring
   - Performance profiling

## Development

### Testing the UI Locally

Simply open `ui/index.html` in a browser. Demo logs will appear after 1 second.

### Customizing Styles

- Light/Dark theme support (follows system preference)
- Edit `css/main.css` for theme colors
- Edit `css/logging.css` for logging-specific styles

### Adding New Views

1. Add tab button in `index.html`
2. Add tab pane in `index.html`
3. Create CSS file in `css/`
4. Create JavaScript file in `js/`
5. Link files in `index.html`

## Browser Support

- Chrome/Edge 90+
- Firefox 88+
- Safari 14+

## License

Same as Stratos project license.
