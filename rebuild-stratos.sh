#!/bin/bash
# Quick rebuild script for Stratos compiler

echo "Rebuilding Stratos compiler..."
cd /home/kamau/Development/Projects/stratos/src/interpreter/cpp

./build.sh

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Build successful!"
    echo ""
    echo "To test the standalone compilation:"
    echo "  cd /home/kamau/Development/Projects/stratos"
    echo "  ./src/interpreter/cpp/build/stratos compile ./examples/test-static-compile/ -v"
    echo ""
else
    echo ""
    echo "✗ Build failed - check errors above"
    echo ""
fi
