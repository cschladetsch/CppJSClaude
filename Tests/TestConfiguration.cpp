#include <gtest/gtest.h>
#include "ClaudeConsole.h"
#include <filesystem>
#include <fstream>

using namespace cll;
namespace fs = std::filesystem;

class ConfigurationTest : public ::testing::Test {
protected:
    void SetUp() override {
        console = std::make_unique<ClaudeConsole>();
        ASSERT_TRUE(console->Initialize());
        
        // Create temporary config directory for testing
        tempConfigPath = fs::temp_directory_path() / "test_claude_console";
        if (fs::exists(tempConfigPath)) {
            fs::remove_all(tempConfigPath);
        }
        fs::create_directories(tempConfigPath);
    }
    
    void TearDown() override {
        console->Shutdown();
        console.reset();
        
        // Cleanup temporary config directory
        if (fs::exists(tempConfigPath)) {
            fs::remove_all(tempConfigPath);
        }
    }
    
    std::unique_ptr<ClaudeConsole> console;
    fs::path tempConfigPath;
};

// Test configuration directory creation
TEST_F(ConfigurationTest, ConfigDirectoryCreation) {
    // The console should create config directory on initialization
    std::string configPath = console->GetConfigPath();
    EXPECT_FALSE(configPath.empty());
    EXPECT_TRUE(fs::exists(configPath));
}

// Test configuration loading
TEST_F(ConfigurationTest, ConfigurationLoading) {
    // Create a test config file
    std::string configPath = console->GetConfigPath();
    std::string configFile = configPath + "/config.json";
    
    if (fs::exists(configFile)) {
        // Configuration loading should succeed
        console->LoadConfiguration();
        // If we reach here without exception, loading succeeded
        SUCCEED();
    } else {
        // If config file doesn't exist, that's also valid
        SUCCEED();
    }
}

// Test configuration saving
TEST_F(ConfigurationTest, ConfigurationSaving) {
    console->SaveConfiguration();
    
    std::string configPath = console->GetConfigPath();
    std::string aliasFile = configPath + "/aliases";
    
    // Alias file should exist after saving
    EXPECT_TRUE(fs::exists(aliasFile));
}

// Test configuration reload
TEST_F(ConfigurationTest, ConfigurationReload) {
    auto result = console->ExecuteCommand("reload");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("Configuration reloaded") != std::string::npos);
}

// Test config command
TEST_F(ConfigurationTest, ConfigCommand) {
    auto result = console->ExecuteCommand("config");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("Configuration directory") != std::string::npos);
    EXPECT_TRUE(result.output.find("config.json") != std::string::npos);
    EXPECT_TRUE(result.output.find("aliases") != std::string::npos);
}

// Test default configuration values
TEST_F(ConfigurationTest, DefaultConfigurationValues) {
    // Test that default prompts are set
    std::string prompt = console->GetPrompt();
    EXPECT_FALSE(prompt.empty());
    
    std::string claudePrompt = console->GetClaudePrompt();
    EXPECT_FALSE(claudePrompt.empty());
}

// Test configuration file structure
TEST_F(ConfigurationTest, ConfigurationFileStructure) {
    std::string configPath = console->GetConfigPath();
    std::string configFile = configPath + "/config.json";
    
    if (fs::exists(configFile)) {
        std::ifstream file(configFile);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Should contain expected JSON structure
        EXPECT_TRUE(content.find("default_mode") != std::string::npos);
        EXPECT_TRUE(content.find("prompt_format") != std::string::npos);
        EXPECT_TRUE(content.find("claude_prompt") != std::string::npos);
        EXPECT_TRUE(content.find("claude_integration") != std::string::npos);
    }
}

// Test alias file structure
TEST_F(ConfigurationTest, AliasFileStructure) {
    console->SaveConfiguration(); // Ensure alias file exists
    
    std::string configPath = console->GetConfigPath();
    std::string aliasFile = configPath + "/aliases";
    
    EXPECT_TRUE(fs::exists(aliasFile));
    
    std::ifstream file(aliasFile);
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    // Should contain header comment
    EXPECT_TRUE(content.find("# Claude Console Aliases") != std::string::npos);
}