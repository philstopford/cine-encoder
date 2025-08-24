@echo off
REM Windows build and deployment script for Cine Encoder
REM This script helps developers build and package the application locally

echo Building Cine Encoder for Windows...

REM Set up build directory
if exist build rmdir /s /q build
mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release ..
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