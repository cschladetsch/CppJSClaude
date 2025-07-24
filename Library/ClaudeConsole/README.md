# ClaudeConsole Library

This is the core library that provides Claude AI integration and command processing functionality for the CLL (Claude Command Line) project.

## Overview

The ClaudeConsole library provides a C++ interface for:
- Command execution and processing
- Shell and JavaScript mode switching
- Claude AI integration via subprocess calls
- Configuration management
- Alias system
- Command history

## Files

### Include/ClaudeConsole.h
Header file containing all public API declarations:
- `ClaudeConsole` class - main console functionality
- `CommandResult` struct - command execution results
- `CommandHistory` class - command history management
- `ConsoleMode` enum - shell vs JavaScript modes

### Source/ClaudeConsole.cpp
Implementation of all ClaudeConsole functionality:
- Command parsing and execution
- Claude AI integration
- Configuration file management
- Alias expansion
- Subprocess execution

## Usage

### Basic Usage
```cpp
#include "ClaudeConsole.h"

claude_console::ClaudeConsole console;
if (!console.Initialize()) {
    // Handle error
    return false;
}

// Execute a command
auto result = console.ExecuteCommand("ask What is 2+2?");
if (result.success) {
    std::cout << result.output << std::endl;
} else {
    std::cerr << "Error: " << result.error << std::endl;
}
```

### Advanced Features
```cpp
// Set custom output callbacks
console.SetOutputCallback([](const std::string& text) {
    std::cout << "[OUT] " << text;
});

console.SetErrorCallback([](const std::string& text) {
    std::cerr << "[ERR] " << text;
});

// Mode switching
console.SetMode(claude_console::ConsoleMode::JavaScript);
auto jsResult = console.ExecuteCommand("Math.PI * 2");

// Configuration management
console.LoadConfiguration();
console.SaveConfiguration();
```

## Dependencies

### Required
- C++20 compatible compiler
- Standard library with filesystem support
- POSIX-compatible system for subprocess execution

### Optional
- rang library (for colored output)
- boost libraries (for additional utilities)

### Runtime Dependencies
- Python 3.x (for Claude AI integration)
- PyClaudeCli or compatible 'ask' command

## Configuration

The library creates configuration files in `~/.config/claude-console/`:

### config.json
```json
{
  "default_mode": "shell",
  "prompt_format": "[{mode}] λ ",
  "show_execution_time": true,
  "history_size": 1000,
  "enable_colors": true,
  "claude_integration": {
    "enabled": true,
    "timeout_seconds": 30
  },
  "aliases": {
    "ll": "ls -la",
    "la": "ls -la",
    "...": "cd ../.."
  }
}
```

### aliases
```
# Command aliases
ll=ls -la
la=ls -la
...=cd ../..
cls=clear
q=quit
```

## API Reference

### ClaudeConsole Class

#### Public Methods
```cpp
// Initialization
bool Initialize();
void Shutdown();

// Command Execution
CommandResult ExecuteCommand(const std::string& command);
CommandResult ExecuteJavaScript(const std::string& code);
CommandResult ExecuteShellCommand(const std::string& command);
CommandResult ExecuteClaudeQuery(const std::string& question);

// Mode Management
void SetMode(ConsoleMode mode);
ConsoleMode GetMode() const;
bool IsJavaScriptMode() const;

// Built-in Commands
bool IsBuiltinCommand(const std::string& command) const;
CommandResult ExecuteBuiltinCommand(const std::string& command);

// Configuration
void LoadConfiguration();
void SaveConfiguration();

// Aliases
void SetAlias(const std::string& name, const std::string& value);
std::string ExpandAlias(const std::string& command) const;

// Output Callbacks
void SetOutputCallback(OutputCallback callback);
void SetErrorCallback(OutputCallback callback);

// Utilities
static std::string FormatExecutionTime(const std::chrono::microseconds& us);
static std::vector<std::string> SplitCommand(const std::string& command);
```

#### CommandResult Structure
```cpp
struct CommandResult {
    bool success;                          // Command executed successfully
    std::string output;                    // Standard output
    std::string error;                     // Error message (if any)
    std::chrono::microseconds executionTime; // Time taken to execute
    int exitCode;                          // Process exit code
};
```

### Built-in Commands

- `help` - Show available commands
- `ask <question>` - Query Claude AI
- `js` / `javascript` - Switch to JavaScript mode
- `shell` / `sh` - Switch to shell mode
- `config [alias name=value]` - Manage configuration
- `reload` - Reload configuration from files
- `clear` - Clear console
- `quit` / `exit` - Exit console

### Special Features

#### & Prefix
Execute JavaScript code from shell mode:
```
[sh] λ &Math.sqrt(64)
// JavaScript execution simulated
// Code: Math.sqrt(64)
```

#### Alias System
```
[sh] λ config alias myalias="echo hello"
Alias set: myalias = 'echo hello'
[sh] λ myalias
hello
```

## Building

### As Part of Main Project
The library is automatically built when building the main CLL project.

### Standalone
```bash
mkdir build && cd build
cmake ..
make ClaudeConsole
```

### Integration in Other Projects
```cmake
# In your CMakeLists.txt
add_subdirectory(path/to/ClaudeConsole)
target_link_libraries(your_target ClaudeConsole)
```

## Thread Safety

**⚠️ Not Thread Safe**: The library is not designed for multi-threaded use. Use external synchronization if needed.

## Error Handling

All methods use return codes and error messages rather than exceptions. Check `CommandResult.success` and `CommandResult.error` for error information.

## Contributing

When contributing to the library:
1. Maintain C++20 compatibility
2. Follow existing code style
3. Update this README for API changes
4. Add appropriate error handling
5. Test with both shell and JavaScript modes