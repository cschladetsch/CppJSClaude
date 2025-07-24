#include <gtest/gtest.h>
#include "ClaudeConsole.h"

using namespace cll;

class PromptManagementTest : public ::testing::Test {
protected:
    void SetUp() override {
        console = std::make_unique<ClaudeConsole>();
        ASSERT_TRUE(console->Initialize());
    }
    
    void TearDown() override {
        console->Shutdown();
        console.reset();
    }
    
    std::unique_ptr<ClaudeConsole> console;
};

// Test default prompt format
TEST_F(PromptManagementTest, DefaultPromptFormat) {
    std::string prompt = console->GetPrompt();
    EXPECT_FALSE(prompt.empty());
    EXPECT_TRUE(prompt.find("❯") != std::string::npos);
}

// Test Claude prompt format
TEST_F(PromptManagementTest, ClaudePromptFormat) {
    std::string claudePrompt = console->GetClaudePrompt();
    EXPECT_FALSE(claudePrompt.empty());
    EXPECT_TRUE(claudePrompt.find("?") != std::string::npos);
}

// Test prompt changes with mode switching
TEST_F(PromptManagementTest, PromptModeSwitch) {
    // Shell mode prompt
    console->SetMode(ConsoleMode::Shell);
    std::string shellPrompt = console->GetPrompt();
    EXPECT_FALSE(shellPrompt.empty());
    
    // JavaScript mode prompt  
    console->SetMode(ConsoleMode::JavaScript);
    std::string jsPrompt = console->GetPrompt();
    EXPECT_FALSE(jsPrompt.empty());
    EXPECT_NE(shellPrompt, jsPrompt);
    EXPECT_TRUE(jsPrompt.find("js") != std::string::npos);
}

// Test multi-line prompt changes
TEST_F(PromptManagementTest, MultiLinePromptChanges) {
    std::string normalPrompt = console->GetPrompt();
    
    // JavaScript multi-line mode
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    std::string jsMultiPrompt = console->GetPrompt();
    EXPECT_NE(normalPrompt, jsMultiPrompt);
    EXPECT_TRUE(jsMultiPrompt.find("js") != std::string::npos);
    
    console->EndMultiLineMode();
    
    // Ask multi-line mode
    console->StartMultiLineMode(MultiLineMode::Ask);
    std::string askMultiPrompt = console->GetPrompt();
    EXPECT_NE(normalPrompt, askMultiPrompt);
    EXPECT_NE(jsMultiPrompt, askMultiPrompt);
}

// Test multi-line prompt methods directly
TEST_F(PromptManagementTest, MultiLinePromptMethods) {
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    std::string jsPrompt = console->GetMultiLinePrompt();
    EXPECT_FALSE(jsPrompt.empty());
    EXPECT_TRUE(jsPrompt.find("js") != std::string::npos);
    
    console->StartMultiLineMode(MultiLineMode::Ask);
    std::string askPrompt = console->GetMultiLinePrompt();
    EXPECT_FALSE(askPrompt.empty());
    EXPECT_NE(jsPrompt, askPrompt);
}

// Test prompt consistency
TEST_F(PromptManagementTest, PromptConsistency) {
    // Multiple calls should return same prompt
    std::string prompt1 = console->GetPrompt();
    std::string prompt2 = console->GetPrompt();
    EXPECT_EQ(prompt1, prompt2);
    
    std::string claudePrompt1 = console->GetClaudePrompt();
    std::string claudePrompt2 = console->GetClaudePrompt();
    EXPECT_EQ(claudePrompt1, claudePrompt2);
}

// Test prompt format with mode switching
TEST_F(PromptManagementTest, PromptFormatWithModeSwitch) {
    // Test shell mode
    console->SetMode(ConsoleMode::Shell);
    std::string shellPrompt = console->GetPrompt();
    EXPECT_FALSE(shellPrompt.empty());
    
    // Test JavaScript mode
    console->SetMode(ConsoleMode::JavaScript);
    std::string jsPrompt = console->GetPrompt();
    EXPECT_FALSE(jsPrompt.empty());
    EXPECT_NE(shellPrompt, jsPrompt);
    
    // Test switching back
    console->SetMode(ConsoleMode::Shell);
    std::string shellPrompt2 = console->GetPrompt();
    EXPECT_EQ(shellPrompt, shellPrompt2);
}

// Test prompt in various states
TEST_F(PromptManagementTest, PromptInVariousStates) {
    // Normal state
    std::string normalPrompt = console->GetPrompt();
    EXPECT_FALSE(normalPrompt.empty());
    
    // JavaScript mode
    console->SetMode(ConsoleMode::JavaScript);
    std::string jsPrompt = console->GetPrompt();
    EXPECT_NE(normalPrompt, jsPrompt);
    
    // Multi-line JavaScript mode
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    std::string jsMultiPrompt = console->GetPrompt();
    EXPECT_NE(jsPrompt, jsMultiPrompt);
    
    // Back to shell mode
    console->EndMultiLineMode();
    console->SetMode(ConsoleMode::Shell);
    std::string backToShell = console->GetPrompt();
    EXPECT_EQ(normalPrompt, backToShell);
}