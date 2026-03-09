# Build the Stratos Compiler

Build or rebuild the Stratos C++ interpreter/compiler.

## Arguments
- $ARGUMENTS: Optional flags like "--clean" or "--verbose"

## Instructions

1. If `--clean` is passed, remove the build directory first:
   ```bash
   rm -rf src/build/runtime src/build/stratos src/build/libstratos_runtime.a src/build/sqlite3.o
   ```

2. Run the build script:
   ```bash
   cd /home/kamau/Development/Projects/stratos/src && bash build.sh
   ```

3. If the build succeeds, verify the binary:
   ```bash
   ./src/build/stratos --version
   ```

4. Report the build result including binary size and path.

5. If the build fails:
   - Read the error output carefully
   - Common issues:
     - Missing system libraries: `sudo apt install libssl-dev libpq-dev libmysqlclient-dev libhiredis-dev`
     - C++20 not supported: need GCC 10+ or Clang 12+
     - SQLite compilation issues: check `src/libs/sqlite/sqlite3.c` exists
   - Suggest fixes based on the error
