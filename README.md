# CppV8-Claude Integration

This repository demonstrates the integration of Claude AI capabilities into a C++ V8 JavaScript engine console.

## Features

- **V8 JavaScript Engine**: Execute JavaScript code within a C++ application
- **Claude AI Integration**: Ask questions to Claude directly from the console using the `ask` command
- **Hybrid Console Mode**: Switch between shell and JavaScript execution modes
- **Cross-platform Support**: Works on Linux, macOS, and Windows

## Quick Start

```bash
# Build the project
mkdir build && cd build
cmake ..
make

# Run the console
./bin/v8_claude_console

# Use the ask command
ask "What is the capital of France?"

# Switch to JavaScript mode
js
console.log("Hello from V8!");

# Switch back to shell mode
shell
ls -la
```

## Architecture

The integration works by:

1. **Subprocess Execution**: The C++ console spawns the PyClaudeCli Python script as a subprocess
2. **Command Processing**: Built-in command handler recognizes `ask` commands
3. **Output Handling**: Claude responses are captured and displayed in the console
4. **Error Management**: Graceful handling of network issues and API errors

## Dependencies

- C++20 compatible compiler
- V8 JavaScript engine
- Python 3.x with PyClaudeCli
- CMake 3.15+

## Commands

- `ask <question>` - Ask Claude AI a question
- `js` - Switch to JavaScript mode
- `shell` - Switch to shell mode
- `help` - Show available commands
- `quit` - Exit the console