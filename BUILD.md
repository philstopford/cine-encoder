# Build Infrastructure

This document describes the build and deployment infrastructure for Cine Encoder across different platforms.

## Supported Platforms

- **Windows** (x64): Automated builds with Inno Setup installer
- **macOS** (Intel/Apple Silicon): Automated builds with DMG packaging  
- **Linux** (x64): Automated builds with AppImage packaging

## GitHub Actions CI/CD

The repository includes automated build workflows:

### Build Workflow (`.github/workflows/build.yml`)
- Triggers on pushes to main/master branches and pull requests
- Builds for all three platforms
- Deploys Qt dependencies including **SVG support**
- Creates platform-specific packages
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
- Visual Studio 2022 (Community or higher)
- Qt6 with SVG and Multimedia modules
- CMake 3.16+

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

# Build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## SVG Icon Support

All builds include proper SVG support through Qt6's SVG module:

- **Windows**: `qsvg.dll` is deployed via `windeployqt --svg`
- **macOS**: `libqsvg.dylib` is deployed via `macdeployqt -svg`  
- **Linux**: SVG support is included in Qt6 packages

The application uses SVG icons from `app/resources/icons/svg/` which are embedded in the binary via Qt's resource system and will render correctly on all platforms with high DPI support.

## Package Outputs

### Windows
- `cine-encoder-{version}-windows-x64.exe` - Inno Setup installer
- Includes all Qt6 dependencies and MediaInfo library
- Registers file associations for video files
- Creates desktop shortcuts

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

## Deployment Notes

1. **SVG Support**: All packages ensure SVG icons render correctly by including the appropriate Qt6 image format plugins.

2. **Dependencies**: Each platform package is self-contained with all required libraries bundled.

3. **File Associations**: Windows installer registers video file associations; other platforms rely on desktop environment configuration.

4. **Updates**: Future releases can be deployed by creating new version tags, triggering automatic builds and releases.

## Testing Deployment

After building locally or downloading from GitHub releases:

1. **Verify SVG icons display correctly** in the application UI
2. **Test video file opening** and encoding functionality  
3. **Check multimedia playback** works properly
4. **Confirm window management** (resize, minimize, etc.) functions

The modernized Qt6 codebase ensures consistent behavior across all platforms while maintaining native look-and-feel.