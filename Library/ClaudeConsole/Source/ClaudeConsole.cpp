#include "ClaudeConsole.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <format> 
#include <cstdlib>
#include <chrono>
#include <fstream>

#ifdef HAS_V8
#include "DllLoader.h"
#include "V8Compat.h"
#include <libplatform/libplatform.h>
#endif

#ifdef HAS_JSON
#include <nlohmann/json.hpp>
#endif

namespace fs = std::filesystem;

namespace cll {

#ifdef HAS_V8
// Static instance for V8 callbacks
ClaudeConsole* ClaudeConsole::instance_ = nullptr;
#endif

ClaudeConsole::ClaudeConsole()
    : mode_(ConsoleMode::Shell), multiLineMode_(MultiLineMode::None),
      promptFormat_("❯ [{mode}] "), claudePrompt_("? "), claudePromptColor_("orange")
#ifdef HAS_V8
      , platform_(nullptr), isolate_(nullptr)
#endif
{
    
    // Create config directory if it doesn't exist
    CreateConfigDirectory();
    
    // Load user configuration
    LoadConfiguration();
    
    // Initialize built-in commands
    builtinCommands_ = {
        {"help", "Show help message"},
        {"quit", "Exit the console"},
        {"exit", "Exit the console"},
        {"clear", "Clear the console"},
        {"js", "Switch to JavaScript mode"},
        {"javascript", "Switch to JavaScript mode"},
        {"shell", "Switch to shell mode"},
        {"sh", "Switch to shell mode"},
        {"config", "Manage configuration and aliases"},
        {"reload", "Reload configuration from files"}
    };
}

ClaudeConsole::~ClaudeConsole() {
    Shutdown();
}

bool ClaudeConsole::Initialize() {
#ifdef HAS_V8
    // Set static instance for callbacks
    instance_ = this;
    
    // Initialize V8 platform
    v8::V8::InitializeICUDefaultLocation("");
    v8::V8::InitializeExternalStartupData("");
    platform_ = v8_compat::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform_.get());
    v8::V8::Initialize();
    
    // Create a new Isolate
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = 
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate_ = v8::Isolate::New(create_params);
    
    if (!isolate_) {
        return false;
    }
    
    // Create V8 context
    {
        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope handle_scope(isolate_);
        
        v8::Local<v8::Context> context = v8::Context::New(isolate_);
        context_.Reset(isolate_, context);
        
        // Enter context scope before registering builtins
        v8::Context::Scope context_scope(context);
        
        // Register built-in functions
        RegisterBuiltins(context);
    }
    
    // Initialize DLL loader
    dllLoader_ = std::make_unique<DllLoader>();
#endif
    
    return true;
}

void ClaudeConsole::Shutdown() {
#ifdef HAS_V8
    if (!isolate_) return;
    
    // Clean up V8
    context_.Reset();
    isolate_->Dispose();
    isolate_ = nullptr;
    v8::V8::Dispose();
    
    // Clear instance
    instance_ = nullptr;
#endif
}

