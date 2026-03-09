# Run Stratos Tests

Run the test suite or specific tests for the Stratos project.

## Arguments
- $ARGUMENTS: Optional test name, category, or flags (e.g., "hello-world", "--verbose", "--all", "--memory")

## Instructions

1. Parse $ARGUMENTS for:
   - Specific test name (maps to an example directory)
   - `--verbose` or `-v` for detailed output
   - `--all` to run all examples
   - `--memory` to run memory/GC tests
   - `--devtools` to run devtools tests

2. **Run all tests:**
   ```bash
   cd /home/kamau/Development/Projects/stratos/scripts && bash test-all.sh
   ```

3. **Run a specific example as test:**
   ```bash
   ./src/build/stratos run ./examples/<test-name>/
   ```

4. **Run memory tests:**
   Look in `tests/memory/` for memory-related test files and run them.

5. **Run the batch test script for all examples:**
   ```bash
   cd examples && bash run-all-examples.sh
   ```

6. **Quick validation (no codegen):**
   ```bash
   ./src/build/stratos check ./examples/<test-name>/src/main.st
   ```

7. Report results:
   - Number of tests passed/failed
   - Any error output from failed tests
   - Suggestions for fixing failures

8. If the stratos binary is not found, suggest running `/build` first.
