# Stratos Standard Library

A comprehensive standard library for the Stratos programming language, inspired by Go, Kotlin, and TypeScript.

## Overview

The Stratos standard library provides a rich set of modules for common programming tasks, from basic math and string operations to advanced features like concurrent programming, async/await, HTTP servers, and cryptography.

## Module Organization

```
std/
├── math/              # Mathematical functions
├── strings/           # String manipulation
├── io/                # File and stream I/O
├── log/               # Structured logging
├── time/              # Date and time operations
├── collections/       # Data structures (List, Map, Set, Queue, Stack)
├── encoding/
│   ├── json/          # JSON parsing and serialization
│   ├── base64/        # Base64 encoding/decoding
│   └── csv/           # CSV parsing and writing
├── concurrent/        # Goroutines, channels, mutexes
├── async/             # Promises and async/await
├── net/               # TCP/UDP sockets
│   └── http/          # HTTP server and client
├── os/                # Operating system interface
├── crypto/            # Cryptographic functions
└── testing/           # Unit testing framework
```

## Implementation Status

✅ **Completed**:
- 16 core modules with full API definitions
- Hybrid native/pure implementation design
- Comprehensive examples (6 example programs)
- Design documentation

⏳ **Pending**:
- Native function bindings in C++ interpreter
- Runtime type system for generics
- Full async/await compiler support

## Quick Start

See `samples/stdlib_examples/` for complete examples.

For detailed documentation, see `design/standard_library.md`.
