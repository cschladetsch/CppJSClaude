#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <functional>

namespace claude_console {

// Command result structure
struct CommandResult {
    bool success;
    std::string output;
    std::string error;
    std::chrono::microseconds executionTime;
    int exitCode;
};

// Console mode
enum class ConsoleMode {
    Shell,
    JavaScript
};

// Multi-line input mode
enum class MultiLineMode {
    None,
    JavaScript,
    Ask
};

// Main console class with Claude AI integration
class ClaudeConsole {
public:
    ClaudeConsole();
    virtual ~ClaudeConsole();

    // Initialize console
    bool Initialize();
    void Shutdown();

    // Execute commands
    CommandResult ExecuteCommand(const std::string& command);
    CommandResult ExecuteJavaScript(const std::string& code);
    CommandResult ExecuteShellCommand(const std::string& command);
    CommandResult ExecuteClaudeQuery(const std::string& question);
    
    // Mode management
    void SetMode(ConsoleMode mode) { mode_ = mode; }
    ConsoleMode GetMode() const { return mode_; }
    bool IsJavaScriptMode() const { return mode_ == ConsoleMode::JavaScript; }
    bool IsAskMode() const { return multiLineMode_ == MultiLineMode::Ask; }
    
    // Multi-line input management
    bool IsInMultiLineMode() const { return multiLineMode_ != MultiLineMode::None; }
    MultiLineMode GetMultiLineMode() const { return multiLineMode_; }
    void StartMultiLineMode(MultiLineMode mode);
    void EndMultiLineMode();
    void AppendMultiLineInput(const std::string& line);
    CommandResult ExecuteMultiLineInput();
    
    // Built-in commands
    bool IsBuiltinCommand(const std::string& command) const;
    CommandResult ExecuteBuiltinCommand(const std::string& command);
    
    // Utilities
    static std::string FormatExecutionTime(const std::chrono::microseconds& us);
    static std::vector<std::string> SplitCommand(const std::string& command);
    
    // Output callbacks
    using OutputCallback = std::function<void(const std::string&)>;
    void SetOutputCallback(OutputCallback callback) { outputCallback_ = callback; }
    void SetErrorCallback(OutputCallback callback) { errorCallback_ = callback; }
    
    // Configuration management
    void CreateConfigDirectory();
    void LoadConfiguration();
    void LoadSharedConfiguration();
    void SaveConfiguration();
    std::string GetConfigPath() const;
    std::string GetSharedConfigPath() const;
    
    // Alias management
    void SetAlias(const std::string& name, const std::string& value);
    std::string ExpandAlias(const std::string& command) const;
    
    // Prompt management
    std::string GetPrompt() const;
    std::string GetMultiLinePrompt() const;
    std::string GetClaudePrompt() const;

protected:
    // Output handling
    void Output(const std::string& text);
    void Error(const std::string& text);

private:
    ConsoleMode mode_;
    MultiLineMode multiLineMode_;
    std::string multiLineBuffer_;
    std::map<std::string, std::string> builtinCommands_;
    std::map<std::string, std::string> aliases_;
    
    // Configuration
    std::string promptFormat_;
    std::string claudePrompt_;
    std::string claudePromptColor_;
    
    OutputCallback outputCallback_;
    OutputCallback errorCallback_;
    
    // Claude integration helpers
    bool CheckClaudeAvailability();
    std::string FindPyClaudeCliPath();
    CommandResult ExecuteSubprocess(const std::string& command);
};

// History management
class CommandHistory {
public:
    CommandHistory(size_t maxSize = 1000);
    
    void Add(const std::string& command);
    const std::string& Get(size_t index) const;
    size_t Size() const { return history_.size(); }
    
    // Navigation
    void ResetPosition() { position_ = -1; }
    std::string GetPrevious();
    std::string GetNext();
    
private:
    std::vector<std::string> history_;
    size_t maxSize_;
    int position_;
};

} // namespace claude_console