# Entry Point Resolution Feature

**Date**: December 27, 2025
**Status**: ✅ IMPLEMENTED AND TESTED

---

## Overview

The Stratos interpreter now features intelligent entry point resolution. When you run `stratos run <file>`, it automatically searches for the entry point in multiple locations, making it more convenient to execute programs.

---

## Resolution Order

When you execute `stratos run <file>`, the interpreter searches for the entry point in the following order:

1. **Exact file path**: If `<file>` exists as a regular file, use it
2. **With .st extension**: If `<file>.st` exists, use it
3. **From stratos.conf**: Search up to 5 parent directories for `stratos.conf`
   - If found, parse the `entry` field in the `[build]` section
   - If entry is set and the file exists, use it
4. **main.st in current directory**: If `./main.st` exists, use it
5. **main.st in src directory**: If `./src/main.st` exists, use it
6. **Error**: If none of the above exist, show a helpful error message

---

## Examples

### Example 1: Using stratos.conf Entry Point

**Project Structure**:
```
hello_world/
├── stratos.conf
└── src/
    └── main.st
```

**stratos.conf**:
```ini
[build]
entry = src/main.st
```

**Command** (from inside hello_world/):
```bash
$ stratos run anything
```

**Result**: Executes `src/main.st` (from stratos.conf)

**Verbose Output**:
```bash
$ stratos run anything -v
Resolved entry point: /path/to/hello_world/./src/main.st
Executing: /path/to/hello_world/./src/main.st
  [Lexer]     OK (25 tokens)
  [Parser]    OK (2 statements)
  [Semantics] OK
  [Executing...]
Hello, Stratos!
42
  [Execution] Complete
```

---

### Example 2: Fallback to main.st

**Project Structure**:
```
my_project/
└── main.st
```

**Command**:
```bash
$ stratos run anything
```

**Result**: Executes `main.st` (fallback)

---

### Example 3: Auto .st Extension

**Project Structure**:
```
scripts/
└── hello.st
```

**Command**:
```bash
$ stratos run hello
```

**Result**: Executes `hello.st` (auto-added .st extension)

---

### Example 4: Explicit File Path

**Project Structure**:
```
project/
└── src/
    └── app.st
```

**Command**:
```bash
$ stratos run src/app.st
```

**Result**: Executes `src/app.st` (explicit path)

---

### Example 5: Error When File Not Found

**Command**:
```bash
$ stratos run nonexistent
```

**Result** (from directory without stratos.conf or main.st):
```
Error: Could not find entry point file.
Searched for:
  - nonexistent
  - nonexistent.st
  - Entry point from stratos.conf
  - main.st
  - src/main.st
```

---

## Implementation Details

### File: `src/main.cpp`

**New Functions**:

1. **`findProjectConfig()`**
   - Searches up to 5 parent directories for `stratos.conf`
   - Returns the path if found, or `std::nullopt`

2. **`resolveEntryPoint()`**
   - Implements the entry point resolution logic
   - Tries multiple strategies in order
   - Returns resolved path or `std::nullopt`

**Modified Function**:

- **`handleRun()`**
  - Uses `resolveEntryPoint()` instead of direct file check
  - Provides verbose output showing resolved path
  - Shows helpful error message when file not found

### Code Changes

**Helper Functions** (added before `handleRun()`):

