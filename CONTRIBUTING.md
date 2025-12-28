# Contributing to Stratos

Thank you for your interest in contributing to Stratos! This guide will help you get started with development.

## Table of Contents

- [Development Setup](#development-setup)
- [Building from Source](#building-from-source)
- [Making the Binary Globally Available](#making-the-binary-globally-available)
- [Running Tests](#running-tests)
- [Git Hooks](#git-hooks)
- [Making Changes](#making-changes)
- [Submitting Changes](#submitting-changes)
- [Code Style](#code-style)
- [Project Structure](#project-structure)

## Development Setup

### Prerequisites

Make sure you have the following installed:

- **C++ compiler** (g++ 9+ or clang 10+)
- **CMake** 3.15 or later
- **LLVM** 14 or later
- **Git**

#### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install build-essential cmake g++ git
```

#### macOS

```bash
brew install cmake llvm@14
# Xcode command line tools
xcode-select --install
```

### Clone the Repository

```bash
git clone https://github.com/Arthur-Kamau/stratos.git
cd stratos
```

## Building from Source

### Build the Interpreter/Compiler

```bash
cd interpreter/C++
bash build.sh
```

This creates the `stratos` binary at `interpreter/C++/build/stratos`.

### Verify the Build

```bash
./interpreter/C++/build/stratos --version
```

You should see output like:
```
Stratos Compiler v0.1.0
LLVM Backend
Built: Dec 28 2025 17:57:24
```

## Making the Binary Globally Available

During development, you'll want to use the `stratos` command from anywhere without typing the full path `./interpreter/C++/build/stratos` every time.

### Recommended: Create a Symlink

This approach ensures you always use the latest build automatically:

```bash
# Create ~/bin directory if it doesn't exist
mkdir -p ~/bin

# Create a symlink to the build directory
ln -sf "$(pwd)/interpreter/C++/build/stratos" ~/bin/stratos
```

### Add ~/bin to PATH

Add this to your shell configuration file (`~/.bashrc`, `~/.zshrc`, or `~/.profile`):

```bash
export PATH="$HOME/bin:$PATH"
```

Then reload your shell:

```bash
source ~/.bashrc  # or ~/.zshrc
```

### Verify Global Access

```bash
# Should show: /home/yourusername/bin/stratos
which stratos

# Should work from any directory
stratos --version
```

### Benefits of This Approach

- **Automatic updates**: Every time you rebuild, the global `stratos` command uses the new binary
- **No reinstall needed**: Just run `bash build.sh` and the changes are live
- **Easy cleanup**: Just `rm ~/bin/stratos` to remove
- **Non-invasive**: Doesn't interfere with system-wide installations

### After Rebuilding

Whenever you make changes and rebuild:

```bash
cd interpreter/C++
bash build.sh

# The new binary is immediately available globally:
stratos --version  # Shows the newly built version
```

## Running Tests

### Automated Test Suite

We have an automated test suite that runs all sample projects:

```bash
# From project root
./test-all.sh
```

### Test Options

```bash
# Verbose output
./test-all.sh -v

# Skip rebuild (faster when binary hasn't changed)
./test-all.sh --no-rebuild

# Run specific test
./test-all.sh --test hello_world

# Stop on first failure
./test-all.sh --stop-on-fail
```

### Running Individual Tests

```bash
# Run a specific project
stratos run samples/hello_world/src/main.st

# Run with verbose output
stratos run samples/dependency_test/src/main.st -v
```

### Creating New Tests

To add a new test:

1. Create a project in `samples/`:
   ```bash
   mkdir -p samples/my_test/src
   ```

2. Create `samples/my_test/stratos.conf`:
   ```hocon
   project {
     name = my_test
     version = "1.0.0"
     type = executable
   }

   build {
     entry = src/main.st
   }
   ```

3. Add your test code in `samples/my_test/src/main.st`

4. Add the test name to `test-all.sh`:
   ```bash
   RUNNABLE_SAMPLES=(
       "hello_world"
       "my_test"  # Add here
       # ... other tests
   )
   ```

## Git Hooks

### Pre-commit Hook

We have a pre-commit hook that automatically runs tests before allowing commits. This helps maintain code quality.

#### Automatic Setup

The pre-commit hook is already set up at `.git/hooks/pre-commit`. It runs automatically when you commit.

#### How it Works

```bash
git add .
git commit -m "Your message"

# Tests run automatically:
# - If tests pass: commit proceeds
# - If tests fail: commit is aborted
```

#### Bypassing the Hook (Not Recommended)

Only in emergencies:

```bash
git commit --no-verify -m "Emergency fix"
```

#### Hook Features

- Runs all tests before commit
- Uses `--no-rebuild` for speed (only builds if needed)
- Color-coded output
- Clear error messages if tests fail

## Making Changes

### Development Workflow

1. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes**:
   - Edit code in `interpreter/C++/src/` or `interpreter/C++/include/`
   - Add tests if needed

3. **Build and test**:
   ```bash
   cd interpreter/C++
   bash build.sh
   cd ../..
   ./test-all.sh
   ```

4. **Commit your changes**:
   ```bash
   git add .
   git commit -m "Brief description of changes"
   # Tests run automatically via pre-commit hook
   ```

5. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

### Common Development Tasks

#### Adding a New Feature

1. Plan the feature (update documentation if needed)
2. Implement in appropriate source files
3. Add test cases in `samples/`
4. Run tests to verify
5. Commit changes

#### Fixing a Bug

1. Create a test that reproduces the bug
2. Fix the bug in source code
3. Verify the test now passes
4. Run full test suite
5. Commit both the fix and the test

#### Updating the Lexer/Parser

Files to modify:
- Lexer: `interpreter/C++/src/lexer/Lexer.cpp`
- Parser: `interpreter/C++/src/parser/Parser.cpp`
- AST: `interpreter/C++/include/stratos/AST.h`

#### Updating the Interpreter

Files to modify:
- Interpreter: `interpreter/C++/src/runtime/Interpreter.cpp`
- Native functions: `interpreter/C++/src/runtime/NativeRegistry.cpp`

## Submitting Changes

### Pull Request Guidelines

1. **Ensure all tests pass**: Run `./test-all.sh` before submitting
2. **Write clear commit messages**:
   - First line: Brief summary (50 chars or less)
   - Blank line
   - Detailed explanation if needed

   Example:
   ```
   Add support for string escape sequences

   - Modified Lexer to process \n, \t, \r, \\, \", \0
   - Added test_escape_sequences sample project
   - All existing tests still pass
   ```

3. **Update documentation**: If you add features, update relevant docs
4. **Keep PRs focused**: One feature/fix per PR when possible
5. **Reference issues**: Mention related issues with `Fixes #123`

### PR Checklist

- [ ] Code builds successfully
- [ ] All tests pass (`./test-all.sh`)
- [ ] New features have tests
- [ ] Documentation updated if needed
- [ ] Commit messages are clear
- [ ] No unrelated changes included

## Code Style

### C++ Code Style

- **Indentation**: 4 spaces (no tabs)
- **Naming**:
  - Classes/Structs: `PascalCase`
  - Functions/Methods: `camelCase`
  - Variables: `camelCase`
  - Constants: `UPPER_SNAKE_CASE`
  - Private members: prefix with `m_` (optional)

- **Braces**: Opening brace on same line
  ```cpp
  if (condition) {
      // code
  }
  ```

- **Pointers/References**: Attach to type
  ```cpp
  int* ptr;
  const std::string& str;
  ```

### Stratos Language Style

For sample code and tests:

- **Indentation**: 4 spaces
- **Naming**: Follow language conventions
- **Comments**: Use `//` for single line, avoid excessive comments

## Project Structure

```
stratos/
├── interpreter/C++/          # C++ interpreter/compiler
│   ├── include/stratos/      # Header files
│   │   ├── AST.h             # Abstract Syntax Tree definitions
│   │   ├── Lexer.h           # Lexer interface
│   │   ├── Parser.h          # Parser interface
│   │   ├── Interpreter.h     # Interpreter interface
│   │   └── ...
│   ├── src/                  # Source files
│   │   ├── lexer/            # Lexer implementation
│   │   ├── parser/           # Parser implementation
│   │   ├── runtime/          # Interpreter runtime
│   │   └── main.cpp          # Entry point
│   ├── build/                # Build output (gitignored)
│   └── build.sh              # Build script
├── samples/                  # Test projects and examples
│   ├── hello_world/
│   ├── dependency_test/
│   ├── oop_demo/
│   └── ...
├── docs/                     # Documentation
├── tooling/vscode/           # VS Code extension
├── test-all.sh               # Automated test runner
├── install.sh                # Installation script
└── .git/hooks/pre-commit     # Git hooks

Key Files:
├── README.md                 # Project overview
├── CONTRIBUTING.md           # This file
├── install.md                # Installation system docs
├── TESTING_PLAN.md           # Testing strategy
└── GIT_HOOKS_SETUP.md        # Git hooks documentation
```

### Important Directories

- **interpreter/C++/src/**: All C++ source code
- **interpreter/C++/include/stratos/**: All header files
- **samples/**: Example programs and test cases
- **docs/**: User-facing documentation

## Getting Help

If you need help or have questions:

- **Documentation**: Check `docs/` directory
- **Issues**: Search existing issues on GitHub
- **Discussions**: Start a discussion on GitHub
- **Discord**: Join our Discord server (if available)

## Common Issues

### Build Fails

```bash
# Clean build and try again
cd interpreter/C++
rm -rf build/
bash build.sh
```

### Tests Fail After Changes

1. Check if your changes broke existing functionality
2. Run specific test to isolate: `./test-all.sh --test <name>`
3. Run with verbose: `./test-all.sh -v`
4. Check test output for error messages

### Symlink Not Working

```bash
# Verify symlink exists
ls -lh ~/bin/stratos

# Recreate if needed
rm ~/bin/stratos
ln -sf "$(pwd)/interpreter/C++/build/stratos" ~/bin/stratos

# Ensure ~/bin is in PATH
echo $PATH | grep "$HOME/bin"
```

### Pre-commit Hook Doesn't Run

```bash
# Ensure hook is executable
chmod +x .git/hooks/pre-commit

# Verify hook exists
ls -lh .git/hooks/pre-commit
```

## License

By contributing to Stratos, you agree that your contributions will be licensed under the same license as the project (Apache License 2.0 / MIT, depending on the submodule).

## Code of Conduct

- Be respectful and constructive
- Welcome newcomers
- Focus on what's best for the project
- Show empathy towards other contributors

## Questions?

Feel free to open an issue on GitHub if you have questions about contributing!

---

Thank you for contributing to Stratos!
