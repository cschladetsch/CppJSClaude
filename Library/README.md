# Library

This folder contains all library components of the CLL (Claude Command Line) project. The library-based architecture separates core functionality from the main application, enabling reusability and modular development.

## Overview

The Library folder houses the **ClaudeConsole** static library that provides the core functionality for:
- Command execution and processing
- Claude AI integration via subprocess calls
- Configuration management with JSON and aliases
- Shell and JavaScript mode switching
- Built-in command system
- Error handling and performance monitoring

## Architecture

```
Library/
└── ClaudeConsole/           # Core console library
    ├── CMakeLists.txt       # Library build configuration
    ├── README.md            # Detailed library documentation
    ├── Include/             # Public header files
    │   └── ClaudeConsole.h  # Main library API
    └── Source/              # Implementation files
        └── ClaudeConsole.cpp # Core library implementation
```

## ClaudeConsole Library

The **ClaudeConsole** library is the heart of the CLL system. It provides a complete C++20 interface for creating interactive command-line environments with AI integration.

### Key Features

- **🔧 Built-in Commands**: `help`, `ask`, `js`, `shell`, `config`, `reload`, `clear`, `quit`
- **🤖 Claude AI Integration**: Seamless subprocess integration with PyClaudeCli
- **📋 Configuration System**: JSON-based config with aliases in `~/.config/claude-console/`
- **⚡ JavaScript Support**: Execute JavaScript code with `&` prefix or dedicated mode
- **🎨 Performance Monitoring**: Microsecond-precision execution timing
- **🛡️ Error Handling**: Comprehensive error reporting and recovery

### Technical Specifications

- **Language**: C++20
- **Build Type**: Static library
- **Dependencies**: Standard library + optional rang/boost
- **Thread Safety**: Not thread-safe (requires external synchronization)
- **Platform**: POSIX-compatible systems

### Integration Example

```cpp
#include "ClaudeConsole.h"

claude_console::ClaudeConsole console;
if (!console.Initialize()) {
    std::cerr << "Failed to initialize console" << std::endl;
    return 1;
}

// Execute commands
auto result = console.ExecuteCommand("ask What is 2+2?");
if (result.success) {
    std::cout << "Claude: " << result.output << std::endl;
}

// Mode switching
console.SetMode(claude_console::ConsoleMode::JavaScript);
auto jsResult = console.ExecuteCommand("Math.PI * 2");
```

## Building

### As Part of Main Project
The library is automatically built when building the main CLL project:

```bash
./build.sh release
```

### Standalone Library Build
To build just the ClaudeConsole library:

```bash
mkdir build && cd build
cmake ..
make ClaudeConsole
```

### Integration in Other Projects
To use ClaudeConsole in your own project:

```cmake
# In your CMakeLists.txt
add_subdirectory(path/to/Library/ClaudeConsole)
target_link_libraries(your_target ClaudeConsole)
```

## Configuration

The library creates and manages configuration files in `~/.config/claude-console/`:

- **config.json**: Main configuration (modes, prompts, timing, Claude settings)
- **aliases**: Command aliases (shell-style format)

Example configuration:
```json
{
  "default_mode": "shell",
  "prompt_format": "[{mode}] λ ",
  "show_execution_time": true,
  "claude_integration": {
    "enabled": true,
    "timeout_seconds": 30
  }
}
```

## Dependencies

### Required
- C++20 compatible compiler (GCC 10+, Clang 10+)
- CMake 3.15+
- Standard library with filesystem support

### Optional (Auto-detected)
- **rang**: For colored terminal output
- **boost**: For additional utilities

### Runtime Dependencies
- Python 3.x (for Claude AI integration)
- PyClaudeCli or compatible 'ask' command

## Documentation

For detailed API documentation, usage examples, and advanced features, see:
- [ClaudeConsole/README.md](ClaudeConsole/README.md) - Complete library documentation
- [ClaudeConsole/Include/ClaudeConsole.h](ClaudeConsole/Include/ClaudeConsole.h) - API reference

## Contributing

When contributing to the library:

1. **Maintain C++20 compatibility**
2. **Follow existing code style and conventions**
3. **Update documentation for API changes**
4. **Test with both shell and JavaScript modes**
5. **Ensure proper error handling**

## Performance Characteristics

- **Startup Time**: ~50-100ms (cold start)
- **Built-in Commands**: <1ms execution time
- **Shell Commands**: ~1-2ms overhead (near-native performance)
- **JavaScript Execution**: Instant (simulated)
- **Claude AI Queries**: 1-5 seconds (network dependent)
- **Memory Usage**: ~2-5MB base footprint

## License

This library is part of the CLL (Claude Command Line) project. See the main project README for licensing information.