# Changelog

All notable changes to CLL (Claude Command Line) will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Comprehensive documentation system overhaul
- Enhanced CONTRIBUTING.md with V8 integration guidelines
- Detailed Library/ClaudeConsole/README.md with full API reference
- Updated main README.md with current feature set

### Changed
- Documentation reflects current CLL capabilities and architecture
- All guides updated for V8 JavaScript engine integration
- Configuration system documentation updated for shared config approach

## [2.0.0] - 2024-11-24

### 🚀 Major Architecture Overhaul

#### **V8 JavaScript Engine Integration**
- **Real JavaScript Execution**: Full V8 JavaScript engine integration with conditional compilation
- **DLL Hot-Loading**: Dynamic library loading and reloading capabilities for development
- **JavaScript Built-ins**: Enhanced JavaScript environment with file I/O, DLL management
- **Conditional Compilation**: Graceful fallback when V8 is not available
- **V8 Compatibility Layer**: Version-independent V8 integration via V8Compat.h

#### **Advanced Command Interface**
- **Zsh-Like Shell**: Comprehensive shell integration with advanced features
- **Command Prefixes**: 
  - `&` prefix for JavaScript injection from any mode
  - `?` prefix for Claude AI queries from any mode
- **Multi-Mode Operations**: Seamless switching between Shell and JavaScript modes
- **Advanced Shell Features**: Globbing, pipes, redirection, command substitution

#### **Intelligent Configuration System**
- **Shared Configuration**: Cross-application config in `~/.config/shared/`
- **Application-Specific Config**: CLL-specific settings in `~/.config/cll/`
- **Configuration Wizard**: Interactive `configure` command for setup
- **Shared Prompt Management**: Centralized prompt configuration across applications

### Added

#### **Core Features**
- **V8 JavaScript Engine**: Real JavaScript execution with full ES2020+ support
- **DLL Hot-Loading System**: Load, reload, and unload native libraries dynamically
- **Zsh-Like Shell Interface**: Advanced shell capabilities with modern features
- **Claude AI Integration**: Enhanced AI queries with `?` prefix
- **Configuration Wizard**: Interactive setup via `configure` command
- **Shared Configuration**: Cross-application configuration management

#### **JavaScript Capabilities**
```javascript
// New JavaScript built-ins available in V8 mode
print("Hello World");                    // Console output
load("script.js");                       // Load JavaScript file  
loadDll("/path/to/library.so");          // Load native library
reloadDll("/path/to/library.so");        // Hot-reload library
listDlls();                              // Show loaded libraries
unloadDll("/path/to/library.so");        // Unload library
```

#### **Advanced Shell Features**
```bash
# Zsh-like capabilities now available
❯ ls -la *.{cpp,h}              # Brace expansion
❯ echo **/*.cpp                 # Recursive globbing
❯ echo "Today is $(date +%A)"   # Command substitution
❯ find . -name "*.cpp" | wc -l  # Complex pipes
❯ env | grep HOME               # Environment variables
```

#### **Build System Enhancements**
- **Intelligent Dependency Management**: Auto-fetching of lightweight dependencies
- **V8 Build Options**: Support for both system V8 and building from source
- **Submodule Management**: Automatic handling of git submodules
- **Cross-Platform Support**: Enhanced Linux, macOS, and WSL2 compatibility

#### **New Commands**
- **`configure`**: Interactive configuration wizard
- **`version`**: Show CLL version and build information
- **Enhanced `help`**: Comprehensive command reference
- **DLL Management**: `loadDll()`, `reloadDll()`, `listDlls()`, `unloadDll()`

### Changed

#### **Interface Improvements**
- **Command Prefixes**: Unified `&` for JavaScript, `?` for Claude AI
- **Mode Switching**: Enhanced mode management with persistent contexts
- **Error Handling**: Improved error messages and recovery
- **Performance**: Optimized command execution and V8 integration

