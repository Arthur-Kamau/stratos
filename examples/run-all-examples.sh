#!/usr/bin/env bash
# Run all Stratos examples
# Usage: ./run-all-examples.sh

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"
STRATOS_BIN="$PROJECT_ROOT/src/interpreter/cpp/build/stratos.exe"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Running All Stratos Examples ===${NC}\n"

# Check if stratos binary exists
if [ ! -f "$STRATOS_BIN" ]; then
    echo -e "${RED}Error: Stratos binary not found at $STRATOS_BIN${NC}"
    echo "Please build the interpreter first."
    exit 1
fi

# Counter for statistics
TOTAL=0
PASSED=0
FAILED=0
SKIPPED=0

# List of examples to skip (if any need to be skipped)
SKIP_EXAMPLES=(
    "dependency-test"
    "expect"
    "ffi_c_math"
    "ffi_cpp_string"
)

# Function to check if example should be skipped
should_skip() {
    local example="$1"
    for skip in "${SKIP_EXAMPLES[@]}"; do
        if [ "$example" = "$skip" ]; then
            return 0
        fi
    done
    return 1
}

# Find all example directories (those with src/main.st)
for example_dir in "$SCRIPT_DIR"/*; do
    if [ -d "$example_dir" ]; then
        example_name=$(basename "$example_dir")

        # Check if this example should be skipped
        if should_skip "$example_name"; then
            echo -e "${YELLOW}[SKIP]${NC} $example_name (requires special setup)"
            ((SKIPPED++))
            ((TOTAL++))
            continue
        fi

        # Check if it has a main.st file
        if [ -f "$example_dir/src/main.st" ]; then
            ((TOTAL++))
            echo -e "${BLUE}[RUN]${NC} Running example: $example_name"

            # Run the example and capture output
            if "$STRATOS_BIN" run "$example_dir/src/main.st" > /dev/null 2>&1; then
                echo -e "${GREEN}[PASS]${NC} $example_name\n"
                ((PASSED++))
            else
                echo -e "${RED}[FAIL]${NC} $example_name"
                echo "  Run manually to see error: $STRATOS_BIN run $example_dir/src/main.st\n"
                ((FAILED++))
            fi
        fi
    fi
done

# Print summary
echo -e "${BLUE}=== Summary ===${NC}"
echo -e "Total:   $TOTAL"
echo -e "${GREEN}Passed:  $PASSED${NC}"
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}Failed:  $FAILED${NC}"
else
    echo -e "Failed:  $FAILED"
fi
if [ $SKIPPED -gt 0 ]; then
    echo -e "${YELLOW}Skipped: $SKIPPED${NC}"
fi

# Exit with error code if any tests failed
if [ $FAILED -gt 0 ]; then
    exit 1
fi
