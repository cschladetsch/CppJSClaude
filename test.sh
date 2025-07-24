#!/bin/bash

# CLL (Claude Command Line) - Comprehensive Test Script
# This script runs the complete GTest test suite with detailed reporting

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
BUILD_DIR="build"
TEST_EXECUTABLE="$BUILD_DIR/Tests/cll_tests"
PROJECT_NAME="CLL (Claude Command Line)"

# Function to print colored headers
print_header() {
    echo ""
    echo -e "${CYAN}╔════════════════════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║${NC} ${WHITE}$1${NC}"
    echo -e "${CYAN}╚════════════════════════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

# Function to print status messages
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

# Function to show colored banner
show_banner() {
    echo -e "${PURPLE}╔══════════════════════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${PURPLE}║${NC}                                                                              ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}████████╗███████╗███████╗████████╗    ███████╗██╗   ██╗██╗████████╗███████╗${NC}  ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝    ██╔════╝██║   ██║██║╚══██╔══╝██╔════╝${NC}  ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}   ██║   █████╗  ███████╗   ██║       ███████╗██║   ██║██║   ██║   █████╗${NC}    ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}   ██║   ██╔══╝  ╚════██║   ██║       ╚════██║██║   ██║██║   ██║   ██╔══╝${NC}    ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}   ██║   ███████╗███████║   ██║       ███████║╚██████╔╝██║   ██║   ███████╗${NC}  ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}  ${WHITE}   ╚═╝   ╚══════╝╚══════╝   ╚═╝       ╚══════╝ ╚═════╝ ╚═╝   ╚═╝   ╚══════╝${NC}  ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}                                                                              ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}                    ${YELLOW}Comprehensive Testing Framework${NC}                       ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}                         ${GREEN}Google Test Integration${NC}                          ${PURPLE}║${NC}"
    echo -e "${PURPLE}║${NC}                                                                              ${PURPLE}║${NC}"
    echo -e "${PURPLE}╚══════════════════════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

# Function to check prerequisites
check_prerequisites() {
    print_status "Checking prerequisites..."
    
    # Check if cmake is available
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is required but not installed"
        exit 1
    fi
    
    # Check if build directory exists
    if [ ! -d "$BUILD_DIR" ]; then
        print_warning "Build directory not found. Building project first..."
        return 1
    fi
    
    # Check if test executable exists
    if [ ! -f "$TEST_EXECUTABLE" ]; then
        print_warning "Test executable not found. Building tests first..."
        return 1
    fi
    
    print_success "Prerequisites check completed"
    return 0
}

# Function to build project with tests
build_with_tests() {
    print_header "BUILDING PROJECT WITH TESTS"
    
    print_status "Configuring CMake with testing enabled..."
    cmake -B "$BUILD_DIR" -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release
    
    print_status "Building project and tests..."
    cmake --build "$BUILD_DIR" --parallel $(nproc)
    
    if [ -f "$TEST_EXECUTABLE" ]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed - test executable not found"
        exit 1
    fi
}

# Function to run all tests
run_tests() {
    print_header "RUNNING COMPREHENSIVE TEST SUITE"
    
    print_status "Executing GTest suite..."
    echo ""
    
    # Run tests with colored output and detailed reporting
    if "$TEST_EXECUTABLE" --gtest_color=yes --gtest_output=xml:test_results.xml; then
        test_result=0
        print_success "All tests passed!"
    else
        test_result=$?
        print_error "Some tests failed (exit code: $test_result)"
    fi
    
    echo ""
    return $test_result
}

# Function to run tests with specific filters
run_filtered_tests() {
    local filter="$1"
    print_header "RUNNING FILTERED TESTS: $filter"
    
    print_status "Executing filtered test suite..."
    echo ""
    
    if "$TEST_EXECUTABLE" --gtest_color=yes --gtest_filter="$filter"; then
        print_success "Filtered tests passed!"
        return 0
    else
        print_error "Some filtered tests failed"
        return 1
    fi
}

# Function to run tests with verbose output
run_verbose_tests() {
    print_header "RUNNING TESTS WITH VERBOSE OUTPUT"
    
    print_status "Executing tests with detailed output..."
    echo ""
    
    if "$TEST_EXECUTABLE" --gtest_color=yes --gtest_print_time=1 --gtest_brief=0; then
        print_success "Verbose tests completed!"
        return 0
    else
        print_error "Some verbose tests failed"
        return 1
    fi
}

# Function to run CTest integration
run_ctest() {
    print_header "RUNNING CTEST INTEGRATION"
    
    print_status "Executing CTest runner..."
    cd "$BUILD_DIR"
    
    if ctest --output-on-failure --parallel $(nproc); then
        cd ..
        print_success "CTest execution completed!"
        return 0
    else
        cd ..
        print_error "CTest execution failed"
        return 1
    fi
}

# Function to generate test report
generate_report() {
    print_header "GENERATING TEST REPORT"
    
    if [ -f "test_results.xml" ]; then
        print_status "XML test results generated: test_results.xml"
        
        # Parse basic statistics from XML
        if command -v xmllint &> /dev/null; then
            local total_tests=$(xmllint --xpath "count(//testcase)" test_results.xml 2>/dev/null || echo "N/A")
            local failed_tests=$(xmllint --xpath "count(//failure)" test_results.xml 2>/dev/null || echo "0")
            local error_tests=$(xmllint --xpath "count(//error)" test_results.xml 2>/dev/null || echo "0")
            
            echo ""
            echo -e "${WHITE}Test Statistics:${NC}"
            echo -e "${BLUE}  Total Tests: ${NC}$total_tests"
            echo -e "${GREEN}  Passed Tests: ${NC}$((total_tests - failed_tests - error_tests))"
            echo -e "${RED}  Failed Tests: ${NC}$failed_tests"
            echo -e "${YELLOW}  Error Tests: ${NC}$error_tests"
        fi
    fi
    
    # Show test executable info
    if [ -f "$TEST_EXECUTABLE" ]; then
        local test_size=$(ls -lh "$TEST_EXECUTABLE" | awk '{print $5}')
        local test_date=$(date -r "$TEST_EXECUTABLE" "+%Y-%m-%d %H:%M:%S")
        echo ""
        echo -e "${WHITE}Test Executable Info:${NC}"
        echo -e "${BLUE}  Size: ${NC}$test_size"
        echo -e "${BLUE}  Built: ${NC}$test_date"
        echo -e "${BLUE}  Path: ${NC}$TEST_EXECUTABLE"
    fi
}

# Function to show help
show_help() {
    echo "CLL (Claude Command Line) Test Suite"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help          Show this help message"
    echo "  -v, --verbose       Run tests with verbose output"
    echo "  -f, --filter FILTER Run only tests matching the filter"
    echo "  -c, --ctest         Run tests using CTest runner"
    echo "  -b, --build         Force rebuild before testing"
    echo "  -r, --report        Generate detailed test report"
    echo "  --list              List all available tests"
    echo "  --quick             Run quick smoke tests only"
    echo ""
    echo "Examples:"
    echo "  $0                          # Run all tests"
    echo "  $0 --verbose                # Run with verbose output"  
    echo "  $0 --filter '*Console*'     # Run only console-related tests"
    echo "  $0 --ctest                  # Use CTest runner"
    echo "  $0 --build                  # Force rebuild and test"
    echo ""
    echo "Test Categories:"
    echo "  ClaudeConsoleTest.*         # Core console functionality"
    echo "  CommandExecutionTest.*      # Command execution tests"
    echo "  MultiLineModeTest.*         # Multi-line input tests"
    echo "  ConfigurationTest.*         # Configuration system tests"
    echo "  PromptManagementTest.*      # Prompt formatting tests"
    echo "  AliasSystemTest.*           # Alias functionality tests"
    echo "  UtilitiesTest.*             # Utility function tests"
}

# Function to list available tests
list_tests() {
    print_header "AVAILABLE TESTS"
    
    if [ -f "$TEST_EXECUTABLE" ]; then
        print_status "Discovering test cases..."
        echo ""
        "$TEST_EXECUTABLE" --gtest_list_tests
    else
        print_error "Test executable not found. Build tests first."
        return 1
    fi
}

# Function to run quick smoke tests
run_quick_tests() {
    print_header "RUNNING QUICK SMOKE TESTS"
    
    # Run a subset of critical tests
    local quick_filter="ClaudeConsoleTest.InitializationTest:CommandExecutionTest.JavaScriptPrefixExecution:MultiLineModeTest.MultiLineModeInitialization"
    
    print_status "Running critical functionality tests..."
    run_filtered_tests "$quick_filter"
}

# Function to show system info
show_system_info() {
    print_header "SYSTEM INFORMATION"
    
    echo -e "${BLUE}Project:${NC} $PROJECT_NAME"
    echo -e "${BLUE}Test Framework:${NC} Google Test (GTest)"
    echo -e "${BLUE}Build System:${NC} CMake with CTest integration"
    echo -e "${BLUE}Language:${NC} C++20"
    
    if command -v cmake &> /dev/null; then
        echo -e "${BLUE}CMake Version:${NC} $(cmake --version | head -n1 | cut -d' ' -f3)"
    fi
    
    if command -v g++ &> /dev/null; then
        echo -e "${BLUE}Compiler:${NC} $(g++ --version | head -n1)"
    fi
    
    echo -e "${BLUE}Platform:${NC} $(uname -s) $(uname -m)"
    echo -e "${BLUE}CPU Cores:${NC} $(nproc)"
    
    if [ -f "$TEST_EXECUTABLE" ]; then
        echo -e "${BLUE}Test Executable:${NC} Present ($(ls -lh "$TEST_EXECUTABLE" | awk '{print $5}'))"
    else
        echo -e "${BLUE}Test Executable:${NC} Not built"
    fi
}

# Main execution logic
main() {
    local verbose=false
    local filter=""
    local use_ctest=false
    local force_build=false
    local generate_report_flag=false
    local show_help_flag=false
    local list_tests_flag=false
    local quick_tests=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help_flag=true
                shift
                ;;
            -v|--verbose)
                verbose=true
                shift
                ;;
            -f|--filter)
                filter="$2"
                shift 2
                ;;
            -c|--ctest)
                use_ctest=true
                shift
                ;;
            -b|--build)
                force_build=true
                shift
                ;;
            -r|--report)
                generate_report_flag=true
                shift
                ;;
            --list)
                list_tests_flag=true
                shift
                ;;
            --quick)
                quick_tests=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
    
    # Show banner
    show_banner
    
    # Handle help
    if [ "$show_help_flag" = true ]; then
        show_help
        exit 0
    fi
    
    # Show system info
    show_system_info
    
    # Handle list tests
    if [ "$list_tests_flag" = true ]; then
        if ! check_prerequisites && [ "$force_build" = false ]; then
            build_with_tests
        fi
        list_tests
        exit 0
    fi
    
    # Check prerequisites or force build
    if ! check_prerequisites || [ "$force_build" = true ]; then
        build_with_tests
    fi
    
    # Run tests based on options
    local test_result=0
    
    if [ "$quick_tests" = true ]; then
        run_quick_tests
        test_result=$?
    elif [ "$use_ctest" = true ]; then
        run_ctest
        test_result=$?
    elif [ "$verbose" = true ]; then
        run_verbose_tests
        test_result=$?
    elif [ -n "$filter" ]; then
        run_filtered_tests "$filter"
        test_result=$?
    else
        run_tests
        test_result=$?
    fi
    
    # Generate report if requested
    if [ "$generate_report_flag" = true ]; then
        generate_report
    fi
    
    # Final status
    echo ""
    if [ $test_result -eq 0 ]; then
        print_success "🎉 All tests completed successfully! 🎉"
    else
        print_error "❌ Test execution failed with exit code: $test_result"
    fi
    
    echo ""
    echo -e "${CYAN}For more options, run: $0 --help${NC}"
    
    exit $test_result
}

# Run main function with all arguments
main "$@"