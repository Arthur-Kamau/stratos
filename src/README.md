# Stratos Source Code

## Directory Structure

- **interpreter/cpp/** - C++ interpreter implementation
- **compiler/** - Future LLVM-based compiler
- **devtools/** - DevTools web UI and backend

## Building

### Interpreter

```bash
cd src/interpreter/cpp
./build.sh
```

### Running

```bash
./build/stratos run path/to/program.st
```

### With DevTools

```bash
./build/stratos run --devtools path/to/program.st
```
