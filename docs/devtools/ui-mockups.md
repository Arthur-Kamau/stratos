# Stratos DevTools UI Mockups

This document provides visual mockups and descriptions for each DevTools view.

## Overall Layout

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│  Stratos DevTools                                          Connected • localhost:9222    │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│  [Debugger] [Memory] [Network] [Logging] [Performance]                                  │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                           │
│                          View Content Area                                                │
│                                                                                           │
│                                                                                           │
│                                                                                           │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

## 1. Debugger View

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│  Debugger                                                                                 │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────┬──────────────────────────────────────────────────┐ │
│  │ Source Code                     │ Debug Info                                       │ │
│  │                                 │                                                  │ │
│  │ main.st                         │ ┌─ Call Stack ──────────────────────────────┐  │ │
│  │ ─────────────────────────────── │ │ ▼ main() - main.st:42                     │  │ │
│  │  1  package main;               │ │   processData() - utils.st:15             │  │ │
│  │  2                              │ │   calculateSum() - math.st:8              │  │ │
│  │  3  class Node {                │ └───────────────────────────────────────────┘  │ │
│  │  4    var value: int;           │                                                  │ │
│  │  5    var next: Node;           │ ┌─ Variables ───────────────────────────────┐  │ │
│  │  6  }                           │ │ Local                                      │  │ │
│  │  7                              │ │ ▼ x: int = 42                             │  │ │
│  │  8  fn processData(x: int) {    │ │ ▼ person: Person = {name: "John"...}      │  │ │
│  │  9    val result = x * 2;       │ │   > address: Address                      │  │ │
│  │ 10    println(result);           │ │                                            │  │ │
│  │ 11  }                           │ │ Global                                     │  │ │
│  │ 12                              │ │ ▼ config: Config = {...}                  │  │ │
│  │ 13  fn main() {                 │ └───────────────────────────────────────────┘  │ │
│  │ 14    val a = Node(1);          │                                                  │ │
│  │ 15    val b = Node(2);          │ ┌─ Watch ───────────────────────────────────┐  │ │
│  │ 16                              │ │ + Add expression                           │  │ │
│  │ 17    a.setNext(b);             │ │ x * 2: int = 84                           │  │ │
│  │ 18    b.setNext(a);             │ │ a.value: int = 1                          │  │ │
│  │ 19                              │ └───────────────────────────────────────────┘  │ │
│  │ 20    processData(a.value);     │                                                  │ │
│  │ 21  }                           │ ┌─ Breakpoints ─────────────────────────────┐  │ │
│  │                                 │ │ ☑ main.st:42                              │  │ │
│  │ ● Breakpoint                    │ │ ☑ utils.st:15  (x > 10)                   │  │ │
│  │ → Current line                  │ └───────────────────────────────────────────┘  │ │
│  │                                 │                                                  │ │
│  └─────────────────────────────────┴──────────────────────────────────────────────────┘ │
│  [▶ Resume] [⏸ Pause] [⤴ Step Over] [⤵ Step Into] [⤴ Step Out] [🔄 Restart]          │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### Features:
- **Source pane**: Syntax-highlighted code with line numbers
- **Breakpoint gutter**: Click to toggle breakpoints (●)
- **Current line indicator**: Shows execution position (→)
- **Call stack**: Navigate between stack frames
- **Variables panel**: Inspect local and global variables, expandable objects
- **Watch panel**: User-defined expressions evaluated on each pause
- **Breakpoints panel**: List of all breakpoints with conditions
- **Debug controls**: Resume, pause, step over/into/out, restart

