# Stratos DevTools Protocol Specification

Version: 1.0.0

## Overview

The Stratos DevTools Protocol is a bidirectional communication protocol based on JSON-RPC 2.0, used for communication between the Stratos interpreter and DevTools UI.

## Transport

- **Primary**: WebSocket (ws://localhost:9222)
- **Fallback**: HTTP long-polling
- **Port**: Configurable, default 9222

## Message Format

All messages follow JSON-RPC 2.0 specification.

### Request (Client → Server)

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "Domain.methodName",
  "params": {
    "param1": "value1",
    "param2": "value2"
  }
}
```

### Response (Server → Client)

**Success:**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "data": "value"
  }
}
```

**Error:**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "error": {
    "code": -32601,
    "message": "Method not found",
    "data": {
      "details": "Additional error information"
    }
  }
}
```

### Event (Server → Client)

```json
{
  "jsonrpc": "2.0",
  "method": "Domain.eventName",
  "params": {
    "eventData": "value"
  }
}
```

## Error Codes

Standard JSON-RPC 2.0 codes plus custom codes:

- `-32700`: Parse error
- `-32600`: Invalid request
- `-32601`: Method not found
- `-32602`: Invalid params
- `-32603`: Internal error
- `1000`: Debugger not enabled
- `1001`: Invalid breakpoint
- `1002`: Execution not paused
- `1003`: Invalid frame id
- `1004`: Evaluation failed

## API Domains

## 1. Runtime Domain

General runtime control and information.

### Runtime.enable

Enable runtime events.

**Request:**
```json
{
  "method": "Runtime.enable",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Runtime.getVersion

Get Stratos version information.

**Request:**
```json
{
  "method": "Runtime.getVersion",
  "params": {}
}
```

**Response:**
```json
{
  "result": {
    "version": "0.1.0",
    "protocolVersion": "1.0.0",
    "buildDate": "2025-12-30"
  }
}
```

### Runtime.evaluate

Evaluate expression in global context.

**Request:**
```json
{
  "method": "Runtime.evaluate",
  "params": {
    "expression": "2 + 2"
  }
}
```

**Response:**
```json
{
  "result": {
    "value": {
      "type": "int",
      "value": 4
    }
  }
}
```

## 2. Debugger Domain

Control program execution and set breakpoints.

### Debugger.enable

Enable debugging support.

**Request:**
```json
{
  "method": "Debugger.enable",
  "params": {}
}
```

**Response:**
```json
{
  "result": {
    "debuggerId": "dbg-1"
  }
}
```

### Debugger.disable

Disable debugging support.

**Request:**
```json
{
  "method": "Debugger.disable",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Debugger.setBreakpoint

Set a breakpoint at specified location.

**Request:**
```json
{
  "method": "Debugger.setBreakpoint",
  "params": {
    "file": "main.st",
    "line": 42,
    "column": 0,
    "condition": "x > 10"  // optional
  }
}
```

**Response:**
```json
{
  "result": {
    "breakpointId": "bp-1",
    "actualLocation": {
      "file": "main.st",
      "line": 42,
      "column": 0
    }
  }
}
```

### Debugger.removeBreakpoint

Remove a breakpoint.

**Request:**
```json
{
  "method": "Debugger.removeBreakpoint",
  "params": {
    "breakpointId": "bp-1"
  }
}
```

**Response:**
```json
{
  "result": {}
}
```

### Debugger.pause

Pause execution.

**Request:**
```json
{
  "method": "Debugger.pause",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Debugger.resume

Resume execution.

**Request:**
```json
{
  "method": "Debugger.resume",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Debugger.stepOver

Step over current statement.

**Request:**
```json
{
  "method": "Debugger.stepOver",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Debugger.stepInto

Step into function call.

**Request:**
```json
{
  "method": "Debugger.stepInto",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Debugger.stepOut

Step out of current function.

**Request:**
```json
{
  "method": "Debugger.stepOut",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Debugger.getCallStack

Get current call stack.

**Request:**
```json
{
  "method": "Debugger.getCallStack",
  "params": {}
}
```

**Response:**
```json
{
  "result": {
    "frames": [
      {
        "frameId": "frame-0",
        "functionName": "main",
        "location": {
          "file": "main.st",
          "line": 42,
          "column": 5
        }
      },
      {
        "frameId": "frame-1",
        "functionName": "processData",
        "location": {
          "file": "utils.st",
          "line": 15,
          "column": 10
        }
      }
    ]
  }
}
```

### Debugger.getVariables

Get variables in specified scope.

**Request:**
```json
{
  "method": "Debugger.getVariables",
  "params": {
    "frameId": "frame-0",
    "scopeType": "local"  // "local", "global", "closure"
  }
}
```

**Response:**
```json
{
  "result": {
    "variables": [
      {
        "name": "x",
        "type": "int",
        "value": "42"
      },
      {
        "name": "person",
        "type": "Person",
        "objectId": "obj-123",
        "preview": "{name: \"John\", age: 30}"
      }
    ]
  }
}
```

### Debugger.getObjectProperties

Get properties of an object.

**Request:**
```json
{
  "method": "Debugger.getObjectProperties",
  "params": {
    "objectId": "obj-123"
  }
}
```

**Response:**
```json
{
  "result": {
    "properties": [
      {
        "name": "name",
        "type": "string",
        "value": "John"
      },
      {
        "name": "age",
        "type": "int",
        "value": "30"
      }
    ]
  }
}
```

### Debugger.evaluateOnFrame

Evaluate expression in context of a stack frame.

**Request:**
```json
{
  "method": "Debugger.evaluateOnFrame",
  "params": {
    "frameId": "frame-0",
    "expression": "x * 2"
  }
}
```

**Response:**
```json
{
  "result": {
    "value": {
      "type": "int",
      "value": 84
    }
  }
}
```

### Event: Debugger.paused

Execution paused (breakpoint, step, or manual pause).

```json
{
  "method": "Debugger.paused",
  "params": {
    "reason": "breakpoint",  // "breakpoint", "step", "pause", "exception"
    "location": {
      "file": "main.st",
      "line": 42,
      "column": 5
    },
    "breakpointId": "bp-1",  // if reason is "breakpoint"
    "exception": {           // if reason is "exception"
      "type": "RuntimeError",
      "message": "Division by zero"
    },
    "callStack": [...]
  }
}
```

### Event: Debugger.resumed

Execution resumed.

```json
{
  "method": "Debugger.resumed",
  "params": {}
}
```

### Event: Debugger.scriptParsed

New script/source file parsed.

```json
{
  "method": "Debugger.scriptParsed",
  "params": {
    "scriptId": "script-1",
    "file": "main.st",
    "source": "package main;\n\nfn main() {\n  ...\n}"
  }
}
```

## 3. Memory Domain

Memory profiling and heap analysis.

### Memory.enable

Enable memory tracking.

**Request:**
```json
{
  "method": "Memory.enable",
  "params": {
    "trackAllocations": true,  // optional
    "sampleInterval": 1000     // optional, ms
  }
}
```

**Response:**
```json
{
  "result": {}
}
```

### Memory.getStats

Get current memory statistics.

**Request:**
```json
{
  "method": "Memory.getStats",
  "params": {}
}
```

**Response:**
```json
{
  "result": {
    "totalAllocated": 1048576,
    "currentUsage": 524288,
    "objectCount": 42,
    "objectsByType": {
      "Node": 10,
      "Person": 5,
      "Array": 3
    },
    "gcStats": {
      "collectionsTotal": 5,
      "cyclesBroken": 2,
      "lastCollectionTime": 1735573200000,
      "avgPauseTime": 2.5
    }
  }
}
```

### Memory.takeHeapSnapshot

Capture complete heap snapshot.

**Request:**
```json
{
  "method": "Memory.takeHeapSnapshot",
  "params": {
    "includeReferences": true  // optional
  }
}
```

**Response:**
```json
{
  "result": {
    "snapshotId": "snap-1",
    "timestamp": 1735573200000,
    "totalSize": 524288,
    "objects": [
      {
        "id": "obj-1",
        "type": "Node",
        "size": 64,
        "retainedSize": 128,
        "references": ["obj-2", "obj-3"]
      }
    ]
  }
}
```

### Memory.getHeapSnapshot

Get previously captured snapshot.

**Request:**
```json
{
  "method": "Memory.getHeapSnapshot",
  "params": {
    "snapshotId": "snap-1"
  }
}
```

**Response:**
```json
{
  "result": {
    "snapshot": { /* same as takeHeapSnapshot */ }
  }
}
```

### Memory.compareSnapshots

Compare two heap snapshots.

**Request:**
```json
{
  "method": "Memory.compareSnapshots",
  "params": {
    "baselineId": "snap-1",
    "comparisonId": "snap-2"
  }
}
```

**Response:**
```json
{
  "result": {
    "allocated": [
      {
        "type": "Node",
        "count": 5,
        "size": 320
      }
    ],
    "freed": [
      {
        "type": "Person",
        "count": 2,
        "size": 128
      }
    ],
    "sizeDelta": 192
  }
}
```

### Memory.getRetainerPath

Get path from object to GC root.

**Request:**
```json
{
  "method": "Memory.getRetainerPath",
  "params": {
    "objectId": "obj-123"
  }
}
```

**Response:**
```json
{
  "result": {
    "path": [
      {
        "objectId": "root",
        "type": "GCRoot",
        "name": "global"
      },
      {
        "objectId": "obj-456",
        "type": "Environment",
        "name": "main"
      },
      {
        "objectId": "obj-123",
        "type": "Node",
        "name": "list"
      }
    ]
  }
}
```

### Event: Memory.statsUpdated

Periodic memory statistics update.

```json
{
  "method": "Memory.statsUpdated",
  "params": {
    "timestamp": 1735573200000,
    "currentUsage": 524288,
    "objectCount": 42
  }
}
```

### Event: Memory.gcPerformed

Garbage collection performed.

```json
{
  "method": "Memory.gcPerformed",
  "params": {
    "timestamp": 1735573200000,
    "pauseTime": 2.5,
    "freedBytes": 1024,
    "freedObjects": 5,
    "cyclesBroken": 1
  }
}
```

## 4. Log Domain

Logging and console output.

### Log.enable

Enable log capture.

**Request:**
```json
{
  "method": "Log.enable",
  "params": {
    "minLevel": "debug"  // "debug", "info", "warn", "error", "fatal"
  }
}
```

**Response:**
```json
{
  "result": {}
}
```

### Log.clear

Clear log history.

**Request:**
```json
{
  "method": "Log.clear",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Event: Log.entryAdded

New log entry added.

```json
{
  "method": "Log.entryAdded",
  "params": {
    "entry": {
      "timestamp": 1735573200000,
      "level": "info",
      "message": "Server started on port 8080",
      "source": {
        "file": "server.st",
        "line": 42,
        "function": "start"
      },
      "data": {
        "port": 8080
      }
    }
  }
}
```

## 5. Network Domain

Network request monitoring.

### Network.enable

Enable network monitoring.

**Request:**
```json
{
  "method": "Network.enable",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Network.getRequest

Get details of a specific request.

**Request:**
```json
{
  "method": "Network.getRequest",
  "params": {
    "requestId": "req-123"
  }
}
```

**Response:**
```json
{
  "result": {
    "request": {
      "requestId": "req-123",
      "url": "https://api.example.com/users",
      "method": "GET",
      "headers": {
        "User-Agent": "Stratos/0.1.0"
      },
      "body": null,
      "timestamp": 1735573200000
    },
    "response": {
      "status": 200,
      "statusText": "OK",
      "headers": {
        "Content-Type": "application/json"
      },
      "body": "{\"users\": [...]}",
      "size": 1024,
      "timestamp": 1735573201500
    },
    "timing": {
      "dns": 10.5,
      "connect": 25.3,
      "send": 0.5,
      "wait": 1450.2,
      "receive": 15.8,
      "total": 1502.3
    }
  }
}
```

### Event: Network.requestStarted

HTTP request initiated.

```json
{
  "method": "Network.requestStarted",
  "params": {
    "requestId": "req-123",
    "url": "https://api.example.com/users",
    "method": "GET",
    "headers": {...},
    "timestamp": 1735573200000
  }
}
```

### Event: Network.responseReceived

HTTP response received.

```json
{
  "method": "Network.responseReceived",
  "params": {
    "requestId": "req-123",
    "status": 200,
    "statusText": "OK",
    "headers": {...},
    "timestamp": 1735573201500
  }
}
```

### Event: Network.requestFinished

HTTP request completed.

```json
{
  "method": "Network.requestFinished",
  "params": {
    "requestId": "req-123",
    "size": 1024,
    "timing": {...}
  }
}
```

### Event: Network.requestFailed

HTTP request failed.

```json
{
  "method": "Network.requestFailed",
  "params": {
    "requestId": "req-123",
    "error": "Connection timeout",
    "timestamp": 1735573205000
  }
}
```

## 6. Profiler Domain

CPU and performance profiling.

### Profiler.enable

Enable CPU profiler.

**Request:**
```json
{
  "method": "Profiler.enable",
  "params": {}
}
```

**Response:**
```json
{
  "result": {}
}
```

### Profiler.start

Start profiling session.

**Request:**
```json
{
  "method": "Profiler.start",
  "params": {
    "sampleInterval": 100  // microseconds, optional
  }
}
```

**Response:**
```json
{
  "result": {
    "sessionId": "prof-1"
  }
}
```

### Profiler.stop

Stop profiling session and get results.

**Request:**
```json
{
  "method": "Profiler.stop",
  "params": {
    "sessionId": "prof-1"
  }
}
```

**Response:**
```json
{
  "result": {
    "profile": {
      "startTime": 1735573200000,
      "endTime": 1735573210000,
      "samples": [
        {
          "timestamp": 1735573200100,
          "stack": [
            {
              "function": "main",
              "file": "main.st",
              "line": 42
            },
            {
              "function": "processData",
              "file": "utils.st",
              "line": 15
            }
          ]
        }
      ],
      "summary": {
        "totalSamples": 1000,
        "functionStats": [
          {
            "function": "processData",
            "selfTime": 5000,
            "totalTime": 8000,
            "callCount": 100
          }
        ]
      }
    }
  }
}
```

### Profiler.getProfile

Get previously captured profile.

**Request:**
```json
{
  "method": "Profiler.getProfile",
  "params": {
    "sessionId": "prof-1"
  }
}
```

**Response:**
```json
{
  "result": {
    "profile": { /* same as stop */ }
  }
}
```

## Connection Lifecycle

### 1. Client Connects

```
Client → Server: WebSocket connection
Server → Client: { "method": "Runtime.ready", "params": {} }
```

### 2. Client Enables Domains

```
Client → Server: { "method": "Debugger.enable", ... }
Server → Client: { "result": { "debuggerId": "..." } }

Client → Server: { "method": "Memory.enable", ... }
Server → Client: { "result": {} }
```

### 3. Client Subscribes to Events

Events are automatically sent after domain is enabled.

### 4. Client Disconnects

```
Client → Server: WebSocket close
Server: Cleanup resources
```

## Type Definitions

### Location
```typescript
{
  "file": string,
  "line": number,
  "column": number
}
```

### RemoteValue
```typescript
{
  "type": "int" | "double" | "string" | "bool" | "object" | "void",
  "value"?: string | number | boolean,
  "objectId"?: string,
  "preview"?: string
}
```

### StackFrame
```typescript
{
  "frameId": string,
  "functionName": string,
  "location": Location
}
```

### Variable
```typescript
{
  "name": string,
  "type": string,
  "value"?: string,
  "objectId"?: string,
  "preview"?: string
}
```

## Versioning

Protocol version follows semantic versioning:
- Major: Breaking changes
- Minor: Backward-compatible additions
- Patch: Bug fixes

Clients should check `Runtime.getVersion` on connect.

## Security

- DevTools server binds to localhost by default
- Authentication token support (optional)
- CORS headers for browser access
- TLS support for remote debugging