```cpp
// Helper function to find stratos.conf in current or parent directories
std::optional<std::string> findProjectConfig(const std::string& startDir = ".") {
    fs::path currentPath = fs::absolute(startDir);

    // Search up to 5 levels up
    for (int i = 0; i < 5; i++) {
        fs::path configPath = currentPath / "stratos.conf";
        if (fs::exists(configPath)) {
            return configPath.string();
        }

        // Move to parent directory
        fs::path parent = currentPath.parent_path();
        if (parent == currentPath) {
            break; // Reached root
        }
        currentPath = parent;
    }

    return std::nullopt;
}

// Helper function to resolve entry point file
std::optional<std::string> resolveEntryPoint(const std::string& inputPath) {
    // If the input path exists as-is, use it
    if (fs::is_regular_file(inputPath)) {
        return inputPath;
    }

    // Try adding .st extension
    std::string withExtension = inputPath + ".st";
    if (fs::is_regular_file(withExtension)) {
        return withExtension;
    }

    // Look for stratos.conf
    auto configPathOpt = findProjectConfig();
    if (configPathOpt) {
        auto configOpt = ProjectConfigParser::parse(*configPathOpt);
        if (configOpt) {
            // Check if entry is set
            if (!configOpt->entry.empty()) {
                fs::path entryPath = fs::path(*configPathOpt).parent_path() / configOpt->entry;
                if (fs::is_regular_file(entryPath)) {
                    return entryPath.string();
                }
            }
        }
    }

    // Try main.st in current directory
    if (fs::is_regular_file("main.st")) {
        return "main.st";
    }

    // Try main.st in src/ directory
    if (fs::is_regular_file("src/main.st")) {
        return "src/main.st";
    }

    return std::nullopt;
}
```

**Modified `handleRun()` function**:

```cpp
int handleRun(int argc, char* argv[]) {
    std::string inputPath;
    bool verbose = false;

    if (argc < 3) {
        std::cerr << "Error: No input file specified\n\n";
        printHelp();
        return 1;
    }

    inputPath = argv[2];

    // Parse options
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
    }

    // Resolve the entry point file
    auto resolvedPathOpt = resolveEntryPoint(inputPath);
    if (!resolvedPathOpt) {
        std::cerr << "Error: Could not find entry point file.\n";
        std::cerr << "Searched for:\n";
        std::cerr << "  - " << inputPath << "\n";
        std::cerr << "  - " << inputPath << ".st\n";
        std::cerr << "  - Entry point from stratos.conf\n";
        std::cerr << "  - main.st\n";
        std::cerr << "  - src/main.st\n";
        return 1;
    }

    std::string resolvedPath = *resolvedPathOpt;

    if (verbose) {
        std::cout << "Resolved entry point: " << resolvedPath << std::endl;
    }

    // Execute the file
    CompileResult result = compileFile(resolvedPath, "", verbose, true);

    if (!result.success) {
        std::cerr << "Execution failed: " << result.errorMessage << std::endl;
        return 1;
    }

    return 0;
}
```

---

## Benefits

✅ **Convenience**: No need to type full paths or .st extension
✅ **Project-aware**: Automatically uses stratos.conf entry point
✅ **Smart fallbacks**: Falls back to main.st if no config
✅ **Clear errors**: Helpful error messages showing all search locations
✅ **Verbose mode**: See exactly which file was resolved

---

## Testing Results

### Test 1: stratos.conf Entry Point ✅
```bash
$ cd samples/hello_world
$ stratos run anything -v
Resolved entry point: /path/to/samples/hello_world/./src/main.st
Hello, Stratos!
42
```

### Test 2: Fallback to main.st ✅
```bash
$ cd test-fallback  # Directory with only main.st
$ stratos run anything
Fallback to main.st works!
```

### Test 3: Auto .st Extension ✅
```bash
$ stratos run main  # Finds main.st
Fallback to main.st works!
```

### Test 4: Error Message ✅
```bash
$ cd empty-dir
$ stratos run nonexistent
Error: Could not find entry point file.
Searched for:
  - nonexistent
  - nonexistent.st
  - Entry point from stratos.conf
  - main.st
  - src/main.st
```

---

## Build Instructions

A simple build script has been created for convenience:

```bash
cd interpreter/C++
./build.sh
```

This compiles the interpreter without requiring cmake or ninja.

---

## Summary

This feature makes the Stratos CLI more user-friendly by automatically finding entry points through multiple strategies. It's especially useful for:

- 🚀 Quick script execution without typing full paths
- 📦 Project-based development with stratos.conf
- 🔧 Consistent entry point conventions (main.st)
- 💡 Reduced friction for new users

**Implementation Date**: December 27, 2025
**Lines of Code**: ~60 new, ~35 modified
**Test Success Rate**: 100%
**Status**: PRODUCTION READY ✅