CommandResult ClaudeConsole::ExecuteCommand(const std::string& command) {
    if (command.empty()) {
        return {true, "", "", std::chrono::microseconds(0), 0};
    }
    
    // Trim whitespace for consistent command handling
    std::string trimmed = command;
    // Remove leading whitespace
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) {
        trimmed = trimmed.substr(start);
    } else {
        trimmed.clear();
    }
    // Remove trailing whitespace
    size_t end = trimmed.find_last_not_of(" \t\n\r");
    if (end != std::string::npos) {
        trimmed = trimmed.substr(0, end + 1);
    }
    
    if (trimmed.empty()) {
        return {true, "", "", std::chrono::microseconds(0), 0};
    }
    
    // Handle comments - lines starting with # are ignored (like bash/zsh)
    if (!trimmed.empty() && trimmed[0] == '#') {
        return {true, "", "", std::chrono::microseconds(0), 0};
    }
    
    // Check for mode switch commands
    if (trimmed == "js" || trimmed == "javascript") {
        SetMode(ConsoleMode::JavaScript);
        return {true, "Switched to JavaScript mode", "", std::chrono::microseconds(0), 0};
    } else if (trimmed == "shell" || trimmed == "sh") {
        SetMode(ConsoleMode::Shell);
        return {true, "Switched to Shell mode", "", std::chrono::microseconds(0), 0};
    } else if (trimmed == "claude") {
        SetMode(ConsoleMode::Ask);
        return {true, "Switched to Ask mode", "", std::chrono::microseconds(0), 0};
    }
    
    // Check for command substitution with backticks `cmd`
    size_t backtickStart = 0;
    std::string processed = trimmed;
    
    while ((backtickStart = processed.find('`', backtickStart)) != std::string::npos) {
        size_t backtickEnd = processed.find('`', backtickStart + 1);
        if (backtickEnd == std::string::npos) {
            break; // No closing backtick found
        }
        
        std::string shellCommand = processed.substr(backtickStart + 1, backtickEnd - backtickStart - 1);
        
        // Execute the shell command and capture output
        FILE* pipe = popen((shellCommand + " 2>&1").c_str(), "r");
        if (pipe == nullptr) {
            return {false, "Failed to execute command: " + shellCommand, "", std::chrono::microseconds(0), 1};
        }
        
        std::string output;
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }
        pclose(pipe);
        
        // Remove trailing newline if present
        if (!output.empty() && output.back() == '\n') {
            output.pop_back();
        }
        
        // Replace the `cmd` with the output
        processed.replace(backtickStart, backtickEnd - backtickStart + 1, output);
        backtickStart += output.length();
    }
    
    // If command substitution occurred, process the result
    if (processed != trimmed) {
        if (processed.empty()) {
            return {true, "", "", std::chrono::microseconds(0), 0};
        }
        return ExecuteCommand(processed);
    }
    
    // Check for command prefixes
    if (!trimmed.empty()) {
        char prefix = trimmed[0];
        
        // Handle & prefix (JavaScript)
        if (prefix == '&') {
            if (trimmed.length() == 1) {
                // Just '&' pressed - start multi-line JavaScript mode
                StartMultiLineMode(MultiLineMode::JavaScript);
                return {true, "Multi-line JavaScript mode (Ctrl-D to execute)", "", std::chrono::microseconds(0), 0};
            } else {
                std::string jsCode = trimmed.substr(1);
                return ExecuteJavaScript(jsCode);
            }
        }
        
        // Handle ? prefix (Ask Claude)
        if (prefix == '?') {
            if (trimmed.length() == 1) {
                SetMode(ConsoleMode::Ask);
                return {true, "Switched to Ask mode", "", std::chrono::microseconds(0), 0};
            } else {
                std::string question = trimmed.substr(1);
                return ExecuteAsk(question);
            }
        }
        
        
        // Handle $ prefix (Shell)
        if (prefix == '$') {
            if (trimmed.length() == 1) {
                SetMode(ConsoleMode::Shell);
                return {true, "Switched to Shell mode", "", std::chrono::microseconds(0), 0};
            } else {
                std::string command = trimmed.substr(1);
                return ExecuteShellCommand(command);
            }
        }
        
        // Handle φ prefix (JavaScript) - UTF-8 handling
        if (trimmed == "φ") {
            SetMode(ConsoleMode::JavaScript);
            return {true, "Switched to JavaScript mode", "", std::chrono::microseconds(0), 0};
        } else if (trimmed.length() > 2 && trimmed.substr(0, 2) == "φ") {
            // UTF-8 φ is 2 bytes
            std::string jsCode = trimmed.substr(2);
            return ExecuteJavaScript(jsCode);
        }
        
        // Handle θ prefix (Ask) - UTF-8 handling
        if (trimmed == "θ") {
            SetMode(ConsoleMode::Ask);
            return {true, "Switched to Ask mode", "", std::chrono::microseconds(0), 0};
        } else if (trimmed.length() > 2 && trimmed.substr(0, 2) == "θ") {
            // UTF-8 θ is 2 bytes
            std::string question = trimmed.substr(2);
            return ExecuteAsk(question);
        }
    }
    
    // Check for ask lines
    auto words = SplitCommand(trimmed);
    if (!words.empty() && words[0] == "ask") {
        if (words.size() == 1) {
            // Just 'ask' pressed - start multi-line ask mode
            StartMultiLineMode(MultiLineMode::Ask);
            return {true, "Multi-line ask mode (Ctrl-D to send to Claude)", "", std::chrono::microseconds(0), 0};
        } else {
            // 'ask' with content - execute immediately
            std::string question = trimmed.substr(4); // Remove "ask "
            return ExecuteClaudeQuery(question);
        }
    }
    
    // Handle built-in commands
    if (IsBuiltinCommand(trimmed)) {
        return ExecuteBuiltinCommand(trimmed);
    }
    
    // Execute based on mode
    if (mode_ == ConsoleMode::JavaScript) {
        return ExecuteJavaScript(trimmed);
    } else if (mode_ == ConsoleMode::Ask) {
        return ExecuteAsk(trimmed);
    } else {
        return ExecuteShellCommand(trimmed);
    }
}

