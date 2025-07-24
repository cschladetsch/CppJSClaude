#include "ClaudeConsole.h"
#include <iostream>
#include <string>
#include <memory>
#include <filesystem>
#include <fstream>
#include <cstdlib>

#ifndef NO_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

// Shared prompt configuration implementation
namespace SharedConfig {

inline void RunPromptWizard() {
    std::cout << "\033[H\033[2J"; // Clear screen
    std::cout << "\033[1;36m╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║            Shared Prompt Configuration Wizard                  ║\n";
    std::cout << "║         Configure prompts for both v8c and cll                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\033[0m\n\n";
    
    // Get home directory using environment variable
    const char* home = std::getenv("HOME");
    if (!home) {
        std::cerr << "\033[1;31mError:\033[0m Could not find HOME directory\n";
        return;
    }
    
    // Use $HOME/.config/shared for configuration
    std::filesystem::path sharedConfigPath = std::filesystem::path(home) / ".config" / "shared" / "prompts.json";
    
    // Create directory if it doesn't exist
    try {
        std::filesystem::create_directories(sharedConfigPath.parent_path());
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31mError:\033[0m Failed to create config directory: " << e.what() << "\n";
        return;
    }
    
    // Prompt style selection
    std::cout << "Select prompt style:\n\n";
    std::cout << "  \033[1;36m1)\033[0m Lambda style     \033[1;36mλ \033[0m(clean and minimal)\n";
    std::cout << "  \033[1;36m2)\033[0m Traditional      \033[1;32m$ \033[0m(classic shell)\n";
    std::cout << "  \033[1;36m3)\033[0m Arrow            \033[1;35m❯ \033[0m(modern)\n";
    std::cout << "  \033[1;36m4)\033[0m App names        \033[1;34mcll>\033[0m / \033[1;34mv8c>\033[0m\n";
    std::cout << "  \033[1;36m5)\033[0m Custom          (enter your own)\n";
    std::cout << "\nChoice [1]: ";
    
    std::string choice;
    std::getline(std::cin, choice);
    if (choice.empty()) choice = "1";
    
    std::string promptChar = "λ ";
    std::string jsPrompt = "js> ";
    
    if (choice == "2") {
        promptChar = "$ ";
    } else if (choice == "3") {
        promptChar = "❯ ";
    } else if (choice == "4") {
        // This will be overridden by each app
        promptChar = "{app}> ";
    } else if (choice == "5") {
        std::cout << "\nEnter custom shell prompt: ";
        std::getline(std::cin, promptChar);
        std::cout << "Enter custom JavaScript prompt [js> ]: ";
        std::string customJs;
        std::getline(std::cin, customJs);
        if (!customJs.empty()) jsPrompt = customJs;
    }
    
    // Color selection
    std::cout << "\nSelect prompt color:\n\n";
    std::cout << "  1) \033[1;36mCyan\033[0m (default)\n";
    std::cout << "  2) \033[1;32mGreen\033[0m\n";
    std::cout << "  3) \033[1;34mBlue\033[0m\n";
    std::cout << "  4) \033[1;35mMagenta\033[0m\n";
    std::cout << "  5) \033[1;33mYellow\033[0m\n";
    std::cout << "  6) \033[1;37mWhite\033[0m\n";
    std::cout << "  7) No color\n";
    std::cout << "\nChoice [1]: ";
    
    std::string colorChoice;
    std::getline(std::cin, colorChoice);
    if (colorChoice.empty()) colorChoice = "1";
    
    std::string shellColor = "\\033[1;36m";  // Default cyan
    if (colorChoice == "2") shellColor = "\\033[1;32m";
    else if (colorChoice == "3") shellColor = "\\033[1;34m";
    else if (colorChoice == "4") shellColor = "\\033[1;35m";
    else if (colorChoice == "5") shellColor = "\\033[1;33m";
    else if (colorChoice == "6") shellColor = "\\033[1;37m";
    else if (colorChoice == "7") shellColor = "";
    
    // Write configuration to $HOME/.config/shared/prompts.json
    std::ofstream config(sharedConfigPath);
    if (!config) {
        std::cerr << "\033[1;31mError:\033[0m Failed to open config file for writing\n";
        return;
    }
    
    config << "{\n";
    config << "  \"shared_prompt\": {\n";
    config << "    \"style\": \"" << choice << "\",\n";
    config << "    \"shell\": \"" << promptChar << "\",\n";
    config << "    \"javascript\": \"" << jsPrompt << "\",\n";
    config << "    \"multiline\": \"... \",\n";
    config << "    \"colors\": {\n";
    config << "      \"shell\": \"" << shellColor << "\",\n";
    config << "      \"javascript\": \"\\033[1;33m\",\n";
    config << "      \"multiline\": \"\\033[1;37m\",\n";
    config << "      \"reset\": \"\\033[0m\"\n";
    config << "    }\n";
    config << "  }\n";
    config << "}\n";
    config.close();
    
    std::cout << "\n\033[1;32m✓\033[0m Configuration saved to: \033[1;36m$HOME/.config/shared/prompts.json\033[0m\n";
    std::cout << "\nBoth \033[1;34mv8c\033[0m and \033[1;34mcll\033[0m will use this prompt configuration.\n";
    std::cout << "Restart the applications to see the changes.\n\n";
}

} // namespace SharedConfig

namespace fs = std::filesystem;
using namespace cll;

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
                if (console_->IsInMultiLineMode()) {
                    // Execute multi-line input on Ctrl+D
                    auto result = console_->ExecuteMultiLineInput();
                    ProcessResult(result);
                    continue;
                } else {
                    std::cout << "\nGoodbye!\n";
                    break;
                }
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
                if (console_->IsInMultiLineMode()) {
                    // Execute multi-line input on Ctrl+D
                    auto result = console_->ExecuteMultiLineInput();
                    ProcessResult(result);
                    continue;
                } else {
                    std::cout << "\nGoodbye!\n";
                    break;
                }
            }
#endif
            
            if (input.empty()) {
                if (console_->IsInMultiLineMode()) {
                    console_->AppendMultiLineInput("");
                }
                continue;
            }
            
            // Handle multi-line mode
            if (console_->IsInMultiLineMode()) {
                console_->AppendMultiLineInput(input);
                continue;
            }
            
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
        // No banner - start clean
    }
    
    std::string GetPrompt() {
        // Create prompt based on console mode
        if (console_->IsInMultiLineMode()) {
            return "\033[1;37m... \033[0m";  // White for multiline
        }
        
        if (console_->IsJavaScriptMode()) {
            return "\033[1;33mφ \033[0m";  // Yellow for JavaScript
        } else if (console_->IsAskMode()) {
            return "\033[1;35mθ \033[0m";  // Magenta for Ask
        } else {
            return "\033[1;36mλ \033[0m";  // Cyan lambda for shell
        }
    }
    
    void ProcessCommand(const std::string& input) {
        auto result = console_->ExecuteCommand(input);
        ProcessResult(result);
    }
    
    void ProcessResult(const CommandResult& result) {
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


int main(int argc, char* argv[]) {
    // Handle command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  --help, -h      Show this help message\n";
            std::cout << "  --configure     Run the interactive prompt configuration wizard\n";
            std::cout << "  --version, -v   Show version information\n";
            return 0;
        } else if (arg == "--configure") {
            SharedConfig::RunPromptWizard();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "cll (Claude Command Line) version 1.0.0\n";
            return 0;
        }
    }
    
    ConsoleUI ui;
    
    if (!ui.Initialize()) {
        std::cerr << "Failed to initialize console\n";
        return 1;
    }
    
    ui.Run();
    return 0;
}