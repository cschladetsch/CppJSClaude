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
find_package(Git QUIET)

# Function to initialize Git submodules
function(git_submodule_update)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        # Update submodules as needed
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                           RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        endif()
    endif()
endfunction()

# Function to add a submodule if it doesn't exist
function(ensure_submodule SUBMODULE_PATH SUBMODULE_URL)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${SUBMODULE_PATH}/.git")
            message(STATUS "Adding submodule: ${SUBMODULE_PATH}")
            execute_process(
                COMMAND ${GIT_EXECUTABLE} submodule add ${SUBMODULE_URL} ${SUBMODULE_PATH}
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT
                OUTPUT_QUIET
                ERROR_QUIET
            )
            if(GIT_SUBMOD_RESULT EQUAL "0")
                message(STATUS "Successfully added submodule: ${SUBMODULE_PATH}")
            else()
                message(WARNING "Failed to add submodule: ${SUBMODULE_PATH}")
            endif()
        endif()
    endif()
endfunction()

# Initialize existing submodules
git_submodule_update()

# Build options
option(BUILD_V8_FROM_SOURCE "Build V8 from source instead of using system package" ON)
option(FORCE_V8_BUILD "Force V8 build even if system package exists" OFF)
option(FETCH_ONLY_ESSENTIAL "Only auto-fetch essential lightweight dependencies" ON)
option(AUTO_FETCH_V8 "Automatically fetch and build V8 from source" OFF)

# Find V8 dependencies
find_package(PkgConfig REQUIRED)
set(HAS_V8 FALSE)

# Default to building V8 from source if AUTO_FETCH_V8 is enabled
if(AUTO_FETCH_V8 AND BUILD_V8_FROM_SOURCE)
    message(STATUS "Auto-fetching and building V8 from source (default behavior)")
    set(FORCE_V8_BUILD ON)
endif()

# Try system V8 first (unless forced to build from source)
if(NOT FORCE_V8_BUILD AND NOT AUTO_FETCH_V8)
    pkg_check_modules(V8 v8)
    
    if(V8_FOUND)
        # Find additional V8 libraries
        find_library(V8_PLATFORM_LIB v8_libplatform)
        if(V8_PLATFORM_LIB)
            message(STATUS "Found system V8: ${V8_VERSION}")
            message(STATUS "Found V8 platform library: ${V8_PLATFORM_LIB}")
            set(HAS_V8 TRUE)
            set(V8_SOURCE "system")
        else()
            message(WARNING "System V8 found but platform library missing")
        endif()
    endif()
endif()

