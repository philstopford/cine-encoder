#!/bin/bash
# macOS build and deployment script for Cine Encoder
# This script helps developers build and package the application locally

set -e

echo "Building Cine Encoder for macOS..."

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check if Qt6 is installed
echo "Checking for Qt6 installation..."
QT6_FOUND=false

# Check for qmake6 first (preferred)
if command_exists qmake6; then
    QT6_PATH=$(dirname $(which qmake6))
    QT6_DIR=$(dirname "$QT6_PATH")
    QT6_FOUND=true
    echo "✓ Found Qt6 via qmake6 at: $QT6_DIR"
elif command_exists qmake; then
    # Check if this qmake is Qt6
    QT_VERSION=$(qmake -query QT_VERSION 2>/dev/null || echo "")
    if [[ "$QT_VERSION" =~ ^6\. ]]; then
        QT6_PATH=$(dirname $(which qmake))
        QT6_DIR=$(dirname "$QT6_PATH")
        QT6_FOUND=true
        echo "✓ Found Qt6 via qmake at: $QT6_DIR (version: $QT_VERSION)"
    fi
fi

# Try common Homebrew locations if not found
if [ "$QT6_FOUND" = false ]; then
    for qt_path in /opt/homebrew/opt/qt@6 /usr/local/opt/qt@6 /opt/homebrew/opt/qt6 /usr/local/opt/qt6; do
        if [ -d "$qt_path" ] && [ -f "$qt_path/bin/qmake" ]; then
            QT6_DIR="$qt_path"
            QT6_FOUND=true
            echo "✓ Found Qt6 at: $QT6_DIR"
            break
        fi
    done
fi

# Try environment variable
if [ "$QT6_FOUND" = false ] && [ -n "$Qt6_DIR" ] && [ -f "$Qt6_DIR/bin/qmake" ]; then
    QT6_DIR="$Qt6_DIR"
    QT6_FOUND=true
    echo "✓ Found Qt6 via Qt6_DIR at: $QT6_DIR"
fi

if [ "$QT6_FOUND" = false ]; then
    echo "Error: Qt6 not found. Please install Qt6 first."
    echo "You can install it via brew: brew install qt@6"
    echo "Or set Qt6_DIR environment variable to Qt6 installation directory"
    exit 1
fi

# Verify Qt6 modules
echo "Verifying Qt6 modules..."
SVG_FOUND=false
MULTIMEDIA_FOUND=false

# Check for SVG module
if [ -f "$QT6_DIR/lib/QtSvg.framework/QtSvg" ] || [ -f "$QT6_DIR/lib/libQt6Svg.dylib" ]; then
    SVG_FOUND=true
    echo "✓ Qt6 SVG module found"
else
    echo "⚠ Qt6 SVG module not found"
fi

# Check for Multimedia module  
if [ -f "$QT6_DIR/lib/QtMultimedia.framework/QtMultimedia" ] || [ -f "$QT6_DIR/lib/libQt6Multimedia.dylib" ]; then
    MULTIMEDIA_FOUND=true
    echo "✓ Qt6 Multimedia module found"
else
    echo "⚠ Qt6 Multimedia module not found"
fi

if [ "$SVG_FOUND" = false ] || [ "$MULTIMEDIA_FOUND" = false ]; then
    echo "Error: Required Qt6 modules missing"
    echo "Please install Qt6 with SVG and Multimedia modules"
    echo "Homebrew: brew install qt@6"
    exit 1
fi

# Add Qt6 to PATH
export PATH="$QT6_DIR/bin:$PATH"
export Qt6_DIR="$QT6_DIR"

# Check if MediaInfo is installed
if ! command -v mediainfo &> /dev/null; then
    echo "MediaInfo not found. Installing MediaInfo..."
    if command -v brew &> /dev/null; then
        brew install mediainfo
    else
        echo "Error: Homebrew not found. Please install MediaInfo manually or install Homebrew"
        exit 1
    fi
fi

# Verify MediaInfo installation
if command -v mediainfo &> /dev/null; then
    echo "✓ MediaInfo found: $(mediainfo --version | head -1)"
else
    echo "Error: MediaInfo installation failed"
    exit 1
fi

# Set up build directory
echo "Setting up build directory..."
rm -rf build
mkdir build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_PREFIX_PATH="$QT6_DIR" -DQt6_DIR="$QT6_DIR" ..

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