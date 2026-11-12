=================
Build from source
=================

Cine Encoder requires CMake 3.16 or newer, a C++ compiler, Qt 6 Core,
Gui, Widgets, Multimedia, MultimediaWidgets, and Svg development modules,
and the MediaInfo development library. At runtime, install FFmpeg (including
ffprobe) and MKVToolNix (including mkvpropedit).

On Debian or Ubuntu::

  sudo apt update
  sudo apt install build-essential cmake qt6-base-dev qt6-svg-dev qt6-multimedia-dev libmediainfo-dev ffmpeg mkvtoolnix

On Arch Linux or Manjaro::

  sudo pacman -S --needed base-devel cmake qt6-base qt6-svg qt6-multimedia libmediainfo ffmpeg mkvtoolnix-cli

On Fedora::

  sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtsvg-devel qt6-qtmultimedia-devel libmediainfo-devel ffmpeg mkvtoolnix

Build from the repository root::

  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build --parallel
  ./build/cine_encoder

See README.md for macOS, Windows, and NixOS instructions.
