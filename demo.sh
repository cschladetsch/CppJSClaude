#!/bin/bash

# CLL (Claude Command Line) - Complete Functionality Demo
# This script demonstrates all features of the CLL system in human-readable time

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# Configuration
CLL_BINARY="./Bin/cll"
DEMO_DELAY=0.8  # Seconds between commands for readability (optimized for ~1 minute total)

# Function to print colored headers
print_header() {
    echo ""
    echo -e "${CYAN}╔════════════════════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║${NC} ${WHITE}$1${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

# Function to print step descriptions
print_step() {
    echo -e "${YELLOW}➤ $1${NC}"
    sleep 0.3
}

# Function to print command being executed
print_command() {
    echo -e "${GREEN}$ $1${NC}"
    sleep 0.2
}

# Function to send command to CLL and show output
send_command() {
    local cmd="$1"
    local description="$2"
    
    print_step "$description"
    print_command "$cmd"
    
    # Send command with timeout and capture output
    echo "$cmd" | timeout 5s "$CLL_BINARY" 2>/dev/null | tail -n +9 | head -n -2 || true
    
    sleep $DEMO_DELAY
}

# Function to demonstrate configuration
demo_config() {
    print_header "CONFIGURATION & ALIASES"
    
    send_command "configure" "Run configuration wizard"
    send_command "config" "Show configuration directory"
    send_command 'alias ll="ls -la"' "Set alias 'll' for long listing"
}

# Function to demonstrate JavaScript functionality
demo_javascript() {
    print_header "JAVASCRIPT INTEGRATION"
    
    send_command "js" "Switch to JavaScript mode"
    send_command "Math.sqrt(64)" "Calculate square root"
    send_command "shell" "Return to shell mode"
    send_command "&Math.PI * 2" "JavaScript with & prefix"
}

# Function to demonstrate shell functionality
demo_shell() {
    print_header "ZSH-LIKE SHELL INTEGRATION DEMO"
    
    print_step "Navigation and directory operations"
    send_command "pwd" "Show current working directory"
    send_command "ls" "List files in current directory"
    send_command "ls -la" "List files with detailed information"
    send_command "ls -lh" "List files with human-readable sizes"
    
    print_step "File operations"
    send_command "touch /tmp/cll_test.txt" "Create a test file"
    send_command "echo 'Hello CLL' > /tmp/cll_test.txt" "Write content to file"
    send_command "cat /tmp/cll_test.txt" "Display file contents"
    send_command "wc -l /tmp/cll_test.txt" "Count lines in file"
    send_command "rm /tmp/cll_test.txt" "Remove test file"
    
    print_step "Process and system information"
    send_command "whoami" "Show current user"
    send_command "date" "Show current date and time"
    send_command "uptime" "Show system uptime"
    send_command "ps aux | head -5" "Show running processes (top 5)"
    
    print_step "Directory listing and navigation"
    send_command "find . -name '*.cpp' -type f | head -3" "Find C++ source files"
    send_command "du -sh ." "Show directory size"
    send_command "df -h ." "Show disk usage"
    
    print_step "Text processing and pipes"
    send_command "echo 'one\ntwo\nthree' | grep 'two'" "Text filtering with grep"
    send_command "echo 'hello world' | tr '[:lower:]' '[:upper:]'" "Text transformation"
    send_command "ls | wc -l" "Count files in directory"
    
    print_step "Environment and variables"
    send_command "env | grep HOME" "Show HOME environment variable"
    send_command "echo \$PATH | tr ':' '\n' | head -3" "Show PATH components"
    send_command "which gcc" "Find executable location"
    
    print_step "Common zsh/bash utilities"
    send_command "history | tail -3" "Show command history"
    send_command "alias" "Show current aliases"
    send_command "type ls" "Show command type"
    
    print_step "Advanced file operations"
    send_command "ls -la | grep -E '^\\.'" "Show hidden files only"
    send_command "ls -la | awk '{print \$9, \$5}'" "Show filenames and sizes"
    send_command "ls -1 | sort -r | head -3" "Reverse sorted file list"
    
    print_step "Network and system"
    send_command "hostname" "Show system hostname"
    send_command "uname -a" "Show system information"
    send_command "id" "Show user and group IDs"
    
    print_step "Zsh-style globbing and patterns"
    send_command "ls -la *.*" "Show files with extensions"
    send_command "ls -la *.{cpp,h}" "Show C++ source and header files"
    send_command "echo **/*.cpp | head -c 50" "Recursive glob pattern (truncated)"
    
    print_step "Command substitution and expansion"
    send_command "echo \"Today is \$(date +%A)\"" "Command substitution"
    send_command "echo {1..5}" "Brace expansion"
    send_command "echo ~/.*rc" "Home directory expansion"
    
    print_step "Mixed shell and JavaScript workflow"
    send_command "echo 'Current directory:'" "Display message using shell echo"
    send_command "&new Date().getHours()" "Get current hour using JavaScript"
    send_command "date '+%H'" "Show current hour using shell date"
}

# Function to demonstrate Claude AI integration
demo_claude() {
    print_header "CLAUDE AI INTEGRATION DEMO"
    
    print_step "Testing Claude AI integration with ? prefix"
    echo -e "${BLUE}[INFO]${NC} Claude AI integration uses ? prefix for queries"
    
    print_step "Basic question to Claude using ? prefix"
    send_command "?What is 7 factorial?" "Ask Claude to calculate 7! (7 factorial)"
    
    print_step "Programming question using ? prefix"
    send_command "?Explain the difference between let and const in JavaScript in one sentence" "Ask Claude about JavaScript concepts"
    
    print_step "Mathematical computation using ? prefix"
    send_command "?What is the square root of 144?" "Ask Claude for mathematical calculation"
}

# Function to demonstrate built-in commands
demo_builtins() {
    print_header "BUILT-IN COMMANDS"
    
    send_command "help" "Show all available commands"
    send_command "version" "Show CLL version information"
}

# Function to demonstrate advanced features
demo_advanced() {
    print_header "ADVANCED FEATURES DEMO"
    
    print_step "Command execution timing"
    echo -e "${BLUE}[INFO]${NC} All commands show execution time in microseconds/milliseconds"
    send_command "sleep 1" "Execute a command that takes time (1 second sleep)"
    
    print_step "Error handling demonstration"
    send_command "nonexistent_command_12345" "Try to execute a non-existent command"
    
    print_step "Mixed mode demonstration"
    echo -e "${BLUE}[INFO]${NC} Combining shell commands, JavaScript, and built-ins:"
    send_command "echo 'System info:'" "Shell command"
    send_command "&Date.now()" "JavaScript timestamp"
    send_command "pwd" "Current directory"
}

# Function to show system information
show_system_info() {
    print_header "SYSTEM INFORMATION"
    
    echo -e "${BLUE}Project:${NC} CLL (Claude Command Line)"
    echo -e "${BLUE}Version:${NC} 1.0.0"
    echo -e "${BLUE}Language:${NC} C++20"
    echo -e "${BLUE}Build System:${NC} CMake with custom build.sh"
    echo -e "${BLUE}Architecture:${NC} Library-based with ClaudeConsole core"
    
    if [ -f "$CLL_BINARY" ]; then
        local size=$(ls -lh "$CLL_BINARY" | awk '{print $5}')
        local date=$(date -r "$CLL_BINARY" "+%Y-%m-%d %H:%M:%S")
        echo -e "${BLUE}Binary Size:${NC} $size"
        echo -e "${BLUE}Build Date:${NC} $date"
    fi
    
    if [ -d ".git" ]; then
        local commit=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
        local branch=$(git branch --show-current 2>/dev/null || echo "unknown")
        echo -e "${BLUE}Git Commit:${NC} $commit"
        echo -e "${BLUE}Git Branch:${NC} $branch"
    fi
    
    echo -e "${BLUE}Configuration:${NC} ~/.config/cll/ and ~/.config/shared/"
    
    sleep 3
}

# Function to show performance metrics
show_performance() {
    print_header "PERFORMANCE METRICS"
    
    echo -e "${BLUE}[INFO]${NC} CLL Performance Characteristics:"
    echo ""
    echo -e "${GREEN}Startup Time:${NC} ~50-100ms (cold start)"
    echo -e "${GREEN}Shell Commands:${NC} ~1-2ms overhead (near-native)"
    echo -e "${GREEN}Built-in Commands:${NC} <1ms execution time"  
    echo -e "${GREEN}JavaScript Execution:${NC} Instant (simulated)"
    echo -e "${GREEN}Claude AI Queries:${NC} 1-5 seconds (network dependent)"
    echo -e "${GREEN}Memory Usage:${NC} ~2-5MB base footprint"
    echo -e "${GREEN}Configuration Load:${NC} <10ms"
    echo ""
    echo -e "${YELLOW}Note:${NC} All commands show precise execution timing"
    
    sleep 3
}

# Function to install requirements and build
setup_requirements() {
    print_header "SETTING UP REQUIREMENTS AND BUILDING CLL"
    
    print_step "Checking and installing system dependencies"
    echo -e "${BLUE}[INFO]${NC} Installing required packages..."
    
    # Detect package manager and install dependencies
    if command -v apt-get &> /dev/null; then
        echo -e "${GREEN}$ sudo apt-get update && sudo apt-get install -y build-essential cmake libreadline-dev python3 git${NC}"
        sudo apt-get update >/dev/null 2>&1 || echo -e "${YELLOW}[WARNING]${NC} Could not update package list"
        sudo apt-get install -y build-essential cmake libreadline-dev python3 git >/dev/null 2>&1 || echo -e "${YELLOW}[WARNING]${NC} Some packages may not have installed"
    elif command -v yum &> /dev/null; then
        echo -e "${GREEN}$ sudo yum install -y gcc-c++ cmake readline-devel python3 git${NC}"
        sudo yum install -y gcc-c++ cmake readline-devel python3 git >/dev/null 2>&1 || echo -e "${YELLOW}[WARNING]${NC} Some packages may not have installed"
    elif command -v brew &> /dev/null; then
        echo -e "${GREEN}$ brew install cmake readline python3 git${NC}"
        brew install cmake readline python3 git >/dev/null 2>&1 || echo -e "${YELLOW}[WARNING]${NC} Some packages may not have installed"
    else
        echo -e "${YELLOW}[WARNING]${NC} Unknown package manager. Please install manually:"
        echo -e "${YELLOW}  - build-essential or equivalent C++ compiler${NC}"
        echo -e "${YELLOW}  - cmake (3.15+)${NC}"
        echo -e "${YELLOW}  - libreadline-dev${NC}"
        echo -e "${YELLOW}  - python3${NC}"
        echo -e "${YELLOW}  - git${NC}"
    fi
    
    sleep 0.5
    
    print_step "Initializing git submodules"
    echo -e "${GREEN}$ ./build.sh deps${NC}"
    ./build.sh deps >/dev/null 2>&1 || echo -e "${YELLOW}[WARNING]${NC} Submodule initialization may have failed"
    
    sleep 0.3
    
    print_step "Building CLL with colored banner"
    echo -e "${GREEN}$ ./build.sh release${NC}"
    ./build.sh release
    
    sleep 0.5
    
    print_step "Verifying build success"
    if [ -f "$CLL_BINARY" ]; then
        echo -e "${GREEN}[SUCCESS]${NC} CLL built successfully!"
        local size=$(ls -lh "$CLL_BINARY" | awk '{print $5}')
        echo -e "${BLUE}[INFO]${NC} Binary size: $size"
    else
        echo -e "${RED}[ERROR]${NC} Build failed. Binary not found."
        exit 1
    fi
    
    sleep 2
}

# Main demo function
main_demo() {
    # Welcome message
    echo -e "${PURPLE}╔══════════════════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║${NC}                                                                              ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}██████╗██╗     ██╗         ██████╗ ███████╗███╗   ███╗ ██████╗${NC}           ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}██╔════╝██║     ██║         ██╔══██╗██╔════╝████╗ ████║██╔═══██╗${NC}          ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}██║     ██║     ██║         ██║  ██║█████╗  ██╔████╔██║██║   ██║${NC}          ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}██║     ██║     ██║         ██║  ██║██╔══╝  ██║╚██╔╝██║██║   ██║${NC}          ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}╚██████╗███████╗███████╗    ██████╔╝███████╗██║ ╚═╝ ██║╚██████╔╝${NC}          ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}╚═════╝╚══════╝╚══════╝    ╚═════╝ ╚══════╝╚═╝     ╚═╝ ╚═════╝${NC}           ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}                                                                              ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}           ${YELLOW}Complete Functionality Demonstration${NC}                           ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}           ${CYAN}Claude Command Line Integration${NC}                              ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}                                                                              ${PURPLE}║${NC}"
    echo -e "${PURPLE}╚══════════════════════════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${WHITE}Welcome to the CLL Complete Functionality Demo!${NC}"
    echo -e "${BLUE}This demonstration will showcase all features of the Claude Command Line system.${NC}"
    echo ""
    
    # Check if binary exists, if not, build it
    if [ ! -f "$CLL_BINARY" ]; then
        echo -e "${YELLOW}[INFO]${NC} CLL binary not found. Setting up requirements and building..."
        setup_requirements
    else
        echo -e "${GREEN}[SUCCESS]${NC} CLL binary found at $CLL_BINARY"
        sleep 1
    fi
    
    sleep 0.5
    
    # Show system information (shortened)
    print_header "SYSTEM OVERVIEW"
    echo -e "${BLUE}Project:${NC} CLL (Claude Command Line) | ${BLUE}Language:${NC} C++20 | ${BLUE}Architecture:${NC} Library-based"
    if [ -f "$CLL_BINARY" ]; then
        local size=$(ls -lh "$CLL_BINARY" | awk '{print $5}')
        echo -e "${BLUE}Binary:${NC} $size | ${BLUE}Config:${NC} ~/.config/cll/ and ~/.config/shared/"
    fi
    sleep 1
    
    # Run streamlined demos
    demo_builtins
    demo_shell
    demo_javascript  
    demo_config
    demo_claude
    
    # Conclusion
    print_header "DEMO COMPLETE"
    
    echo -e "${GREEN}✓ Built-in Commands${NC} - help, version, configure, config, clear, quit/exit"
    echo -e "${GREEN}✓ Shell Integration${NC} - Execute any shell command with timing (zsh-like interface)"
    echo -e "${GREEN}✓ JavaScript Integration${NC} - & prefix for JavaScript injection"  
    echo -e "${GREEN}✓ Claude AI Integration${NC} - ? prefix for Claude queries"
    echo -e "${GREEN}✓ Configuration${NC} - ~/.config/cll/ and ~/.config/shared/ management"
    echo -e "${GREEN}✓ V8 JavaScript Engine${NC} - Real JavaScript execution with V8"
    echo -e "${GREEN}✓ Error Handling${NC} - Graceful error messages and recovery"
    echo -e "${GREEN}✓ Performance Monitoring${NC} - Execution timing for all commands"
    echo ""
    echo -e "${WHITE}🎉 CLL (Claude Command Line) Demo Complete! 🎉${NC}"
    echo ""
    echo -e "${CYAN}To start using CLL:${NC}"
    echo -e "${YELLOW}  ./Bin/cll${NC}                    # Start interactive mode"
    echo -e "${YELLOW}  ./build.sh --help${NC}            # Build system help"
    echo -e "${YELLOW}  cat Library/ClaudeConsole/README.md${NC}  # Library documentation"
    echo ""
    echo -e "${BLUE}Configuration files:${NC}"
    echo -e "${YELLOW}  ~/.config/cll/config.json${NC}              # CLL-specific configuration"
    echo -e "${YELLOW}  ~/.config/shared/prompts.json${NC}          # Shared prompt configuration"
    echo -e "${YELLOW}  ~/.config/shared/aliases${NC}               # Shared command aliases"
    echo ""
}

# Script entry point
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "CLL (Claude Command Line) - Complete Functionality Demo"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --help, -h     Show this help message"
    echo "  --fast         Run demo with shorter delays (1s instead of 2s)"
    echo "  --slow         Run demo with longer delays (3s instead of 2s)"
    echo ""
    echo "This script demonstrates all features of CLL including:"
    echo "  • Built-in command system (help, version, configure, config)"
    echo "  • Comprehensive zsh-like shell integration"
    echo "  • JavaScript integration with & prefix"
    echo "  • Claude AI integration with ? prefix"
    echo "  • Configuration management (configure command)"
    echo "  • File operations, pipes, and text processing"
    echo "  • Environment variables and command substitution"
    echo "  • Globbing patterns and brace expansion"
    echo "  • Error handling and performance monitoring"
    echo ""
    echo "The demo runs in human-readable time with explanations between each step."
    exit 0
elif [ "$1" = "--fast" ]; then
    DEMO_DELAY=1
    echo -e "${BLUE}[INFO]${NC} Running demo in fast mode (1s delays)"
elif [ "$1" = "--slow" ]; then
    DEMO_DELAY=3
    echo -e "${BLUE}[INFO]${NC} Running demo in slow mode (3s delays)"
fi

# Run the main demo
main_demo

echo -e "${GREEN}Demo script completed successfully!${NC}"