CommandResult ClaudeConsole::ExecuteJavaScript(const std::string& code) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CommandResult result;
#ifdef HAS_V8
    result.success = ExecuteString(code, "<repl>");
#else
    // Simulate JavaScript execution when V8 is not available
    result.success = true;
    result.output = std::format("// JavaScript execution simulated (V8 not available)\n// Code: {}\n", code);
#endif
    
    result.executionTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - startTime);
    result.exitCode = result.success ? 0 : 1;
    
    return result;
}

CommandResult ClaudeConsole::ExecuteAsk(const std::string& question) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CommandResult result;
    result.success = true;
    result.output = std::format("// Ask mode - Claude AI integration would handle: {}\n// (Claude AI integration not implemented yet)\n", question);
    
    result.executionTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - startTime);
    result.exitCode = 0;
    
    return result;
}

CommandResult ClaudeConsole::ExecuteShellCommand(const std::string& command) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Execute shell command
    FILE* pipe = popen((command + " 2>&1").c_str(), "r");
    if (!pipe) {
        return {false, "", "Failed to execute command", std::chrono::microseconds(0), 127};
    }
    
    std::string output;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }
    
    int exitCode = pclose(pipe);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    CommandResult result;
    result.success = (WEXITSTATUS(exitCode) == 0);
    result.output = output;
    result.exitCode = WEXITSTATUS(exitCode);
    result.executionTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    return result;
}

CommandResult ClaudeConsole::ExecuteClaudeQuery(const std::string& question) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Check if PyClaudeCli is available via 'ask' command
    FILE* checkPipe = popen("which ask 2>/dev/null", "r");
    if (checkPipe) {
        char buffer[256];
        bool hasAsk = (fgets(buffer, sizeof(buffer), checkPipe) != nullptr);
        pclose(checkPipe);
        
        if (hasAsk) {
            // Execute ask command with the question
            std::string askCommand = "ask \"" + question + "\" 2>&1";
            return ExecuteSubprocess(askCommand);
        }
    }
    
    // Try to find PyClaudeCli in common locations
    std::string pyClaudePath = FindPyClaudeCliPath();
    if (!pyClaudePath.empty()) {
        std::string pythonCmd = "python3 \"" + pyClaudePath + "\" \"" + question + "\" 2>&1";
        return ExecuteSubprocess(pythonCmd);
    }
    
    // If Claude is not available, return helpful error
    CommandResult result;
    result.success = false;
    result.error = "Claude AI not found. Please ensure 'ask' is in your PATH or install PyClaudeCli.";
    result.exitCode = 1;
    result.executionTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - startTime);
    
    return result;
}

CommandResult ClaudeConsole::ExecuteSubprocess(const std::string& command) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return {false, "", "Failed to execute command", std::chrono::microseconds(0), 1};
    }
    
    std::string output;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }
    
    int exitCode = pclose(pipe);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    CommandResult result;
    result.success = (WEXITSTATUS(exitCode) == 0);
    result.output = output;
    result.exitCode = WEXITSTATUS(exitCode);
    result.executionTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    if (!result.success && !output.empty()) {
        result.error = output;
        result.output = "";
    }
    
    return result;
}

bool ClaudeConsole::IsBuiltinCommand(const std::string& command) const {
    auto words = SplitCommand(command);
    if (words.empty()) return false;
    
    return builtinCommands_.find(words[0]) != builtinCommands_.end();
}

