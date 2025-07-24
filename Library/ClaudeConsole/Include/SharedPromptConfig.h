#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>

namespace fs = std::filesystem;

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
    fs::path sharedConfigPath = fs::path(home) / ".config" / "shared" / "prompts.json";
    
    // Create directory if it doesn't exist
    try {
        fs::create_directories(sharedConfigPath.parent_path());
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

// Helper function to load shared prompt config from $HOME/.config/shared/
inline std::string GetSharedPrompt(const std::string& appName = "") {
    const char* home = std::getenv("HOME");
    if (!home) return "λ ";  // Default
    
    fs::path configPath = fs::path(home) / ".config" / "shared" / "prompts.json";
    if (!fs::exists(configPath)) return "λ ";
    
    // For now, return default lambda prompt
    // In a real implementation, you'd parse the JSON
    return "\033[1;36mλ \033[0m";
}

} // namespace SharedConfig