## 2. Memory View

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│  Memory                                                                                   │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│  [Overview] [Snapshots] [Allocations] [GC Stats]                                         │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                           │
│  ┌─ Memory Usage Over Time ────────────────────────────────────────────────────────┐   │
│  │                                                                                    │   │
│  │  512 KB ┤                                                        ╭───╮           │   │
│  │         │                                              ╭────╮    │   │           │   │
│  │  384 KB ┤                                    ╭────╮   │    │    │   │           │   │
│  │         │                          ╭────╮   │    │   │    ╰────╯   │           │   │
│  │  256 KB ┤                ╭────╮   │    │   │    ╰───╯              ╰───        │   │
│  │         │      ╭────╮   │    │   │    ╰───╯                                     │   │
│  │  128 KB ┤╭────╯    ╰───╯    ╰───╯                                               │   │
│  │         │                                                                         │   │
│  │    0 KB └┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴──  │   │
│  │         0s   10s  20s  30s  40s  50s  60s  70s  80s  90s  100s 110s 120s        │   │
│  │                                                                                    │   │
│  │  ▼ GC Collection    ▼ GC Collection          ▼ GC Collection                     │   │
│  └────────────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                           │
│  ┌─ Current Stats ─────────────┬─ Objects by Type ───────────────────────────────┐     │
│  │ Total Allocated: 1.5 MB     │                                                  │     │
│  │ Current Usage:   512 KB     │  Node        ████████████░░░░░░░░  45% (1200)   │     │
│  │ Object Count:    2,847      │  Person      ██████░░░░░░░░░░░░░░  25% (670)    │     │
│  │ GC Collections:  12         │  Array       ████░░░░░░░░░░░░░░░░  15% (427)    │     │
│  │ Cycles Broken:   3          │  String      ██░░░░░░░░░░░░░░░░░░  10% (284)    │     │
│  │ Avg Pause Time:  2.3ms      │  Other       █░░░░░░░░░░░░░░░░░░░   5% (266)    │     │
│  └─────────────────────────────┴──────────────────────────────────────────────────┘     │
│                                                                                           │
│  ┌─ Actions ────────────────────────────────────────────────────────────────────────┐   │
│  │ [📸 Take Snapshot] [🗑️ Force GC] [📊 Compare Snapshots]                         │   │
│  └──────────────────────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### Snapshots Tab:

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│  ┌─ Heap Snapshots ────────────────────────────────────────────────────────────────┐   │
│  │                                                                                    │   │
│  │  Snapshot #1  •  12:34:56  •  512 KB  •  2,847 objects  [📊 Compare] [🗑️ Delete] │   │
│  │  Snapshot #2  •  12:35:12  •  548 KB  •  2,915 objects  [📊 Compare] [🗑️ Delete] │   │
│  │  Snapshot #3  •  12:35:28  •  524 KB  •  2,861 objects  [📊 Compare] [🗑️ Delete] │   │
│  │                                                                                    │   │
│  └────────────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                           │
│  ┌─ Snapshot #2 Details ──────────────────────────────────────────────────────────┐    │
│  │                                                                                   │    │
│  │  [Summary] [Containment] [Dominators]                                            │    │
│  │                                                                                   │    │
│  │  Type         Count    Self Size   Retained Size                                 │    │
│  │  ──────────────────────────────────────────────────────────────────────          │    │
│  │  Node         1,200    76.8 KB     245 KB          ▶                             │    │
│  │  Person         670    42.8 KB     128 KB          ▶                             │    │
│  │  Array          427    27.3 KB      68 KB          ▶                             │    │
│  │  String         284    18.2 KB      18 KB          ▶                             │    │
│  │  Config          45     2.9 KB      12 KB          ▶                             │    │
│  │                                                                                   │    │
│  │  ▼ Node (1,200 objects)                                                          │    │
│  │    @obj-123 • value: 42, next: @obj-456 • 64 bytes                               │    │
│  │    @obj-456 • value: 15, next: @obj-789 • 64 bytes                               │    │
│  │    ...                                                                            │    │
│  │                                                                                   │    │
│  └───────────────────────────────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### Features:
- **Memory timeline**: Real-time memory usage graph
- **GC markers**: Show when garbage collections occurred
- **Object count by type**: Pie chart or bar graph
- **Current stats**: Summary of memory metrics
- **Heap snapshots**: Capture and analyze heap state
- **Snapshot comparison**: Diff between snapshots
- **Retained size**: Show memory held by objects
- **Retainer paths**: Trace references to GC roots

