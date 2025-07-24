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
include_directories(${CMAKE_SOURCE_DIR}/Include)

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

# Print build info
message(STATUS "")
message(STATUS "CLL (Claude Command Line) Configuration:")
message(STATUS "  CMake version: ${CMAKE_VERSION}")
message(STATUS "  C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Readline support: ${HAS_READLINE}")
message(STATUS "")
message(STATUS "Build with: cmake --build .")
message(STATUS "Run with: ./Bin/cll")
message(STATUS "")