#!/bin/bash

# V8 Setup Script for CLL (Claude Command Line)
# Downloads, configures, and builds V8 JavaScript engine from source

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m'

# Configuration
V8_DIR="External/v8"
DEPOT_TOOLS_DIR="External/depot_tools"
BUILD_CONFIG="out.gn/x64.release"

print_header() {
    echo ""
    echo -e "${CYAN}╔══════════════════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║${NC} ${WHITE}$1${NC}"
    echo -e "${CYAN}╚══════════════════════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

print_step() {
    echo -e "${YELLOW}➤ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -d "Library/ClaudeConsole" ]; then
    print_error "This script must be run from the CppV8ClaudeIntegration root directory"
    exit 1
fi

print_header "V8 JavaScript Engine Setup for CLL"

print_info "This script will download and build V8 from source."
print_info "This process requires:"
print_info "• ~2GB disk space"
print_info "• 30-60 minutes build time"
print_info "• Internet connection"
print_info "• Git and Python 3"
echo ""

# Check prerequisites
print_step "Checking prerequisites"

if ! command -v git &> /dev/null; then
    print_error "Git is required but not installed"
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    print_error "Python 3 is required but not installed"
    exit 1
fi

print_success "Prerequisites check passed"

# Create External directory
mkdir -p External

# Step 1: Download depot_tools
if [ ! -d "$DEPOT_TOOLS_DIR" ]; then
    print_step "Downloading depot_tools"
    git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git "$DEPOT_TOOLS_DIR"
    print_success "depot_tools downloaded"
else
    print_info "depot_tools already exists"
fi

# Add depot_tools to PATH
export PATH="$(pwd)/$DEPOT_TOOLS_DIR:$PATH"

# Step 2: Fetch V8
if [ ! -d "$V8_DIR" ]; then
    print_step "Fetching V8 source code (this may take 10-20 minutes)"
    cd External
    fetch v8
    cd ..
    print_success "V8 source fetched"
else
    print_info "V8 source already exists"
fi

# Step 3: Configure V8 build
print_step "Configuring V8 build"
cd "$V8_DIR"

# Generate build configuration
gn gen "$BUILD_CONFIG" --args='
    is_debug=false
    target_cpu="x64"
    v8_target_cpu="x64"
    use_custom_libcxx=false
    v8_use_external_startup_data=false
    v8_enable_i18n_support=false
    treat_warnings_as_errors=false
'

print_success "V8 build configured"

# Step 4: Build V8
print_step "Building V8 (this will take 30-60 minutes)"
print_info "Building v8_monolith target..."

# Build with progress indication
ninja -C "$BUILD_CONFIG" v8_monolith

if [ $? -eq 0 ]; then
    print_success "V8 built successfully!"
    
    # Verify build artifacts
    if [ -f "$BUILD_CONFIG/obj/libv8_monolith.a" ]; then
        print_success "V8 monolith library found"
        ls -lh "$BUILD_CONFIG/obj/libv8_monolith.a"
    fi
    
    cd ../..
    
    print_step "Testing V8 integration"
    
    # Rebuild CLL with V8
    ./build.sh clean
    ./build.sh release
    
    print_success "V8 setup complete!"
    print_info "CLL will now use real V8 JavaScript engine"
    print_info "Test with: ./Bin/cll"
    print_info "Try: &console.log('Hello V8!')"
    
else
    print_error "V8 build failed"
    print_info "Check build logs above for errors"
    print_info "Common issues:"
    print_info "• Insufficient disk space (need ~2GB)"
    print_info "• Network connectivity issues"
    print_info "• Missing system dependencies"
    exit 1
fi