CommandResult ClaudeConsole::ExecuteBuiltinCommand(const std::string& command) {
    auto words = SplitCommand(command);
    if (words.empty()) {
        return {false, "", "Empty command", std::chrono::microseconds(0), 1};
    }
    
    const std::string& cmd = words[0];
    CommandResult result;
    result.success = true;
    result.exitCode = 0;
    
    if (cmd == "help") {
        result.output = "Available commands:\n";
        for (const auto& [name, desc] : builtinCommands_) {
            result.output += std::format("  {} - {}\n", name, desc);
        }
        result.output += "\nSpecial features:\n";
        result.output += "  &<javascript> - Execute JavaScript from shell mode (e.g., &Math.sqrt(16))\n";
        result.output += "\nCurrent mode: " + std::string(mode_ == ConsoleMode::JavaScript ? "JavaScript" : "Shell");
    } else if (cmd == "quit" || cmd == "exit") {
        result.output = "Exiting...";
        // The UI layer should handle actual exit
    } else if (cmd == "clear") {
        result.output = "\033[2J\033[H"; // ANSI clear screen
    } else if (cmd == "config") {
        if (words.size() == 1) {
            // Show config directory
            result.output = "Configuration directory: " + GetConfigPath() + "\n";
            result.output += "Configuration files:\n";
            result.output += "  config.json - Main configuration\n";
            result.output += "  aliases - Command aliases\n";
            result.output += "\nUse 'reload' to reload configuration from files\n";
        } else if (words.size() >= 3 && words[1] == "alias") {
            // Set alias: config alias name=value
            std::string aliasCmd = command.substr(command.find("alias") + 6); // Skip "config alias "
            size_t eq = aliasCmd.find('=');
            if (eq != std::string::npos) {
                std::string name = aliasCmd.substr(0, eq);
                std::string value = aliasCmd.substr(eq + 1);
                // Remove quotes if present
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                SetAlias(name, value);
                SaveConfiguration();
                result.output = std::format("Alias set: {} = '{}'", name, value);
            } else {
                result.success = false;
                result.error = "Usage: config alias name=value";
                result.exitCode = 1;
            }
        } else {
            result.success = false;
            result.error = "Usage: config [alias name=value]";
            result.exitCode = 1;
        }
    } else if (cmd == "reload") {
        LoadConfiguration();
        result.output = "Configuration reloaded from " + GetConfigPath();
    } else {
        result.success = false;
        result.error = "Unknown command: " + cmd;
        result.exitCode = 1;
    }
    
    return result;
}

std::string ClaudeConsole::FormatExecutionTime(const std::chrono::microseconds& us) {
    if (us.count() < 1000) {
        return std::format("{}μs", us.count());
    } else if (us.count() < 1000000) {
        return std::format("{:.1f}ms", us.count() / 1000.0);
    } else {
        return std::format("{:.2f}s", us.count() / 1000000.0);
    }
}

std::vector<std::string> ClaudeConsole::SplitCommand(const std::string& command) {
    std::vector<std::string> words;
    std::istringstream iss(command);
    std::string word;
    
    while (iss >> word) {
        words.push_back(word);
    }
    
    return words;
}

std::string ClaudeConsole::FindPyClaudeCliPath() {
    // Check common locations for PyClaudeCli
    std::vector<std::string> searchPaths = {
        "../PyClaudeCli/main.py",
        "../../PyClaudeCli/main.py",
        "../../../PyClaudeCli/main.py",
        fs::path(fs::current_path()).parent_path() / "PyClaudeCli" / "main.py"
    };
    
    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            return fs::absolute(path).string();
        }
    }
    
    return "";
}

void ClaudeConsole::CreateConfigDirectory() {
    std::string configDir = GetConfigPath();
    if (!fs::exists(configDir)) {
        fs::create_directories(configDir);
        
        // Create default configuration files
        std::string configFile = configDir + "/config.json";
        if (!fs::exists(configFile)) {
            std::ofstream config(configFile);
            config << "{\n";
            config << "  \"default_mode\": \"shell\",\n";
            config << "  \"prompt_format\": \"❯ \",\n";
            config << "  \"claude_prompt\": \"? \",\n";
            config << "  \"claude_prompt_color\": \"orange\",\n";
            config << "  \"show_execution_time\": true,\n";
            config << "  \"history_size\": 1000,\n";
            config << "  \"enable_colors\": true,\n";
            config << "  \"claude_integration\": {\n";
            config << "    \"enabled\": true,\n";
            config << "    \"timeout_seconds\": 30\n";
            config << "  },\n";
            config << "  \"aliases\": {\n";
            config << "    \"ll\": \"ls -la\",\n";
            config << "    \"la\": \"ls -la\",\n";
            config << "    \"...\": \"cd ../..\"\n";
            config << "  }\n";
            config << "}\n";
            config.close();
        }
        
        // Create aliases file
        std::string aliasFile = configDir + "/aliases";
        if (!fs::exists(aliasFile)) {
            std::ofstream aliases(aliasFile);
            aliases << "# Claude Console Aliases\n";
            aliases << "# Format: alias_name=command\n";
            aliases << "ll=ls -la\n";
            aliases << "la=ls -la\n";
            aliases << "...=cd ../..\n";
            aliases << "cls=clear\n";
            aliases << "q=quit\n";
            aliases.close();
        }
    }
}

