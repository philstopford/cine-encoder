# Cine Encoder

Cine Encoder is a desktop media converter built around FFmpeg, MKVToolNix, and MediaInfo. It can transcode video and audio, copy source streams, extract audio, and burn subtitles into video. Its preset options include H.265/HEVC, H.264/AVC, SVT-AV1, VP9, MPEG-2, XDCAM, XAVC, DNxHR, and ProRes. Supported 10-bit and professional formats offer HDR metadata options; HDR preservation depends on the selected codec, container, and source.

Hardware encoding options include Apple VideoToolbox on macOS, NVIDIA NVENC, Intel QSV, and Intel VAAPI on Linux. VideoToolbox presets are available on both Apple Silicon and Intel Macs. NVENC, QSV, and VAAPI codecs appear only when a short FFmpeg encode probe succeeds on the host. This checks basic encoder and device availability; a particular source or filter combination may still fail.

![View](./images/View_1.png)

The preset library groups common output settings by use. You can edit existing presets or add your own. A preset can specify the video codec, quality or bitrate mode, resolution, frame rate, audio codec, container, subtitle appearance, and deinterlacing options. Available choices vary by codec and operating system.

![View](./images/View_2.png)

### Using Cine Encoder

1. Add a media file and inspect its video, audio, and subtitle streams. You can also add an external stream.
2. Select an output preset, then adjust its video, audio, container, and filter settings as needed. Choose **Source** for a stream you want to copy where the output format supports it.
3. Choose the output location and start the conversion. Cine Encoder warns about incompatible stream and container combinations before encoding.

Text subtitle burning requires an FFmpeg build with the `subtitles` filter (usually supplied by `libass`). The available encoders and hardware presets also depend on your FFmpeg build and hardware.

### Why fork?

The original project seems to have stalled a few years ago. I wrote and submitted a bunch of pull requests that have been languishing. I fear that the original developer got impacted by the Ukraine conflict and rather than let this wonderful project sit unloved, I decided to fork it and do my best not to ruin their original vision, while bringing it up to date and adding some features and fixes that I felt were important.
This fork brings modern cross-platform support across Linux (x86_64), macOS (Apple Silicon / Intel), and Windows (x86_64).

### Differences in this Fork

- Use of CMake over QMake.
- Native Apple Silicon (macOS ARM64) and Intel macOS support, including Apple VideoToolbox hardware-accelerated encoding (`hevc_videotoolbox`, `h264_videotoolbox`, `prores_videotoolbox`) with quality-based (`-q:v`) rate control.
- Adoption of Qt6 for user interface and removal of hacks around high DPI scaling, with proper macOS Retina font rendering.
- Wayland-ready on Linux, avoiding XWayland.
- Presets stored in XML and application preferences stored in a configuration file, so preset definitions remain separate from preferences.
- Enhanced preset features that work together with preference options.
- Dynamic filtering of hardware presets and codecs based on operating system and a short FFmpeg encode probe.
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
- **Windows** (x86_64): Portable `.zip` with Qt and MediaInfo libraries; FFmpeg and MKVToolNix must be installed separately for CI ZIPs. A local `build-windows.bat` deployment can bundle those tools.

---

### Build Instructions for macOS

#### 1. Prerequisites (Homebrew)
Install CMake, Qt6, MediaInfo, and runtime utilities using [Homebrew](https://brew.sh/):

```bash
# Build & library dependencies
brew install cmake qt@6 mediainfo

# Runtime dependencies (used by Cine Encoder for encoding, muxing, and analysis)
# ffmpeg-full includes libass, which Cine Encoder needs to hard-burn text subtitles.
brew install ffmpeg-full mkvtoolnix

# ffmpeg-full is keg-only. Make it the ffmpeg command Cine Encoder launches.
brew unlink ffmpeg && brew link --overwrite ffmpeg-full

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

CMake 3.16+, a C++ compiler, Qt 6 (Core, Gui, Widgets, Multimedia, MultimediaWidgets, and Svg), and the MediaInfo development library are needed to build. At runtime, install `ffmpeg` (including `ffprobe`) and MKVToolNix (`mkvpropedit`). An FFmpeg build with `libass` is needed for text subtitle burning. Hardware encoding also requires a suitable FFmpeg build and GPU driver.

#### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-svg-dev qt6-multimedia-dev libmediainfo-dev ffmpeg mkvtoolnix
```

#### Arch Linux / Manjaro

```bash
sudo pacman -S --needed base-devel cmake qt6-base qt6-svg qt6-multimedia libmediainfo ffmpeg mkvtoolnix-cli
```

The `distros/archlinux/PKGBUILD` file still targets the older Qt 5/qmake build. Use the CMake commands below for a current source build.

#### Fedora

```bash
sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtsvg-devel qt6-qtmultimedia-devel libmediainfo-devel ffmpeg mkvtoolnix
```

On Fedora releases where `ffmpeg` is unavailable from the enabled repositories, enable an appropriate repository that provides it. The RPM spec under `distros/fedora` also targets the older Qt 5 build; use CMake for the current source.

#### NixOS

Enter a development shell with the build and runtime packages:

```bash
nix-shell -p cmake pkg-config gcc qt6.qtbase qt6.qtsvg qt6.qtmultimedia libmediainfo ffmpeg-full mkvtoolnix
```

Run the CMake commands below from that shell. NixOS may require a Qt application wrapper to set plugin paths when running the executable outside the shell; for a permanent package, wrap the installed binary with the Qt 6 wrapper hook.

#### Build from source

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/cine_encoder
```

On NixOS, run `build/cine_encoder` from the development shell. To install on a conventional Linux distribution, use `sudo cmake --install build` after building.

---

### Build Instructions for Windows

The local build script targets **Windows x64, Visual Studio 2022, and the matching Qt 6 MSVC x64 kit**. Install the Visual Studio C++ desktop workload, CMake, and Qt 6 with Widgets, Multimedia, and SVG. Open an **x64 Native Tools Command Prompt for VS 2022** in the repository root.

Download the x64 `MediaInfo.dll` from [MediaArea](https://mediaarea.net/en/MediaInfo/Download/Windows) and obtain Windows builds of [FFmpeg](https://ffmpeg.org/download.html#build-windows) and [MKVToolNix](https://mkvtoolnix.download/downloads.html#windows). Use an FFmpeg build with `libass` if you need to burn text subtitles. Keep each tool's supporting DLLs next to its executable in the directory you supply. Set these paths before running the build:

```bat
set "Qt6_DIR=C:\Qt\6.7.2\msvc2022_64"
set "MEDIAINFO_DLL=C:\path\to\MediaInfo.dll"
set "FFMPEG_BIN=C:\path\to\ffmpeg\bin"
set "MKVTOOLNIX_BIN=C:\path\to\mkvtoolnix"
build-windows.bat
```

The script configures and builds with CMake, runs `windeployqt`, and copies MediaInfo, FFmpeg, FFprobe, and MKVToolNix into `deploy\` alongside the application. It checks the main DLLs and executables before reporting success. Distribute the **whole `deploy` directory**, including Qt plugin directories and any DLLs supplied with FFmpeg and MKVToolNix. Test `deploy\cine_encoder.exe` on a clean Windows x64 machine before shipping.

The CI workflows use a separate MinGW build. Do not mix their MinGW Qt kit or runtime DLLs with the MSVC kit used by the local script. The CI ZIP packaging currently covers Qt, MediaInfo, and MinGW libraries; it does not bundle FFmpeg or MKVToolNix, so users of that ZIP must install those tools and put them on `PATH`.

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
See [LICENSE](LICENSE).
