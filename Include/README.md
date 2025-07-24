# Include Directory

This directory contains the header files that define the public API and interfaces for the CppV8-Claude Integration project.

## Files Overview

### ClaudeConsole.h
**Main header defining the console interface and data structures**

## Public API Reference

### Core Classes

#### `class ClaudeConsole`
**Main console class with Claude AI integration**

```cpp
namespace claude_console {
    class ClaudeConsole {
    public:
        ClaudeConsole();
        virtual ~ClaudeConsole();
        
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
        
        // Utilities
        static std::string FormatExecutionTime(const std::chrono::microseconds& us);
        static std::vector<std::string> SplitCommand(const std::string& command);
        
        // Output Callbacks
        using OutputCallback = std::function<void(const std::string&)>;
        void SetOutputCallback(OutputCallback callback);
        void SetErrorCallback(OutputCallback callback);
    };
}
```

#### `class CommandHistory`
**Command history management with navigation**

```cpp
class CommandHistory {
public:
    CommandHistory(size_t maxSize = 1000);
    
    // History Management
    void Add(const std::string& command);
    const std::string& Get(size_t index) const;
    size_t Size() const;
    
    // Navigation
    void ResetPosition();
    std::string GetPrevious();
    std::string GetNext();
};
```

### Data Structures

#### `struct CommandResult`
**Result of command execution with timing and error information**

```cpp
struct CommandResult {
    bool success;                          // Command executed successfully
    std::string output;                    // Standard output
    std::string error;                     // Error message (if any)
    std::chrono::microseconds executionTime; // Time taken to execute
    int exitCode;                          // Process exit code
};
```

#### `enum class ConsoleMode`
**Console execution modes**

```cpp
enum class ConsoleMode {
    Shell,      // Execute commands as shell commands
    JavaScript  // Execute commands as JavaScript code
};
```

## Design Patterns

### RAII (Resource Acquisition Is Initialization)
- All resources are properly managed in constructors/destructors
- Automatic cleanup on scope exit
- Exception-safe resource management

### Strategy Pattern
- Different execution strategies based on console mode
- Pluggable command handlers for built-in commands
- Flexible output callback system

### Template Callback Pattern
```cpp
using OutputCallback = std::function<void(const std::string&)>;
```
- Type-safe function callbacks for output handling
- Allows custom output formatting and redirection
- Separation of concerns between logic and presentation

## Usage Examples

### Basic Console Setup
```cpp
#include "ClaudeConsole.h"

claude_console::ClaudeConsole console;
if (!console.Initialize()) {
    // Handle initialization error
    return false;
}

// Set custom output handlers
console.SetOutputCallback([](const std::string& text) {
    std::cout << "[OUT] " << text;
});

console.SetErrorCallback([](const std::string& text) {
    std::cerr << "[ERR] " << text;
});
```

### Command Execution
```cpp
// Execute shell command
auto result = console.ExecuteCommand("ls -la");
if (result.success) {
    std::cout << "Output: " << result.output;
    std::cout << "Time: " << ClaudeConsole::FormatExecutionTime(result.executionTime);
}

// Execute JavaScript with & prefix
result = console.ExecuteCommand("&Math.sqrt(64)");

// Ask Claude AI
result = console.ExecuteCommand("ask What is the meaning of life?");
```

### Mode Management
```cpp
// Switch to JavaScript mode
console.SetMode(claude_console::ConsoleMode::JavaScript);
auto result = console.ExecuteCommand("Math.PI * 2");

// Check current mode
if (console.IsJavaScriptMode()) {
    std::cout << "Currently in JavaScript mode";
}
```

### Command History
```cpp
claude_console::CommandHistory history;
history.Add("ls -la");
history.Add("ask Hello Claude");

// Navigate history
std::string prev = history.GetPrevious(); // "ask Hello Claude"
std::string prev2 = history.GetPrevious(); // "ls -la"
```

## Thread Safety

**⚠️ Important**: The current implementation is **not thread-safe**. If using in a multi-threaded environment:

1. **External Synchronization**: Use mutexes or other synchronization primitives
2. **Single Consumer**: Ensure only one thread calls methods at a time
3. **Callback Safety**: Output callbacks may be called from different contexts

## Memory Management

- **Automatic**: All memory is managed automatically using RAII
- **String Handling**: Uses move semantics where possible for efficiency
- **No Manual Allocation**: No `new`/`delete` or `malloc`/`free` calls
- **Exception Safe**: Strong exception safety guarantee

## Error Handling Strategy

### Exception Policy
- **No Exceptions Thrown**: All methods use return codes and error messages
- **Graceful Degradation**: Failed operations return meaningful error information
- **Resource Cleanup**: Guaranteed cleanup even on errors

### Error Reporting
```cpp
CommandResult result = console.ExecuteCommand("invalid_command");
if (!result.success) {
    std::cerr << "Error: " << result.error << std::endl;
    std::cerr << "Exit code: " << result.exitCode << std::endl;
}
```

## Compiler Requirements

- **C++20**: Required for `std::format`, concepts, and other modern features
- **Standard Library**: Full C++20 standard library support
- **Platform**: POSIX-compliant system for subprocess execution
- **Optional**: GNU Readline for enhanced command-line editing

## Header Dependencies

```cpp
#include <string>          // String operations
#include <vector>          // Command splitting
#include <map>             // Built-in command storage
#include <chrono>          // Execution timing
#include <memory>          // Smart pointers
#include <functional>      // Callback functions
```

## Future API Extensions

### Planned Additions
```cpp
// Plugin system
class CommandPlugin {
public:
    virtual CommandResult Execute(const std::vector<std::string>& args) = 0;
    virtual std::string GetHelp() const = 0;
};

void RegisterPlugin(const std::string& name, std::unique_ptr<CommandPlugin> plugin);

// Configuration management
struct ConsoleConfig {
    std::string prompt;
    bool enableColors;
    size_t historySize;
    std::map<std::string, std::string> aliases;
};

void LoadConfig(const std::string& configPath);
void SaveConfig(const std::string& configPath) const;
```