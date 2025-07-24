# External Dependencies

This directory contains external libraries and dependencies used by CLL (Claude Command Line). CLL uses intelligent dependency management to automatically fetch and build required dependencies while maintaining graceful degradation when they're unavailable.

## Overview

CLL employs a sophisticated dependency management system that:
- **Auto-fetches** lightweight dependencies as needed
- **Gracefully degrades** when optional dependencies are unavailable  
- **Supports both** system packages and building from source
- **Minimizes bloat** by only including essential components

## Auto-Managed Dependencies

### rang - Colored Terminal Output
**Location**: `External/rang/` (auto-fetched)  
**Repository**: https://github.com/agauniyal/rang.git  
**Type**: Header-only library  
**License**: Unlicense (Public Domain)  
**Status**: ✅ Auto-fetched by CMake

#### Features
- Cross-platform colored output for enhanced UX
- Header-only design (zero compilation overhead)
- Automatic terminal capability detection
- Support for 8-bit, 16-bit, and true-color modes
- Zero runtime performance impact

#### Usage in CLL
```cpp
#ifdef HAS_RANG
#include <rang.hpp>
using namespace rang;

// Colorized output for better user experience
std::cout << fg::cyan << "λ " << style::reset;           // Prompt
std::cout << fg::green << "✓ " << style::reset;          // Success
std::cout << fg::red << "✗ " << style::reset;            // Error
std::cout << fg::yellow << "⚠ " << style::reset;         // Warning
#else
// Graceful fallback to plain text
std::cout << "λ ";
std::cout << "✓ ";
std::cout << "✗ ";
std::cout << "⚠ ";
#endif
```

### nlohmann/json - Modern C++ JSON
**Location**: `External/json/` (auto-fetched)  
**Repository**: https://github.com/nlohmann/json.git  
**Type**: Header-only library  
**License**: MIT License  
**Status**: ✅ Auto-fetched by CMake

#### Features
- Modern C++20 JSON parsing and generation
- Intuitive API with standard library integration
- Header-only design
- Excellent error reporting
- High performance with minimal memory usage

#### Usage in CLL
```cpp
#ifdef HAS_JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Configuration file parsing
json config;
std::ifstream configFile(configPath);
if (configFile.is_open()) {
    configFile >> config;
    
    // Easy access to configuration values
    defaultMode_ = config.value("default_mode", "shell");
    showTiming_ = config.value("show_execution_time", true);
    historySize_ = config.value("history_size", 1000);
}
#else
// Fallback to manual parsing for essential config
// (Reduced functionality but still operational)
#endif
```

## Optional Heavy Dependencies

### V8 JavaScript Engine
**Location**: `External/v8/` (manual setup) or system package  
**Repository**: https://chromium.googlesource.com/v8/v8.git  
**Type**: Large compiled library (~100MB)  
**License**: BSD 3-Clause  
**Status**: 🔄 Optional with intelligent detection

#### Setup Options

**Option 1: System Package (Recommended)**
```bash
# Ubuntu/Debian
sudo apt-get install libv8-dev

# macOS
brew install v8

# CMake will auto-detect system V8
./build.sh release
```

**Option 2: Build from Source**
```bash
# Enable source build
cmake .. -DBUILD_V8_FROM_SOURCE=ON

# Or force build even if system package exists
cmake .. -DFORCE_V8_BUILD=ON
```

**Option 3: Manual Setup**
```bash
# V8 requires special setup with depot_tools
# See V8 documentation for complete setup process
```

#### V8 Integration Features
```cpp
#ifdef HAS_V8
// Real JavaScript execution with V8 engine
bool ExecuteJavaScript(const std::string& code) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    
    auto source = v8::String::NewFromUtf8(isolate, code.c_str()).ToLocalChecked();
    auto script = v8::Script::Compile(context, source).ToLocalChecked();
    auto result = script->Run(context);
    
    return !result.IsEmpty();
}

// DLL hot-loading capabilities
bool LoadDll(const std::string& path);
bool ReloadDll(const std::string& path);
bool UnloadDll(const std::string& path);
#else
// Graceful simulation when V8 unavailable
bool ExecuteJavaScript(const std::string& code) {
    std::cout << "// JavaScript execution simulated (V8 not available)\n";
    std::cout << "// Code: " << code << "\n";
    return true;
}
#endif
```

## Intelligent Dependency Management

### CMake Auto-Fetching
CLL's CMake system automatically manages dependencies:

```cmake
# Auto-fetch lightweight dependencies
option(AUTO_FETCH_DEPENDENCIES "Automatically fetch missing dependencies" ON)
option(FETCH_ONLY_ESSENTIAL "Only auto-fetch essential lightweight dependencies" ON)

if(AUTO_FETCH_DEPENDENCIES)
    if(FETCH_ONLY_ESSENTIAL)
        # Fetch only lightweight dependencies
        ensure_submodule("External/rang" "https://github.com/agauniyal/rang.git")
        ensure_submodule("External/json" "https://github.com/nlohmann/json.git")
    else()
        # Fetch all dependencies (warning: includes large dependencies)
        # Note: Boost is ~500MB, V8 requires special setup
    endif()
endif()
```

### Build Options
```bash
# Default: Auto-fetch essential dependencies
./build.sh release

# Build with all dependencies
cmake .. -DFETCH_ONLY_ESSENTIAL=OFF

# Build without auto-fetching
cmake .. -DAUTO_FETCH_DEPENDENCIES=OFF

# V8-specific options
cmake .. -DBUILD_V8_FROM_SOURCE=ON      # Build V8 from source
cmake .. -DFORCE_V8_BUILD=ON            # Force build over system package
```