#### **Configuration System**
- **Directory Structure**: 
  ```
  ~/.config/
  ├── cll/                    # CLL-specific configuration
  │   ├── config.json        # Main CLL settings  
  │   └── aliases            # CLL-specific aliases
  └── shared/                 # Shared configuration
      ├── prompts.json       # Shared prompt configuration
      └── aliases            # Shared command aliases
  ```

#### **Build Architecture**
- **CMake Enhancements**: Intelligent dependency detection and fetching
- **V8 Integration**: Optional V8 with automatic fallback
- **Modular Design**: Enhanced library architecture
- **Testing Framework**: Expanded test coverage for new features

### Enhanced

#### **JavaScript Integration**
- **Real Execution**: V8 engine provides actual JavaScript execution
- **Performance**: Near-native JavaScript performance when V8 available
- **Error Handling**: Detailed JavaScript error reporting with stack traces
- **Context Persistence**: Persistent JavaScript context across commands

#### **Claude AI Integration**  
- **Improved Queries**: Enhanced AI interaction with `?` prefix
- **Multi-line Support**: Complex multi-line AI queries
- **Error Recovery**: Better handling of AI service availability
- **Context Awareness**: Improved context handling for AI queries

#### **Shell Capabilities**
- **Advanced Features**: Full zsh-like shell functionality
- **Performance**: Optimized shell command execution
- **Compatibility**: Enhanced cross-platform shell support
- **Error Handling**: Improved shell error reporting

### Developer Experience

#### **Documentation**
- **Comprehensive Guides**: Updated README with current features
- **API Documentation**: Complete ClaudeConsole library reference
- **Development Guide**: Enhanced CONTRIBUTING.md with V8 guidelines
- **Code Examples**: Extensive usage examples throughout

#### **Testing & Quality**
- **V8 Testing**: Comprehensive tests for JavaScript integration
- **Conditional Testing**: Tests work with and without V8
- **Performance Tests**: Benchmarks for all major operations
- **Demo System**: Enhanced demo.sh with comprehensive feature showcase

#### **Build & Development**
- **Dependency Management**: Automatic handling of optional dependencies
- **Cross-Platform**: Improved support across different platforms
- **Development Tools**: Enhanced build scripts and testing framework
- **IDE Support**: Better integration with modern development environments

### Performance

#### **Optimizations**
- **V8 Integration**: Efficient JavaScript execution when available
- **Memory Management**: Optimized memory usage for large codebases
- **Startup Time**: Reduced initialization overhead
- **Command Execution**: Minimized latency for shell commands

#### **Benchmarks**
- **Startup Time**: ~50-100ms (cold start)
- **Shell Commands**: ~1-2ms overhead (near-native)
- **JavaScript (V8)**: Near-native performance
- **JavaScript (Simulated)**: <0.1ms fallback
- **Claude AI Queries**: 1-5 seconds (network dependent)
- **DLL Loading**: ~10-50ms depending on library size

### Dependencies

#### **Build Requirements**
- **C++20 Compiler**: GCC 10+, Clang 10+ with full C++20 support
- **CMake**: 3.15+ for intelligent build management
- **Git**: For submodule management

#### **Auto-Managed Dependencies**
- **V8 JavaScript Engine**: Optional, auto-detected or built from source
- **rang**: Colored terminal output (auto-fetched)
- **nlohmann/json**: JSON configuration (auto-fetched)
- **readline**: Enhanced input editing (system package)

#### **Runtime Dependencies**  
- **Python 3.x**: With claude-cli for AI integration
- **Standard POSIX**: For shell command execution

### Migration from 1.x

#### **Breaking Changes**
- **Command Interface**: New prefix system (`&` for JS, `?` for AI)
- **Configuration**: New shared configuration system
- **Build System**: Enhanced CMake with dependency management
- **API Changes**: Updated ClaudeConsole library API

