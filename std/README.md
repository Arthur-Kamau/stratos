# Stratos Standard Library

A comprehensive standard library for the Stratos programming language, inspired by Go, Kotlin, and TypeScript.

## Overview

The Stratos standard library provides a rich set of modules for common programming tasks, from basic math and string operations to advanced features like concurrent programming, async/await, HTTP servers, and cryptography.

## Module Organization

```
std/
├── math/              # Mathematical functions
├── strings/           # String manipulation
├── convert/           # Type conversions and number formatting
├── regex/             # Regular expressions (pattern matching and text manipulation)
├── io/                # File and stream I/O
├── log/               # Structured logging
├── time/              # Date and time operations
├── collections/       # Data structures (List, Map, Set, Queue, Stack)
├── terminal/          # ANSI terminal control and colors
├── encoding/
│   ├── json/          # JSON parsing and serialization
│   ├── base64/        # Base64 encoding/decoding
│   └── csv/           # CSV parsing and writing
├── concurrent/        # Goroutines, channels, mutexes
├── async/             # Promises and async/await
├── net/               # TCP/UDP sockets
│   └── http/          # HTTP server and client
│   └── websocket/     # WebSocket client and server
├── os/                # Operating system interface
├── crypto/            # Cryptographic functions
├── zip/               # Zip compression
├── ffi/               # Foreign Function Interface
└── testing/           # Unit testing framework
```

## Implementation Status

✅ **Completed**:
- 20+ core modules with full API definitions
- Hybrid native/pure implementation design
- Convert module for type conversions and number formatting
- Terminal module for ANSI terminal control
- WebSocket module for real-time communication
- Comprehensive examples (20+ example programs)
- Design documentation

⏳ **Pending**:
- Native function bindings in C++ interpreter
- Runtime type system for generics
- Full async/await compiler support

## Quick Start

See `samples/stdlib_examples/` for complete examples.

For detailed documentation, see `design/standard_library.md`.
