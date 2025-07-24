# Source Directory

This directory contains the main application entry point for CLL (Claude Command Line).

## Files Overview

### Main.cpp
**Main application entry point and CLI interface**

- **Purpose**: Provides the command-line interface and integrates with ClaudeConsole library
- **Key Features**:
  - Command-line argument processing
  - Interactive terminal interface
  - Integration with ClaudeConsole library
  - Colored terminal output with ANSI support
  - Professional welcome screen
  - Real-time execution timing display
  - Graceful error handling and shutdown

- **Key Functions**:
  ```cpp
  int main(int argc, char* argv[]);           // Main entry point
  void ShowHelp();                            // Display usage information
  void ShowVersion();                         // Display version information
  void RunInteractiveMode();                  // Start interactive console
  ```

## Architecture

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Main.cpp      │    │ ClaudeConsole   │    │ V8 JavaScript   │
│  (CLI Entry)    │───▶│   Library       │───▶│    Engine       │
└─────────────────┘    └─────────────────┘    └─────────────────┘
        │                       │                       │
        ▼                       ▼                       ▼
  • CLI Arguments         • Command Parsing        • Real JS Execution
  • Interactive Mode      • Mode Management        • DLL Hot-Loading
  • User Interface        • V8 Integration         • Error Handling
```

## Application Flow

1. **Startup** → `main()` processes command-line arguments
2. **Initialization** → Creates ClaudeConsole instance and initializes
3. **Mode Selection**:
   - `--help` → Display usage information and exit
   - `--version` → Display version information and exit
   - `--configure` → Run configuration wizard
   - Interactive mode (default) → Start console interface
4. **Command Processing** → Delegate to ClaudeConsole library
5. **Output Display** → Format and display results with timing
6. **Shutdown** → Clean shutdown of all components

## Command Line Interface

### Arguments
```bash
./Bin/cll                    # Start interactive mode
./Bin/cll --help             # Show help information
./Bin/cll --version          # Show version information
./Bin/cll --configure        # Run configuration wizard
```

### Interactive Mode Features
- **Zsh-like Interface**: Advanced shell command support
- **Command Prefixes**:
  - `&<javascript>` → Execute JavaScript with V8 engine
  - `?<question>` → Query Claude AI
- **Built-in Commands**: help, version, configure, clear, quit/exit
- **Mode Switching**: js/javascript, shell/sh
- **Real-time Timing**: Execution time display for all commands

## Integration with ClaudeConsole Library

### Core Integration
```cpp
#include "ClaudeConsole.h"

// Initialize console
cll::ClaudeConsole console;
if (!console.Initialize()) {
    std::cerr << "Failed to initialize console" << std::endl;
    return 1;
}

// Execute commands
auto result = console.ExecuteCommand(userInput);
if (result.success) {
    std::cout << result.output << std::endl;
    std::cout << "Executed in: " << 
        cll::ClaudeConsole::FormatExecutionTime(result.executionTime) << std::endl;
}