## 3. Network View

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│  Network                                                                                  │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│  [🔍 Filter] [🗑️ Clear] [⏸ Preserve Log]    Type: [All ▾]  Status: [All ▾]             │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                           │
│  Name                          Method  Status  Type          Size    Time   Timeline     │
│  ──────────────────────────────────────────────────────────────────────────────────────  │
│  api.example.com/users         GET     200     json          2.4 KB  1.5s  ▓▓▓▓▓░░      │
│  api.example.com/posts         GET     200     json          5.1 KB  2.3s  ▓▓▓▓▓▓▓░     │
│  cdn.example.com/logo.png      GET     200     image        45.2 KB  0.8s  ▓▓░░░░░░     │
│  api.example.com/data          POST    201     json          1.2 KB  1.9s  ▓▓▓▓▓▓░░     │
│  api.example.com/profile       GET     404     json          0.3 KB  0.5s  ▓░░░░░░░     │
│  ws://live.example.com         WS      101     websocket     -       -     [Active]     │
│                                                                                           │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│  ┌─ Request Details ────────────────────────────────────────────────────────────────┐  │
│  │                                                                                    │  │
│  │  GET https://api.example.com/users                                                │  │
│  │                                                                                    │  │
│  │  [Headers] [Preview] [Response] [Timing]                                          │  │
│  │                                                                                    │  │
│  │  ┌─ Request Headers ─────────────────────────────────────────────────────────┐  │  │
│  │  │ User-Agent: Stratos/0.1.0                                                   │  │  │
│  │  │ Accept: application/json                                                    │  │  │
│  │  │ Authorization: Bearer eyJhbGc...                                            │  │  │
│  │  └─────────────────────────────────────────────────────────────────────────────┘  │  │
│  │                                                                                    │  │
│  │  ┌─ Response Headers ────────────────────────────────────────────────────────┐  │  │
│  │  │ Status: 200 OK                                                              │  │  │
│  │  │ Content-Type: application/json                                              │  │  │
│  │  │ Content-Length: 2457                                                        │  │  │
│  │  │ Cache-Control: max-age=300                                                  │  │  │
│  │  └─────────────────────────────────────────────────────────────────────────────┘  │  │
│  │                                                                                    │  │
│  │  [Copy as cURL] [Copy Response]                                                   │  │
│  │                                                                                    │  │
│  └────────────────────────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### Timing Tab:

```
│  ┌─ Timing Breakdown ────────────────────────────────────────────────────────────────┐│
│  │                                                                                     ││
│  │  DNS Lookup        ████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  10.5ms                   ││
│  │  TCP Connect       ██████████░░░░░░░░░░░░░░░░░░░░░░░░░  25.3ms                   ││
│  │  TLS Handshake     ████████████░░░░░░░░░░░░░░░░░░░░░░░  30.8ms                   ││
│  │  Request Sent      █░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░   0.5ms                   ││
│  │  Waiting (TTFB)    ████████████████████████████████████ 1450.2ms                  ││
│  │  Content Download  ██████████░░░░░░░░░░░░░░░░░░░░░░░░░  15.8ms                   ││
│  │                                                                                     ││
│  │  Total Time: 1,533.1ms                                                             ││
│  │                                                                                     ││
│  └─────────────────────────────────────────────────────────────────────────────────────┘│
```

### Features:
- **Request list**: All HTTP requests with method, status, size, time
- **Timeline bars**: Visual representation of request timing
- **Filtering**: By type, status, URL pattern
- **Request details**: Headers, body, preview, timing waterfall
- **WebSocket tracking**: Active connections and messages
- **Copy as cURL**: Export request for debugging
- **HAR export**: Save network activity

