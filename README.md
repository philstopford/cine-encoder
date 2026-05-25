# Cine Encoder

Cine Encoder is an application, uses the FFmpeg, MKVToolNix and MediaInfo utilities, that allows to convert media files while preserving HDR metadata. Supported hardware encoding NVENC and Intel QSV (for Windows and experimental for Linux). The following encoding modes are implemented: H265, H264, AV1, VP9, MPEG-2, XDCAM, XAVC, DNxHR, ProRes.

![View](./images/View_1.png)

By default, there are several categories of presets. You can change existing or add your own presets.

![View](./images/View_2.png)

### Why fork?

The original project seems to have stalled a few years ago. I wrote and submitted a bunch of pull requests that have been languishing. I fear that the original developer got impacted by the Ukraine conflict and rather than let this wonderful project sit unloved, I decided to fork it and do my best not to ruin their original vision, while bringing it up to date and adding some features and fixes that I felt were important.
At the same time, I don't have Windows installations around and also have no Mac environment, so the cross-platform nature of this fork may be affected. I run Linux full time and know that there are competing commercial offerings on Windows and Mac (iFFMpeg, myFFMpeg) that people can turn to there.

### Differences in this Fork

- Use of CMake over QMake.
- Adoption of Qt6 for user interface and removal of hacks around high DPI scaling, etc.
- Wayland-ready, avoiding XWayland.
- Use of XML files for presets and preferences, avoiding loss of preset information when tool feature set is changed.
- Enhanced preset features that work together with preference options.
- Extracting audio tracks maintains metadata if destination format supports it.
- Ability to hard burn subtitles with customizable background color, opacity, text appearance, etc.
- Warning in case transcode settings are incompatible with target format, with advice on how to mitigate (e.g. transcode audio, hard-burn subtitles, etc.)
- Ability to dynamically adjust task priority for background ffmpeg sessions to help maintain usability.
- Improved error handling for ffmpeg issues.
- Improved handling for filenames, streams, etc. that have special characters.
- Heavily refactored code to make it more understandable and maintainable. Addressed numerous weaknesses in the original design.

### Installation

Supported operating system: Linux x86_64. I tried to make this fork still work on Windows, but it's untested. Mac support is untested as well.

### Build instruction for Linux
In general, you need mkvtoolnix (the terminal version is fine), and mediainfo. The GUI will call ffmpeg but it is not required for building. Qt6 development libraries (multimedia, etc.) are also needed.
The build system is based on CMake.
With something like CLion, you can just open the project folder, set up the build configurations and build. There should be no issues.
This fork removed various X.org hangovers, so the output should work well with high DPI settinsg (including scaling) and on Wayland.

Windows support in this fork is largely untested. I took a stab at it, but don't run Windows often enough to know if there are issues on that platform.

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
