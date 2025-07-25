# CLL (Claude Command Line)

<img src="Resources/Title.png" alt="CLL Title" width="400" height="auto">

## Demo

![Demo](Resources/Demo1.gif)

## Description

A high-performance C++20 command-line interface with a sophisticated three-mode system: Shell (λ), JavaScript (φ), and Ask (θ). CLL provides seamless switching between shell commands, V8 JavaScript execution, and Claude AI interactions with visual prompt indicators and simple text commands.

## Features

- **🎯 Three-Mode System**: Shell (λ), JavaScript (φ), and Ask (θ) modes with visual prompts
- **🔄 Simple Mode Switching**: Type `sh`, `js`, or `ask` to change modes instantly
- **🐚 Zsh-Like Shell**: Full shell command support (globbing, pipes, redirection, timing)
- **⚡ V8 JavaScript Engine**: Real JavaScript execution with Math, Date, console support
- **🤖 Claude AI Integration**: Natural language queries and programming assistance
- **⚡ Single-Command Prefixes**: `&<code>` for JS, `?<question>` for Claude, `$<command>` for shell
- **🎨 Greek Letter Prompts**: Visual indicators - λ (shell), φ (JavaScript), θ (ask)
- **📋 Smart Configuration**: Auto-setup in `~/.config/cll/` and `~/.config/shared/`
- **🏗️ Library Architecture**: Modular ClaudeConsole library for integration
- **⏱️ Performance Monitoring**: Precise execution timing for all commands

## Quick Start

```bash
# Build and run comprehensive demo (installs dependencies and shows all features)
./demo.sh

# Or build manually
./build.sh release

# Run CLL (starts in Shell mode with λ prompt)
./Bin/cll

# Shell mode - execute any shell command
λ ls -la | grep README
-rw-r--r-- 1 user user 12543 Nov 24 10:30 README.md
(2.1ms)

# Switch to JavaScript mode
λ js
φ Math.sqrt(64)
8
φ console.log("Hello World!")
Hello World

# Switch to Ask mode
φ ask  
θ What is the capital of France?
Paris

# Switch back to Shell mode
θ sh
λ pwd
/home/user/project

# Single-command prefixes (stay in current mode)
λ &Math.PI * 2        # JavaScript
6.283185307179586
λ ?What is capital of canada        # Ask Claude
Ottawa
λ $pwd                # Shell command from any mode
/home/user/project

# Comments work like bash/zsh
λ # This is a comment and is ignored

# cmd command - execute shell and process output in current mode
λ js
φ cmd echo `Math.sqrt(16)`
4
φ ask  
θ cmd echo `What is JavaScript?`
JavaScript is a programming language...
```

## Three-Mode System

CLL operates in three distinct modes, each with its own visual prompt and behavior:

### 🐚 Shell Mode (λ)
- **Default mode** - starts here automatically
- **Prompt**: `λ` (lambda in cyan)
- **Behavior**: Execute shell commands, pipes, globbing
- **Switch**: Type `sh` or `shell`

### ⚡ JavaScript Mode (φ) 
- **Prompt**: `φ` (phi in yellow)
- **Behavior**: Everything you type is JavaScript code
- **Features**: Math, Date, console, all V8 JavaScript
- **Switch**: Type `js` or `javascript`

### 🤖 Ask Mode (θ)
- **Prompt**: `θ` (theta in magenta)  
- **Behavior**: Everything you type goes to Claude AI
- **Features**: Natural language queries with common knowledge, external Claude CLI fallback
- **Switch**: Type `ask`

#### Common Knowledge Support
CLL can answer common questions directly:
- **World Capitals**: `what is capital of canada` → `Ottawa`
- **Greetings**: `hello` → `Hello! How can I help you today?`
- **Time/Date**: `what time is it` → Current system time
- **Help**: `help` → Shows available topics and usage
- **Fallback**: Unknown queries suggest external Claude CLI installation

### Single-Command Prefixes
Execute once and stay in current mode:
- `&<code>` - Run JavaScript code
- `?<question>` - Ask Claude a question
- `$<command>` - Run shell command

### Comments
- `# anything` - Ignored as comments (like bash/zsh)

## Architecture

CLL uses a modern library-based architecture with V8 JavaScript engine integration:

```
CLL Architecture:
┌─────────────────┐    ┌────────────────────┐    ┌─────────────────────┐
│  CLL Binary     │───▶│ ClaudeConsole Lib  │───▶│ V8 JavaScript       │
│  (Main App)     │    │ (Core Engine)      │    │ Engine              │
└─────────────────┘    └────────────────────┘    └─────────────────────┘
                                │                           │
                                ▼                           ▼
                       ┌────────────────────┐    ┌─────────────────────┐
                       │ Configuration      │    │ DLL Hot-Loading     │
                       │ ~/.config/cll/     │    │ System              │
                       │ ~/.config/shared/  │    │                     │
                       └────────────────────┘    └─────────────────────┘
                                │
                                ▼
                       ┌────────────────────┐
                       │ Claude AI          │
                       │ Integration        │
                       │ (via Python CLI)   │
                       └────────────────────┘
```

