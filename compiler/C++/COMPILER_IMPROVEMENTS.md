# Stratos Compiler Improvements Plan

## Current State Analysis

The compiler currently:
- Takes a directory path and compiles all `.st` files in it
- Defaults to `../../cases` directory
- Has a hardcoded fallback path
- No proper CLI argument parsing
- No help or version information
- No test validation mode

## Proposed Improvements

### 1. Enhanced CLI with Proper Argument Parsing

#### Command Structure
```bash
# Compile a single file
stratos compile <file.st>
stratos <file.st>                    # Short form

# Compile all files in a directory
stratos compile <directory>

# Run test suite
stratos test                         # Run all tests in cases/
stratos test --verbose               # Verbose output
stratos test --report                # Generate test report

# Show help and version
stratos --help
stratos --version

# Output options
stratos compile file.st -o output.ll
stratos compile file.st --emit-llvm  # Generate .ll file
stratos compile file.st --emit-asm   # Generate .s file
stratos compile file.st --emit-obj   # Generate .o file
```

#### Subcommands
- `compile` - Compile source files (default)
- `test` - Run test cases
- `new` - Create new project (separate tool, see below)
- `build` - Build entire project
- `run` - Compile and run
- `check` - Parse and analyze without code generation
- `fmt` - Format Stratos code (future)

---

## Implementation Guide

### Phase 1: Improved main.cpp

Create a proper CLI structure with command parsing.

```cpp
// main.cpp - Improved structure
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <map>

#include "stratos/Compiler.h"
#include "stratos/TestRunner.h"
#include "stratos/CLIParser.h"

namespace fs = std::filesystem;

void printHelp() {
    std::cout << "Stratos Compiler v0.1.0\n\n";
    std::cout << "Usage:\n";
    std::cout << "  stratos <file.st>              Compile a single file\n";
    std::cout << "  stratos compile <file.st>      Compile a single file\n";
    std::cout << "  stratos compile <directory>    Compile all .st files in directory\n";
    std::cout << "  stratos test                   Run test cases\n";
    std::cout << "  stratos test --verbose         Run tests with detailed output\n";
    std::cout << "  stratos check <file.st>        Parse and check without codegen\n";
    std::cout << "  stratos --help                 Show this help\n";
    std::cout << "  stratos --version              Show version\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output <file>            Specify output file\n";
    std::cout << "  -v, --verbose                  Verbose output\n";
    std::cout << "  --emit-llvm                    Generate LLVM IR (.ll)\n";
    std::cout << "  --no-optimize                  Disable optimizations\n";
}

void printVersion() {
    std::cout << "Stratos Compiler v0.1.0\n";
    std::cout << "LLVM Backend\n";
    std::cout << "Copyright (c) 2024\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No input file specified.\n\n";
        printHelp();
        return 1;
    }

    std::string command = argv[1];

    // Handle flags
    if (command == "--help" || command == "-h") {
        printHelp();
        return 0;
    }

    if (command == "--version" || command == "-v") {
        printVersion();
        return 0;
    }

    // Handle subcommands
    if (command == "test") {
        return runTests(argc, argv);
    }

    if (command == "compile" || command.ends_with(".st")) {
        return compileFile(argc, argv);
    }

    if (command == "check") {
        return checkFile(argc, argv);
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    printHelp();
    return 1;
}
```

### Phase 2: Separate Compilation Logic

Create `Compiler.h` and `Compiler.cpp` to encapsulate compilation logic.

```cpp
// include/stratos/Compiler.h
#pragma once

#include <string>
#include <vector>
#include <optional>

namespace stratos {

struct CompilerOptions {
    std::string inputPath;
    std::optional<std::string> outputPath;
    bool verbose = false;
    bool emitLLVM = true;
    bool emitAsm = false;
    bool emitObj = false;
    bool optimize = true;
    bool checkOnly = false;  // Parse and analyze only
};

class Compiler {
public:
    Compiler(const CompilerOptions& options);

    // Compile a single file
    bool compileFile(const std::string& path);

    // Compile all files in directory
    bool compileDirectory(const std::string& dirPath);

    // Get compilation result
    const std::string& getOutput() const { return output_; }
    const std::vector<std::string>& getErrors() const { return errors_; }

private:
    CompilerOptions options_;
    std::string output_;
    std::vector<std::string> errors_;

    bool runPipeline(const std::string& source, const std::string& filepath);
};

} // namespace stratos
```

