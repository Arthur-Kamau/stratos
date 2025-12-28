# Stratos Testing Plan

This document outlines the testing strategy for the Stratos compiler/interpreter to ensure no breaking changes are introduced during development.

---

## Quick Start

### Run All Tests
```bash
./test-all.sh
```

### Run Tests Without Rebuilding
```bash
./test-all.sh --no-rebuild
```

### Run Tests with Verbose Output
```bash
./test-all.sh -v
```

### Run a Specific Test
```bash
./test-all.sh --test hello_world
```

### Stop on First Failure
```bash
./test-all.sh --stop-on-fail
```

---

## Test Suite Structure

### 1. Automated Test Runner (`test-all.sh`)

**Location:** `/home/kamau/Development/Projects/stratos/test-all.sh`

**What it does:**
1. Rebuilds the interpreter (optional)
2. Discovers all runnable sample projects
3. Runs each sample and captures output
4. Reports pass/fail status with colored output
5. Generates summary report

**Features:**
- ✅ Automatic build verification
- ✅ Colored output for easy reading
- ✅ Timeout protection (10s per test)
- ✅ Verbose mode for debugging
- ✅ Stop-on-fail for quick debugging
- ✅ Run specific tests
- ✅ Pass rate calculation
- ✅ Exit codes (0 = all pass, 1 = failures)

### 2. Test Categories

#### Category A: Core Language Features

**Tests:**
- `hello_world` - Basic program execution
- `variables` - Variable declarations and assignments
- `function_definition` - Function definitions and calls
- `loops` - For loops, while loops
- `class_definition` - Class definition and instantiation
- `oop_demo` - Object-oriented programming

**Purpose:** Verify fundamental language features work correctly

#### Category B: Module System

**Tests:**
- `dependency_test` - External dependency loading
- `package` - Package declarations
- `imports` - Import statements
- `prelude_demo` - Prelude (stdlib) functions

**Purpose:** Verify module loading, dependency management, and standard library

#### Category C: Advanced Features

**Tests:**
- `math_lib` - Math operations and native functions
- Any additional complex samples

**Purpose:** Verify advanced language features and integrations

#### Category D: Skipped/Manual Tests

**Projects not automatically tested:**
- `empty_project` - Template, no executable code
- `comments_and_documentation` - Documentation only
- `app_config_options` - Configuration demo
- `enum_type` - May be incomplete
- `expect` - Test framework demo
- `interface_demo` - May be incomplete
- `struct_type` - May be incomplete
- `stdlib_examples` - May have special setup
- `threads` - Requires special runtime support
- Single `.st` files - Not projects

**Reason:** These require manual testing or have special requirements

---

## Testing Workflow

### Before Making Changes

1. **Run baseline tests:**
   ```bash
   ./test-all.sh > baseline-results.txt
   ```

2. **Verify all tests pass:**
   - Should show 100% pass rate
   - Exit code should be 0

### After Making Changes

1. **Rebuild and test:**
   ```bash
   ./test-all.sh
   ```

2. **Compare results:**
   - Check that pass count didn't decrease
   - Investigate any new failures

3. **If tests fail:**
   ```bash
   # Run with verbose output
   ./test-all.sh -v

   # Or test specific failing sample
   ./test-all.sh --test failing_sample -v
   ```

### Before Committing

1. **Run full test suite:**
   ```bash
   ./test-all.sh
   ```

2. **Verify 100% pass rate**

3. **Commit with test results:**
   ```bash
   git add .
   git commit -m "Your changes

   Tests: All passing (X/X)"
   ```

---

## Manual Testing Checklist

For changes that require manual verification:

### Interpreter Changes
- [ ] Basic program execution works
- [ ] Error messages are clear
- [ ] Exit codes are correct
- [ ] Memory management is sound (no leaks/crashes)

### Parser/Lexer Changes
- [ ] Valid syntax is accepted
- [ ] Invalid syntax is rejected with clear errors
- [ ] Edge cases are handled

