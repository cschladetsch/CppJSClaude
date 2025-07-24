# Source Directory

This directory contains the core implementation of the CLL (Claude Command Line) project.

## Files Overview

### ClaudeConsole.cpp
**Legacy core console implementation (moved to Library)**

⚠️ **Note**: This file is legacy code. The active implementation is now in `Library/ClaudeConsole/Source/ClaudeConsole.cpp`

- **Purpose**: Original console engine prototype (kept for reference)
- **Key Features**:
  - Hybrid shell/JavaScript execution modes
  - Built-in command processing (`help`, `ask`, `clear`, etc.)
  - `&` prefix for JavaScript execution in shell mode
  - Subprocess integration with PyClaudeCli
  - Command history and timing
  - Error handling and output formatting

- **Key Methods**:
  ```cpp
  CommandResult ExecuteCommand(const std::string& command);     // Main command dispatcher
  CommandResult ExecuteJavaScript(const std::string& code);    // JavaScript execution (simulated)
  CommandResult ExecuteShellCommand(const std::string& command); // Shell command execution
  CommandResult ExecuteClaudeQuery(const std::string& question); // Claude AI integration
  ```

### Main.cpp
**Interactive terminal UI with readline support**

- **Purpose**: Provides the interactive console interface for users
- **Key Features**:
  - Colored terminal output with ANSI escape codes
  - Readline integration for command history and editing
  - Professional welcome screen with ASCII art
  - Real-time execution timing display
  - Mode indicator in prompt (`[js]` vs `[sh]`)
  - Graceful error handling and display

- **Key Classes**:
  ```cpp
  class ConsoleUI {
      void Run();                    // Main interaction loop
      void ProcessCommand();         // Handle user input
      std::string GetPrompt();       // Dynamic prompt generation
  };
  ```

## Architecture

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   main.cpp      │    │ ClaudeConsole   │    │   PyClaudeCli   │
│  (Terminal UI)  │───▶│   (Core Logic)  │───▶│  (AI Backend)   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
        │                       │                       │
        ▼                       ▼                       ▼
  • Readline           • Command Parsing         • Claude API
  • Color Output       • Mode Management         • Response Processing
  • User Interaction   • Subprocess Execution    • Error Handling
```

## Command Flow

1. **User Input** → `ConsoleUI::ProcessCommand()`
2. **Command Dispatch** → `ClaudeConsole::ExecuteCommand()`
3. **Mode Detection**:
   - Built-in command → `ExecuteBuiltinCommand()`
   - Shell mode → `ExecuteShellCommand()`
   - JavaScript mode → `ExecuteJavaScript()`
   - `&` prefix → `ExecuteJavaScript(command.substr(1))`
   - `ask` command → `ExecuteClaudeQuery()`
4. **Output Processing** → Terminal display with formatting

## Build Dependencies

- **C++20 Standard**: Required for `std::format` and modern features
- **GNU Readline**: Optional, for command history and editing
- **POSIX System Calls**: For subprocess execution (`popen`, `pclose`)
- **Standard Library**: `<filesystem>`, `<chrono>`, `<format>`

## Integration Points

### Claude AI Integration
```cpp
// Checks for PyClaudeCli availability
bool CheckClaudeAvailability();

// Finds PyClaudeCli in common locations
std::string FindPyClaudeCliPath();

// Executes subprocess with proper error handling
CommandResult ExecuteSubprocess(const std::string& command);
```

### JavaScript Integration (Placeholder)
```cpp
// Currently simulated, ready for V8 integration
CommandResult ExecuteJavaScript(const std::string& code) {
    // TODO: Replace with actual V8 execution
    // For now: simulate execution for demo purposes
}
```

## Error Handling

- **Subprocess Errors**: Captured via stderr redirection (`2>&1`)
- **Command Not Found**: Graceful error messages
- **Invalid Syntax**: Proper error reporting
- **Network Issues**: Claude API error handling
- **Memory Management**: RAII patterns throughout

## Performance Considerations

- **Execution Timing**: All commands are timed using `std::chrono::high_resolution_clock`
- **Memory Efficiency**: String operations use move semantics where possible
- **Subprocess Overhead**: Minimal overhead for shell command execution
- **Buffer Management**: Fixed-size buffers for subprocess communication

## Current Implementation Status

✅ **Completed Features:**
- **Configuration System**: JSON config in `~/.config/cll/`
- **Library Architecture**: Modular ClaudeConsole library
- **Comprehensive Testing**: GTest integration with `./test.sh`
- **Build System**: CMake with external dependencies
- **Demo System**: Automated GIF generation tools

📍 **Active Development:**
- **Main Implementation**: Now in `Library/ClaudeConsole/`
- **Static Library**: Reusable ClaudeConsole component
- **Modern C++20**: Updated architecture and features

## Demo and Testing

### Creating Source Code Demos

Generate demonstrations of the source architecture:

```bash
# From project root - automated demo creation
./Tools/write-demo.sh

# Manual demo with source code focus
./Tools/demo_screengif.sh

# Show all demo options
./Tools/write-demo.sh --help
```

### Testing Source Implementation

```bash
# Run comprehensive test suite
../test.sh

# Test specific components
../test.sh --filter "*Console*"

# Performance testing
../test.sh --verbose
```

## Future Enhancements

1. **Real V8 Integration**: Replace simulated JavaScript with actual V8 engine
2. **Async Execution**: Non-blocking command execution for long-running tasks
3. **Plugin System**: Dynamic loading of additional commands
4. **Advanced Configuration**: Enhanced user customization
5. **Multi-threading**: Parallel execution of independent commands