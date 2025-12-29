# Stratos Memory Leak Detection Tests

This directory contains test cases and tools for detecting memory leaks in Stratos programs.

## Overview

Stratos uses reference counting for garbage collection, which has a known limitation: **circular references cause memory leaks**. These tests help verify this behavior and ensure non-cyclic structures are properly freed.

## Test Cases

### 1. `no_leak_acyclic.st` - Tree Structure (No Leak Expected)

Creates an acyclic tree data structure. All nodes should be freed when the program exits.

**Expected Result**: No memory leaks

```stratos
val root = TreeNode(1);
root.left = TreeNode(2);
root.right = TreeNode(3);
// All freed when main exits
```

### 2. `circular_reference.st` - Circular Reference (Leak Expected)

Creates a circular reference between two objects.

**Expected Result**: Memory leak (documented limitation)

```stratos
val a = Node(1);
val b = Node(2);
a.next = b;
b.next = a;
// Both leak when main exits (refcount never reaches 0)
```

### 3. `circular_with_cleanup.st` - Manual Cleanup Pattern

Demonstrates the workaround pattern for circular references (conceptual).

**Expected Result**: Currently leaks (null not implemented)

## Running Tests

### Prerequisites

**Option 1: With Valgrind (Recommended)**

Install Valgrind for accurate leak detection:

```bash
sudo apt-get install valgrind
```

**Option 2: Without Valgrind**

Tests will run but won't detect leaks (only verifies execution).

### Run All Tests

```bash
cd tests/memory
./run_leak_tests.sh
```

### Sample Output

```
===========================================
Stratos Memory Leak Detection Tests
===========================================

Using Valgrind for leak detection

Running test cases...

-------------------------------------------
Test: no_leak_acyclic
Expected: NO_LEAK

✓ No memory leaks detected
✓ Result matches expected

-------------------------------------------
Test: circular_reference
Expected: LEAK

✗ Memory leak detected
  See valgrind_circular_reference.log for details
✓ Result matches expected

===========================================
Test Summary
===========================================
no_leak_acyclic: PASS (expected NO_LEAK, got NO_LEAK)
circular_reference: PASS (expected LEAK, got LEAK)
circular_with_cleanup: PASS (expected LEAK, got LEAK)

Total: 3
Passed: 3
Failed: 0
Skipped: 0
```

## Manual Testing with Valgrind

To manually test a specific file:

```bash
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         ../../interpreter/C++/build/stratos run your_test.st
```

### Understanding Valgrind Output

**No Leaks:**
```
HEAP SUMMARY:
    in use at exit: 0 bytes in 0 blocks
  total heap usage: 50 allocs, 50 frees, 1,234 bytes allocated

All heap blocks were freed -- no leaks are possible
```

**Memory Leak:**
```
HEAP SUMMARY:
    in use at exit: 64 bytes in 2 blocks
  total heap usage: 50 allocs, 48 frees, 1,234 bytes allocated

definitely lost: 64 bytes in 2 blocks
```

## Building with Address Sanitizer

For even more detailed memory error detection during development:

```bash
cd interpreter/C++
g++ -fsanitize=address -fsanitize=undefined -g \
    -o build/stratos_asan \
    src/**/*.cpp
```

Run tests:

```bash
ASAN_OPTIONS=detect_leaks=1 ./build/stratos_asan run tests/memory/circular_reference.st
```

## Creating New Test Cases

### Template for Leak Test

```stratos
package main;

// Description of what this test does
// EXPECTED: [LEAK | NO_LEAK]

class YourClass {
    var field: YourClass;
}

fn main() {
    // Your test code
}
```

### Adding to Test Suite

1. Create test file in `tests/memory/`
2. Add to `run_leak_tests.sh`:

```bash
run_test "$TEST_DIR/your_test.st" "[LEAK|NO_LEAK]"
```

## Interpreting Results

### Expected Leaks (Documented Limitations)

These patterns **will leak** in Stratos:

✗ **Circular References**
```stratos
a.next = b;
b.next = a;
```

✗ **Self-References**
```stratos
node.next = node;
```

✗ **Graph Structures**
```stratos
// Arbitrary connections between nodes
```

### No Leaks Expected

These patterns should **not leak**:

✓ **Trees** (parent → child only)
```stratos
root.left = child1;
root.right = child2;
```

✓ **Lists** (forward references only)
```stratos
head.next = node2;
node2.next = node3;
```

✓ **Temporary Objects**
```stratos
fn process() {
    val temp = Object();
    // Freed when function returns
}
```

## CI/CD Integration

Add to your CI pipeline:

```yaml
# .github/workflows/memory-tests.yml
- name: Install Valgrind
  run: sudo apt-get install -y valgrind

- name: Run Memory Leak Tests
  run: cd tests/memory && ./run_leak_tests.sh
```

## Troubleshooting

### Valgrind shows leaks in standard library

Some C++ standard library implementations have known "still reachable" memory. Focus on:
- "definitely lost" - real leaks
- "indirectly lost" - also real leaks

Ignore:
- "still reachable" - often library caches

### False Positives

If Valgrind reports leaks but you expect none:

1. Check if you have circular references
2. Verify all objects go out of scope
3. Check for global variables

### Performance Impact

Valgrind slows down execution 10-50x. This is normal.

## References

- [Stratos Memory Management Guide](../../docs/MEMORY_MANAGEMENT.md)
- [Valgrind Documentation](https://valgrind.org/docs/manual/quick-start.html)
- [AddressSanitizer Documentation](https://github.com/google/sanitizers/wiki/AddressSanitizer)

## Future Improvements

Planned enhancements:
- Weak reference support to break cycles
- Automated cycle detection with warnings
- Mark-and-sweep GC for optional cycle collection
- Memory profiling tools
