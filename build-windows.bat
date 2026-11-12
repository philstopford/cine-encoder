@echo off
setlocal EnableExtensions
REM Run from an x64 Visual Studio Developer Command Prompt.
cd /d "%~dp0"

if not defined Qt6_DIR (
    echo Set Qt6_DIR to the Qt MSVC x64 kit, for example C:\Qt\6.7.2\msvc2022_64
    exit /b 1
)
if not exist "%Qt6_DIR%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo Qt6_DIR does not point to a Qt installation: %Qt6_DIR%
    exit /b 1
)
if not exist "%Qt6_DIR%\bin\windeployqt.exe" (
    echo windeployqt.exe is missing from %Qt6_DIR%\bin
    exit /b 1
)
where cl.exe >nul 2>nul || (echo Run this from an x64 Visual Studio Developer Command Prompt. & exit /b 1)
where cmake.exe >nul 2>nul || (echo CMake is required on PATH. & exit /b 1)

REM MediaInfoDLL.h loads MediaInfo.dll at runtime; no import library is needed.
if not defined MEDIAINFO_DLL set "MEDIAINFO_DLL=%~dp0app\MediaInfo.dll"
if not exist "%MEDIAINFO_DLL%" (
    echo Set MEDIAINFO_DLL to the x64 MediaInfo.dll downloaded from MediaArea.
    exit /b 1
)

REM Point these at directories containing the executable and its supporting DLLs.
if not defined FFMPEG_BIN set "FFMPEG_BIN=%~dp0runtime\ffmpeg\bin"
if not defined MKVTOOLNIX_BIN set "MKVTOOLNIX_BIN=%~dp0runtime\mkvtoolnix"
if not exist "%FFMPEG_BIN%\ffmpeg.exe" (
    echo Set FFMPEG_BIN to a directory containing ffmpeg.exe and ffprobe.exe.
    exit /b 1
)
if not exist "%FFMPEG_BIN%\ffprobe.exe" (
    echo ffprobe.exe is missing from %FFMPEG_BIN%
    exit /b 1
)
if not exist "%MKVTOOLNIX_BIN%\mkvpropedit.exe" (
    echo Set MKVTOOLNIX_BIN to a directory containing mkvpropedit.exe.
    exit /b 1
)

cmake -S . -B build-windows -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%Qt6_DIR%" -DBUILD_TESTING=OFF || exit /b 1
cmake --build build-windows --config Release || exit /b 1
if not exist "build-windows\Release\cine_encoder.exe" exit /b 1

if not exist deploy mkdir deploy
copy /y "build-windows\Release\cine_encoder.exe" "deploy\" >nul || exit /b 1
"%Qt6_DIR%\bin\windeployqt.exe" --release --dir "deploy" "deploy\cine_encoder.exe" || exit /b 1
copy /y "%MEDIAINFO_DLL%" "deploy\" >nul || exit /b 1
xcopy /e /i /y "%FFMPEG_BIN%" "deploy\" >nul || exit /b 1
xcopy /e /i /y "%MKVTOOLNIX_BIN%" "deploy\" >nul || exit /b 1

for %%F in (cine_encoder.exe MediaInfo.dll ffmpeg.exe ffprobe.exe mkvpropedit.exe Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll Qt6Multimedia.dll) do (
    if not exist "deploy\%%F" (echo Missing deployed file: %%F & exit /b 1)
)
if not exist "deploy\platforms\qwindows.dll" (echo Missing Qt Windows platform plugin. & exit /b 1)
echo Deployment ready in deploy\
exit /b 0