## Core Interface

CLL provides a zsh-like interface with special prefixes for enhanced functionality:

### Mode Switching Commands
- **`sh` / `shell`** - Switch to Shell mode (λ prompt)
- **`js` / `javascript`** - Switch to JavaScript mode (φ prompt)
- **`ask`** - Switch to Ask mode (θ prompt)

### Single-Command Prefixes (stay in current mode)
- **`&<code>`** - Execute JavaScript once (`&Math.sqrt(16)`)
- **`?<question>`** - Ask Claude once (`?What is recursion?`)
- **`$<command>`** - Execute shell command once (`$pwd`)

### Comments  
- **`# anything`** - Ignored as comments (like bash/zsh)

### Built-in Commands
- **`help`** - Show available commands and usage
- **`ask <question>`** - Ask Claude AI a question (e.g., `ask what is capital of canada`)
- **`config`** - Show configuration directory location
- **`reload`** - Reload configuration from files
- **`clear`** - Clear the console screen
- **`quit` / `exit`** - Exit CLL

## Dependencies

### Build Requirements
- C++20 compatible compiler (GCC 10+, Clang 10+)
- CMake 3.15+
- Git (for submodule management)

### Auto-Managed Dependencies
CLL automatically fetches and builds required dependencies:
- **V8 JavaScript Engine** (optional, builds from source or uses system package)
- **rang**: Colored terminal output (auto-fetched)
- **nlohmann/json**: JSON configuration (auto-fetched)
- **readline**: Enhanced input editing (system package)

### Runtime Dependencies
- Standard POSIX shell environment
- Optional: Python 3.x with claude-cli (for advanced Claude AI integration beyond common knowledge)

## Configuration

CLL uses a sophisticated shared configuration system:

### Directory Structure
```
~/.config/
├── cll/                    # CLL-specific configuration
│   ├── config.json        # Main CLL settings
│   └── aliases            # CLL-specific aliases
└── shared/                 # Shared configuration across applications
    ├── prompts.json       # Shared prompt configuration
    └── aliases            # Shared command aliases
```

### config.json (CLL Configuration)
```json
{
  "default_mode": "shell",
  "prompt_format": "λ ",
  "claude_prompt_format": "? ",
  "claude_prompt_color": "cyan",
  "show_execution_time": true,
  "history_size": 1000,
  "enable_colors": true,
  "v8_integration": {
    "enabled": true,
    "compile_timeout_ms": 5000
  },
  "claude_integration": {
    "enabled": true,
    "timeout_seconds": 30,
    "python_cli_path": "claude"
  }
}
```

### Shared Configuration (prompts.json)
```json
{
  "shell_prompt": {
    "format": "λ ",
    "color": "cyan"
  },
  "javascript_prompt": {
    "format": "[js] λ ",
    "color": "yellow"
  },
  "claude_prompt": {
    "format": "? ",
    "color": "cyan"
  }
}
```

## JavaScript Integration

CLL integrates the V8 JavaScript engine for real JavaScript execution:

### Single-line JavaScript
```bash
# Execute JavaScript with & prefix
❯ &Math.PI * 2
6.283185307179586

❯ &new Date().toISOString()
2024-01-15T10:30:45.123Z

❯ &[1,2,3,4,5].reduce((a,b) => a+b, 0)
15
```

### JavaScript Mode
```bash
# Switch to persistent JavaScript context
❯ js
[js] ❯ const name = "CLL";
[js] ❯ console.log(`Hello from ${name}!`);
Hello from CLL!
[js] ❯ shell
❯ 
```

### V8 Features
- Full ES2020+ JavaScript support
- Built-in console object
- File I/O capabilities
- DLL loading functions
- Error handling with stack traces

## Claude AI Integration

CLL seamlessly integrates with Claude AI for intelligent assistance:

### Single Queries
```bash
❯ ?What is the time complexity of quicksort?
Claude: Quicksort has an average time complexity of O(n log n)...

❯ ?Explain this code: &console.log('Hello')
Claude: This JavaScript code uses console.log() to output...
```

### Multi-line Queries
```bash
❯ ?
Multi-line Claude query mode (Ctrl-D to send)
? Please explain the differences between:
? 1. Promises
? 2. Async/await  
? 3. Callbacks
? [Ctrl-D]
Claude: [Comprehensive explanation...]
```

## Shell Integration

CLL provides comprehensive zsh-like shell capabilities:

### File Operations
```bash
❯ ls -la *.{cpp,h}          # Brace expansion
❯ find . -name "*.cpp" | head -5
❯ cat file.txt | grep "pattern" | wc -l
❯ echo "Hello" > test.txt && cat test.txt
```

### Advanced Features
```bash
❯ echo {1..10}              # Brace expansion
❯ ls **/*.cpp               # Recursive globbing
❯ echo "Today is $(date +%A)"  # Command substitution
❯ env | grep HOME           # Environment variables
❯ history | tail -5         # Command history
```

## Building