void ClaudeConsole::LoadConfiguration() {
    // First load shared configuration
    LoadSharedConfiguration();
    
    // Then load app-specific configuration
    std::string configFile = GetConfigPath() + "/config.json";
    if (fs::exists(configFile)) {
        // TODO: Parse JSON configuration
        // For now, just load basic aliases from aliases file
        std::string aliasFile = GetConfigPath() + "/aliases";
        if (fs::exists(aliasFile)) {
            std::ifstream file(aliasFile);
            std::string line;
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') continue;
                
                size_t eq = line.find('=');
                if (eq != std::string::npos) {
                    std::string name = line.substr(0, eq);
                    std::string value = line.substr(eq + 1);
                    SetAlias(name, value);
                }
            }
        }
    }
}

void ClaudeConsole::SaveConfiguration() {
    // Ensure config directory exists
    std::string configPath = GetConfigPath();
    fs::create_directories(configPath);
    
    // Save configuration to JSON
    std::string configFile = configPath + "/config.json";
    
#ifdef HAS_JSON
    nlohmann::json config;
    config["default_mode"] = (mode_ == ConsoleMode::JavaScript) ? "javascript" : 
                            (mode_ == ConsoleMode::Ask) ? "ask" : "shell";
    config["prompt_format"] = promptFormat_;
    config["claude_prompt"] = claudePrompt_;
    config["claude_integration"] = {
        {"enabled", true},
        {"api_key", ""}  // API key would be set via environment variable
    };
    
    std::ofstream jsonFile(configFile);
    if (jsonFile.is_open()) {
        jsonFile << config.dump(4);
        jsonFile.close();
    }
#endif
    
    // Also save aliases separately
    std::string aliasFile = configPath + "/aliases";
    std::ofstream file(aliasFile);
    file << "# Claude Console Aliases\n";
    file << "# Format: alias_name=command\n";
    for (const auto& [name, value] : aliases_) {
        file << name << "=" << value << "\n";
    }
}

std::string ClaudeConsole::GetConfigPath() const {
    const char* home = std::getenv("HOME");
    if (!home) {
        home = std::getenv("USERPROFILE"); // Windows fallback
    }
    
    if (home) {
        return std::string(home) + "/.config/cll";
    }
    
    return "./.config/cll"; // Fallback to current directory
}

std::string ClaudeConsole::GetSharedConfigPath() const {
    const char* home = std::getenv("HOME");
    if (!home) {
        home = std::getenv("USERPROFILE"); // Windows fallback
    }
    
    if (home) {
        return std::string(home) + "/.config/shared";
    }
    
    return "./.config/shared"; // Fallback to current directory
}

void ClaudeConsole::LoadSharedConfiguration() {
    std::string sharedAliasFile = GetSharedConfigPath() + "/aliases";
    if (fs::exists(sharedAliasFile)) {
        std::ifstream file(sharedAliasFile);
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            size_t eq = line.find('=');
            if (eq != std::string::npos) {
                std::string name = line.substr(0, eq);
                std::string value = line.substr(eq + 1);
                SetAlias(name, value);
            }
        }
    }
}

void ClaudeConsole::SetAlias(const std::string& name, const std::string& value) {
    aliases_[name] = value;
}

std::string ClaudeConsole::ExpandAlias(const std::string& command) const {
    auto words = SplitCommand(command);
    if (words.empty()) return command;
    
    auto it = aliases_.find(words[0]);
    if (it != aliases_.end()) {
        std::string expanded = it->second;
        for (size_t i = 1; i < words.size(); ++i) {
            expanded += " " + words[i];
        }
        return expanded;
    }
    
    return command;
}

void ClaudeConsole::Output(const std::string& text) {
    if (outputCallback_) {
        outputCallback_(text);
    } else {
        std::cout << text;
    }
}

void ClaudeConsole::Error(const std::string& text) {
    if (errorCallback_) {
        errorCallback_(text);
    } else {
        std::cerr << text;
    }
}

// Multi-line input methods
void ClaudeConsole::StartMultiLineMode(MultiLineMode mode) {
    multiLineMode_ = mode;
    multiLineBuffer_.clear();
}

void ClaudeConsole::EndMultiLineMode() {
    multiLineMode_ = MultiLineMode::None;
    multiLineBuffer_.clear();
}

void ClaudeConsole::AppendMultiLineInput(const std::string& line) {
    if (!multiLineBuffer_.empty()) {
        multiLineBuffer_ += "\n";
    }
    multiLineBuffer_ += line;
}

CommandResult ClaudeConsole::ExecuteMultiLineInput() {
    CommandResult result;
    
    if (multiLineMode_ == MultiLineMode::JavaScript) {
        result = ExecuteJavaScript(multiLineBuffer_);
    } else if (multiLineMode_ == MultiLineMode::Ask) {
        result = ExecuteClaudeQuery(multiLineBuffer_);
    } else {
        result.success = false;
        result.error = "Not in multi-line mode";
        result.exitCode = 1;
    }
    
    // Clear multi-line state after execution
    EndMultiLineMode();
    
    return result;
}

