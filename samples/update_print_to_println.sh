#!/bin/bash

# Script to update all print() calls to println() for backwards compatibility

find . -name "*.st" -type f -exec sed -i 's/print(/println(/g' {} \;

echo "Updated all print() calls to println()"