### Quick Build (Recommended)
```bash
# Build with all dependencies and run demo
./build.sh && ./demo.sh

# Configure and build manually
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### Build Options
```bash
./build.sh            # Default: optimized release build
./build.sh clean      # Clean all artifacts
./build.sh debug      # Debug build with symbols  
./build.sh release    # Optimized release build
./build.sh test       # Build and run tests
./build.sh deps       # Initialize git submodules
./build.sh --help     # Show all options
```

### CMake Build Options
```bash
# V8 JavaScript Engine Options
cmake .. -DBUILD_V8_FROM_SOURCE=ON      # Build V8 from source
cmake .. -DFORCE_V8_BUILD=ON            # Force V8 build even if system package exists
cmake .. -DFETCH_ONLY_ESSENTIAL=OFF     # Fetch all dependencies including Boost

# Dependency Management
cmake .. -DAUTO_FETCH_DEPENDENCIES=ON   # Auto-fetch missing dependencies (default)
cmake .. -DBUILD_TESTS=ON               # Enable test suite (default)
```

## Project Structure

```
CppV8ClaudeIntegration/
├── Bin/                        # Compiled binaries
│   └── cll                    # Main CLL executable
├── Source/                     # Main application source
│   └── Main.cpp               # Application entry point
├── Library/                    # Core library components
│   └── ClaudeConsole/         # ClaudeConsole library
│       ├── Include/           # Library headers
│       │   ├── ClaudeConsole.h
│       │   ├── DllLoader.h
│       │   └── V8Compat.h
│       └── Source/            # Library implementation
│           ├── ClaudeConsole.cpp
│           └── DllLoader.cpp
├── External/                   # Auto-managed dependencies
│   ├── rang/                  # Colored output (auto-fetched)
│   ├── json/                  # JSON library (auto-fetched)
│   └── v8/                    # V8 engine (optional, manual setup)
├── Tests/                      # Test suite
├── Tools/                      # Development tools
├── build.sh                   # Build script with dependency management
├── demo.sh                    # Comprehensive feature demonstration
├── CMakeLists.txt             # Main build configuration
└── README.md                  # This file
```

## Performance

CLL is optimized for high performance and low latency:

- **Startup Time**: ~50-100ms (cold start)
- **Built-in Commands**: <1ms execution time
- **Shell Commands**: ~1-2ms overhead (near-native performance)
- **JavaScript Execution**: Real V8 performance (when available)
- **Claude AI Queries**: 1-5 seconds (network dependent)
- **Memory Usage**: ~2-5MB base footprint
- **Configuration Load**: <10ms

## Advanced Features

### DLL Hot-Loading
```bash
[js] ❯ loadDll('/path/to/library.so')    # Load native library
[js] ❯ reloadDll('/path/to/library.so')  # Hot-reload library
[js] ❯ listDlls()                        # Show loaded libraries
[js] ❯ unloadDll('/path/to/library.so')  # Unload library
```

### Configuration Wizard
```bash
❯ configure
CLL Configuration Wizard
========================
Setting up shared configuration in ~/.config/shared/
Setting up CLL configuration in ~/.config/cll/
Configure Claude AI integration? [y/N]: y
Enter Claude CLI path [claude]: claude
Configuration complete!
```

### Mode Switching
```bash
❯ js                    # Enter JavaScript mode
[js] ❯ shell           # Return to shell mode
❯ ?Mode question       # Ask Claude about current mode
```

## Documentation

### 📚 Complete Documentation Suite

- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Development guide and standards
- **[CHANGELOG.md](CHANGELOG.md)** - Project evolution history  
- **[Library Documentation](Library/ClaudeConsole/README.md)** - Complete API reference
- **[Build System](build.sh)** - Intelligent dependency management

### 🔧 Development

```bash
# Development workflow
git clone <repository>
cd CppV8ClaudeIntegration
./build.sh deps          # Initialize dependencies  
./build.sh debug         # Build with debug symbols
./demo.sh --fast         # Quick feature demo
./Tests/run_tests.sh     # Run test suite (if available)
```

## Contributing

We welcome contributions! Key areas:

1. **V8 Integration**: Enhance JavaScript capabilities
2. **Claude AI**: Improve AI integration and context handling
3. **Shell Features**: Add more zsh-like functionality
4. **Performance**: Optimize command execution
5. **Testing**: Expand test coverage
6. **Documentation**: Improve guides and examples

**Development Setup:**
```bash
git clone https://github.com/user/CppV8ClaudeIntegration.git
cd CppV8ClaudeIntegration
./build.sh deps  # Initialize dependencies
./build.sh debug # Build with debug info
./demo.sh        # Test all features
```

## License

This project is open source under the MIT License. See LICENSE file for details.

## Related Projects

- **claude-cli**: Python CLI for Claude AI integration
- **V8**: Google's high-performance JavaScript engine
- **rang**: Header-only colored terminal output library
- **nlohmann/json**: Modern C++ JSON library

---

**CLL (Claude Command Line)** - Where shell, JavaScript, and AI converge. 🚀
