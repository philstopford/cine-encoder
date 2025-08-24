#!/bin/bash
# macOS build and deployment script for Cine Encoder
# This script helps developers build and package the application locally

set -e

echo "Building Cine Encoder for macOS..."

# Check if Qt6 is installed
if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
    echo "Error: Qt6 not found. Please install Qt6 first."
    echo "You can install it via brew: brew install qt@6"
    exit 1
fi

# Check if MediaInfo is installed
if ! command -v mediainfo &> /dev/null; then
    echo "Installing MediaInfo..."
    brew install mediainfo
fi

# Set up build directory
echo "Setting up build directory..."
rm -rf build
mkdir build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 ..

# Build the application
echo "Building application..."
cmake --build . --config Release

cd ..

# Create deployment directory
echo "Creating deployment..."
rm -rf deploy
mkdir deploy

# Copy app bundle
cp -R build/cine_encoder.app deploy/

# Deploy Qt dependencies including SVG support
echo "Deploying Qt dependencies..."
macdeployqt deploy/cine_encoder.app -always-overwrite -svg

# Check if SVG plugin was deployed
if [ -f "deploy/cine_encoder.app/Contents/PlugIns/imageformats/libqsvg.dylib" ]; then
    echo "✓ SVG plugin deployed successfully - SVG icons will work"
else
    echo "⚠ WARNING: SVG plugin not found - SVG icons may not display correctly"
fi

# Create DMG (optional)
if command -v create-dmg &> /dev/null; then
    echo "Creating DMG..."
    create-dmg \
        --volname "Cine Encoder" \
        --window-pos 200 120 \
        --window-size 800 400 \
        --icon-size 100 \
        --icon "cine_encoder.app" 200 190 \
        --hide-extension "cine_encoder.app" \
        --app-drop-link 600 185 \
        "cine-encoder-macos.dmg" \
        "deploy/"
    echo "✓ DMG created: cine-encoder-macos.dmg"
else
    echo "Note: create-dmg not found. Install it with: brew install create-dmg"
fi

echo ""
echo "Build and deployment completed successfully!"
echo "You can run the application from: deploy/cine_encoder.app"
echo ""