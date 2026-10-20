# Cine Encoder

Cine Encoder is an application, using the FFmpeg, MKVToolNix and MediaInfo utilities, that allows you to convert media files while preserving HDR metadata. Supported hardware encoding includes Apple VideoToolbox (on macOS / Apple Silicon), NVIDIA NVENC, and Intel QSV (on Windows and experimental on Linux). The following encoding modes are implemented: H265, H264, AV1, VP9, MPEG-2, XDCAM, XAVC, DNxHR, and ProRes.

![View](./images/View_1.png)

By default, there are several categories of presets. You can change existing or add your own presets.

![View](./images/View_2.png)

### Why fork?

The original project seems to have stalled a few years ago. I wrote and submitted a bunch of pull requests that have been languishing. I fear that the original developer got impacted by the Ukraine conflict and rather than let this wonderful project sit unloved, I decided to fork it and do my best not to ruin their original vision, while bringing it up to date and adding some features and fixes that I felt were important.
This fork brings modern cross-platform support across Linux (x86_64), macOS (Apple Silicon / Intel), and Windows (x86_64).

### Differences in this Fork

- Use of CMake over QMake.
- Native Apple Silicon (macOS ARM64) and Intel macOS support, including Apple VideoToolbox hardware-accelerated encoding (`hevc_videotoolbox`, `h264_videotoolbox`, `prores_videotoolbox`) with quality-based (`-q:v`) rate control.
- Adoption of Qt6 for user interface and removal of hacks around high DPI scaling, with proper macOS Retina font rendering.
- Wayland-ready on Linux, avoiding XWayland.
- Use of XML files for presets and preferences, avoiding loss of preset information when tool feature set is changed.
- Enhanced preset features that work together with preference options.
- Dynamic filtering of hardware presets and codecs based on host OS compatibility.
- Extracting audio tracks maintains metadata if destination format supports it.
- Ability to hard burn subtitles with customizable background color, opacity, text appearance, etc.
- Warning in case transcode settings are incompatible with target format, with advice on how to mitigate (e.g. transcode audio, hard-burn subtitles, etc.)
- Ability to dynamically adjust task priority for background ffmpeg sessions to help maintain usability.
- Improved error handling for ffmpeg issues.
- Improved handling for filenames, streams, etc. that have special characters.
- Single-instance lock protection using `QLockFile`.
- Heavily refactored code to make it more understandable and maintainable. Addressed numerous weaknesses in the original design.

### Installation & Supported Platforms

- **Linux** (x86_64): AppImage, native `.deb` (Ubuntu/Debian), and native `.pkg.tar.zst` (Arch Linux).
- **macOS** (Apple Silicon / Intel): Native app bundle (`.app`) and packaged `.dmg`.
- **Windows** (x86_64): Portable `.zip` with bundled runtime dependencies.

---

### Build Instructions for macOS

#### 1. Prerequisites (Homebrew)
Install CMake, Qt6, MediaInfo, and runtime utilities using [Homebrew](https://brew.sh/):

```bash
# Build & library dependencies
brew install cmake qt@6 mediainfo

# Runtime dependencies (used by Cine Encoder for encoding, muxing, and analysis)
brew install ffmpeg mkvtoolnix

# Optional (for building DMG disk image packages)
brew install create-dmg
```

#### 2. Building from Command Line
You can build with standard CMake:

```bash
# Set Qt6 path for CMake
export CMAKE_PREFIX_PATH="$(brew --prefix qt@6):$CMAKE_PREFIX_PATH"

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(sysctl -n hw.ncpu)
```

#### 3. Automated Packaging (DMG & App Bundle)
To build a standalone `.app` bundle with Qt frameworks deployed and generate a `.dmg` installer:

```bash
./build-macos.sh
```

#### 4. Building with CLion
1. Open the repository root directory in CLion.
2. In **Settings / Preferences** → **Build, Execution, Deployment** → **CMake**, add the Qt6 prefix to **CMake options**:
   ```text
   -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt@6
   ```
   *(or `/usr/local/opt/qt@6` for Intel Macs)*.
3. Build the `cine_encoder` target.

---

### Build Instructions for Linux
In general, you need `mkvtoolnix` (the terminal version is fine) and `mediainfo`. The GUI calls `ffmpeg` at runtime. Qt6 development libraries (base, svg, multimedia) are required for building.

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install qt6-base-dev qt6-svg-dev qt6-multimedia-dev libmediainfo-dev cmake build-essential ffmpeg mkvtoolnix

# Install dependencies (Arch Linux)
sudo pacman -S qt6-base qt6-svg qt6-multimedia libmediainfo cmake base-devel ffmpeg mkvtoolnix

# Install dependencies (Fedora/RHEL)
sudo dnf install qt6-qtbase-devel qt6-qtsvg-devel qt6-qtmultimedia-devel libmediainfo-devel cmake gcc-c++ ffmpeg mkvtoolnix

# Build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

---

### Build Instructions for Windows
See `build-windows.bat` and `BUILD.md` for building and packaging on Windows x64.

---

### Code Quality & Architecture

I have used Copilot from time to time on this project to help with some of the tedious work. I use this tool ~daily and am reasonably confident that nothing is broken, but I have not used all features in the tool.

This project follows modern C++ best practices and Qt coding standards. Recent improvements include:

- **Namespace Hygiene**: Removed `using namespace` directives from header files to prevent namespace pollution
- **Type Safety**: Replaced preprocessor macros with `constexpr` variables and inline functions for better type checking
- **Documentation**: Comprehensive Doxygen-style documentation for all public APIs
- **Architecture**: Separation of concerns with dedicated service layers (see [ARCHITECTURE_IMPROVEMENTS.md](ARCHITECTURE_IMPROVEMENTS.md))
- **Testing**: Unit test infrastructure with Qt Test framework
- **Error Handling**: Centralized error handling and logging systems

For detailed information about architectural improvements, see [ARCHITECTURE_IMPROVEMENTS.md](ARCHITECTURE_IMPROVEMENTS.md).

### Licence

GNU GPL v.3
See [LICENSE.md](https://github.com/CineEncoder/CineEncoder/blob/master/LICENSE)
