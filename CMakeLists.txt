cmake_minimum_required(VERSION 3.15)
project(cll VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Find packages
find_package(PkgConfig QUIET)

# Check for readline
find_path(READLINE_INCLUDE_DIR readline/readline.h)
find_library(READLINE_LIBRARY readline)

if(READLINE_INCLUDE_DIR AND READLINE_LIBRARY)
    message(STATUS "Found GNU Readline: ${READLINE_LIBRARY}")
    set(HAS_READLINE TRUE)
else()
    message(STATUS "GNU Readline not found - building without readline support")
    set(HAS_READLINE FALSE)
endif()

# Add library subdirectory
add_subdirectory(Library/ClaudeConsole)

# Include directories for main application
include_directories(${CMAKE_SOURCE_DIR}/Library/ClaudeConsole/Include)

# Source files for main executable
set(SOURCES
    Source/Main.cpp
)

# Create executable
add_executable(cll ${SOURCES})

# Link with ClaudeConsole library
target_link_libraries(cll ClaudeConsole)

# Link libraries
if(HAS_READLINE)
    target_link_libraries(cll ${READLINE_LIBRARY})
    target_include_directories(cll PRIVATE ${READLINE_INCLUDE_DIR})
else()
    target_compile_definitions(cll PRIVATE NO_READLINE)
endif()

# Set output directory
set_target_properties(cll PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin
)

# Install target
install(TARGETS cll DESTINATION bin)

# Testing support
option(BUILD_TESTS "Build test suite" ON)

if(BUILD_TESTS)
    # Fetch Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Enable testing
    enable_testing()
    
    # Add test subdirectory
    add_subdirectory(Tests)
    
    message(STATUS "Testing enabled - Google Test will be fetched and built")
endif()

# Print build info
message(STATUS "")
message(STATUS "CLL (Claude Command Line) Configuration:")
message(STATUS "  CMake version: ${CMAKE_VERSION}")
message(STATUS "  C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Readline support: ${HAS_READLINE}")
message(STATUS "  Tests enabled: ${BUILD_TESTS}")
message(STATUS "")
message(STATUS "Build with: cmake --build .")
message(STATUS "Run with: ./Bin/cll")
if(BUILD_TESTS)
    message(STATUS "Run tests with: ctest or ./test.sh")
endif()
message(STATUS "")