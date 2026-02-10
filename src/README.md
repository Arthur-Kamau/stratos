# Stratos Source Code

## Directory Structure

- **interpreter/cpp/** - C++ interpreter implementation
- **compiler/** - Future LLVM-based compiler
- **devtools/** - DevTools web UI and backend


## Building the Compiler

### Prerequisites

Ensure you have the following installed:
- **C++ compiler** (g++ 9+ or clang 10+)
- **CMake** 3.15 or later
- **LLVM** 14 or later (optional, for LLVM IR generation)
- **Git**

### System Dependencies (Ubuntu/Debian)

Install the required development libraries:

```bash
sudo apt install libpq-dev libmysqlclient-dev libhiredis-dev
```

| Package | Description |
|---------|-------------|
| `libpq-dev` | PostgreSQL client library |
| `libmysqlclient-dev` | MySQL client library |
| `libhiredis-dev` | Redis client library |

### Interpreter

```bash
cd src/
./build.sh
```

### Basic Usage
- Running

```bash
./build/stratos run path/to/program.st
```

- With DevTools

```bash
./build/stratos run --devtools path/to/program.st
# Open http://localhost:9222 in your browser
```

## DevTools

Stratos includes a built-in browser-based DevTools system. Launch any program with the `--devtools` flag and open http://localhost:9222 to access five integrated views:

| View | Description |
|------|-------------|
| **Logging** | Real-time structured logs with level filtering, search, and export |
| **Memory** | Memory usage dashboard, object count, GC history with pause times |
| **Debugger** | Set breakpoints, step through code, inspect call stack and variables |
| **Network** | Monitor HTTP requests with headers, response bodies, and timing |
| **Performance** | Record CPU profiles, view flame graphs, and analyze function timing |

### Example: Debugging a Program

```bash
./build/stratos run --devtools tests/devtools/debugger_demo.st
```

Then open http://localhost:9222, switch to the **Debugger** tab, select a source file, click the gutter to set breakpoints, and use F10/F11 to step through your code.

### Example: Profiling Performance

```bash
./build/stratos run --devtools tests/devtools/profiler_demo.st
```

Open the **Performance** tab, click **Record**, let the program run, then click **Stop** to see the flame graph and function-level timing.

### Using Logging in Code

```stratos
package main;

use log;

fn main() {
    log.info("Application starting");
    log.debug("Debug details");
    log.warn("Warning message");
    log.error("Error occurred");
}
```

See `docs/devtools/README.md` for full documentation including the JSON-RPC protocol, architecture, and all available features.

### Manual Build (Alternative)

If you prefer to build manually:

1.  **Navigate to the C++ directory:**
    ```bash
    cd interpreter/C++
    mkdir -p build
    cd build
    ```
2.  **Configure CMake:**
    ```bash
    cmake ..
    ```
3.  **Build the executable:**
    ```bash
    cmake --build .
    ```


### Making the Binary Globally Available (For Developers)

During development, you can make `stratos` available globally so you don't need to type the full path:

```bash
# Create symlink (run from project root)
mkdir -p ~/bin
ln -sf "$(pwd)/build/stratos" ~/bin/stratos

# Add to PATH (add this to ~/.bashrc or ~/.zshrc)
export PATH="$HOME/bin:$PATH"

# Reload shell
source ~/.bashrc  # or ~/.zshrc

# Now you can use 'stratos' from anywhere:
stratos --version
stratos run samples/hello_world/src/main.st
```

**Benefits**: Every time you rebuild, the global `stratos` command automatically uses the new binary.




### Running Tests

From the project root:

```bash
# Run all tests
../../test-all.sh

# Run with verbose output
../../test-all.sh -v

# Run specific test
../../test-all.sh --test hello_world
```