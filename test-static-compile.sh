#!/bin/bash
# Test script for static compilation feature

cd /home/kamau/Development/Projects/stratos

echo "=========================================="
echo "Testing Stratos Static Compilation"
echo "=========================================="
echo ""

# Test 1: Compile with default optimization (-O2)
echo "[Test 1] Compiling with default optimization (-O2)..."
./src/interpreter/cpp/build/stratos compile ./examples/test-static-compile/ -v

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Compilation successful!"
    echo ""

    # Show binary info
    echo "Binary information:"
    file ./examples/test-static-compile/build/variables
    echo ""

    # Show size
    echo "Binary size:"
    ls -lh ./examples/test-static-compile/build/variables
    echo ""

    # Check dependencies
    echo "Dependency check (should be static):"
    ldd ./examples/test-static-compile/build/variables 2>&1 | head -10
    echo ""

    # Run the binary
    echo "Running the compiled binary:"
    ./examples/test-static-compile/build/variables
    echo ""

else
    echo "✗ Compilation failed!"
    echo ""
fi

# Test 2: Compile with size optimization
echo "=========================================="
echo "[Test 2] Compiling with size optimization (-Os)..."
./src/interpreter/cpp/build/stratos compile ./examples/test-static-compile/ -Os -o ./examples/test-static-compile/build/variables-Os

if [ $? -eq 0 ]; then
    echo "✓ Size-optimized compilation successful!"
    ls -lh ./examples/test-static-compile/build/variables-Os
    echo ""
else
    echo "✗ Size-optimized compilation failed!"
    echo ""
fi

echo "=========================================="
echo "Testing complete!"
echo "=========================================="
