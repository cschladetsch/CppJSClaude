#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>

namespace config_prompt {

// Configuration structure
struct Configuration {
    std::string defaultMode;
    std::string promptFormat;
    std::string claudePrompt;
    std::string claudePromptColor;
    bool showExecutionTime;
    size_t historySize;
    bool enableColors;
    
    struct ClaudeIntegration {
        bool enabled;
        int timeoutSeconds;
        std::string apiKey;
        std::string model;
    } claudeIntegration;
    
    std::map<std::string, std::string> aliases;
    std::map<std::string, std::string> customPrompts;
};

// Prompt template structure
struct PromptTemplate {
    std::string name;
    std::string template_;
    std::string description;
    std::map<std::string, std::string> variables;
};

// Configuration manager interface
class IConfigManager {
public:
    virtual ~IConfigManager() = default;
    
    // Configuration management
    virtual bool LoadConfiguration() = 0;
    virtual bool SaveConfiguration() = 0;
    virtual Configuration GetConfiguration() const = 0;
    virtual void SetConfiguration(const Configuration& config) = 0;
    
    // Path management
    virtual std::string GetConfigPath() const = 0;
    virtual void SetConfigPath(const std::string& path) = 0;
    virtual bool CreateConfigDirectory() = 0;
    
    // Alias management
    virtual void SetAlias(const std::string& name, const std::string& value) = 0;
    virtual std::string GetAlias(const std::string& name) const = 0;
    virtual bool RemoveAlias(const std::string& name) = 0;
    virtual std::map<std::string, std::string> GetAllAliases() const = 0;
    virtual std::string ExpandAlias(const std::string& command) const = 0;
};

// Prompt manager interface
class IPromptManager {
public:
    virtual ~IPromptManager() = default;
    
    // Prompt template management
    virtual bool LoadPromptTemplates() = 0;
    virtual bool SavePromptTemplates() = 0;
    virtual void AddPromptTemplate(const PromptTemplate& template_) = 0;
    virtual bool RemovePromptTemplate(const std::string& name) = 0;
    virtual PromptTemplate GetPromptTemplate(const std::string& name) const = 0;
    virtual std::vector<PromptTemplate> GetAllPromptTemplates() const = 0;
    
    // Prompt generation
    virtual std::string GeneratePrompt(const std::string& templateName, 
                                     const std::map<std::string, std::string>& variables = {}) const = 0;
    virtual std::string FormatPrompt(const std::string& format, 
                                   const std::map<std::string, std::string>& variables) const = 0;
    
    // Built-in prompt utilities
    virtual std::string GetShellPrompt(const std::string& mode = "shell") const = 0;
    virtual std::string GetJavaScriptPrompt() const = 0;
    virtual std::string GetClaudePrompt() const = 0;
    virtual std::string GetMultiLinePrompt(const std::string& mode) const = 0;
};

// Combined manager interface
class IConfigPromptManager : public IConfigManager, public IPromptManager {
public:
    virtual ~IConfigPromptManager() = default;
    
    // Initialization
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    // Event callbacks
    using ConfigChangedCallback = std::function<void(const Configuration&)>;
    using PromptChangedCallback = std::function<void(const std::string&)>;
    
    virtual void SetConfigChangedCallback(ConfigChangedCallback callback) = 0;
    virtual void SetPromptChangedCallback(PromptChangedCallback callback) = 0;
};

// Factory function
std::unique_ptr<IConfigPromptManager> CreateConfigPromptManager(const std::string& configPath = "");

// Utility functions
namespace utils {
    std::string GetDefaultConfigPath();
    std::string ExpandPath(const std::string& path);
    bool FileExists(const std::string& path);
    bool CreateDirectories(const std::string& path);
    std::vector<std::string> SplitCommand(const std::string& command);
    std::string JoinPath(const std::string& base, const std::string& path);
}

} // namespace config_prompt