### Dependency Detection Status
The build system provides clear feedback on dependency status:

```
CLL (Claude Command Line) Configuration:
  V8 JavaScript: YES (system)
  DLL Loading: YES  
  Readline support: YES
  Colored output: YES (submodule)
  JSON support: YES (submodule)
  Auto-fetch deps: YES
```

## Build Integration

### CMake Detection Logic
```cmake
# Check for system packages first
find_package(PkgConfig REQUIRED)
pkg_check_modules(V8 v8)

if(V8_FOUND)
    set(HAS_V8 TRUE)
    set(V8_SOURCE "system")
else()
    # Try submodule or build from source
    if(EXISTS ${CMAKE_SOURCE_DIR}/External/v8)
        set(HAS_V8 TRUE)
        set(V8_SOURCE "submodule")
    endif()
endif()

# Auto-fetch lightweight dependencies
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/External/rang/include/rang/rang.hpp")
    ensure_submodule("External/rang" "https://github.com/agauniyal/rang.git")
endif()
```

### Conditional Compilation
```cpp
// All features use conditional compilation
#ifdef HAS_V8
    #include "V8Compat.h"
    // V8 JavaScript engine functionality
#endif

#ifdef HAS_RANG
    #include <rang.hpp>
    // Colored terminal output
#endif

#ifdef HAS_JSON
    #include <nlohmann/json.hpp>
    // JSON configuration parsing
#endif
```

## Directory Structure

```
External/
├── README.md                   # This file
├── rang/                       # Auto-fetched colored output library
│   ├── include/
│   │   └── rang.hpp           # Main header
│   └── README.md
├── json/                       # Auto-fetched JSON library  
│   ├── include/
│   │   └── nlohmann/
│   │       └── json.hpp       # Main header
│   └── README.md
└── v8/                         # Optional V8 JavaScript engine
    ├── include/               # V8 headers (if built from source)
    ├── out.gn/               # Build output
    └── [V8 source tree]      # Complete V8 source (if using source build)
```

## Dependency Philosophy

### Selection Criteria
Dependencies are carefully evaluated based on:

1. **Essential Need**: Provides significant value that justifies inclusion
2. **Quality & Maintenance**: Well-maintained with active development
3. **Performance**: Minimal impact on build time and runtime performance
4. **Compatibility**: Works with C++20 and target platforms
5. **License**: Compatible with project licensing requirements
6. **Size & Complexity**: Reasonable footprint and integration complexity

### Graceful Degradation
All dependencies are designed to be optional:

- **Core Functionality**: Always available regardless of dependencies
- **Enhanced Features**: Additional capabilities when dependencies are present
- **Clear Feedback**: Users know exactly what features are available
- **No Surprises**: Missing dependencies don't cause build failures

### Dependency Tiers

**Tier 1: Essential (Auto-fetched)**
- `rang`: Colored output for better UX
- `nlohmann/json`: Configuration file parsing

**Tier 2: Optional System (Auto-detected)**
- `readline`: Enhanced command-line editing
- `V8`: Real JavaScript execution engine

**Tier 3: Optional Large (Manual)**
- `boost`: Additional utilities (if needed)
- `V8 source`: Build V8 from source

## Performance Impact

### Build Time
- **Auto-fetch**: ~5-10 seconds for initial clone
- **Lightweight deps**: Negligible build time impact
- **V8 from source**: ~30-60 minutes (system package recommended)

### Runtime Performance
- **rang**: Zero overhead (compile-time)
- **nlohmann/json**: Minimal parsing overhead
- **V8**: Near-native JavaScript performance

### Binary Size
- **Base CLL**: ~2-5MB
- **With dependencies**: ~3-7MB
- **With V8**: ~10-15MB additional

## Troubleshooting

### Auto-Fetch Issues
```bash
# Network issues
git config --global http.proxy http://proxy:port
./build.sh deps

# Permission issues  
chmod -R u+w External/
./build.sh deps

# Reset and retry
./build.sh clean
rm -rf External/rang External/json
./build.sh deps
```

### V8 Issues
```bash
# Use system package instead of building
sudo apt-get install libv8-dev
./build.sh clean && ./build.sh release

# Force no V8 (simulation mode)
cmake .. -DFORCE_V8_BUILD=OFF -DBUILD_V8_FROM_SOURCE=OFF
```

### Verification
```bash
# Check dependency status
./build.sh release | grep -E "(Found|Auto-fetch)"

# Verify functionality
./demo.sh --fast

# Check available features
./Bin/cll
❯ help                    # Should show all available commands
❯ &console.log('test')    # JavaScript (V8 or simulated)
❯ configure              # Should show colorized output
```

## Contributing

### Adding Dependencies
1. **Evaluate necessity** and alignment with project goals
2. **Check licensing** compatibility
3. **Implement auto-detection** in CMakeLists.txt
4. **Add conditional compilation** guards
5. **Provide graceful fallbacks** for missing dependencies
6. **Update documentation** and this README
7. **Test both** with and without the dependency

### Guidelines
- **Prefer lightweight**: Header-only when possible
- **Auto-fetch carefully**: Only for essential, small dependencies
- **Document thoroughly**: Explain purpose and integration
- **Test fallbacks**: Ensure code works without each dependency
- **Performance conscious**: Measure impact on build and runtime

---

**External Dependencies** - Enhancing CLL with carefully curated, intelligently managed external libraries. 🚀