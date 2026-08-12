# Test Configuration for Cine Encoder
cmake_minimum_required(VERSION 3.16)

# Find Qt Test framework and other components
find_package(Qt6 REQUIRED COMPONENTS Test Core Gui Widgets)

# Enable testing
enable_testing()

# Include the main app directory for headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../app)

# Define core testable sources - only include what's actually needed for tests
set(CORE_TESTABLE_SOURCES
    ../app/logger.cpp
    ../app/settingsdata.cpp
    ../app/servicecontainer.cpp
)

# Function to create a test executable with minimal dependencies
function(add_cine_test test_name test_source additional_sources)
    set(target_name test_${test_name})
    add_executable(${target_name}
        ${test_source}
        ${additional_sources}
    )
    
    target_link_libraries(${target_name} PRIVATE
        Qt6::Test
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
    )
    
    # Only link mediainfo on Linux
    if(UNIX AND NOT APPLE)
        target_link_libraries(${target_name} PRIVATE mediainfo)
    endif()
    
    target_include_directories(${target_name} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../app
    )
    
    add_test(NAME ${test_name} COMMAND ${target_name})
endfunction()

# Add test executables with minimal dependencies
add_cine_test(logger test_logger.cpp "../app/logger.cpp")
add_cine_test(settings_data test_settings_data.cpp "../app/settingsdata.cpp")
add_cine_test(service_container test_service_container.cpp "../app/servicecontainer.cpp")
add_cine_test(helper test_helper.cpp "../app/helper.cpp")
add_cine_test(ffmpeg_compatibility test_ffmpeg_compatibility.cpp "../app/ffmpegcompatibility.cpp")

# Note: Configuration and input validator tests are disabled due to external dependencies
# They can be re-enabled when those dependencies are properly abstracted

# Test runner script
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_logger test_settings_data test_service_container test_helper test_ffmpeg_compatibility
    COMMENT "Running all tests"
)
