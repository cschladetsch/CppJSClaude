#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <functional>

// V8 integration (conditional)
#ifdef HAS_V8
#include <v8.h>
#endif

namespace cll {

#ifdef HAS_V8
// Forward declaration for DLL loader
class DllLoader;
#endif

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
    JavaScript,
    Ask
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
    CommandResult ExecuteAsk(const std::string& question);
    CommandResult ExecuteShellCommand(const std::string& command);
    CommandResult ExecuteClaudeQuery(const std::string& question);
    
    // V8 JavaScript execution
    bool ExecuteFile(const std::string& path);
    bool ExecuteString(const std::string& source, const std::string& name = "<eval>");
    
    // DLL loading
    bool LoadDll(const std::string& path);
    bool UnloadDll(const std::string& path);
    bool ReloadDll(const std::string& path);
    std::vector<std::string> GetLoadedDlls() const;
    
    // Mode management
    void SetMode(ConsoleMode mode) { mode_ = mode; }
    ConsoleMode GetMode() const { return mode_; }
    bool IsJavaScriptMode() const { return mode_ == ConsoleMode::JavaScript; }
    bool IsAskMode() const { return mode_ == ConsoleMode::Ask; }
    
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
    
#ifdef HAS_V8
    // V8 JavaScript engine
    std::unique_ptr<v8::Platform> platform_;
    v8::Isolate* isolate_;
    v8::Persistent<v8::Context> context_;
    
    // DLL loader for hot-loading native libraries
    std::unique_ptr<DllLoader> dllLoader_;
    
    // V8 helper methods
    bool CompileAndRun(const std::string& source, const std::string& name);
    std::string ReadFile(const std::string& path);
    void ReportException(v8::TryCatch* tryCatch);
    void RegisterBuiltins(v8::Local<v8::Context> context);
    void PrintResult(v8::Local<v8::Value> value);
    
    // V8 built-in functions
    static void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Load(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void LoadDllFunc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void UnloadDllFunc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ReloadDllFunc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ListDllsFunc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void QuitFunc(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void HelpFunc(const v8::FunctionCallbackInfo<v8::Value>& args);
    
    // Static instance for V8 callbacks
    static ClaudeConsole* instance_;
#endif
    
public:
    // Claude integration helpers
    bool CheckClaudeAvailability();
    std::string FindPyClaudeCliPath();
    CommandResult ExecuteSubprocess(const std::string& command);
    
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

} // namespace cll