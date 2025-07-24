#include <gtest/gtest.h>
#include "ClaudeConsole.h"

using namespace cll;

class MultiLineModeTest : public ::testing::Test {
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

// Test multi-line mode initialization
TEST_F(MultiLineModeTest, MultiLineModeInitialization) {
    EXPECT_FALSE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::None);
}

// Test JavaScript multi-line mode start
TEST_F(MultiLineModeTest, JavaScriptMultiLineModeStart) {
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    EXPECT_TRUE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::JavaScript);
}

// Test Ask multi-line mode start
TEST_F(MultiLineModeTest, AskMultiLineModeStart) {
    console->StartMultiLineMode(MultiLineMode::Ask);
    EXPECT_TRUE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::Ask);
}

// Test multi-line mode end
TEST_F(MultiLineModeTest, MultiLineModeEnd) {
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    EXPECT_TRUE(console->IsInMultiLineMode());
    
    console->EndMultiLineMode();
    EXPECT_FALSE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::None);
}

// Test multi-line input appending
TEST_F(MultiLineModeTest, MultiLineInputAppending) {
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    
    console->AppendMultiLineInput("console.log('line 1');");
    console->AppendMultiLineInput("const x = 42;");
    console->AppendMultiLineInput("console.log('Answer:', x);");
    
    EXPECT_TRUE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::JavaScript);
}

// Test multi-line JavaScript execution
TEST_F(MultiLineModeTest, MultiLineJavaScriptExecution) {
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    console->AppendMultiLineInput("console.log('Hello');");
    console->AppendMultiLineInput("Math.PI * 2");
    
    auto result = console->ExecuteMultiLineInput();
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.output.empty());
    EXPECT_TRUE(result.output.find("console.log('Hello')") != std::string::npos);
    EXPECT_TRUE(result.output.find("Math.PI * 2") != std::string::npos);
    
    // Should exit multi-line mode after execution
    EXPECT_FALSE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::None);
}

// Test multi-line Ask execution
TEST_F(MultiLineModeTest, MultiLineAskExecution) {
    console->StartMultiLineMode(MultiLineMode::Ask);
    console->AppendMultiLineInput("What is JavaScript?");
    console->AppendMultiLineInput("Please provide a brief explanation.");
    
    auto result = console->ExecuteMultiLineInput();
    EXPECT_TRUE(result.success);
    // Note: We don't test Claude AI response content as it may not be available
    
    // Should exit multi-line mode after execution
    EXPECT_FALSE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::None);
}

// Test prompt changes in multi-line mode
TEST_F(MultiLineModeTest, MultiLinePromptChanges) {
    // Normal prompt
    std::string normalPrompt = console->GetPrompt();
    EXPECT_FALSE(normalPrompt.empty());
    
    // JavaScript multi-line prompt
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    std::string jsPrompt = console->GetPrompt();
    EXPECT_FALSE(jsPrompt.empty());
    EXPECT_NE(jsPrompt, normalPrompt);
    EXPECT_TRUE(jsPrompt.find("js") != std::string::npos);
    
    console->EndMultiLineMode();
    
    // Ask multi-line prompt
    console->StartMultiLineMode(MultiLineMode::Ask);
    std::string askPrompt = console->GetPrompt();
    EXPECT_FALSE(askPrompt.empty());
    EXPECT_NE(askPrompt, normalPrompt);
    EXPECT_NE(askPrompt, jsPrompt);
}

// Test multi-line mode switching
TEST_F(MultiLineModeTest, MultiLineModeSwitching) {
    // Start with JavaScript mode
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::JavaScript);
    
    // Switch to Ask mode
    console->StartMultiLineMode(MultiLineMode::Ask);
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::Ask);
    
    // End mode
    console->EndMultiLineMode();
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::None);
}

// Test error handling in multi-line mode
TEST_F(MultiLineModeTest, MultiLineErrorHandling) {
    // Try to execute multi-line input when not in multi-line mode
    auto result = console->ExecuteMultiLineInput();
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error.empty());
    EXPECT_TRUE(result.error.find("Not in multi-line mode") != std::string::npos);
}

// Test empty multi-line input handling
TEST_F(MultiLineModeTest, EmptyMultiLineInput) {
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    
    // Execute without adding any input
    auto result = console->ExecuteMultiLineInput();
    EXPECT_TRUE(result.success); // Empty input should still succeed
    
    EXPECT_FALSE(console->IsInMultiLineMode());
}

// Test multi-line input with empty lines
TEST_F(MultiLineModeTest, MultiLineInputWithEmptyLines) {
    console->StartMultiLineMode(MultiLineMode::JavaScript);
    
    console->AppendMultiLineInput("console.log('First line');");
    console->AppendMultiLineInput(""); // Empty line
    console->AppendMultiLineInput("console.log('Third line');");
    
    auto result = console->ExecuteMultiLineInput();
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("First line") != std::string::npos);
    EXPECT_TRUE(result.output.find("Third line") != std::string::npos);
}