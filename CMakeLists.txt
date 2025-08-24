cmake_minimum_required(VERSION 3.16)
project(cine_encoder VERSION 3.5.5 LANGUAGES C CXX)

if(WIN32)
    set(CMAKE_PREFIX_PATH  "C:/Qt/6.9.0/mingw_64/lib/cmake/")
    # Support for GitHub Actions Qt installation path
    if(DEFINED ENV{Qt6_DIR})
        set(CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")
    endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia Svg)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets) # X11Extras)

add_executable(cine_encoder WIN32 MACOSX_BUNDLE
    app/basedialog.cpp app/basedialog.h
    app/basewindow.cpp app/basewindow.h
    app/constants.h
    app/encoder.cpp app/encoder.h
    app/encoderstream.cpp app/encoderstream.h
    app/fileiconprovider.cpp app/fileiconprovider.h
    app/helper.cpp app/helper.h
    app/main.cpp
    app/mainwindow.cpp app/mainwindow.h app/mainwindow.ui
    app/message.cpp app/message.h app/message.ui
    app/notification.cpp app/notification.h app/notification.ui
    app/popupmessage.cpp app/popupmessage.h app/popupmessage.ui
    app/preset.cpp app/preset.h app/preset.ui
    app/progress.cpp app/progress.h app/progress.ui
    app/report.cpp app/report.h app/report.ui
    app/settings.cpp app/settings.h app/settings.ui
    app/streamconverter.cpp app/streamconverter.h app/streamconverter.ui
    app/tables.h
    app/widgets/qanimatedsvg.cpp app/widgets/qanimatedsvg.h
    app/widgets/qdoublebutton.cpp app/widgets/qdoublebutton.h
    app/widgets/qstreamview.cpp app/widgets/qstreamview.h
    app/files.qrc
)
target_link_libraries(cine_encoder PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    Qt::Svg
)

target_include_directories(cine_encoder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(cine_encoder PRIVATE
        Qt::Widgets
    )
endif()

if(DEFINES___contains___WM_CUSTOM)
    target_sources(cine_encoder PUBLIC
        app/basewindow_cwm.cpp app/basewindow_cwm.h
    )
    target_compile_definitions(cine_encoder PRIVATE WM_CUSTOM)
endif()

# Platform-specific sources are no longer needed as we use unified implementation
# Legacy platform-specific implementations are kept for reference but not built
if(FALSE) # Disabled - using unified implementation
    if(UNIX AND NOT DEFINES___contains___WM_CUSTOM AND NOT MACOS)
        target_sources(cine_encoder PUBLIC
            app/platform_unix/basewindow.cpp app/platform_unix/basewindow.h
            app/platform_unix/xutil.cpp app/platform_unix/xutil.h
        )
    endif()

    if(WIN32 AND NOT DEFINES___contains___WM_CUSTOM)
        target_sources(cine_encoder PUBLIC
            app/platform_win/basewindow.cpp app/platform_win/basewindow.h
        )
    endif()
endif()

if(WIN32)
    target_link_libraries(cine_encoder PRIVATE
        dwmapi
        user32
    )
    
    # Try to find MediaInfo library in the MediaInfoDLL directory
    find_library(MEDIAINFO_LIB 
        NAMES MediaInfoDLL mediainfo
        PATHS ${CMAKE_CURRENT_SOURCE_DIR}/app/MediaInfoDLL
        NO_DEFAULT_PATH
    )
    
    if(MEDIAINFO_LIB)
        target_link_libraries(cine_encoder PRIVATE ${MEDIAINFO_LIB})
        target_include_directories(cine_encoder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app/MediaInfoDLL)
    else()
        message(WARNING "MediaInfo library not found. Some features may not work.")
    endif()
endif()

if(UNIX AND NOT MACOS)
    target_link_libraries(cine_encoder PRIVATE
        # Removed X11 dependencies - now using public Qt6 APIs only
        # X11
        # Xext
        # Qt::GuiPrivate
        mediainfo
    )
endif()

install(TARGETS cine_encoder
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
