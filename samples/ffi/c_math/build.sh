#!/bin/bash
# Build script for mathlib C library

echo "Building mathlib.so..."

gcc -shared -fPIC -o libmathlib.so mathlib.c

if [ $? -eq 0 ]; then
    echo "✓ Build successful! Created libmathlib.so"
    ls -lh libmathlib.so
else
    echo "✗ Build failed"
    exit 1
fi
