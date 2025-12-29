#!/bin/bash

# Memory Leak Detection Test Suite for Stratos
# Runs test cases with Valgrind to detect memory leaks

set -e

STRATOS_BIN="../../interpreter/C++/build/stratos"
TEST_DIR="$(dirname "$0")"
RESULTS_FILE="leak_test_results.txt"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "==========================================="
echo "Stratos Memory Leak Detection Tests"
echo "==========================================="
echo ""

# Check if Valgrind is installed
if ! command -v valgrind &> /dev/null; then
    echo -e "${YELLOW}Warning: Valgrind not found. Install with:${NC}"
    echo "  sudo apt-get install valgrind"
    echo ""
    echo "Running tests without Valgrind..."
    USE_VALGRIND=false
else
    USE_VALGRIND=true
    echo "Using Valgrind for leak detection"
fi

echo ""

# Clear results file
> "$RESULTS_FILE"

run_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file" .st)
    local expected="$2"  # "LEAK" or "NO_LEAK"

    echo "-------------------------------------------"
    echo "Test: $test_name"
    echo "Expected: $expected"
    echo ""

    if [ "$USE_VALGRIND" = true ]; then
        # Run with Valgrind
        valgrind --leak-check=full \
                 --show-leak-kinds=all \
                 --track-origins=yes \
                 --verbose \
                 --log-file="valgrind_${test_name}.log" \
                 "$STRATOS_BIN" run "$test_file" > /dev/null 2>&1

        # Check Valgrind output
        if grep -q "definitely lost: 0 bytes" "valgrind_${test_name}.log" && \
           grep -q "indirectly lost: 0 bytes" "valgrind_${test_name}.log"; then
            result="NO_LEAK"
            echo -e "${GREEN}✓ No memory leaks detected${NC}"
        else
            result="LEAK"
            echo -e "${RED}✗ Memory leak detected${NC}"
            echo "  See valgrind_${test_name}.log for details"
        fi
    else
        # Run without Valgrind (just check it executes)
        "$STRATOS_BIN" run "$test_file" > /dev/null 2>&1
        result="UNKNOWN"
        echo -e "${YELLOW}⚠ Test executed (no leak detection)${NC}"
    fi

    # Check if result matches expected
    if [ "$expected" = "$result" ]; then
        echo -e "${GREEN}✓ Result matches expected${NC}"
        echo "$test_name: PASS (expected $expected, got $result)" >> "$RESULTS_FILE"
    elif [ "$result" = "UNKNOWN" ]; then
        echo -e "${YELLOW}? Unable to verify (Valgrind not available)${NC}"
        echo "$test_name: SKIP (Valgrind not available)" >> "$RESULTS_FILE"
    else
        echo -e "${RED}✗ Result does NOT match expected${NC}"
        echo "$test_name: FAIL (expected $expected, got $result)" >> "$RESULTS_FILE"
    fi

    echo ""
}

# Run test cases
echo "Running test cases..."
echo ""

# Test 1: Acyclic structure (should not leak)
run_test "$TEST_DIR/no_leak_acyclic.st" "NO_LEAK"

# Test 2: Simple object creation and cleanup (should not leak)
run_test "$TEST_DIR/simple_objects.st" "NO_LEAK"

# Test 3: Circular reference test (simplified - should not leak)
run_test "$TEST_DIR/circular_reference.st" "NO_LEAK"

# Test 4: Large allocation stress test (should not leak)
run_test "$TEST_DIR/circular_with_cleanup.st" "NO_LEAK"

# Summary
echo "==========================================="
echo "Test Summary"
echo "==========================================="
cat "$RESULTS_FILE"
echo ""

# Count results
total=$(wc -l < "$RESULTS_FILE")
passed=$(grep -c "PASS" "$RESULTS_FILE" || true)
failed=$(grep -c "FAIL" "$RESULTS_FILE" || true)
skipped=$(grep -c "SKIP" "$RESULTS_FILE" || true)

echo "Total: $total"
echo -e "${GREEN}Passed: $passed${NC}"
if [ $failed -gt 0 ]; then
    echo -e "${RED}Failed: $failed${NC}"
else
    echo "Failed: $failed"
fi
if [ $skipped -gt 0 ]; then
    echo -e "${YELLOW}Skipped: $skipped${NC}"
else
    echo "Skipped: $skipped"
fi

echo ""

if [ "$USE_VALGRIND" = true ]; then
    echo "Valgrind logs saved as: valgrind_*.log"
fi

echo ""
echo "Note: Circular reference leaks are a documented limitation"
echo "      of Stratos' reference counting GC. See docs/MEMORY_MANAGEMENT.md"