# If system V8 not found or build from source requested, try to build from source
if(NOT HAS_V8 AND (BUILD_V8_FROM_SOURCE OR FORCE_V8_BUILD OR AUTO_FETCH_V8))
    message(STATUS "Attempting to build V8 from source...")
    
    # Auto-fetch and setup V8 if enabled
    if(AUTO_FETCH_V8 AND NOT EXISTS "${CMAKE_SOURCE_DIR}/External/v8")
        message(STATUS "Auto-fetching V8 from source...")
        
        # Create External directory if it doesn't exist
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/External")
        
        # Download depot_tools if not available
        find_program(GCLIENT_EXECUTABLE gclient)
        if(NOT GCLIENT_EXECUTABLE)
            message(STATUS "Setting up depot_tools for V8 build...")
            execute_process(
                COMMAND git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/External"
                RESULT_VARIABLE DEPOT_TOOLS_RESULT
                OUTPUT_QUIET
            )
            
            if(DEPOT_TOOLS_RESULT EQUAL 0)
                set(ENV{PATH} "${CMAKE_SOURCE_DIR}/External/depot_tools:$ENV{PATH}")
                message(STATUS "depot_tools downloaded successfully")
            else()
                message(WARNING "Failed to download depot_tools")
            endif()
        endif()
        
        # Fetch V8 using gclient
        if(EXISTS "${CMAKE_SOURCE_DIR}/External/depot_tools" OR GCLIENT_EXECUTABLE)
            message(STATUS "Fetching V8 source code...")
            execute_process(
                COMMAND ${CMAKE_SOURCE_DIR}/External/depot_tools/fetch v8
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/External"
                RESULT_VARIABLE V8_FETCH_RESULT
                OUTPUT_QUIET
            )
            
            if(V8_FETCH_RESULT EQUAL 0)
                message(STATUS "V8 source fetched successfully")
                
                # Configure V8 build
                message(STATUS "Configuring V8 build...")
                execute_process(
                    COMMAND ${CMAKE_SOURCE_DIR}/External/depot_tools/gn gen out.gn/x64.release --args="is_debug=false target_cpu=\"x64\" v8_target_cpu=\"x64\" use_custom_libcxx=false"
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/External/v8"
                    RESULT_VARIABLE V8_CONFIG_RESULT
                    OUTPUT_QUIET
                )
                
                if(V8_CONFIG_RESULT EQUAL 0)
                    message(STATUS "V8 build configured")
                    
                    # Build V8 (run in background to avoid blocking CMake)
                    message(STATUS "V8 source configured. Building V8 in background...")
                    message(STATUS "This will take 30-60 minutes. Monitor with: tail -f ${CMAKE_SOURCE_DIR}/External/v8_build.log")
                    execute_process(
                        COMMAND bash -c "${CMAKE_SOURCE_DIR}/External/depot_tools/ninja -C out.gn/x64.release v8_monolith > ${CMAKE_SOURCE_DIR}/External/v8_build.log 2>&1 &"
                        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/External/v8"
                        RESULT_VARIABLE V8_BUILD_RESULT
                    )
                    
                    if(V8_BUILD_RESULT EQUAL 0)
                        message(STATUS "V8 built successfully")
                    else()
                        message(WARNING "V8 build failed or timed out")
                    endif()
                else()
                    message(WARNING "V8 build configuration failed")
                endif()
            else()
                message(WARNING "Failed to fetch V8 source")
            endif()
        endif()
    endif()
    
    # Check if External/v8 directory exists
    if(EXISTS ${CMAKE_SOURCE_DIR}/External/v8)
        message(STATUS "Found V8 source in External/v8")
        
        # Set V8 build paths
        set(V8_BUILD_DIR ${CMAKE_SOURCE_DIR}/External/v8/out.gn/x64.release)
        set(V8_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/External/v8/include)
        
        # Check if V8 is already built
        if(EXISTS ${V8_BUILD_DIR}/obj/libv8_monolith.a)
            message(STATUS "Found pre-built V8 libraries")
            set(V8_LIBRARIES ${V8_BUILD_DIR}/obj/libv8_monolith.a)
            set(V8_PLATFORM_LIB ${V8_BUILD_DIR}/obj/libv8_libplatform.a)
            set(V8_INCLUDE_DIRS ${V8_INCLUDE_DIR})
            set(HAS_V8 TRUE)
            set(V8_SOURCE "source")
        else()
            message(STATUS "V8 source found but not built. Run build script to build V8.")
            message(STATUS "  cd External/v8")
            message(STATUS "  ./build.sh")
        endif()
    else()
        if(AUTO_FETCH_V8)
            message(STATUS "V8 auto-fetch was attempted but may have failed")
            message(STATUS "Check above messages for any download or build errors")
        else()
            message(STATUS "V8 source not found in External/v8")
            message(STATUS "To build V8 from source:")
            message(STATUS "  - Enable auto-fetch: cmake -DAUTO_FETCH_V8=ON (default)")
            message(STATUS "  - Or manually: fetch V8 using depot_tools into External/v8")
        endif()
    endif()
endif()

# Final V8 status
if(NOT HAS_V8)
    if(AUTO_FETCH_V8)
        message(WARNING "V8 auto-build in progress or failed - JavaScript execution will be simulated")
        message(STATUS "V8 will be available after successful build completion")
        message(STATUS "Monitor build progress above, then re-run cmake")
    else()
        message(WARNING "V8 not available - JavaScript execution will be simulated")
        message(STATUS "To enable real V8:")
        message(STATUS "  - Auto-build from source: cmake -DAUTO_FETCH_V8=ON (default)")
        message(STATUS "  - Install system V8: sudo apt-get install libv8-dev")
        message(STATUS "  - Disable auto-fetch: cmake -DAUTO_FETCH_V8=OFF")
    endif()
endif()

# Ensure required submodules are available
option(AUTO_FETCH_DEPENDENCIES "Automatically fetch missing dependencies as submodules" ON)

