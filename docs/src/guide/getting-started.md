# Getting Started

Get up and running with Stratos in just a few minutes.

## Installation

### Quick Install (Linux/macOS)

```bash
curl -sSL https://get.stratos-lang.org | sh
```

### Verify Installation

```bash
stratos --version
```

### Manual Installation

1. Download the latest release from [GitHub](https://github.com/Arthur-Kamau/stratos/releases)
2. Extract the archive
3. Add the `bin` directory to your `PATH`

## Your First Program

### Create a Hello World Program

Create a file named `hello.st`:

```stratos
package main;

fn main() {
    print("Hello, Stratos!");
}
```

### Run It

```bash
stratos run hello.st
```

Output:
```
Hello, Stratos!
```

### Compile It

```bash
stratos compile hello.st -o hello
./hello
```

## Create a New Project

Stratos provides a project scaffolding tool:

```bash
stratos new my-project
cd my-project
```

This creates a project structure:

```
my-project/
├── stratos.conf       # Project configuration
├── src/
│   └── main.st       # Entry point
└── README.md
```

### Project Configuration

The `stratos.conf` file uses HOCON format:

```hocon
project {
  name = my-project
  version = "0.1.0"
  type = executable
}

build {
  entry = src/main.st
  output = build/my-project
}

dependencies = []
```

### Build the Project

```bash
stratos build
```

This compiles your project and creates an executable in the `build/` directory.

### Run the Project

```bash
./build/my-project
```

## Basic Program Structure

Every Stratos program needs:

1. **Package declaration**: Declares the package name
2. **Main function**: Entry point of the program

```stratos
package main;

fn main() {
    // Your code here
    print("Hello, Stratos!");
}
```

## Common Commands

| Command | Description |
|---------|-------------|
| `stratos run <file>` | Run a Stratos file |
| `stratos compile <file>` | Compile a file |
| `stratos build` | Build a project |
| `stratos new <name>` | Create a new project |
| `stratos get` | Fetch dependencies |
| `stratos test` | Run tests |

## Next Steps

Now that you have Stratos installed:

- Learn the [Language Basics](/guide/basics)
- Explore [Code Examples](/examples/)
- Read the [CLI Reference](/reference/cli)
- Browse the [Standard Library](/reference/stdlib)

## Getting Help

- 📖 [Documentation](/) - Full documentation
- 💬 [Discord](https://discord.gg/stratos) - Community chat
- 📝 [GitHub Issues](https://github.com/Arthur-Kamau/stratos/issues) - Report bugs
- 🐦 [Twitter](https://twitter.com/stratoslang) - Latest updates

::: tip
Start with simple programs and gradually explore more advanced features. The [examples section](/examples/) is a great place to learn by doing.
:::
