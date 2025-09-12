# Build Infrastructure

This document describes the build and deployment infrastructure for Cine Encoder across different platforms.

## Supported Platforms

- **Windows** (x64): Automated builds with ZIP packaging
- **macOS** (Intel/Apple Silicon): Automated builds with DMG packaging  
- **Linux** (x64): Automated builds with AppImage packaging
- **Ubuntu** (x64): Native .deb packages with proper dependencies
- **Arch Linux** (x64): Native .pkg.tar.zst packages for Arch/Manjaro

## GitHub Actions CI/CD

The repository includes automated build workflows:

### Build Workflow (`.github/workflows/build.yml`)
- Triggers on pushes to main/master branches and pull requests
- Builds for all platforms:
  - **Windows**: ZIP package with Qt6 + Multimedia deployment
  - **macOS**: DMG with universal binary and Qt6 + SVG deployment
  - **Linux**: AppImage with all dependencies bundled
  - **Ubuntu**: Native .deb packages with apt-compatible dependencies
  - **Arch Linux**: Native .pkg.tar.zst packages with pacman dependencies
- Uploads artifacts for testing

### Release Workflow (`.github/workflows/release.yml`)
- Triggers on version tags (e.g., `v3.5.5`)
- Builds release packages for all platforms
- Automatically creates GitHub releases with binaries

## Local Development

### Windows
```bash
# Run the automated build script
build-windows.bat
```

Requirements:
- Qt6 with Multimedia modules (MinGW or MSVC)
- MediaInfo library
- CMake 3.16+

For automated GitHub Actions builds, see `docs/WINDOWS_BUILD.md` for detailed information.

### macOS
```bash
# Run the automated build script
./build-macos.sh
```

Requirements:
- Xcode Command Line Tools
- Qt6 (install via `brew install qt@6`)
- MediaInfo (install via `brew install mediainfo`)
- CMake 3.16+

### Linux
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install qt6-base-dev qt6-svg-dev qt6-multimedia-dev libmediainfo-dev cmake build-essential

# Install dependencies (Arch Linux)  
sudo pacman -S qt6-base qt6-svg qt6-multimedia libmediainfo cmake base-devel

# Install dependencies (Fedora/RHEL)
sudo dnf install qt6-qtbase-devel qt6-qtsvg-devel qt6-qtmultimedia-devel libmediainfo-devel cmake gcc-c++

# Build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Multimedia Support

All builds include proper multimedia support through Qt6's Multimedia module:

- **Windows**: `Qt6Multimedia.dll` is deployed via `windeployqt --multimedia`
- **macOS**: Multimedia support is deployed via `macdeployqt`  
- **Linux**: Multimedia support is included in Qt6 packages

Note: SVG icon support is available on macOS and Linux through qt6-svg packages.

## Package Outputs

### Windows
- `cine-encoder-{version}-windows-x64.zip` - Portable application package
- Includes all Qt6 dependencies, MediaInfo library, and plugins
- Ready to run without installation
- Contains all required DLLs and plugin folders

### macOS  
- `cine-encoder-{version}-macos.dmg` - Disk image with app bundle
- Universal binary supporting Intel and Apple Silicon
- Includes all Qt6 dependencies via `macdeployqt`
- Drag-and-drop installation

### Linux
- `Cine_Encoder-{version}-x86_64.AppImage` - Portable application
- Includes all dependencies (Qt6, MediaInfo)
- Runs on most Linux distributions without installation
- Self-contained with desktop integration

### Ubuntu
- `cine-encoder-{version}-ubuntu-amd64.deb` - Native Debian package
- Proper apt dependency management (Qt6, MediaInfo)
- Installs system-wide with desktop integration
- Compatible with Ubuntu 20.04+ and derivatives

### Arch Linux  
- `cine-encoder-{version}-arch-x86_64.pkg.tar.zst` - Native Arch package
- Proper pacman dependency management
- Installs system-wide with desktop integration
- Compatible with Arch Linux, Manjaro, and derivatives

## Deployment Notes

1. **Multimedia Support**: All packages ensure multimedia functionality works correctly by including the appropriate Qt6 multimedia components.

2. **Dependencies**: Each platform package is self-contained with all required libraries bundled.

3. **File Associations**: Windows installer registers video file associations; other platforms rely on desktop environment configuration.

4. **Updates**: Future releases can be deployed by creating new version tags, triggering automatic builds and releases.

## Testing Deployment

After building locally or downloading from GitHub releases:

1. **Test video file opening** and encoding functionality  
2. **Check multimedia playback** works properly
3. **Confirm window management** (resize, minimize, etc.) functions
4. **Verify UI elements display correctly** in the application

The modernized Qt6 codebase ensures consistent behavior across all platforms while maintaining native look-and-feel.