// Prompt methods
std::string ClaudeConsole::GetPrompt() const {
    if (multiLineMode_ != MultiLineMode::None) {
        return GetMultiLinePrompt();
    }
    
    std::string prompt = promptFormat_;
    
    // Replace {mode} placeholder
    std::string modeStr = (mode_ == ConsoleMode::JavaScript) ? "js" : 
                          (mode_ == ConsoleMode::Ask) ? "ask" : "sh";
    size_t pos = prompt.find("{mode}");
    if (pos != std::string::npos) {
        prompt.replace(pos, 6, modeStr);
    }
    
    return prompt;
}

std::string ClaudeConsole::GetMultiLinePrompt() const {
    switch (multiLineMode_) {
        case MultiLineMode::JavaScript:
            return "  ...js ";
        case MultiLineMode::Ask:
            return GetClaudePrompt();
        default:
            return promptFormat_;
    }
}

std::string ClaudeConsole::GetClaudePrompt() const {
    // For now, return simple orange-colored prompt
    // In a full implementation with rang, this would use actual colors
    return claudePrompt_;
}

#ifdef HAS_V8
// V8 JavaScript execution methods
bool ClaudeConsole::ExecuteFile(const std::string& path) {
    std::string source = ReadFile(path);
    if (source.empty()) {
        Error(std::format("Error: Could not read file: \"{}\"\n", path));
        return false;
    }
    
    return ExecuteString(source, path);
}

bool ClaudeConsole::ExecuteString(const std::string& source, const std::string& name) {
    if (!isolate_) return false;
    
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Context> context = context_.Get(isolate_);
    v8::Context::Scope context_scope(context);
    
    return CompileAndRun(source, name);
}

bool ClaudeConsole::CompileAndRun(const std::string& source, const std::string& name) {
    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Context> context = context_.Get(isolate_);
    v8::Context::Scope context_scope(context);
    
    v8::TryCatch tryCatch(isolate_);
    
    // Compile the script
    v8::Local<v8::String> sourceV8 = v8::String::NewFromUtf8(isolate_, source.c_str()).ToLocalChecked();
    v8::Local<v8::String> nameV8 = v8::String::NewFromUtf8(isolate_, name.c_str()).ToLocalChecked();
    
    v8::ScriptOrigin origin = v8_compat::CreateScriptOrigin(isolate_, nameV8);
    v8::Local<v8::Script> script;
    if (!v8::Script::Compile(context, sourceV8, &origin).ToLocal(&script)) {
        ReportException(&tryCatch);
        return false;
    }
    
    // Run the script
    v8::Local<v8::Value> result;
    if (!script->Run(context).ToLocal(&result)) {
        ReportException(&tryCatch);
        return false;
    }
    
    // Print result in REPL mode
    if (name == "<repl>" && !result->IsUndefined()) {
        PrintResult(result);
    }
    
    return true;
}

std::string ClaudeConsole::ReadFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (file) {
        return std::string(std::istreambuf_iterator<char>(file), 
                          std::istreambuf_iterator<char>());
    }
    return "";
}

void ClaudeConsole::ReportException(v8::TryCatch* tryCatch) {
    v8::HandleScope handle_scope(isolate_);
    v8::String::Utf8Value exception(isolate_, tryCatch->Exception());
    const char* exception_string = *exception ? *exception : "Error: <unknown exception>";
    
    v8::Local<v8::Message> message = tryCatch->Message();
    if (message.IsEmpty()) {
        // V8 didn't provide any extra information about this error; just print the exception
        Error(std::format("{}\n", exception_string));
    } else {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(isolate_, message->GetScriptOrigin().ResourceName());
        v8::Local<v8::Context> context(isolate_->GetCurrentContext());
        const char* filename_string = *filename ? *filename : "<unknown>";
        int linenum = message->GetLineNumber(context).FromMaybe(-1);
        Error(std::format("{}:{}: {}\n", filename_string, linenum, exception_string));
        
        // Print line of source code.
        v8::String::Utf8Value sourceline(isolate_, message->GetSourceLine(context).ToLocalChecked());
        const char* sourceline_string = *sourceline ? *sourceline : " ";
        Error(std::format("{}\n", sourceline_string));
        
        // Print wavy underline
        int start = message->GetStartColumn(context).FromMaybe(0);
        for (int i = 0; i < start; i++) {
            Error(" ");
        }
        int end = message->GetEndColumn(context).FromMaybe(0);
        for (int i = start; i < end; i++) {
            Error("^");
        }
        Error("\n");
        
        v8::Local<v8::Value> stack_trace_string;
        if (tryCatch->StackTrace(context).ToLocal(&stack_trace_string) &&
            stack_trace_string->IsString() &&
            v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
            v8::String::Utf8Value stack_trace(isolate_, stack_trace_string);
            const char* stack_trace_string_value = *stack_trace ? *stack_trace : " ";
            Error(std::format("{}\n", stack_trace_string_value));
        }
    }
}