if(AUTO_FETCH_DEPENDENCIES)
    # Only fetch essential lightweight dependencies
    if(FETCH_ONLY_ESSENTIAL)
        # Ensure rang submodule (lightweight)
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/External/rang/include/rang/rang.hpp")
            ensure_submodule("External/rang" "https://github.com/agauniyal/rang.git")
            git_submodule_update()
        endif()
        
        # Ensure nlohmann/json submodule (lightweight)
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/External/json/include/nlohmann/json.hpp")
            ensure_submodule("External/json" "https://github.com/nlohmann/json.git")
            git_submodule_update()
        endif()
    else()
        # Fetch all dependencies including heavy ones
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/External/rang/include/rang/rang.hpp")
            ensure_submodule("External/rang" "https://github.com/agauniyal/rang.git")
        endif()
        
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/External/json/include/nlohmann/json.hpp")
            ensure_submodule("External/json" "https://github.com/nlohmann/json.git")
        endif()
        
        # Note: Boost is very large (~500MB), only fetch if explicitly requested
        # Use -DFETCH_ONLY_ESSENTIAL=OFF to enable Boost auto-fetch
        
        git_submodule_update()
    endif()
endif()

# Check for rang (system first, then submodule)
find_path(RANG_INCLUDE_DIR rang/rang.hpp PATHS /usr/include /usr/local/include)
if(RANG_INCLUDE_DIR)
    message(STATUS "Found system rang: ${RANG_INCLUDE_DIR}")
    set(HAS_RANG TRUE)
    set(RANG_SOURCE "system")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/External/rang/include/rang/rang.hpp")
    message(STATUS "Found rang submodule: External/rang")
    set(RANG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/External/rang/include")
    set(HAS_RANG TRUE)
    set(RANG_SOURCE "submodule")
else()
    message(STATUS "rang not found - building without colored output")
    set(HAS_RANG FALSE)
endif()

# Check for nlohmann/json (system first, then submodule)
find_path(JSON_INCLUDE_DIR nlohmann/json.hpp PATHS /usr/include /usr/local/include)
if(JSON_INCLUDE_DIR)
    message(STATUS "Found system nlohmann/json: ${JSON_INCLUDE_DIR}")
    set(HAS_JSON TRUE)
    set(JSON_SOURCE "system")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/External/json/include/nlohmann/json.hpp")
    message(STATUS "Found json submodule: External/json")
    set(JSON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/External/json/include")
    set(HAS_JSON TRUE)
    set(JSON_SOURCE "submodule")
else()
    message(STATUS "nlohmann/json not found - some features may be limited")
    set(HAS_JSON FALSE)
endif()

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

# Link V8 libraries if available
if(HAS_V8)
    target_link_libraries(cll ${V8_LIBRARIES} ${V8_PLATFORM_LIB})
    target_include_directories(cll PRIVATE ${V8_INCLUDE_DIRS})
    target_compile_options(cll PRIVATE ${V8_CFLAGS_OTHER})
    target_compile_definitions(cll PRIVATE HAS_V8)
else()
    target_compile_definitions(cll PRIVATE NO_V8)
endif()

# Link additional libraries
target_link_libraries(cll dl pthread)

# Include additional directories
if(HAS_RANG)
    target_include_directories(cll PRIVATE ${RANG_INCLUDE_DIR})
else()
    target_compile_definitions(cll PRIVATE NO_RANG)
endif()

if(HAS_JSON)
    target_include_directories(cll PRIVATE ${JSON_INCLUDE_DIR})
else()
    target_compile_definitions(cll PRIVATE NO_JSON)
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
if(HAS_V8)
    message(STATUS "  V8 JavaScript: YES (${V8_SOURCE})")
else()
    message(STATUS "  V8 JavaScript: NO (simulated)")
endif()
message(STATUS "  DLL Loading: YES")
message(STATUS "  Readline support: ${HAS_READLINE}")
if(HAS_RANG)
    message(STATUS "  Colored output: YES (${RANG_SOURCE})")
else()
    message(STATUS "  Colored output: NO")
endif()
if(HAS_JSON)
    message(STATUS "  JSON support: YES (${JSON_SOURCE})")
else()
    message(STATUS "  JSON support: NO")
endif()
message(STATUS "  Auto-fetch deps: ${AUTO_FETCH_DEPENDENCIES}")
message(STATUS "  Auto-fetch V8: ${AUTO_FETCH_V8}")
message(STATUS "  Build V8 from source: ${BUILD_V8_FROM_SOURCE}")
message(STATUS "  Tests enabled: ${BUILD_TESTS}")
message(STATUS "")
message(STATUS "Build with: cmake --build .")
message(STATUS "Run with: ./Bin/cll")
if(BUILD_TESTS)
    message(STATUS "Run tests with: ctest or ./test.sh")
endif()
message(STATUS "")