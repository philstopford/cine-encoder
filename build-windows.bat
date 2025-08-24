@echo off
REM Windows build and deployment script for Cine Encoder
REM This script helps developers build and package the application locally

echo Building Cine Encoder for Windows...

REM Check if Qt6 is available
echo Checking for Qt6 installation...
if not defined Qt6_DIR (
    REM Try to find Qt6 in common installation paths
    for %%d in ("C:\Qt\6.*\msvc*_64" "C:\Qt\Qt6.*\6.*\msvc*_64" "%USERPROFILE%\Qt\6.*\msvc*_64") do (
        if exist "%%d\bin\qmake.exe" (
            set "Qt6_DIR=%%d"
            echo Found Qt6 at: %%d
            goto qt_found
        )
    )
    echo Error: Qt6 not found. Please ensure Qt6 is installed with:
    echo - Qt6 Core, Gui, Widgets components
    echo - Qt6 SVG module
    echo - Qt6 Multimedia module
    echo.
    echo Set Qt6_DIR environment variable to Qt6 installation directory
    echo Example: set Qt6_DIR=C:\Qt\6.7.0\msvc2022_64
    pause
    exit /b 1
    :qt_found
)

REM Verify Qt6 modules
echo Verifying Qt6 modules...
set "SVG_FOUND=0"
set "MULTIMEDIA_FOUND=0"

if exist "%Qt6_DIR%\lib\Qt6Svg.lib" set "SVG_FOUND=1"
if exist "%Qt6_DIR%\bin\Qt6Svg.dll" set "SVG_FOUND=1"
if exist "%Qt6_DIR%\lib\Qt6Multimedia.lib" set "MULTIMEDIA_FOUND=1"
if exist "%Qt6_DIR%\bin\Qt6Multimedia.dll" set "MULTIMEDIA_FOUND=1"

if "%SVG_FOUND%"=="0" (
    echo Error: Qt6 SVG module not found
    echo Please install Qt6 with SVG support
    pause
    exit /b 1
)

if "%MULTIMEDIA_FOUND%"=="0" (
    echo Error: Qt6 Multimedia module not found
    echo Please install Qt6 with Multimedia support
    pause
    exit /b 1
)

echo ✓ Qt6 Core found at: %Qt6_DIR%
echo ✓ Qt6 SVG module found
echo ✓ Qt6 Multimedia module found

REM Add Qt6 to PATH
set "PATH=%Qt6_DIR%\bin;%PATH%"

REM Check for MediaInfo
echo Checking for MediaInfo library...
if not exist "app\MediaInfoDLL\MediaInfoDLL.h" (
    echo MediaInfo development headers not found
    echo Please download MediaInfo DLL from:
    echo https://mediaarea.net/download/binary/libmediainfo0/
    echo And extract MediaInfoDLL.h and MediaInfoDLL.lib to app\MediaInfoDLL\
    pause
    exit /b 1
)
echo ✓ MediaInfo library found

REM Set up build directory
if exist build rmdir /s /q build
mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%Qt6_DIR%" -DQt6_DIR="%Qt6_DIR%" ..
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build the application
echo Building application...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

cd ..

REM Create deployment directory
echo Creating deployment...
if exist deploy rmdir /s /q deploy
mkdir deploy

REM Copy main executable
copy build\Release\cine_encoder.exe deploy\

REM Deploy Qt dependencies including SVG support
echo Deploying Qt dependencies...
windeployqt.exe --dir deploy --release --svg deploy\cine_encoder.exe

REM Check if SVG plugin was deployed
if exist "deploy\imageformats\qsvg.dll" (
    echo ✓ SVG plugin deployed successfully - SVG icons will work
) else (
    echo ⚠ WARNING: SVG plugin not found - SVG icons may not display correctly
)

REM List all deployed files
echo.
echo Deployed files:
dir /s deploy

echo.
echo Build and deployment completed successfully!
echo You can run the application from: deploy\cine_encoder.exe
echo.
pause