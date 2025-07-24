#include "ClaudeConsole.h"
#include <iostream>
#include <string>
#include <memory>

#ifndef NO_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

using namespace claude_console;

class ConsoleUI {
public:
    ConsoleUI() : console_(std::make_unique<ClaudeConsole>()), shouldExit_(false) {
        console_->SetOutputCallback([this](const std::string& text) {
            std::cout << text;
        });
        console_->SetErrorCallback([this](const std::string& text) {
            std::cerr << "\033[31m" << text << "\033[0m"; // Red color for errors
        });
    }
    
    bool Initialize() {
        return console_->Initialize();
    }
    
    void Run() {
        PrintWelcome();
        
        std::string input;
        while (!shouldExit_) {
            std::string prompt = GetPrompt();
            
#ifndef NO_READLINE
            char* line = readline(prompt.c_str());
            if (!line) {
                // EOF (Ctrl+D)
                std::cout << "\nGoodbye!\n";
                break;
            }
            
            input = std::string(line);
            free(line);
            
            if (!input.empty()) {
                add_history(input.c_str());
            }
#else
            std::cout << prompt;
            if (!std::getline(std::cin, input)) {
                // EOF
                std::cout << "\nGoodbye!\n";
                break;
            }
#endif
            
            if (input.empty()) continue;
            
            // Check for exit commands
            if (input == "quit" || input == "exit") {
                std::cout << "Goodbye!\n";
                break;
            }
            
            ProcessCommand(input);
        }
    }
    
private:
    void PrintWelcome() {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                                                                              ║\n";
        std::cout << "║  \033[32m██╗   ██╗ █████╗ \033[0m     \033[34m██████╗██████╗ ██████╗\033[0m     \033[35m██████╗██╗      █████╗ ██╗   ██╗██████╗ ███████╗\033[0m  ║\n";
        std::cout << "║  \033[32m██║   ██║██╔══██╗\033[0m    \033[34m██╔════╝██╔══██╗██╔══██╗\033[0m    \033[35m██╔════╝██║     ██╔══██╗██║   ██║██╔══██╗██╔════╝\033[0m  ║\n";
        std::cout << "║  \033[32m██║   ██║╚█████╔╝\033[0m    \033[34m██║     ██████╔╝██████╔╝\033[0m    \033[35m██║     ██║     ███████║██║   ██║██║  ██║█████╗\033[0m    ║\n";
        std::cout << "║  \033[32m╚██╗ ██╔╝██╔══██╗\033[0m    \033[34m██║     ██╔═══╝ ██╔═══╝\033[0m     \033[35m██║     ██║     ██╔══██║██║   ██║██║  ██║██╔══╝\033[0m    ║\n";
        std::cout << "║   \033[32m╚████╔╝ ╚█████╔╝\033[0m    \033[34m╚██████╗██║     ██║\033[0m         \033[35m╚██████╗███████╗██║  ██║╚██████╔╝██████╔╝███████╗\033[0m  ║\n";
        std::cout << "║    \033[32m╚═══╝   ╚════╝\033[0m      \033[34m╚═════╝╚═╝     ╚═╝\033[0m          \033[35m╚═════╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═════╝ ╚══════╝\033[0m  ║\n";
        std::cout << "║                                                                              ║\n";
        std::cout << "║                    \033[1mV8 JavaScript Engine with Claude AI Integration\033[0m                ║\n";
        std::cout << "║                                                                              ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
        std::cout << "Type 'help' for available commands, 'ask <question>' to query Claude AI\n";
        std::cout << "Use 'js' to switch to JavaScript mode, 'shell' for shell mode\n\n";
    }
    
    std::string GetPrompt() {
        std::string modeIndicator = (console_->GetMode() == ConsoleMode::JavaScript) ? "js" : "sh";
        return std::string("\033[36m[") + modeIndicator + "]\033[0m λ ";
    }
    
    void ProcessCommand(const std::string& input) {
        auto result = console_->ExecuteCommand(input);
        
        if (!result.output.empty()) {
            std::cout << result.output;
            if (result.output.back() != '\n') {
                std::cout << '\n';
            }
        }
        
        if (!result.error.empty()) {
            std::cerr << "\033[31mError: " << result.error << "\033[0m\n";
        }
        
        // Show execution time for non-trivial commands
        if (result.executionTime.count() > 1000) { // > 1ms
            std::cout << "\033[90m(" << ClaudeConsole::FormatExecutionTime(result.executionTime) << ")\033[0m\n";
        }
    }
    
    std::unique_ptr<ClaudeConsole> console_;
    bool shouldExit_;
};

int main() {
    ConsoleUI ui;
    
    if (!ui.Initialize()) {
        std::cerr << "Failed to initialize console\n";
        return 1;
    }
    
    ui.Run();
    return 0;
}