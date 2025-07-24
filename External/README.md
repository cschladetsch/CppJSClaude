# External Dependencies

This directory contains external libraries and dependencies used by the CLL (Claude Command Line) project. All external dependencies are managed as git submodules to ensure version consistency and reproducible builds.

## Overview

The CLL project uses carefully selected external dependencies to enhance functionality while maintaining a lean codebase. All dependencies are optional and the system gracefully degrades when they're not available.

## Dependencies

### rang - Colored Terminal Output
**Location**: `External/rang/`  
**Repository**: https://github.com/agauniyal/rang.git  
**Type**: Header-only library  
**License**: Unlicense (Public Domain)  
**Purpose**: Provides ANSI color and style codes for terminal output

#### Features
- Cross-platform colored output
- Header-only (no compilation required)
- Automatic terminal capability detection
- Support for 8-bit, 16-bit, and true-color modes
- Zero runtime overhead

#### Usage in CLL
```cpp
#ifdef HAS_RANG
#include <rang.hpp>
using namespace rang;

std::cout << fg::green << "Success: " << style::reset << "Command completed\n";
#endif
```

#### Integration
The library is automatically detected and integrated via CMake:
```cmake
if(EXISTS ${CMAKE_SOURCE_DIR}/External/rang/include)
    target_include_directories(ClaudeConsole PRIVATE ${CMAKE_SOURCE_DIR}/External/rang/include)
    target_compile_definitions(ClaudeConsole PRIVATE HAS_RANG)
endif()
```

### Boost - C++ Utilities Library
**Location**: `External/boost/`  
**Repository**: https://github.com/boostorg/boost.git  
**Type**: Header-only (selective modules)  
**License**: Boost Software License  
**Purpose**: Additional utilities and enhanced C++ functionality

#### Selected Modules
CLL uses a minimal subset of Boost libraries:
- **System**: Error handling and system integration
- **Filesystem**: File system operations (backup for std::filesystem)
- **Program Options**: Command-line parsing utilities

#### Why Boost?
- **Mature**: Battle-tested code with extensive real-world usage
- **Standards Influence**: Many Boost libraries become C++ standard features
- **Selective Use**: Only include needed components to minimize bloat
- **Fallback Support**: Provides alternatives when std library features are unavailable

#### Integration
```cmake
if(EXISTS ${CMAKE_SOURCE_DIR}/External/boost)
    target_include_directories(ClaudeConsole PRIVATE ${CMAKE_SOURCE_DIR}/External/boost)
    target_compile_definitions(ClaudeConsole PRIVATE HAS_BOOST)
endif()
```

## Submodule Management

### Initialization
Initialize all submodules with the build script:
```bash
./build.sh deps
```

Or manually:
```bash
git submodule update --init --recursive
```

### Manual Submodule Operations

#### Adding New Submodules
```bash
# Add new submodule
git submodule add https://github.com/user/repo.git External/repo

# Add with specific depth (shallow clone)
git submodule add --depth 1 https://github.com/user/repo.git External/repo
```

#### Updating Submodules
```bash
# Update all submodules to latest
git submodule update --remote

# Update specific submodule
git submodule update --remote External/rang
```

#### Submodule Status
```bash
# Check submodule status
git submodule status

# Show submodule information
git submodule foreach 'echo $name: $(git describe --tags --always)'
```

## Dependency Philosophy

### Selection Criteria
External dependencies are chosen based on:
1. **Necessity**: Addresses a real need that can't be easily implemented
2. **Quality**: Well-maintained, tested, and documented
3. **Compatibility**: Works with C++20 and target platforms
4. **License**: Compatible with project licensing
5. **Size**: Minimal impact on build time and binary size

### Optional by Design
All external dependencies are optional:
- **Graceful Degradation**: Core functionality works without external deps
- **Conditional Compilation**: Features are enabled only when dependencies are available
- **No Forced Dependencies**: Users can build without external libraries

### Build Integration
Dependencies are integrated through:
- **CMake Detection**: Automatic discovery and configuration
- **Preprocessor Guards**: Conditional compilation based on availability
- **Fallback Implementations**: Alternative code paths when deps are missing

## Directory Structure

```
External/
├── README.md                   # This file
├── boost/                      # Boost C++ Libraries (submodule)
│   ├── CMakeLists.txt
│   ├── README.md
│   ├── libs/
│   │   ├── system/
│   │   ├── filesystem/
│   │   └── program_options/
│   └── [other boost components]
└── rang/                       # rang colored output (submodule)
    ├── CMakeLists.txt
    ├── README.md
    ├── include/
    │   └── rang.hpp
    └── test/
```

## Building Without External Dependencies

CLL can be built without any external dependencies:

```bash
# Skip submodule initialization
./build.sh clean
./build.sh release

# The system will build with reduced functionality:
# - No colored output (falls back to plain text)
# - Standard library only
# - Core functionality unchanged
```

## Troubleshooting

### Common Issues

1. **Submodule Not Initialized**
   ```bash
   # Error: External/rang: No such file or directory
   ./build.sh deps
   ```

2. **Submodule Update Conflicts**
   ```bash
   # Reset submodules to known state
   git submodule foreach --recursive git clean -xfd
   git submodule foreach --recursive git reset --hard
   git submodule update --init --recursive
   ```

3. **Permission Issues**
   ```bash
   # Fix permissions
   chmod -R u+w External/
   ```

4. **Network Issues**
   ```bash
   # Use HTTPS instead of SSH
   git config url."https://github.com/".insteadOf git@github.com:
   ./build.sh deps
   ```

### Verification
Verify submodules are properly initialized:
```bash
# Check if rang is available
ls External/rang/include/rang.hpp

# Check if boost is available
ls External/boost/libs/system/

# Verify in build
./build.sh release
grep "HAS_RANG\|HAS_BOOST" build/CMakeCache.txt
```

## Contributing

### Adding New Dependencies

1. **Evaluate Need**: Ensure the dependency is truly necessary
2. **Research**: Check license, maintenance status, and alternatives
3. **Add as Submodule**: Use shallow clones when possible
4. **Update CMake**: Add detection and integration logic
5. **Document**: Update this README and main project documentation
6. **Test**: Verify builds work with and without the dependency

### Guidelines

- **Prefer Header-Only**: Reduces build complexity
- **Minimize Version Locks**: Use stable releases, not bleeding edge
- **Document Integration**: Explain how and why the dependency is used
- **Test Fallbacks**: Ensure code works without the dependency

## License Compatibility

All external dependencies must be compatible with the main project license:

| Dependency | License | Compatibility |
|------------|---------|---------------|
| rang | Unlicense (Public Domain) | ✅ Compatible |
| Boost | Boost Software License | ✅ Compatible |

## Performance Impact

### Build Time Impact
- **rang**: None (header-only)
- **Boost**: Minimal (selected headers only)
- **Total Overhead**: ~5-10% increase in build time

### Runtime Impact
- **rang**: Zero overhead (compile-time)
- **Boost**: Minimal (used selectively)
- **Binary Size**: ~10-50KB increase depending on usage

### Memory Impact
- **rang**: None (no runtime allocations)
- **Boost**: Depends on usage (~1-5KB typical)

The external dependencies are carefully managed to provide enhanced functionality while maintaining the project's performance and simplicity goals.