// Clean shutdown
console.Shutdown();
```

### Features Provided by Library
- **V8 JavaScript Engine**: Real JavaScript execution (when available)
- **Claude AI Integration**: Seamless AI queries via Python CLI
- **Shell Command Processing**: Full zsh-like shell capabilities
- **Configuration Management**: Shared and application-specific config
- **DLL Hot-Loading**: Dynamic library loading and reloading
- **Performance Monitoring**: Microsecond-precision timing

## Build Dependencies

### Required
- **C++20 Compiler**: GCC 10+, Clang 10+ with full C++20 support
- **CMake**: 3.15+ for build system
- **ClaudeConsole Library**: Core functionality library

### Optional (Auto-managed)
- **V8 JavaScript Engine**: Real JavaScript execution
- **rang**: Colored terminal output
- **readline**: Enhanced command-line editing
- **nlohmann/json**: JSON configuration parsing

### Runtime
- **Python 3.x**: With claude-cli for AI integration
- **Standard POSIX**: For shell command execution

## Error Handling

### Application-Level Errors
- **Library Initialization**: Graceful failure if ClaudeConsole can't initialize
- **Command Execution**: Proper error display with context
- **System Integration**: Handling of missing dependencies
- **Signal Handling**: Clean shutdown on SIGINT/SIGTERM

### User Experience
- **Clear Error Messages**: User-friendly error descriptions
- **Graceful Degradation**: Functionality works even with missing optional components
- **Recovery**: System remains stable after errors
- **Context**: Error messages include helpful context and suggestions

## Performance Characteristics

### Startup Performance
- **Cold Start**: ~50-100ms including library initialization
- **V8 Initialization**: Additional ~50-100ms when V8 available
- **Configuration Load**: ~5-10ms for all configuration files

### Runtime Performance
- **Command Dispatch**: <1ms overhead for built-in commands
- **Shell Commands**: ~1-2ms overhead (near-native performance)
- **JavaScript Execution**: Near-native performance with V8
- **Memory Usage**: ~2-5MB base, ~5-10MB additional with V8

## Configuration Integration

### Application Configuration
```cpp
// Main.cpp handles CLI-specific configuration
void LoadApplicationConfig() {
    // Delegate to ClaudeConsole library
    console.LoadConfiguration();
    console.LoadSharedConfiguration();
}
```

### Configuration Files Used
- **`~/.config/cll/config.json`**: CLL-specific settings
- **`~/.config/shared/prompts.json`**: Shared prompt configuration
- **`~/.config/shared/aliases`**: Shared command aliases

## Development and Testing

### Building
```bash
# From project root
./build.sh release

# Debug build
./build.sh debug

# Clean build
./build.sh clean && ./build.sh release
```

### Testing
```bash
# Run all tests
./build.sh test

# Manual testing
./Bin/cll --help
./Bin/cll --version
./demo.sh
```

### Development Features
- **Debug Builds**: Enhanced error reporting and validation
- **Verbose Mode**: Detailed logging of internal operations
- **Configuration Validation**: Automatic validation of config files
- **Memory Safety**: RAII patterns and smart pointer usage

## Current Implementation Status

### ✅ Completed Features
- **CLI Argument Processing**: Full command-line interface
- **Library Integration**: Seamless ClaudeConsole library usage
- **Interactive Mode**: Advanced console interface
- **Configuration Support**: Full configuration system integration
- **Error Handling**: Comprehensive error management
- **Performance Monitoring**: Real-time execution timing

### 🚀 Modern Architecture
- **Library-Based Design**: Clean separation of CLI and core functionality
- **V8 Integration**: Real JavaScript execution capabilities
- **Shared Configuration**: Cross-application configuration management
- **Conditional Compilation**: Works with or without optional dependencies
- **Cross-Platform Support**: Linux, macOS, Windows (WSL2)

## Demo and Validation

### Testing the Application
```bash
# Comprehensive demo showcasing all features
./demo.sh

# Quick feature demo
./demo.sh --fast

# Interactive testing
./demo.sh --interactive

# Specific feature testing
./Bin/cll
❯ help                           # Built-in commands
❯ &Math.sqrt(64)                # JavaScript execution
❯ ?What is recursion?            # Claude AI query
❯ configure                     # Configuration wizard
❯ ls -la | grep README          # Shell commands
```

### Performance Validation
```bash
# Performance benchmarking
./build.sh test

# Memory usage monitoring
valgrind --tool=memcheck ./Bin/cll --help

# Startup time measurement
time ./Bin/cll --help
```

## Future Enhancements

### Planned Features
1. **Enhanced CLI Options**: More command-line arguments and modes
2. **Batch Processing**: Non-interactive script execution
3. **Configuration Validation**: Enhanced config file validation
4. **Logging System**: Optional logging to files
5. **Plugin Loading**: Dynamic plugin system integration

### Performance Improvements
1. **Faster Startup**: Further optimization of initialization
2. **Memory Efficiency**: Reduced memory footprint
3. **Async Operations**: Non-blocking operations for long commands
4. **Caching**: Intelligent caching of frequently used data

## Related Components

- **[ClaudeConsole Library](../Library/ClaudeConsole/README.md)**: Core functionality
- **[Build System](../CMakeLists.txt)**: Intelligent dependency management
- **[Demo System](../demo.sh)**: Comprehensive feature demonstration
- **[Main README](../README.md)**: Project overview and getting started

---

**Main.cpp** - The gateway to CLL's powerful command-line capabilities. 🚀