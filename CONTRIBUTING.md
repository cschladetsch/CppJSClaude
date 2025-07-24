# Contributing to CLL (Claude Command Line)

Thank you for your interest in contributing to CLL! This comprehensive guide will help you contribute effectively to the project.

## 🚀 Quick Start

1. **Fork the repository**
2. **Clone your fork**: `git clone https://github.com/yourusername/CppV8ClaudeIntegration.git`
3. **Build the project**: `./build.sh`
4. **Run comprehensive demo**: `./demo.sh`
5. **Run tests**: `./build.sh test`
6. **Make your changes**
7. **Test thoroughly**
8. **Submit a pull request**

## 📋 Table of Contents

- [Development Environment](#development-environment)
- [Project Architecture](#project-architecture)
- [Coding Standards](#coding-standards)
- [V8 JavaScript Integration](#v8-javascript-integration)
- [Testing Requirements](#testing-requirements)
- [Submitting Changes](#submitting-changes)
- [Creating Demos](#creating-demos)
- [Getting Help](#getting-help)

## 🛠️ Development Environment

### System Requirements

- **Operating System**: Linux, macOS, or Windows with WSL2
- **Compiler**: GCC 10+ or Clang 10+ with C++20 support
- **Build System**: CMake 3.15+
- **Version Control**: Git (for submodule management)
- **Python**: 3.x with claude-cli (for AI integration)

### Required Tools

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git libreadline-dev python3 libv8-dev

# macOS
brew install cmake readline python3 git v8

# Install Claude AI CLI
pip install claude-cli
claude auth

# Initialize project dependencies
./build.sh deps
```

### Optional Dependencies

CLL uses intelligent dependency management:

```bash
# V8 JavaScript Engine (optional but recommended)
# System package (Ubuntu/Debian)
sudo apt-get install libv8-dev

# Or build from source (automatically managed)
cmake .. -DBUILD_V8_FROM_SOURCE=ON

# Auto-fetched lightweight dependencies
# - rang (colored output)
# - nlohmann/json (JSON configuration)
```

### Environment Setup

```bash
# Clone and build
git clone https://github.com/cschladetsch/CppV8ClaudeIntegration.git
cd CppV8ClaudeIntegration

# Initialize submodules and build
./build.sh deps
./build.sh release

# Verify installation
./Bin/cll --help
./demo.sh --fast
```

### IDE Recommendations

- **VS Code** with C++ extensions and CMake tools
- **CLion** with full CMake and Git integration  
- **Qt Creator** with CMake project support
- **Vim/Neovim** with LSP and C++ language server

## 🏗️ Project Architecture

### High-Level Structure

```
CLL Architecture:
┌─────────────────────────────────────────────────────────────────┐
│                        CLL Project                              │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │ Main App        │  │ ClaudeConsole   │  │ V8 JavaScript   │  │
│  │ - CLI entry     │  │ Library         │  │ Integration     │  │
│  │ - User I/O      │  │ - Core engine   │  │ - Real JS exec  │  │
│  │ - Mode mgmt     │  │ - Command proc  │  │ - DLL loading   │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐  │
│  │ Configuration   │  │ Claude AI       │  │ Demo & Tools    │  │
│  │ - ~/.config/    │  │ Integration     │  │ - demo.sh       │  │
│  │ - Shared setup  │  │ - ? prefix      │  │ - build.sh      │  │
│  │ - JSON/aliases  │  │ - Python CLI    │  │ - Testing       │  │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

### Directory Structure

```
CppV8ClaudeIntegration/
├── Bin/                        # Compiled binaries
│   └── cll                    # Main CLL executable
├── Source/                     # Main application source
│   └── Main.cpp               # Application entry point
├── Library/                    # Core library components
│   └── ClaudeConsole/         # ClaudeConsole library
│       ├── Include/           # Library headers
│       │   ├── ClaudeConsole.h  # Main API
│       │   ├── DllLoader.h      # DLL hot-loading
│       │   └── V8Compat.h       # V8 compatibility
│       ├── Source/            # Implementation
│       │   ├── ClaudeConsole.cpp
│       │   └── DllLoader.cpp
│       └── CMakeLists.txt     # Library build config
├── External/                   # Auto-managed dependencies
│   ├── rang/                  # Colored output (auto-fetched)
│   ├── json/                  # JSON library (auto-fetched)
│   └── v8/                    # V8 engine (optional)
├── Tests/                      # Comprehensive test suite
├── Tools/                      # Development and demo tools
├── Resources/                  # Documentation assets
├── CMakeLists.txt             # Main build configuration
├── build.sh                   # Intelligent build script
├── demo.sh                    # Comprehensive demo
└── README.md                  # Project documentation
```

## 📖 Coding Standards

### C++ Guidelines

- **Standard**: C++20 minimum requirement with full feature usage
- **Style**: Follow existing project patterns and conventions
- **Naming**: 
  - Classes/Files: `UpperCamelCase` (`ClaudeConsole`, `DllLoader`)
  - Methods: `UpperCamelCase` (`ExecuteCommand`, `LoadConfiguration`)
  - Variables: `snake_case_` for members, `snake_case` for locals
  - Constants: `UPPER_SNAKE_CASE`
- **Namespace**: All code in the `cll` namespace
- **Headers**: Use `#pragma once` exclusively
- **Includes**: System headers first, then project headers

### Code Style Example

```cpp
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "ClaudeConsole.h"

namespace cll {

/**
 * @brief Advanced command processor with V8 integration
 * 
 * This class handles command execution, mode switching, and
 * integration with V8 JavaScript engine and Claude AI.
 */
class CommandProcessor {
public:
    CommandProcessor();
    virtual ~CommandProcessor();

    // Core command execution
    CommandResult ExecuteCommand(const std::string& command);
    CommandResult ExecuteJavaScript(const std::string& code);
    CommandResult ExecuteClaudeQuery(const std::string& question);

    // Mode management
    void SetMode(ConsoleMode mode);
    ConsoleMode GetMode() const;

private:
    // Member variables with trailing underscore
    std::vector<std::string> command_history_;
    std::unique_ptr<ClaudeConsole> console_;
    ConsoleMode current_mode_;
    
    // Private helper methods
    bool ValidateCommand(const std::string& command) const;
    void ProcessBuiltinCommand(const std::string& command);
    std::string FormatOutput(const CommandResult& result) const;
};

} // namespace cll
```

### V8 Integration Standards

When working with V8 integration:

```cpp
#ifdef HAS_V8
    // V8-specific implementation
    #include "V8Compat.h"
    
    bool InitializeV8Engine() {
        platform_ = v8_compat::CreateDefaultPlatform();
        v8::V8::InitializePlatform(platform_.get());
        v8::V8::Initialize();
        return true;
    }
#else
    // Graceful fallback for systems without V8
    bool InitializeV8Engine() {
        // Simulation mode - all JavaScript is simulated
        return true;
    }
#endif
```

### Configuration System Standards

```cpp
// Always use shared configuration paths
std::string GetConfigPath() const {
    return std::string(std::getenv("HOME")) + "/.config/cll/";
}

std::string GetSharedConfigPath() const {
    return std::string(std::getenv("HOME")) + "/.config/shared/";
}

// Load configuration with error handling
bool LoadConfiguration() {
    CreateConfigDirectory();
    LoadSharedConfiguration();
    
    try {
        // Load JSON configuration
        auto config_path = GetConfigPath() + "config.json";
        if (std::filesystem::exists(config_path)) {
            // Parse and apply configuration
        }
        return true;
    } catch (const std::exception& e) {
        Error("Failed to load configuration: " + std::string(e.what()));
        return false;
    }
}
```

### Documentation Standards

- **All public APIs must be fully documented**
- **Use Doxygen-style comments**
- **Include usage examples for complex functions**
- **Document thread safety and performance characteristics**
- **Update README files for architectural changes**

```cpp
/**
 * @brief Executes a command with full context and timing
 * 
 * This method handles command execution in the current mode,
 * providing comprehensive timing information and error handling.
 * Supports shell commands, JavaScript execution, and Claude AI queries.
 * 
 * @param command The command string to execute
 * @return CommandResult with output, timing, and error information
 * 
 * @note This method is not thread-safe
 * @warning JavaScript execution requires V8 engine availability
 * 
 * @example
 * ```cpp
 * auto result = console.ExecuteCommand("&Math.sqrt(64)");
 * if (result.success) {
 *     std::cout << "Result: " << result.output << std::endl;
 *     std::cout << "Executed in: " << 
 *         ClaudeConsole::FormatExecutionTime(result.executionTime) << std::endl;
 * }
 * ```
 */
CommandResult ExecuteCommand(const std::string& command);
```

## ⚡ V8 JavaScript Integration

### Conditional Compilation

Always use conditional compilation for V8 features:

```cpp
class ClaudeConsole {
private:
#ifdef HAS_V8
    // V8-specific members
    std::unique_ptr<v8::Platform> platform_;
    v8::Isolate* isolate_;
    v8::Persistent<v8::Context> context_;
    std::unique_ptr<DllLoader> dll_loader_;
#endif

public:
    bool InitializeJavaScript() {
#ifdef HAS_V8
        return InitializeV8Engine();
#else
        // JavaScript execution will be simulated
        return true;
#endif
    }
};
```

### V8 Compatibility

Use the V8Compat layer for version independence:

```cpp
#ifdef HAS_V8
#include "V8Compat.h"

bool ExecuteJavaScriptCode(const std::string& code) {
    v8_compat::TryCatch try_catch(isolate_);
    
    auto context = context_.Get(isolate_);
    auto result = v8_compat::CompileAndRun(isolate_, context, code, "<repl>");
    
    if (try_catch.HasCaught()) {
        std::string error = try_catch.GetDetailedError(isolate_, context);
        Error("JavaScript error: " + error);
        return false;
    }
    
    return true;
}
#endif
```

### DLL Loading Integration

For DLL hot-loading features:

```cpp
#ifdef HAS_V8
bool LoadDynamicLibrary(const std::string& path) {
    if (!dll_loader_) {
        dll_loader_ = std::make_unique<DllLoader>();
    }
    
    if (dll_loader_->LoadDll(path)) {
        Output("Library loaded: " + path);
        return true;
    } else {
        Error("Failed to load library: " + path);
        return false;
    }
}
#endif
```

## 🧪 Testing Requirements

### Test Categories

1. **Unit Tests**: Individual component testing
2. **Integration Tests**: Component interaction testing
3. **V8 Tests**: JavaScript engine functionality
4. **Configuration Tests**: Settings and alias management
5. **Performance Tests**: Timing and memory benchmarks
6. **End-to-End Tests**: Complete workflow validation

### Test Framework Setup

```cpp
#include <gtest/gtest.h>
#include "ClaudeConsole.h"

using namespace cll;

class ClaudeConsoleTest : public ::testing::Test {
protected:
    void SetUp() override {
        console_ = std::make_unique<ClaudeConsole>();
        ASSERT_TRUE(console_->Initialize()) << "Failed to initialize console";
    }
    
    void TearDown() override {
        if (console_) {
            console_->Shutdown();
            console_.reset();
        }
    }
    
    std::unique_ptr<ClaudeConsole> console_;
};

// Test shell command execution
TEST_F(ClaudeConsoleTest, ExecuteShellCommand) {
    auto result = console_->ExecuteCommand("echo 'test'");
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.output.find("test"), std::string::npos);
    EXPECT_GT(result.executionTime.count(), 0);
}

// Test JavaScript integration (conditional)
TEST_F(ClaudeConsoleTest, JavaScriptExecution) {
#ifdef HAS_V8
    auto result = console_->ExecuteCommand("&Math.sqrt(64)");
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.output.find("8"), std::string::npos);
#else
    auto result = console_->ExecuteCommand("&Math.sqrt(64)");
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.output.find("simulated"), std::string::npos);
#endif
}

// Test Claude AI integration
TEST_F(ClaudeConsoleTest, ClaudeAIQuery) {
    auto result = console_->ExecuteCommand("?What is 2+2?");
    // Test should work regardless of claude-cli availability
    EXPECT_TRUE(result.success || !result.error.empty());
}
```

### Performance Testing

```cpp
TEST_F(ClaudeConsoleTest, PerformanceBenchmark) {
    const int iterations = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto result = console_->ExecuteCommand("echo test");
        EXPECT_TRUE(result.success);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Average should be under 5ms per command
    EXPECT_LT(duration.count() / iterations, 5000);
}
```

### Running Tests

```bash
# Run all tests
./build.sh test

# Run specific test patterns
cd build
ctest --test-dir . -R "Console"
ctest --test-dir . -R "JavaScript"
ctest --test-dir . -R "Performance"

# Run with verbose output
ctest --test-dir . --verbose

# Run comprehensive demo
./demo.sh --fast
```

## 📤 Submitting Changes

### Pre-Submission Checklist

1. **Build verification**: `./build.sh clean && ./build.sh release`
2. **Test suite**: `./build.sh test`
3. **Demo verification**: `./demo.sh`
4. **V8 testing**: Test both with and without V8 integration
5. **Documentation updates**: Update relevant README files
6. **Performance check**: Ensure no significant regressions

### Pull Request Process

1. **Create feature branch**: `git checkout -b feature/amazing-feature`
2. **Implement changes** following coding standards
3. **Add comprehensive tests** for new functionality
4. **Update documentation** including README files
5. **Test thoroughly** on target platforms
6. **Create demo** for user-facing features
7. **Commit with clear messages**
8. **Push and create PR** with detailed description

### Commit Message Standards

```
feat(v8): add hot-reloading for DLL libraries

- Implement DLL loading/unloading in V8 context
- Add reloadDll() JavaScript function
- Create comprehensive error handling for library loading
- Add performance benchmarks for DLL operations
- Update demo to showcase hot-reloading workflow

Closes #145
```

**Commit Types:**
- `feat`: New features
- `fix`: Bug fixes  
- `docs`: Documentation updates
- `test`: Test improvements
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `build`: Build system changes
- `chore`: Maintenance tasks

### Pull Request Template

```markdown
## Description
Brief description of changes and motivation.

## Type of Change
- [ ] Bug fix (non-breaking change fixing an issue)
- [ ] New feature (non-breaking change adding functionality)
- [ ] Breaking change (change affecting existing functionality)
- [ ] Performance improvement
- [ ] Documentation update

## V8 Integration Impact
- [ ] V8-related changes tested with engine available
- [ ] V8-related changes tested with engine unavailable
- [ ] No V8 integration impact

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests pass
- [ ] Performance tests pass
- [ ] Demo updated (if applicable)
- [ ] Manual testing completed on target platforms

## Documentation
- [ ] Code comments updated
- [ ] README files updated
- [ ] API documentation updated
- [ ] Demo script updated

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] All tests pass locally
- [ ] Build succeeds in both debug and release modes
- [ ] Demo script runs successfully
```

## 🎬 Creating Demos

### When to Create Demos

- **New user-facing features** requiring demonstration
- **Complex workflows** that benefit from visual explanation
- **Performance improvements** with measurable impact
- **Integration features** showing component interaction

### Demo Creation

```bash
# Run comprehensive demo
./demo.sh

# Create demo with different speeds
./demo.sh --fast     # Quick demonstration
./demo.sh --slow     # Detailed explanation
./demo.sh --help     # See all options

# Test specific demo sections
./demo.sh --interactive  # Step through manually
```

### Demo Guidelines

- **Keep demos focused** on specific functionality
- **Show realistic usage** scenarios
- **Include error handling** examples
- **Demonstrate performance** characteristics
- **Test on clean systems** to verify dependencies

## 🏗️ Architecture Guidelines

### Adding New Features

1. **Library-First Design**
   - Implement core functionality in `Library/ClaudeConsole/`
   - Expose clean, documented APIs
   - Use conditional compilation for optional features

2. **V8 Integration Considerations**
   - Always provide fallback for systems without V8
   - Use V8Compat layer for version independence
   - Test both V8 and simulation modes

3. **Configuration Management**
   - Use shared configuration system
   - Support both JSON and plain text formats
   - Provide migration paths for configuration changes

4. **Error Handling**
   - Use return codes rather than exceptions
   - Provide detailed error messages
   - Implement graceful degradation

### Performance Considerations

- **Memory Efficiency**: Use RAII and smart pointers consistently
- **Startup Performance**: Lazy-load optional components
- **Command Execution**: Minimize overhead for shell commands
- **V8 Performance**: Reuse contexts and optimize compilation

## 🐛 Bug Reports & Feature Requests

### Bug Report Process

1. **Search existing issues** for duplicates
2. **Test with latest version** and clean configuration
3. **Create minimal reproduction** case
4. **Gather system information** and logs
5. **Submit detailed bug report**

### Feature Request Guidelines

1. **Describe the problem** being solved
2. **Propose specific solution** with usage examples
3. **Consider implementation** complexity and impact
4. **Discuss in community** for feedback
5. **Volunteer to implement** if possible

## 🤝 Getting Help

### Community Resources

- **GitHub Discussions**: General questions and design discussions
- **GitHub Issues**: Bug reports and feature requests
- **README Documentation**: Comprehensive usage guides
- **Code Comments**: Implementation details and examples

### Development Support

- **Architecture Questions**: Review existing patterns in codebase
- **V8 Integration**: Check V8Compat.h for version compatibility
- **Testing**: Use existing test files as templates
- **Build Issues**: Check CMakeLists.txt and build.sh

### Contact Information

- **Maintainers**: Listed in CONTRIBUTORS.md
- **Issues**: Create GitHub issue for specific problems
- **Discussions**: Use GitHub Discussions for general questions

## 📄 License & Recognition

By contributing to CLL, you agree that your contributions will be licensed under the same license as the project.

**Contributors are recognized in:**
- CONTRIBUTORS.md with detailed contribution history
- Release notes for significant contributions
- Code comments for major implementations
- GitHub's automatic contribution tracking

---

**Thank you for contributing to CLL!** Your efforts help create a powerful, modern command-line tool that combines the best of shell, JavaScript, and AI capabilities. 🚀