# Contributing to CLL (Claude Command Line)

Thank you for your interest in contributing to CLL! This guide will help you get started with contributing to the project.

## 🚀 Quick Start

1. **Fork the repository**
2. **Clone your fork**: `git clone https://github.com/yourusername/CppV8ClaudeIntegration.git`
3. **Build the project**: `./build.sh`
4. **Run tests**: `./test.sh`
5. **Make your changes**
6. **Test thoroughly**
7. **Submit a pull request**

## 📋 Table of Contents

- [Development Environment](#development-environment)
- [Project Structure](#project-structure)
- [Coding Standards](#coding-standards)
- [Testing Requirements](#testing-requirements)
- [Submitting Changes](#submitting-changes)
- [Creating Demos](#creating-demos)
- [Getting Help](#getting-help)

## 🛠️ Development Environment

### System Requirements

- **Operating System**: Linux, macOS, or Windows with WSL2
- **Compiler**: GCC 10+ or Clang 10+ with C++20 support
- **Build System**: CMake 3.15+
- **Version Control**: Git
- **Optional**: GNU Readline for enhanced terminal support

### Required Tools

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git libreadline-dev python3

# macOS
brew install cmake readline python3 git

# Install development dependencies
./build.sh deps
```

### Recommended IDE Setup

- **VS Code** with C++ extensions
- **CLion** with CMake support  
- **Qt Creator** with CMake project support
- **Vim/Neovim** with C++ language server

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
./test.sh
```

## 📁 Project Structure

```
CppV8ClaudeIntegration/
├── Bin/                    # Compiled binaries and executables
├── Library/               # Core ClaudeConsole library
│   └── ClaudeConsole/     # Main library implementation
│       ├── Include/       # Public API headers
│       ├── Source/        # Implementation files
│       └── CMakeLists.txt # Library build configuration
├── Source/                # Main application source (legacy)
├── Include/               # Legacy headers (use Library/ instead)
├── Tests/                 # Comprehensive test suite
├── External/              # Git submodules (rang, boost)
├── Resources/             # Images, demos, documentation assets
├── Tools/                 # Development tools (excluded from repo)
├── CMakeLists.txt         # Main build configuration
├── build.sh              # Build script with colored output
├── test.sh               # Comprehensive test runner
└── demo.sh               # Feature demonstration script
```

## 📖 Coding Standards

### C++ Guidelines

- **Standard**: C++20 minimum requirement
- **Style**: Follow existing code style in the project
- **Naming**: Use UpperCamelCase for files/classes, snake_case for variables
- **Namespace**: All code must be in the `cll` namespace
- **Headers**: Use `#pragma once` for header guards
- **Includes**: Group system headers, then project headers

### Code Style Example

```cpp
#pragma once

#include <string>
#include <vector>
#include <memory>

#include "ClaudeConsole.h"

namespace cll {

class CommandProcessor {
public:
    CommandProcessor();
    ~CommandProcessor();

    // Method names: UpperCamelCase
    CommandResult ExecuteCommand(const std::string& command);
    bool IsValidCommand(const std::string& command) const;

private:
    // Member variables: snake_case with trailing underscore
    std::vector<std::string> command_history_;
    std::unique_ptr<ClaudeConsole> console_;
    
    // Private methods: UpperCamelCase
    void ProcessBuiltinCommand(const std::string& command);
};

} // namespace cll
```

### File Naming Conventions

- **Source Files**: `ClaudeConsole.cpp`, `CommandProcessor.cpp`
- **Header Files**: `ClaudeConsole.h`, `CommandProcessor.h`
- **Test Files**: `TestClaudeConsole.cpp`, `TestCommandProcessor.cpp`
- **Directories**: `UpperCamelCase` (Library, Source, Tests)

### Documentation Standards

- **All public APIs must be documented**
- **Use Doxygen-style comments**
- **Include usage examples for complex functions**
- **Update README files for new features**

```cpp
/**
 * @brief Executes a command in the specified mode
 * @param command The command string to execute
 * @param mode The execution mode (Shell or JavaScript)
 * @return CommandResult containing output, timing, and error information
 * 
 * @example
 * CommandResult result = ExecuteCommand("ls -la", ConsoleMode::Shell);
 * if (result.success) {
 *     std::cout << result.output;
 * }
 */
CommandResult ExecuteCommand(const std::string& command, ConsoleMode mode);
```

## 🧪 Testing Requirements

### Test Categories

1. **Unit Tests**: Test individual functions and classes
2. **Integration Tests**: Test component interactions
3. **System Tests**: Test complete workflows
4. **Performance Tests**: Verify timing and memory usage

### Writing Tests

```cpp
#include <gtest/gtest.h>
#include "ClaudeConsole.h"

using namespace cll;

class ClaudeConsoleTest : public ::testing::Test {
protected:
    void SetUp() override {
        console = std::make_unique<ClaudeConsole>();
        ASSERT_TRUE(console->Initialize());
    }
    
    void TearDown() override {
        console->Shutdown();
        console.reset();
    }
    
    std::unique_ptr<ClaudeConsole> console;
};

TEST_F(ClaudeConsoleTest, ExecuteSimpleCommand) {
    auto result = console->ExecuteCommand("echo hello");
    EXPECT_TRUE(result.success);
    EXPECT_NE(result.output.find("hello"), std::string::npos);
}

TEST_F(ClaudeConsoleTest, JavaScriptMode) {
    console->SetMode(ConsoleMode::JavaScript);
    auto result = console->ExecuteCommand("Math.sqrt(64)");
    EXPECT_TRUE(result.success);
    // Test JavaScript execution
}
```

### Running Tests

```bash
# Run all tests
./test.sh

# Run specific test categories
./test.sh --filter "*Console*"
./test.sh --filter "*JavaScript*"

# Run with verbose output
./test.sh --verbose

# Run performance tests
./test.sh --performance
```

### Test Requirements for PRs

- **All new features must have tests**
- **Minimum 80% code coverage for new code**
- **All existing tests must pass**
- **Performance tests must not regress**

## 📤 Submitting Changes

### Before Submitting

1. **Run the full test suite**: `./test.sh`
2. **Build in both debug and release**: `./build.sh debug && ./build.sh release`
3. **Test on your target platform**
4. **Update documentation for new features**
5. **Create demo if adding user-facing features**

### Pull Request Process

1. **Create a feature branch**: `git checkout -b feature/amazing-feature`
2. **Make your changes** following coding standards
3. **Add/update tests** for your changes
4. **Update documentation** as needed
5. **Create demos** for new features
6. **Commit with descriptive messages**
7. **Push to your fork**: `git push origin feature/amazing-feature`
8. **Open a Pull Request** with detailed description

### Commit Message Format

```
feat: add multi-line JavaScript input support

- Implement multi-line mode detection for & prefix
- Add Ctrl-D termination for multi-line input
- Update prompt display for multi-line mode
- Add comprehensive tests for new functionality

Closes #123
```

**Commit Types:**
- `feat`: New features
- `fix`: Bug fixes
- `docs`: Documentation updates
- `test`: Test additions/modifications
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `build`: Build system changes

### Pull Request Template

```markdown
## Description
Brief description of changes and motivation.

## Type of Change
- [ ] Bug fix (non-breaking change that fixes an issue)
- [ ] New feature (non-breaking change that adds functionality)
- [ ] Breaking change (fix or feature that causes existing functionality to change)
- [ ] Documentation update

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests pass
- [ ] Manual testing completed
- [ ] Performance impact assessed

## Checklist
- [ ] Code follows project style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] Demo created (if applicable)
- [ ] All tests pass
```

## 🎬 Creating Demos

### When to Create Demos

- **New user-facing features**
- **Significant functionality changes**
- **Complex workflows**
- **Performance improvements**

### Demo Creation Process

```bash
# Create automated demo GIF
./Tools/write-demo.sh

# Create manual demo for screen recording
./Tools/demo_screengif.sh

# Test demo quality
# View generated ./Resources/Demo.gif
```

### Demo Guidelines

- **Keep demos under 60 seconds**
- **Show real-world usage scenarios**
- **Include error handling examples**
- **Demonstrate key features clearly**
- **Use realistic data and commands**

## 🏗️ Architecture Guidelines

### Adding New Features

1. **Library-First Design**: Add core functionality to `Library/ClaudeConsole/`
2. **Public API**: Expose features through clean public interfaces
3. **Testing**: Write tests before implementation (TDD)
4. **Documentation**: Document APIs and update READMEs
5. **Integration**: Update main application to use new features

### Performance Considerations

- **Memory Efficiency**: Use RAII and smart pointers
- **Execution Speed**: Profile and optimize critical paths
- **Startup Time**: Minimize initialization overhead
- **Responsiveness**: Keep UI responsive during long operations

### Error Handling

- **No Exceptions**: Use return codes and error messages
- **Graceful Degradation**: Provide meaningful fallbacks
- **User-Friendly Messages**: Clear, actionable error descriptions
- **Logging**: Include debug information for troubleshooting

## 🐛 Bug Reports

### Before Reporting

1. **Update to latest version**
2. **Check existing issues**
3. **Test with minimal configuration**
4. **Gather system information**

### Bug Report Template

```markdown
**Bug Description**
Clear description of the bug.

**To Reproduce**
1. Run command '...'
2. Enter input '...'
3. See error

**Expected Behavior**
What should have happened.

**System Information**
- OS: [e.g., Ubuntu 22.04]
- Compiler: [e.g., GCC 11.2]
- CLL Version: [e.g., v1.0.0]

**Additional Context**
Logs, screenshots, or other relevant information.
```

## 💡 Feature Requests

### Feature Request Process

1. **Check existing issues** for similar requests
2. **Discuss in discussions** for community feedback
3. **Create detailed feature request** with use cases
4. **Consider implementation complexity**
5. **Volunteer to implement** if possible

### Feature Request Template

```markdown
**Feature Description**
Clear, concise description of the feature.

**Use Case**
Why is this feature needed? What problem does it solve?

**Proposed Solution**
How should this feature work?

**Alternative Solutions**
Other ways to solve the same problem.

**Implementation Notes**
Technical considerations or challenges.
```

## 🤝 Getting Help

### Community Resources

- **GitHub Discussions**: General questions and community support
- **GitHub Issues**: Bug reports and feature requests
- **README Files**: Comprehensive documentation
- **Code Comments**: Implementation details and API docs

### Development Help

- **Architecture Questions**: Review existing code patterns
- **Testing Help**: Look at existing test files for examples
- **Build Issues**: Check build logs and dependency requirements
- **Demo Creation**: Use existing Tools/ scripts as templates

### Contact

- **Maintainer**: Check GitHub repository for current maintainers
- **Contributors**: See CONTRIBUTORS.md for active contributors
- **Issues**: Create GitHub issue for specific problems

## 📄 License

By contributing to CLL, you agree that your contributions will be licensed under the same license as the project (see LICENSE file).

## 🎉 Recognition

Contributors are recognized in:
- **CONTRIBUTORS.md**: List of all contributors
- **Release Notes**: Major contribution acknowledgments
- **Code Comments**: Credit for significant implementations
- **GitHub**: Automatic contribution tracking

---

Thank you for contributing to CLL! Your efforts help make this project better for everyone. 🚀