### Phase 3: Test Runner

Create a dedicated test runner for validation.

```cpp
// include/stratos/TestRunner.h
#pragma once

#include <string>
#include <vector>

namespace stratos {

struct TestCase {
    std::string path;
    std::string name;
    bool shouldPass;  // Expected result
    std::string expectedError;  // For negative tests
};

struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    double executionTime;  // in milliseconds
};

class TestRunner {
public:
    TestRunner(const std::string& casesDir);

    // Run all tests
    std::vector<TestResult> runAll(bool verbose = false);

    // Run specific test
    TestResult runTest(const TestCase& test);

    // Generate report
    void generateReport(const std::vector<TestResult>& results);

private:
    std::string casesDir_;
    std::vector<TestCase> loadTestCases();
};

} // namespace stratos
```

```cpp
// src/TestRunner.cpp
#include "stratos/TestRunner.h"
#include "stratos/Compiler.h"
#include <iostream>
#include <filesystem>
#include <chrono>

namespace stratos {
namespace fs = std::filesystem;

TestRunner::TestRunner(const std::string& casesDir)
    : casesDir_(casesDir) {}

std::vector<TestCase> TestRunner::loadTestCases() {
    std::vector<TestCase> cases;

    for (const auto& entry : fs::directory_iterator(casesDir_)) {
        if (entry.path().extension() == ".st") {
            TestCase tc;
            tc.path = entry.path().string();
            tc.name = entry.path().filename().string();
            tc.shouldPass = true;  // Default: tests should pass
            cases.push_back(tc);
        }
    }

    return cases;
}

TestResult TestRunner::runTest(const TestCase& test) {
    TestResult result;
    result.testName = test.name;

    auto start = std::chrono::high_resolution_clock::now();

    CompilerOptions opts;
    opts.inputPath = test.path;
    opts.verbose = false;

    Compiler compiler(opts);
    bool success = compiler.compileFile(test.path);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    result.executionTime = duration.count();

    result.passed = (success == test.shouldPass);

    if (result.passed) {
        result.message = "PASS";
    } else {
        result.message = "FAIL: " + (compiler.getErrors().empty()
            ? "Unknown error"
            : compiler.getErrors()[0]);
    }

    return result;
}

std::vector<TestResult> TestRunner::runAll(bool verbose) {
    std::cout << "Running Stratos Test Suite...\n";
    std::cout << "Test directory: " << casesDir_ << "\n\n";

    auto testCases = loadTestCases();
    std::vector<TestResult> results;

    int passed = 0;
    int failed = 0;

    for (const auto& test : testCases) {
        if (verbose) {
            std::cout << "Running " << test.name << "... ";
        }

        TestResult result = runTest(test);
        results.push_back(result);

        if (result.passed) {
            passed++;
            if (verbose) {
                std::cout << "✓ PASS (" << result.executionTime << "ms)\n";
            } else {
                std::cout << ".";
            }
        } else {
            failed++;
            if (verbose) {
                std::cout << "✗ FAIL\n";
                std::cout << "  " << result.message << "\n";
            } else {
                std::cout << "F";
            }
        }
    }

    if (!verbose) std::cout << "\n";

    std::cout << "\n";
    std::cout << "==========================================\n";
    std::cout << "Test Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "Total: " << testCases.size() << " tests\n";
    std::cout << "==========================================\n";

    return results;
}

void TestRunner::generateReport(const std::vector<TestResult>& results) {
    // Generate detailed test report
    std::ofstream report("test_report.txt");

    report << "Stratos Compiler Test Report\n";
    report << "============================\n\n";

    for (const auto& result : results) {
        report << result.testName << ": ";
        report << (result.passed ? "PASS" : "FAIL") << " ";
        report << "(" << result.executionTime << "ms)\n";

        if (!result.passed) {
            report << "  Error: " << result.message << "\n";
        }
    }

    report.close();
    std::cout << "Report generated: test_report.txt\n";
}

} // namespace stratos
```

---

## CLI Tool for Project Creation

### Separate Tool: `stratos-cli` or `stratoscli`

Create a separate executable for project management (like `cargo` for Rust).

### Project Structure to Generate

```
my-project/
├── stratos.toml           # Project configuration
├── src/
│   └── main.st           # Entry point
├── tests/
│   └── main_test.st      # Test file
└── README.md
```

### Implementation

