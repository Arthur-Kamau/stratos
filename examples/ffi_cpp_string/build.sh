#!/bin/bash
# Build script for stringlib C++ library

echo "Building libstringlib.so..."

g++ -shared -fPIC -o libstringlib.so stringlib.cpp

if [ $? -eq 0 ]; then
    echo "✓ Build successful! Created libstringlib.so"
    ls -lh libstringlib.so
else
    echo "✗ Build failed"
    exit 1
fi
