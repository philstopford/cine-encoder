# Windows Build and Packaging

This document describes the Windows build process for Cine Encoder using GitHub Actions.

## Build Requirements

The Windows build is configured in `.github/workflows/build.yml` and requires:

### Qt6 Components
- Qt 6.7.2 (MinGW 64-bit)
- Qt6 Core, Gui, Widgets (standard components)
- Qt6 Multimedia (for media processing)
- Qt6 Network (dependency of other components)

### MediaInfo Library
- MediaInfo DLL version 24.06 or compatible
- Downloaded automatically from MediaArea during build
- Required for media file analysis

## Build Process

The Windows build process:

1. **Environment Setup**: Installs Qt6 with MinGW compiler
2. **MediaInfo Setup**: Downloads and configures MediaInfo library
3. **Build**: Uses CMake with MinGW Makefiles generator
4. **Packaging**: Uses `windeployqt` to collect all dependencies

## Runtime Dependencies

The packaged Windows application includes all required runtime dependencies:

### Qt6 Runtime Libraries
- `Qt6Core.dll` - Core Qt functionality
- `Qt6Gui.dll` - GUI components
- `Qt6Widgets.dll` - Widget toolkit
- `Qt6Multimedia.dll` - Multimedia support
- `Qt6Network.dll` - Network functionality

### MinGW Runtime Libraries
- `libgcc_s_seh-1.dll` - GCC runtime
- `libstdc++-6.dll` - C++ standard library
- `libwinpthread-1.dll` - Threading support

### Qt6 Plugins
The following plugin folders are included:
- `iconengines/` - Icon rendering engines
- `imageformats/` - Image format support
- `platforms/` - Platform abstraction layer
- `styles/` - UI styling support

### MediaInfo Library
- `MediaInfo.dll` - Media file analysis

## Package Output

The build produces a ZIP archive: `cine-encoder-{version}-windows-x64.zip`

This archive contains:
- `cine_encoder.exe` - Main application
- All runtime DLLs listed above
- Qt6 plugin directories
- Ready to run without additional installation

## Local Development

For local Windows development, see the `build-windows.bat` script which provides similar functionality for local builds.

## Troubleshooting

### Common Issues

1. **Missing Qt6 modules**: Ensure qtmultimedia module is installed
2. **MediaInfo not found**: The build will warn but continue if MediaInfo is unavailable
3. **Missing DLLs at runtime**: `windeployqt` should handle this automatically

### Verification

The build process includes verification steps to ensure all critical components are present before packaging.