#!/bin/bash
# Quick test script to verify Qt6 detection and basic build configuration
# This script tests the CMake configuration without doing a full build

set -e

echo "Testing Qt6 detection and CMake configuration..."
echo "=============================================="

# Create a temporary build directory
TEST_DIR="$(mktemp -d)"
echo "Using temporary directory: $TEST_DIR"

# Test CMake configuration
echo ""
echo "Running CMake configuration test..."
if cmake -B "$TEST_DIR" -S . -DCMAKE_BUILD_TYPE=Release; then
    echo ""
    echo "✓ CMake configuration successful!"
    echo "✓ Qt6 components detected correctly"
    echo "✓ Build configuration ready"
else
    echo ""
    echo "✗ CMake configuration failed"
    echo "Please check that Qt6 and its dependencies are installed"
    exit 1
fi

# Clean up
rm -rf "$TEST_DIR"

echo ""
echo "Test completed successfully! The build configuration is working."
echo ""
echo "To do a full build, run:"
echo "  mkdir build && cd build"
echo "  cmake -DCMAKE_BUILD_TYPE=Release .."
echo "  cmake --build . --config Release"