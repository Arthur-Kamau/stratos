#!/bin/bash
# Stratos Comprehensive Test Runner
# This script rebuilds the interpreter and runs all sample tests

set -e  # Exit on first error (can be disabled for full test run)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"  # Go up one level since we're in scripts/

# Support both old and new directory structures
if [ -d "$PROJECT_ROOT/src/interpreter/cpp" ]; then
    INTERPRETER_DIR="$PROJECT_ROOT/src/interpreter/cpp"
else
    INTERPRETER_DIR="$PROJECT_ROOT/interpreter/C++"
fi

BUILD_DIR="$INTERPRETER_DIR/build"

# Support both old (samples) and new (examples) directory names
if [ -d "$PROJECT_ROOT/examples" ]; then
    SAMPLES_DIR="$PROJECT_ROOT/examples"
else
    SAMPLES_DIR="$PROJECT_ROOT/samples"
fi

STRATOS_BIN="$BUILD_DIR/stratos"

# Test results
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0
FAILED_TESTS=()

# Parse command line arguments
REBUILD=true
VERBOSE=false
STOP_ON_FAIL=false
RUN_SPECIFIC=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --no-rebuild)
            REBUILD=false
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        --stop-on-fail)
            STOP_ON_FAIL=true
            shift
            ;;
        --test)
            RUN_SPECIFIC="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --no-rebuild      Skip rebuilding the interpreter"
            echo "  -v, --verbose     Show detailed output"
            echo "  --stop-on-fail    Stop on first test failure"
            echo "  --test <name>     Run only specific test"
            echo "  -h, --help        Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Print header
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Stratos Comprehensive Test Suite${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Step 1: Rebuild the interpreter
if [ "$REBUILD" = true ]; then
    echo -e "${YELLOW}[1/3] Building interpreter...${NC}"
    cd "$INTERPRETER_DIR"
    if bash build.sh > /tmp/stratos-build.log 2>&1; then
        echo -e "${GREEN}✓ Build successful${NC}"
        if [ "$VERBOSE" = true ]; then
            tail -5 /tmp/stratos-build.log
        fi
    else
        echo -e "${RED}✗ Build failed${NC}"
        cat /tmp/stratos-build.log
        exit 1
    fi
    echo ""
else
    echo -e "${YELLOW}[1/3] Skipping rebuild (--no-rebuild)${NC}"
    echo ""
fi

# Step 2: Define test suites
echo -e "${YELLOW}[2/3] Discovering tests...${NC}"

# Sample projects that should run successfully
RUNNABLE_SAMPLES=(
    "hello_world"
    "dependency_test"
    "function_definition"
    "oop_demo"
    "class_definition"
    "loops"
    "variables"
    "prelude_demo"
    "package"
)

# Sample projects that are expected to fail or have specific requirements
SKIP_SAMPLES=(
    "empty_project"          # No main function
    "comments_and_documentation"  # Documentation only
    "app_config_options"     # Config demo only
    "enum_type"              # May not have main
    "expect"                 # Test framework demo
    "interface_demo"         # May not be complete
    "struct_type"            # May not be complete
    "stdlib_examples"        # No entry point
    "threads"                # No entry point
    "math_lib"               # No entry point - library only
    "imports"                # May have issues
    "ffi"                    # FFI tests directory (contains subdirs)
    "ffi-c-math"             # Requires building .so library first
    "ffi-cpp-string"         # Requires building .so library first
    "type_checking_test.st"  # Single file, not a project
    "test_prelude.st"        # Single file, not a project
    "PRELUDE_README.md"      # Documentation
    "update_print_to_println.sh"  # Script, not a sample
)

if [ -n "$RUN_SPECIFIC" ]; then
    RUNNABLE_SAMPLES=("$RUN_SPECIFIC")
    echo -e "${BLUE}Running specific test: $RUN_SPECIFIC${NC}"
fi

echo -e "${GREEN}Found ${#RUNNABLE_SAMPLES[@]} tests to run${NC}"
echo ""

# Step 3: Run tests
echo -e "${YELLOW}[3/3] Running tests...${NC}"
echo ""

run_test() {
    local sample=$1
    local sample_path="$SAMPLES_DIR/$sample"

    # If directory doesn't exist, try converting snake_case to kebab-case
    if [ ! -d "$sample_path" ]; then
        local kebab_name=$(echo "$sample" | tr '_' '-')
        local kebab_path="$SAMPLES_DIR/$kebab_name"

        if [ -d "$kebab_path" ]; then
            sample_path="$kebab_path"
            sample="$kebab_name"
        else
            echo -e "${RED}✗ $sample - Directory not found${NC}"
            echo "  Tried: $SAMPLES_DIR/$sample"
            echo "  Tried: $kebab_path"
            return 1
        fi
    fi

    # Find entry point
    local entry_point=""
    if [ -f "$sample_path/src/main.st" ]; then
        entry_point="$sample_path/src/main.st"
    elif [ -f "$sample_path/main.st" ]; then
        entry_point="$sample_path/main.st"
    else
        echo -e "${RED}✗ $sample - No entry point found${NC}"
        return 1
    fi

    # Run the test
    TESTS_RUN=$((TESTS_RUN + 1))

    if [ "$VERBOSE" = true ]; then
        echo -e "${BLUE}Running: $sample${NC}"
        echo "  Entry: $entry_point"
    fi

    # Run with timeout to prevent hanging
    local output
    local exit_code

    cd "$sample_path"
    output=$(timeout 10s "$STRATOS_BIN" run "$entry_point" 2>&1)
    exit_code=$?

    if [ $exit_code -eq 0 ]; then
        echo -e "${GREEN}✓ $sample${NC}"
        TESTS_PASSED=$((TESTS_PASSED + 1))
        if [ "$VERBOSE" = true ]; then
            echo "$output" | sed 's/^/  /'
            echo ""
        fi
        return 0
    else
        echo -e "${RED}✗ $sample (exit code: $exit_code)${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
        FAILED_TESTS+=("$sample")

        if [ "$VERBOSE" = true ] || [ "$STOP_ON_FAIL" = true ]; then
            echo "  Output:"
            echo "$output" | sed 's/^/    /'
            echo ""
        fi

        if [ "$STOP_ON_FAIL" = true ]; then
            exit 1
        fi
        return 1
    fi
}

# Run all tests
for sample in "${RUNNABLE_SAMPLES[@]}"; do
    run_test "$sample"
done

# Print summary
echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Test Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total tests run:    ${TESTS_RUN}"
echo -e "${GREEN}Passed:             ${TESTS_PASSED}${NC}"
echo -e "${RED}Failed:             ${TESTS_FAILED}${NC}"
echo ""

if [ ${#FAILED_TESTS[@]} -gt 0 ]; then
    echo -e "${RED}Failed tests:${NC}"
    for test in "${FAILED_TESTS[@]}"; do
        echo -e "  ${RED}✗${NC} $test"
    done
    echo ""
fi

# Calculate pass rate
if [ $TESTS_RUN -gt 0 ]; then
    PASS_RATE=$((TESTS_PASSED * 100 / TESTS_RUN))
    echo -e "Pass rate: ${PASS_RATE}%"
fi

echo ""

# Exit with appropriate code
if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed! ✓${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed ✗${NC}"
    exit 1
fi