### Semantic Analyzer Changes
- [ ] Type checking works correctly
- [ ] Variable scope is enforced
- [ ] Function calls are validated

### Runtime/Interpreter Changes
- [ ] Native functions work
- [ ] Module loading works
- [ ] Class instantiation works
- [ ] Error handling works

---

## Adding New Tests

### 1. Create Sample Project

```bash
cd samples
mkdir my_new_feature
cd my_new_feature
mkdir src
```

### 2. Create Test File

```bash
cat > src/main.st << 'EOF'
fn main() {
    // Your test code here
    println("Test passed!");
}
EOF
```

### 3. Add to Test Suite

Edit `test-all.sh` and add to `RUNNABLE_SAMPLES`:

```bash
RUNNABLE_SAMPLES=(
    "hello_world"
    "dependency_test"
    # ... existing tests ...
    "my_new_feature"  # ADD HERE
)
```

### 4. Run Test

```bash
./test-all.sh --test my_new_feature -v
```

---

## Continuous Integration (Future)

### GitHub Actions Workflow (Recommended)

```yaml
name: Test Stratos

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y g++ cmake
      - name: Run tests
        run: ./test-all.sh
```

### Pre-commit Hook ✅ **ACTIVE**

**Status:** Pre-commit hook is installed and working!

**Location:** `.git/hooks/pre-commit`

**What it does:**
- Runs automatically when you `git commit`
- Executes `./test-all.sh --no-rebuild`
- If tests pass → commit succeeds
- If tests fail → commit is aborted

**Usage:**
```bash
# Just commit normally
git commit -m "Your changes"

# Hook runs automatically
# Tests must pass for commit to succeed
```

**Bypass (emergency only):**
```bash
git commit --no-verify -m "Emergency fix"
```

**Documentation:** See `GIT_HOOKS_SETUP.md` for full details

---

## Performance Testing

### Benchmark Runner (Future Enhancement)

Create `benchmark.sh` to measure:
- Compilation time
- Execution time
- Memory usage

Track regressions over time.

---

## Test Coverage Goals

### Current Status
- ✅ Core language features covered
- ✅ Module system covered
- ✅ Dependency management covered
- ✅ OOP features covered
- ⚠️ Error handling needs more tests
- ⚠️ Edge cases need more coverage

### Future Goals
- Add negative test cases (should fail)
- Add performance benchmarks
- Add memory leak tests
- Add concurrent execution tests
- Add larger integration tests

---

## Debugging Test Failures

### 1. Run Test Verbose

```bash
./test-all.sh --test failing_test -v
```

### 2. Run Manually

```bash
cd samples/failing_test
../../interpreter/C++/build/stratos run src/main.st -v
```

### 3. Check Build Artifacts

```bash
# Verify binary was rebuilt
ls -lh interpreter/C++/build/stratos

# Check build logs
cat /tmp/stratos-build.log
```

### 4. Use Debugger (if needed)

```bash
cd samples/failing_test
gdb ../../interpreter/C++/build/stratos

# In GDB:
(gdb) run run src/main.st
(gdb) backtrace
```

---

## Test Maintenance

### Weekly
- Run full test suite
- Verify all tests still pass
- Update test list if samples change

### Before Release
- Run tests on clean checkout
- Test on different platforms (if applicable)
- Verify documentation is up to date
- Check for performance regressions

### After Major Changes
- Review test coverage
- Add tests for new features
- Update skipped test list
- Document any new test requirements

---

## Summary

**Before ANY commit to the compiler:**
```bash
./test-all.sh
```

**If all tests pass (exit code 0):**
- ✅ Safe to commit
- ✅ No breaking changes detected

**If any test fails:**
- ❌ DO NOT commit
- ❌ Fix the issue first
- ❌ Re-run tests until all pass

**Remember:** The test suite is your safety net. Use it liberally!