```cpp
// tools/cli/main.cpp
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

void createProject(const std::string& name) {
    std::cout << "Creating new Stratos project: " << name << "\n";

    // Create directory structure
    fs::create_directory(name);
    fs::create_directory(name + "/src");
    fs::create_directory(name + "/tests");

    // Create stratos.toml
    std::ofstream tomlFile(name + "/stratos.toml");
    tomlFile << "[package]\n";
    tomlFile << "name = \"" << name << "\"\n";
    tomlFile << "version = \"0.1.0\"\n";
    tomlFile << "authors = [\"Your Name <you@example.com>\"]\n\n";
    tomlFile << "[dependencies]\n";
    tomlFile.close();

    // Create main.st
    std::ofstream mainFile(name + "/src/main.st");
    mainFile << "// " << name << " - Main Entry Point\n\n";
    mainFile << "fn main() {\n";
    mainFile << "    print(\"Hello, Stratos!\");\n";
    mainFile << "}\n";
    mainFile.close();

    // Create test file
    std::ofstream testFile(name + "/tests/main_test.st");
    testFile << "// Tests for " << name << "\n\n";
    testFile << "fn testExample() {\n";
    testFile << "    val result = 2 + 2;\n";
    testFile << "    // assert(result == 4);\n";
    testFile << "}\n";
    testFile.close();

    // Create README
    std::ofstream readmeFile(name + "/README.md");
    readmeFile << "# " << name << "\n\n";
    readmeFile << "A Stratos project.\n\n";
    readmeFile << "## Building\n\n";
    readmeFile << "```bash\n";
    readmeFile << "stratos build\n";
    readmeFile << "```\n\n";
    readmeFile << "## Running\n\n";
    readmeFile << "```bash\n";
    readmeFile << "stratos run\n";
    readmeFile << "```\n";
    readmeFile.close();

    std::cout << "✓ Project created successfully!\n\n";
    std::cout << "Next steps:\n";
    std::cout << "  cd " << name << "\n";
    std::cout << "  stratos build\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Stratos CLI Tool\n\n";
        std::cout << "Usage:\n";
        std::cout << "  stratoscli new <name>     Create a new project\n";
        std::cout << "  stratoscli init           Initialize project in current directory\n";
        return 1;
    }

    std::string command = argv[1];

    if (command == "new") {
        if (argc < 3) {
            std::cerr << "Error: Project name required\n";
            std::cerr << "Usage: stratoscli new <name>\n";
            return 1;
        }

        std::string projectName = argv[2];
        createProject(projectName);
        return 0;
    }

    if (command == "init") {
        std::string currentDir = fs::current_path().filename().string();
        createProject(".");
        return 0;
    }

    std::cerr << "Unknown command: " << command << "\n";
    return 1;
}
```

---

## CMakeLists.txt Updates

Add the CLI tool to your CMake build:

```cmake
# Add the CLI tool
add_executable(stratoscli
    tools/cli/main.cpp
)

target_include_directories(stratoscli PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
```

---

## Project Configuration File (stratos.toml)

```toml
[package]
name = "my-project"
version = "0.1.0"
authors = ["Your Name <you@example.com>"]
edition = "2024"

[dependencies]
# Add dependencies here
# std = "1.0"

[build]
optimization = "release"  # debug, release
target = "x86_64"

[dev-dependencies]
# test-framework = "1.0"
```

---

## Summary of What to Build

### 1. **Compiler Improvements**
- ✅ Enhanced CLI argument parsing
- ✅ Subcommands: compile, test, check
- ✅ Proper help and version output
- ✅ Compile single file or directory
- ✅ Test runner with validation

### 2. **New Files to Create**
```
compiler/C++/
├── include/stratos/
│   ├── Compiler.h        # Compilation interface
│   ├── TestRunner.h      # Test runner
│   └── CLIParser.h       # CLI argument parsing (optional)
├── src/
│   ├── Compiler.cpp
│   ├── TestRunner.cpp
│   └── main.cpp          # Improved main
└── tools/
    └── cli/
        └── main.cpp      # Project creation tool
```

### 3. **CLI Tool (stratoscli)**
- Project creation (`new`, `init`)
- Project templates
- Configuration file generation
- README generation

### 4. **Future Enhancements**
- `stratos build` - Build entire project
- `stratos run` - Compile and execute
- `stratos fmt` - Format code
- `stratos doc` - Generate documentation
- Package manager integration

This design gives you a professional, extensible compiler toolchain similar to Rust's `cargo` or Go's `go` command!