#### **Migration Guide**
```bash
# Update configuration
mkdir -p ~/.config/cll ~/.config/shared
mv ~/.config/claude-console/* ~/.config/cll/ 2>/dev/null || true

# Rebuild with new dependencies
./build.sh clean
./build.sh deps
./build.sh release

# Test new features
./demo.sh
```

### Security

#### **Security Enhancements**
- **DLL Loading**: Safe library loading with error handling
- **Input Validation**: Enhanced validation for all command types
- **Process Isolation**: Secure subprocess execution
- **Configuration Security**: Secure storage of configuration data

### Platform Support

#### **Enhanced Support**
- **Linux**: Ubuntu 20.04+, Debian 11+, Fedora 36+, Arch Linux
- **macOS**: macOS 11+ with Homebrew or MacPorts
- **Windows**: WSL2 with Ubuntu 20.04+ distribution
- **ARM64**: Native support on Apple Silicon and ARM64 Linux

### Known Issues & Limitations

#### **Current Limitations**
- **Thread Safety**: Library is not thread-safe (external synchronization required)
- **Windows Native**: Requires WSL2 (native Windows support planned for 3.0)
- **Large Dependencies**: V8 is substantial (~100MB when built from source)

#### **Workarounds**
- **V8 Optional**: All functionality available without V8 (JavaScript simulated)
- **System V8**: Use system package to avoid building from source
- **Lightweight Mode**: Use `FETCH_ONLY_ESSENTIAL=ON` for minimal dependencies

---

## [1.0.0] - 2024-10-24

### Added
- **Core Library Architecture**: Modular ClaudeConsole static library
- **Claude AI Integration**: Seamless subprocess integration with PyClaudeCli
- **JavaScript Simulation**: Placeholder execution with `&` prefix
- **Shell Integration**: Full POSIX shell command support with timing
- **Configuration System**: JSON-based config in `~/.config/cll/`
- **Built-in Commands**: `help`, `ask`, `js`, `shell`, `config`, `reload`, `clear`, `quit`
- **Performance Monitoring**: Microsecond-precision execution timing
- **Modern C++20**: Full C++20 standard utilization
- **Build System**: CMake with colored build scripts and dependency management

### Architecture
- **Namespace**: All code uses `cll` namespace for consistency
- **Library Structure**: Separation of core library from main application
- **Configuration**: `~/.config/cll/config.json` and `~/.config/cll/aliases`
- **Testing**: Comprehensive GTest suite with CTest integration

---

## Release Notes

### Version 2.0.0 - "JavaScript & AI Revolution"

This major release transforms CLL into a powerful, modern command-line interface with real JavaScript execution capabilities and advanced AI integration. The focus has been on creating a production-ready tool that combines the best of shell, JavaScript, and AI functionality.

#### 🚀 **Major Highlights**
- **Real JavaScript**: V8 engine integration with hot-loading DLL support
- **Advanced Shell**: Zsh-like interface with modern features
- **Smart AI**: Enhanced Claude integration with intuitive `?` prefix
- **Intelligent Build**: Auto-managing dependencies and cross-platform support
- **Developer-Friendly**: Comprehensive documentation and testing framework

#### 🛠️ **Technical Achievements**
- **Conditional Compilation**: Works with or without V8 JavaScript engine
- **Shared Configuration**: Cross-application configuration management
- **Performance Optimization**: Near-native performance for all operations
- **Memory Efficiency**: Optimized resource usage and smart pointer management
- **Cross-Platform**: Enhanced support for Linux, macOS, and Windows (WSL2)

#### 📈 **Looking Forward to 3.0**
Future releases will focus on:
- **Native Windows Support**: Direct Windows compatibility without WSL2
- **Plugin Architecture**: Extensible plugin system for custom functionality
- **Advanced AI Features**: Enhanced context awareness and multi-modal support
- **Performance Enhancements**: Further optimization and memory improvements
- **Enterprise Features**: Advanced security, logging, and deployment options

---

For detailed technical information, see the [README.md](README.md) and [Library Documentation](Library/ClaudeConsole/README.md).