void ClaudeConsole::PrintResult(v8::Local<v8::Value> value) {
    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Context> context = context_.Get(isolate_);
    
    // Convert result to string
    v8::String::Utf8Value str(isolate_, value->ToString(context).ToLocalChecked());
    const char* cstr = *str ? *str : "<string conversion failed>";
    Output(std::format("{}\n", cstr));
}

// DLL loading methods
bool ClaudeConsole::LoadDll(const std::string& path) {
    if (!dllLoader_ || !isolate_) return false;
    
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Context> context = context_.Get(isolate_);
    v8::Context::Scope context_scope(context);
    
    Output(std::format("Loading DLL: {}\n", path));
    if (dllLoader_->LoadDll(path, isolate_, context)) {
        Output(std::format("✓ Successfully loaded: {}\n", path));
        return true;
    } else {
        Error(std::format("✗ Failed to load: {}\n", path));
        return false;
    }
}

bool ClaudeConsole::UnloadDll(const std::string& path) {
    if (!dllLoader_) return false;
    
    if (dllLoader_->UnloadDll(path)) {
        Output(std::format("Unloaded DLL: {}\n", path));
        return true;
    }
    return false;
}

bool ClaudeConsole::ReloadDll(const std::string& path) {
    if (!dllLoader_ || !isolate_) return false;
    
    v8::Isolate::Scope isolate_scope(isolate_);
    v8::HandleScope handle_scope(isolate_);
    v8::Local<v8::Context> context = context_.Get(isolate_);
    v8::Context::Scope context_scope(context);
    
    return dllLoader_->ReloadDll(path, isolate_, context);
}

std::vector<std::string> ClaudeConsole::GetLoadedDlls() const {
    if (!dllLoader_) return {};
    return dllLoader_->GetLoadedDlls();
}

// V8 built-in functions
void ClaudeConsole::RegisterBuiltins(v8::Local<v8::Context> context) {
    v8::HandleScope handle_scope(isolate_);
    
    // Get the global object
    v8::Local<v8::Object> global = context->Global();
    
    // Register print function
    global->Set(context,
        v8::String::NewFromUtf8(isolate_, "print").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, Print)->GetFunction(context).ToLocalChecked());
    
    // Register load function
    global->Set(context,
        v8::String::NewFromUtf8(isolate_, "load").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, Load)->GetFunction(context).ToLocalChecked());
    
    // Register DLL functions
    global->Set(context,
        v8::String::NewFromUtf8(isolate_, "loadDll").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, LoadDllFunc)->GetFunction(context).ToLocalChecked());
        
    global->Set(context,
        v8::String::NewFromUtf8(isolate_, "unloadDll").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, UnloadDllFunc)->GetFunction(context).ToLocalChecked());
        
    global->Set(context,
        v8::String::NewFromUtf8(isolate_, "reloadDll").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, ReloadDllFunc)->GetFunction(context).ToLocalChecked());
        
    global->Set(context,
        v8::String::NewFromUtf8(isolate_, "listDlls").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, ListDllsFunc)->GetFunction(context).ToLocalChecked());
        
    // Register utility functions
    global->Set(context,
        v8::String::NewFromUtf8(isolate_, "quit").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, QuitFunc)->GetFunction(context).ToLocalChecked());
        
    global->Set(context,
        v8::String::NewFromUtf8(isolate_, "help").ToLocalChecked(),
        v8::FunctionTemplate::New(isolate_, HelpFunc)->GetFunction(context).ToLocalChecked());
}

void ClaudeConsole::Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!instance_) return;
    
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        if (first) {
            first = false;
        } else {
            instance_->Output(" ");
        }
        v8::String::Utf8Value str(args.GetIsolate(), args[i]);
        const char* cstr = *str ? *str : "<string conversion failed>";
        instance_->Output(cstr);
    }
    instance_->Output("\n");
}

void ClaudeConsole::Load(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!instance_ || args.Length() < 1) return;
    
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::String::Utf8Value file(args.GetIsolate(), args[0]);
    const char* filename = *file ? *file : "";
    
    bool success = instance_->ExecuteFile(filename);
    args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), success));
}

