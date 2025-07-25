#include "ClaudeConsole.h"
#include <iostream>

using namespace cll;

int main() {
    std::cout << "=== CppV8-Claude Integration Demo ===\n\n";
    
    // Create console instance
    ClaudeConsole console;
    
    if (!console.Initialize()) {
        std::cerr << "Failed to initialize console\n";
        return 1;
    }
    
    // Test basic commands
    std::cout << "1. Testing help command:\n";
    auto result = console.ExecuteCommand("help");
    std::cout << result.output << "\n";
    
    // Test mode switching
    std::cout << "2. Testing mode switching:\n";
    result = console.ExecuteCommand("js");
    std::cout << result.output << "\n";
    
    // Test JavaScript execution (simulated)
    std::cout << "3. Testing JavaScript execution:\n";
    result = console.ExecuteCommand("console.log('Hello from V8!')");
    std::cout << result.output << "\n";
    
    // Switch back to shell
    result = console.ExecuteCommand("shell");
    std::cout << result.output << "\n";
    
    // Test shell command
    std::cout << "4. Testing shell command:\n";
    result = console.ExecuteCommand("echo 'Hello from shell!'");
    std::cout << result.output << "\n";
    
    // Test Claude integration (will show error if not available)
    std::cout << "5. Testing Claude AI integration:\n";
    result = console.ExecuteCommand("ask What is 2+2?");
    if (result.success) {
        std::cout << "Claude response:\n" << result.output << "\n";
    } else {
        std::cout << "Claude not available: " << result.error << "\n";
    }
    
    std::cout << "Demo completed successfully!\n";
    return 0;
}