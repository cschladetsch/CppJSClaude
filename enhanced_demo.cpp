#include "ClaudeConsole.h"
#include <iostream>

using namespace cll;

int main() {
    std::cout << "=== Enhanced CppV8-Claude Integration Demo ===\n\n";
    
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
    
    // Test shell commands
    std::cout << "2. Testing shell commands:\n";
    result = console.ExecuteCommand("echo 'Hello from shell!'");
    std::cout << "Shell output: " << result.output;
    
    result = console.ExecuteCommand("pwd");
    std::cout << "Current directory: " << result.output;
    
    // Test & prefix for JavaScript in shell mode
    std::cout << "\n3. Testing & prefix for JavaScript execution in shell mode:\n";
    std::cout << "Console is in shell mode, executing JavaScript with & prefix:\n";
    
    result = console.ExecuteCommand("&Math.sqrt(64)");
    std::cout << "JavaScript (&Math.sqrt(64)): " << result.output << "\n";
    
    result = console.ExecuteCommand("&Math.PI * 2");
    std::cout << "JavaScript (&Math.PI * 2): " << result.output << "\n";
    
    result = console.ExecuteCommand("&console.log('Hello from JavaScript via & prefix!')");
    std::cout << "JavaScript (&console.log(...)): " << result.output << "\n";
    
    // Test mixed mode usage
    std::cout << "4. Testing mixed shell and JavaScript commands:\n";
    result = console.ExecuteCommand("echo 'This is a shell command'");
    std::cout << "Shell: " << result.output;
    
    result = console.ExecuteCommand("&new Date().toISOString()");
    std::cout << "JavaScript (&new Date()...): " << result.output << "\n";
    
    result = console.ExecuteCommand("ls -1 | head -3");
    std::cout << "Shell (ls): " << result.output;
    
    result = console.ExecuteCommand("&Math.random()");
    std::cout << "JavaScript (&Math.random()): " << result.output << "\n";
    
    // Test mode switching
    std::cout << "5. Testing explicit mode switching:\n";
    result = console.ExecuteCommand("js");
    std::cout << result.output << "\n";
    
    // Now in JavaScript mode
    result = console.ExecuteCommand("Math.max(10, 20, 30)");
    std::cout << "In JS mode - Math.max(10,20,30): " << result.output << "\n";
    
    // Switch back to shell
    result = console.ExecuteCommand("shell");
    std::cout << result.output << "\n";
    
    // Test Claude integration
    std::cout << "6. Testing Claude AI integration:\n";
    result = console.ExecuteCommand("ask What is 5 factorial?");
    if (result.success) {
        std::cout << "Claude response:\n" << result.output << "\n";
    } else {
        std::cout << "Claude not available: " << result.error << "\n";
    }
    
    std::cout << "Enhanced demo completed successfully!\n";
    std::cout << "\nKey features demonstrated:\n";
    std::cout << "✓ Shell command execution\n";
    std::cout << "✓ JavaScript execution with & prefix in shell mode\n";
    std::cout << "✓ Mixed shell/JavaScript workflow\n";
    std::cout << "✓ Explicit mode switching (js/shell)\n";
    std::cout << "✓ Claude AI integration\n";
    
    return 0;
}