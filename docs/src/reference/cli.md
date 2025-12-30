---
title: CLI Reference
description: Complete reference for all Stratos command-line tools and options
---

# Command-Line Interface Reference

The `stratos` command-line tool provides everything you need to build, run, and manage Stratos projects.

## Installation

```bash
# Install Stratos compiler
curl -sSL https://get.stratos-lang.org | sh

# Verify installation
stratos --version
```

## Quick Reference

| Command | Description |
|---------|-------------|
| `stratos <file.st>` | Compile a single file |
| `stratos run <file.st>` | Execute a program directly |
| `stratos build` | Build project from stratos.conf |
| `stratos new <name>` | Create new project |
| `stratos get` | Fetch dependencies |
| `stratos test` | Run test suite |

## Commands

### stratos compile

Compile Stratos source files to LLVM IR or native code.

**Usage:**
```bash
stratos compile <file.st>
stratos compile <directory>
stratos <file.st>              # Shorthand
```

**Options:**
- `-o, --output <file>` - Specify output file path
- `-v, --verbose` - Enable verbose output
- `-r, --run` - Execute after compiling

**Examples:**
```bash
# Compile single file
stratos compile hello.st

# Compile with custom output
stratos compile hello.st -o bin/hello

# Compile and run
stratos compile hello.st --run

# Compile all files in directory
stratos compile src/
```

### stratos run

Execute a Stratos program directly without explicit compilation step.

**Usage:**
```bash
stratos run <file.st> [args...]
```

**Examples:**
```bash
# Run a simple program
stratos run hello.st

# Run with arguments
stratos run calculator.st 10 20

# Run with verbose output
stratos run -v server.st
```

### stratos build

Build a project using configuration from `stratos.conf`.

**Usage:**
```bash
stratos build [project_dir]
```

**Options:**
- `-v, --verbose` - Show detailed build information
- Project directory (optional, defaults to current directory)

**Examples:**
```bash
# Build current project
stratos build

# Build specific project
stratos build ../my-project

# Build with verbose output
stratos build -v
```

**Project Structure:**
```
my-project/
├── stratos.conf        # Project configuration
├── src/
│   └── main.st        # Source files
├── deps/               # Dependencies
└── build/              # Build output
```

### stratos new

Create a new Stratos project with standard structure.

**Usage:**
```bash
stratos new <project-name>
```

**Examples:**
```bash
# Create new project
stratos new my-app

# Creates:
my-app/
├── stratos.conf
├── src/
│   └── main.st
└── README.md
```

**Generated stratos.conf:**
```hocon
project {
  name = my-app
  version = "0.1.0"
  type = executable
}

build {
  entry = src/main.st
  output = build/my-app
}

dependencies = []
```

### stratos get

Fetch and manage project dependencies (similar to Go modules).

**Usage:**
```bash
stratos get                    # Fetch all dependencies
stratos get <url>              # Fetch specific dependency
stratos get --update           # Update all dependencies
stratos get --verify           # Verify lock file
```

**Options:**
- `-v, --verbose` - Show detailed fetch information
- `--update` - Update dependencies and regenerate lock file
- `--verify` - Verify lock file matches installed dependencies

**Dependency URL Formats:**
```bash
# GitHub with tag
stratos get github.com/user/repo@v1.0.0

# GitHub with branch
stratos get github.com/user/repo@main

# GitHub with commit hash
stratos get github.com/user/repo@abc123

# Full URL
stratos get https://github.com/user/repo

# Local path
stratos get path:../local-lib
```

**Examples:**
```bash
# Fetch all dependencies from stratos.conf
stratos get

# Fetch specific library
stratos get github.com/stratos-lang/http@v2.1.0

# Fetch with verbose output
stratos get -v

# Update all dependencies
stratos get --update

# Verify dependencies match lock file
stratos get --verify
```

**Dependencies in stratos.conf:**
```hocon
dependencies = [
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"
  }
  {
    name = json
    url = "https://github.com/stratos-lang/json"
    branch = main
  }
  {
    name = utils
    url = "../local-utils"
  }
]
```

See [Dependency Management](/docs/v1.0.0/dependency-management) for full details.

### stratos test

Run test suite from the `cases/` directory.

**Usage:**
```bash
stratos test [options]
```

**Options:**
- `-v, --verbose` - Show detailed test output
- `--filter <pattern>` - Run tests matching pattern

**Examples:**
```bash
# Run all tests
stratos test

# Run with verbose output
stratos test --verbose

# Run specific tests
stratos test --filter 01_basics
```

**Test File Structure:**
```
cases/
├── 01_basics.st
├── 02_control_flow.st
├── 03_functions.st
└── test_*.st
```

### stratos --help

Display help information.

**Usage:**
```bash
stratos --help
stratos -h
```

### stratos --version

Display version information.

**Usage:**
```bash
stratos --version
```

## Global Options

These options work with most commands:

| Option | Description |
|--------|-------------|
| `-v, --verbose` | Enable verbose output |
| `-o, --output <file>` | Specify output file |
| `-h, --help` | Show help |

## Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `STRATOS_HOME` | Stratos installation directory | `/usr/local/stratos` |
| `STRATOS_CACHE` | Dependency cache directory | `~/.stratos/cache` |
| `STRATOS_PATH` | Module search path | `./std:./deps` |

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | Compilation/runtime error |
| 2 | Invalid arguments |
| 127 | Command not found |

## Configuration File

The `stratos.conf` file uses HOCON format for project configuration.

**Full Example:**
```hocon
project {
  name = my-project
  version = "1.0.0"
  author = "Your Name"
  type = executable              # or library
  description = "Project description"
}

build {
  entry = src/main.st
  source_dir = src
  output = build/my-project
}

dependencies = [
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"
  }
]

exports {
  main = src/init.st           # For libraries
}
```

## Workflow Examples

### Creating and Running a New Project

```bash
# 1. Create project
stratos new hello-world
cd hello-world

# 2. Edit src/main.st
# 3. Run it
stratos run src/main.st

# 4. Build for distribution
stratos build
```

### Adding Dependencies

```bash
# 1. Add to stratos.conf
dependencies = [
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"
  }
]

# 2. Fetch dependencies
stratos get

# 3. Use in your code
use http;
```

### Testing and Building

```bash
# Run tests
stratos test

# Build for production
stratos build

# Verify everything
stratos get --verify
```

## Tips and Best Practices

::: tip
**Use lock files**: Always commit `stratos.lock` to version control for reproducible builds across environments.
:::

::: tip
**Verbose output**: Use `-v` flag when debugging build or dependency issues to see detailed information.
:::

::: warning
**Breaking changes**: During development (pre-1.0), some CLI options may change. Check release notes for updates.
:::

## See Also

- [Getting Started Guide](/docs/v1.0.0/getting-started)
- [Dependency Management](/docs/v1.0.0/dependency-management)
- [Project Configuration](/docs/v1.0.0/configuration)