## 4. Logging View

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│  Logging                                                                                  │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│  [🔍 Filter] [🗑️ Clear]  Level: [All ▾] [DEBUG] [INFO] [WARN] [ERROR] [FATAL]          │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                           │
│  Time      Level   Source              Message                                           │
│  ───────────────────────────────────────────────────────────────────────────────────────  │
│  12:34:01  DEBUG   server.st:15        Starting server initialization                    │
│  12:34:02  INFO    server.st:42        Server listening on port 8080                     │
│  12:34:05  INFO    router.st:28        Registered 15 routes                              │
│  12:34:12  WARN    db.st:67            Connection pool at 80% capacity                   │
│  12:34:15  INFO    api.st:103          Processing request: GET /api/users                │
│  12:34:16  ERROR   db.st:142           Query timeout after 5000ms                        │
│  12:34:16  ERROR   api.st:108          Failed to fetch users: Database timeout           │
│  12:34:20  INFO    api.st:103          Processing request: POST /api/users               │
│  12:34:21  INFO    db.st:89            Inserted user: id=42, name="John Doe"             │
│  12:34:21  INFO    api.st:115          Successfully created user 42                      │
│  12:34:30  WARN    cache.st:54         Cache miss for key: user:42                       │
│  12:34:35  DEBUG   metrics.st:22       Memory usage: 512MB / 1024MB (50%)                │
│  12:34:40  FATAL   server.st:215       Unhandled exception: OutOfMemory                  │
│                                                                                           │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│  ┌─ Log Entry Details ──────────────────────────────────────────────────────────────┐  │
│  │                                                                                    │  │
│  │  12:34:16  ERROR   db.st:142                                                       │  │
│  │                                                                                    │  │
│  │  Message: Query timeout after 5000ms                                               │  │
│  │                                                                                    │  │
│  │  Source:                                                                           │  │
│  │    File: db.st                                                                     │  │
│  │    Line: 142                                                                       │  │
│  │    Function: executeQuery()                                                        │  │
│  │                                                                                    │  │
│  │  Data:                                                                             │  │
│  │    {                                                                               │  │
│  │      "query": "SELECT * FROM users WHERE active = true",                           │  │
│  │      "timeout": 5000,                                                              │  │
│  │      "elapsed": 5023                                                               │  │
│  │    }                                                                               │  │
│  │                                                                                    │  │
│  │  Stack Trace:                                                                      │  │
│  │    at executeQuery (db.st:142)                                                     │  │
│  │    at fetchUsers (api.st:106)                                                      │  │
│  │    at handleRequest (router.st:85)                                                 │  │
│  │                                                                                    │  │
│  └────────────────────────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### Color Coding:
- DEBUG: Gray
- INFO: Blue
- WARN: Orange
- ERROR: Red
- FATAL: Dark Red (bold)

### Features:
- **Real-time log stream**: Auto-scrolling log entries
- **Level filtering**: Show/hide by log level
- **Search/filter**: Filter by text, source file, function
- **Source links**: Click to open in debugger
- **Structured data**: JSON preview for log data
- **Stack traces**: For errors and exceptions
- **Timestamps**: Precise timing information
- **Export**: Save logs to file

## 5. Performance View

```
┌─────────────────────────────────────────────────────────────────────────────────────────┐
│  Performance                                                                              │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│  [🔴 Start Recording] [⏹ Stop] [💾 Save Profile] [📁 Load Profile]                      │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│  [Flame Graph] [Call Tree] [Function Table] [Timeline]                                   │
├─────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                           │
│  ┌─ Flame Graph ────────────────────────────────────────────────────────────────────┐  │
│  │                                                                                    │  │
│  │  ┌─────────────────────────────────────── main() ──────────────────────────────┐ │  │
│  │  │ ┌────────────── processData() ──────────────┐ ┌── renderUI() ────┐         │ │  │
│  │  │ │ ┌─ calcSum() ─┐ ┌── formatOutput() ─────┐ │ │ ┌─ drawRect() ─┐ │ ┌─init─┐│ │  │
│  │  │ │ │             │ │ ┌─ sprintf() ─┐       │ │ │ │              │ │ │      ││ │  │
│  │  │ │ │   500ms     │ │ │   200ms    │ 300ms │ │ │ │    400ms     │ │ │ 100ms││ │  │
│  │  │ │ └─────────────┘ │ └────────────┘       │ │ │ └──────────────┘ │ └──────┘│ │  │
│  │  │ │      1000ms     │        500ms          │ │ │      400ms       │         │ │  │
│  │  │ └─────────────────┴───────────────────────┘ └──────────────────────────────┘ │  │
│  │  │                    1500ms                              500ms         100ms    │ │  │
│  │  └────────────────────────────────────────────────────────────────────────────────┘ │  │
│  │                                   2100ms                                           │  │
│  │                                                                                    │  │
│  │  Hover to zoom • Click to focus • Drag to pan                                     │  │
│  └────────────────────────────────────────────────────────────────────────────────────┘  │
│                                                                                           │
│  ┌─ Function Details ───────────────────────────────────────────────────────────────┐  │
│  │ processData() - utils.st:15                                                        │  │
│  │                                                                                    │  │
│  │ Total Time:   1500ms  (71.4%)                                                      │  │
│  │ Self Time:      0ms   ( 0.0%)                                                      │  │
│  │ Call Count:     127                                                                │  │
│  │                                                                                    │  │
│  │ Called by:                                                                         │  │
│  │   main() - 127 calls - 1500ms                                                      │  │
│  │                                                                                    │  │
│  │ Calls:                                                                             │  │
│  │   calcSum() - 127 calls - 500ms                                                    │  │
│  │   formatOutput() - 127 calls - 500ms                                               │  │
│  └────────────────────────────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────────────────┘
```