void ClaudeConsole::LoadDllFunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!instance_ || args.Length() < 1) return;
    
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::String::Utf8Value path(args.GetIsolate(), args[0]);
    const char* dllPath = *path ? *path : "";
    
    bool success = instance_->LoadDll(dllPath);
    args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), success));
}

void ClaudeConsole::UnloadDllFunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!instance_ || args.Length() < 1) return;
    
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::String::Utf8Value path(args.GetIsolate(), args[0]);
    const char* dllPath = *path ? *path : "";
    
    bool success = instance_->UnloadDll(dllPath);
    args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), success));
}

void ClaudeConsole::ReloadDllFunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!instance_ || args.Length() < 1) return;
    
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::String::Utf8Value path(args.GetIsolate(), args[0]);
    const char* dllPath = *path ? *path : "";
    
    bool success = instance_->ReloadDll(dllPath);
    args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), success));
}

void ClaudeConsole::ListDllsFunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!instance_) return;
    
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    
    auto dlls = instance_->GetLoadedDlls();
    v8::Local<v8::Array> result = v8::Array::New(isolate, dlls.size());
    
    for (size_t i = 0; i < dlls.size(); ++i) {
        v8::Local<v8::String> str = v8::String::NewFromUtf8(isolate, dlls[i].c_str()).ToLocalChecked();
        result->Set(context, i, str);
    }
    
    args.GetReturnValue().Set(result);
}

void ClaudeConsole::QuitFunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!instance_) return;
    instance_->Output("Goodbye!\n");
    // Note: Actual exit should be handled by the main loop
}

void ClaudeConsole::HelpFunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!instance_) return;
    
    instance_->Output("Available JavaScript functions:\n");
    instance_->Output("  print(...) - Print values to console\n");
    instance_->Output("  load(file) - Load and execute JavaScript file\n");
    instance_->Output("  loadDll(path) - Load a native DLL\n");
    instance_->Output("  unloadDll(path) - Unload a DLL\n");
    instance_->Output("  reloadDll(path) - Reload a DLL\n");
    instance_->Output("  listDlls() - List loaded DLLs\n");
    instance_->Output("  quit() - Exit console\n");
    instance_->Output("  help() - Show this help\n");
}

#else
// Stub implementations when V8 is not available
bool ClaudeConsole::ExecuteFile(const std::string& path) {
    Output(std::format("JavaScript file execution not available (V8 not built): {}\n", path));
    return false;
}

bool ClaudeConsole::ExecuteString([[maybe_unused]] const std::string& source, [[maybe_unused]] const std::string& name) {
    Output(std::format("JavaScript execution not available (V8 not built)\n"));
    return false;
}

bool ClaudeConsole::LoadDll(const std::string& path) {
    Output(std::format("DLL loading not available (V8 not built): {}\n", path));
    return false;
}

bool ClaudeConsole::UnloadDll(const std::string& path) {
    Output(std::format("DLL unloading not available (V8 not built): {}\n", path));
    return false;
}

bool ClaudeConsole::ReloadDll(const std::string& path) {
    Output(std::format("DLL reloading not available (V8 not built): {}\n", path));
    return false;
}

std::vector<std::string> ClaudeConsole::GetLoadedDlls() const {
    return {};
}
#endif

// CommandHistory implementation
CommandHistory::CommandHistory(size_t maxSize) 
    : maxSize_(maxSize), position_(-1) {
}

void CommandHistory::Add(const std::string& command) {
    if (command.empty()) return;
    
    // Don't add duplicates of the last command
    if (!history_.empty() && history_.back() == command) {
        return;
    }
    
    history_.push_back(command);
    
    // Maintain max size
    if (history_.size() > maxSize_) {
        history_.erase(history_.begin());
    }
    
    ResetPosition();
}

const std::string& CommandHistory::Get(size_t index) const {
    static const std::string empty;
    if (index >= history_.size()) {
        return empty;
    }
    return history_[index];
}

std::string CommandHistory::GetPrevious() {
    if (history_.empty()) return "";
    
    if (position_ == -1) {
        position_ = static_cast<int>(history_.size()) - 1;
    } else if (position_ > 0) {
        position_--;
    }
    
    return position_ >= 0 ? history_[position_] : "";
}

std::string CommandHistory::GetNext() {
    if (history_.empty() || position_ == -1) return "";
    
    if (position_ < static_cast<int>(history_.size()) - 1) {
        position_++;
        return history_[position_];
    } else {
        position_ = -1;
        return "";
    }
}

} // namespace cll