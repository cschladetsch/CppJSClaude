#!/bin/bash

# CppV8-Claude Integration Build Script
# Usage: ./build.sh [clean|debug|release|test|help]

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
BIN_DIR="Bin"
PROJECT_NAME="CLL (Claude Command Line)"

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show help
show_help() {
    echo "CppV8-Claude Integration Build Script"
    echo ""
    echo "Usage: $0 [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  clean     - Clean build directory and binaries"
    echo "  debug     - Build in debug mode with symbols"
    echo "  release   - Build in release mode (optimized)"
    echo "  test      - Build and run tests/demos"
    echo "  deps      - Initialize and update submodules"
    echo "  help      - Show this help message"
    echo ""
    echo "If no command is specified, defaults to 'release' build"
    echo ""
    echo "Examples:"
    echo "  $0                # Build in release mode"
    echo "  $0 debug          # Build in debug mode"
    echo "  $0 clean          # Clean all build artifacts"
    echo "  $0 test           # Build and run demos"
}

# Function to check prerequisites
check_prerequisites() {
    print_status "Checking prerequisites..."
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is required but not installed. Please install CMake 3.15 or higher."
        exit 1
    fi
    
    CMAKE_VERSION=$(cmake --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+' | head -1)
    print_status "Found CMake version: $CMAKE_VERSION"
    
    # Check for C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        print_error "No C++ compiler found. Please install g++ or clang++."
        exit 1
    fi
    
    if command -v g++ &> /dev/null; then
        GCC_VERSION=$(g++ --version | head -n1)
        print_status "Found compiler: $GCC_VERSION"
    elif command -v clang++ &> /dev/null; then
        CLANG_VERSION=$(clang++ --version | head -n1)
        print_status "Found compiler: $CLANG_VERSION"
    fi
    
    # Check for Git (for submodules)
    if ! command -v git &> /dev/null; then
        print_warning "Git not found. Submodule operations will not be available."
    fi
    
    # Check for Python (for Claude integration)
    if command -v python3 &> /dev/null; then
        PYTHON_VERSION=$(python3 --version)
        print_status "Found Python: $PYTHON_VERSION"
    else
        print_warning "Python3 not found. Claude AI integration may not work."
    fi
    
    print_success "Prerequisites check completed"
}

# Function to initialize submodules
init_submodules() {
    print_status "Initializing submodules..."
    
    if [ ! -d ".git" ]; then
        print_error "Not a git repository. Cannot initialize submodules."
        return 1
    fi
    
    # Add rang submodule if not exists
    if [ ! -d "external/rang" ]; then
        print_status "Adding rang submodule for colored terminal output..."
        git submodule add https://github.com/agauniyal/rang.git external/rang
    fi
    
    # Add minimal boost headers if not exists
    if [ ! -d "external/boost" ]; then
        print_status "Adding boost submodule for additional utilities..."
        git submodule add --depth 1 https://github.com/boostorg/boost.git external/boost
        cd external/boost
        git submodule update --init --recursive --depth 1 tools/build
        git submodule update --init --recursive --depth 1 libs/system
        git submodule update --init --recursive --depth 1 libs/filesystem
        git submodule update --init --recursive --depth 1 libs/program_options
        cd ../..
    fi
    
    # Update all submodules
    print_status "Updating submodules..."
    git submodule update --init --recursive
    
    print_success "Submodules initialized and updated"
}

# Function to clean build directory
clean_build() {
    print_status "Cleaning build artifacts..."
    
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_status "Removed build directory"
    fi
    
    if [ -d "$BIN_DIR" ]; then
        # Keep README.md but remove binaries
        find "$BIN_DIR" -type f ! -name "README.md" -delete
        print_status "Cleaned bin directory (kept README.md)"
    fi
    
    # Remove any generated executables in root
    rm -f cll demo enhanced_demo
    
    print_success "Clean completed"
}

# Function to configure CMake
configure_cmake() {
    local build_type=$1
    
    print_status "Configuring CMake build (${build_type})..."
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake
    cmake .. \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_VERBOSE_MAKEFILE=OFF
    
    cd ..
    print_success "CMake configuration completed"
}

# Function to build project
build_project() {
    local build_type=$1
    local jobs=${2:-$(nproc)}
    
    print_status "Building project in $build_type mode using $jobs parallel jobs..."
    
    cd "$BUILD_DIR"
    
    # Build with timing
    start_time=$(date +%s)
    make -j"$jobs" || {
        print_error "Build failed"
        cd ..
        exit 1
    }
    end_time=$(date +%s)
    
    cd ..
    
    build_time=$((end_time - start_time))
    print_success "Build completed in ${build_time} seconds"
}

# Function to run tests
run_tests() {
    print_status "Running tests and demos..."
    
    if [ ! -f "$BIN_DIR/cll" ]; then
        print_error "Main executable not found. Build the project first."
        return 1
    fi
    
    # Build and run demo if not exists
    if [ ! -f "$BUILD_DIR/demo" ]; then
        print_status "Building demo..."
        cd "$BUILD_DIR"
        g++ -std=c++20 -I../Include ../demo.cpp ../Source/ClaudeConsole.cpp -o demo
        cd ..
    fi
    
    print_status "Running basic demo..."
    "$BUILD_DIR/demo"
    
    # Build and run enhanced demo if not exists
    if [ ! -f "$BUILD_DIR/enhanced_demo" ]; then
        print_status "Building enhanced demo..."
        cd "$BUILD_DIR"
        g++ -std=c++20 -I../Include ../enhanced_demo.cpp ../Source/ClaudeConsole.cpp -o enhanced_demo
        cd ..
    fi
    
    print_status "Running enhanced demo..."
    "$BUILD_DIR/enhanced_demo"
    
    print_success "All tests completed successfully"
}

# Function to show build info
show_build_info() {
    print_status "Build Information:"
    echo "  Project: $PROJECT_NAME"
    echo "  Build Directory: $BUILD_DIR"
    echo "  Binary Directory: $BIN_DIR"
    
    if [ -f "$BIN_DIR/cll" ]; then
        echo "  Main Executable: $(ls -lh $BIN_DIR/cll | awk '{print $5}')"
        echo "  Build Date: $(date -r $BIN_DIR/cll)"
    fi
    
    if [ -d ".git" ]; then
        echo "  Git Commit: $(git rev-parse --short HEAD)"
        echo "  Git Branch: $(git branch --show-current)"
    fi
}

# Main script logic
main() {
    local command=${1:-release}
    
    echo "=== $PROJECT_NAME Build Script ==="
    echo ""
    
    case "$command" in
        "help"|"-h"|"--help")
            show_help
            ;;
        "clean")
            clean_build
            ;;
        "debug")
            check_prerequisites
            clean_build
            configure_cmake "Debug"
            build_project "Debug"
            show_build_info
            ;;
        "release")
            check_prerequisites
            clean_build
            configure_cmake "Release"
            build_project "Release"
            show_build_info
            ;;
        "test")
            check_prerequisites
            if [ ! -f "$BIN_DIR/cll" ]; then
                print_status "Executable not found, building first..."
                clean_build
                configure_cmake "Release"
                build_project "Release"
            fi
            run_tests
            ;;
        "deps")
            init_submodules
            ;;
        *)
            print_error "Unknown command: $command"
            echo ""
            show_help
            exit 1
            ;;
    esac
    
    print_success "Script completed successfully!"
}

# Run main function with all arguments
main "$@"