### Call Tree Tab:

```
│  ┌─ Call Tree ──────────────────────────────────────────────────────────────────────┐  │
│  │                                                                                    │  │
│  │  Function             Self Time   Total Time   Calls   % of Total                 │  │
│  │  ──────────────────────────────────────────────────────────────────────────────   │  │
│  │  ▼ main()                  0ms       2100ms      1       100.0%                   │  │
│  │    ▼ processData()         0ms       1500ms    127        71.4%                   │  │
│  │      ▶ calcSum()         500ms        500ms    127        23.8%                   │  │
│  │      ▼ formatOutput()      0ms        500ms    127        23.8%                   │  │
│  │        ▶ sprintf()       200ms        200ms    127         9.5%                   │  │
│  │    ▶ renderUI()            0ms        500ms      1        23.8%                   │  │
│  │    ▶ init()              100ms        100ms      1         4.8%                   │  │
│  │                                                                                    │  │
│  │  Sort by: [Total Time ▾]                                                          │  │
│  │                                                                                    │  │
│  └────────────────────────────────────────────────────────────────────────────────────┘  │
```

### Features:
- **Flame graph**: Interactive visualization of call stack
- **Call tree**: Hierarchical view of function calls
- **Function table**: Sortable list of all functions
- **Timeline**: Chronological event timeline
- **Zoom/pan**: Navigate large profiles
- **Self vs total time**: Distinguish function overhead
- **Hot spots**: Quickly identify performance bottlenecks
- **Export/import**: Save and share profiles

## Color Scheme

### Light Theme (Default)
- Background: #FFFFFF
- Panel Background: #F5F5F5
- Border: #E0E0E0
- Text: #212121
- Accent: #2196F3

### Dark Theme
- Background: #1E1E1E
- Panel Background: #252526
- Border: #3E3E42
- Text: #CCCCCC
- Accent: #007ACC

### Syntax Highlighting (similar to VS Code)
- Keywords: #0000FF (blue)
- Strings: #A31515 (red)
- Comments: #008000 (green)
- Numbers: #098658 (teal)
- Functions: #795E26 (brown)

## Responsive Design

All views should be responsive and work on screens from 1280px width up to 4K displays.

## Keyboard Shortcuts

- **Debugger**:
  - `F5` - Resume
  - `F8` - Continue (same as resume)
  - `F9` - Toggle breakpoint
  - `F10` - Step over
  - `F11` - Step into
  - `Shift+F11` - Step out
  - `Ctrl+Shift+F5` - Restart

- **General**:
  - `Ctrl+F` - Search/filter
  - `Ctrl+L` - Clear console/logs
  - `Ctrl+K` - Clear all
  - `Ctrl+Shift+P` - Command palette

## Accessibility

- Keyboard navigation for all controls
- Screen reader support
- High contrast mode
- Focus indicators
- ARIA labels

## Browser Support

- Chrome/Edge 90+
- Firefox 88+
- Safari 14+
