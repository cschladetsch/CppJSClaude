#include <gtest/gtest.h>
#include "ClaudeConsole.h"

using namespace cll;

class CommandExecutionTest : public ::testing::Test {
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

// Test JavaScript execution with & prefix
TEST_F(CommandExecutionTest, JavaScriptPrefixExecution) {
    // Test immediate execution with content
    auto result = console->ExecuteCommand("&Math.sqrt(64)");
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.output.empty());
    EXPECT_TRUE(result.output.find("Math.sqrt(64)") != std::string::npos);
    EXPECT_FALSE(console->IsInMultiLineMode());
}

// Test JavaScript execution with & alone (should enter multi-line mode)
TEST_F(CommandExecutionTest, JavaScriptMultiLineMode) {
    auto result = console->ExecuteCommand("&");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::JavaScript);
    EXPECT_TRUE(result.output.find("Multi-line JavaScript mode") != std::string::npos);
}

// Test ask command with content (immediate execution)
TEST_F(CommandExecutionTest, AskImmediateExecution) {
    auto result = console->ExecuteCommand("ask What is 2+2?");
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(console->IsInMultiLineMode());
    // Note: This test doesn't verify Claude AI response as it may not be available
}

// Test ask command alone (should enter multi-line mode)
TEST_F(CommandExecutionTest, AskMultiLineMode) {
    auto result = console->ExecuteCommand("ask");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::Ask);
    EXPECT_TRUE(result.output.find("Multi-line ask mode") != std::string::npos);
}

// Test shell command execution
TEST_F(CommandExecutionTest, ShellCommandExecution) {
    auto result = console->ExecuteCommand("echo hello");
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.executionTime.count(), 0);
}

// Test built-in command execution
TEST_F(CommandExecutionTest, BuiltinCommandExecution) {
    auto result = console->ExecuteCommand("help");
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.output.empty());
    EXPECT_TRUE(result.output.find("Available commands") != std::string::npos);
    
    result = console->ExecuteCommand("clear");
    EXPECT_TRUE(result.success);
    
    result = console->ExecuteCommand("config");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("Configuration directory") != std::string::npos);
    
    result = console->ExecuteCommand("reload");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("Configuration reloaded") != std::string::npos);
}

// Test error handling
TEST_F(CommandExecutionTest, ErrorHandling) {
    // Test non-existent command
    auto result = console->ExecuteCommand("nonexistent_command_12345");
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.exitCode, 0);
    EXPECT_GT(result.executionTime.count(), 0);
}

// Test command timing
TEST_F(CommandExecutionTest, CommandTiming) {
    auto result = console->ExecuteCommand("sleep 0.1");
    EXPECT_TRUE(result.success);
    // Should take at least 100ms (100,000 microseconds)
    EXPECT_GT(result.executionTime.count(), 50000); // Allow some tolerance
}

// Test complex JavaScript expressions
TEST_F(CommandExecutionTest, ComplexJavaScriptExecution) {
    auto result = console->ExecuteCommand("&new Date().getTime()");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("new Date().getTime()") != std::string::npos);
    
    result = console->ExecuteCommand("&[1,2,3,4,5].reduce((a,b) => a+b, 0)");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("reduce") != std::string::npos);
}

// Test whitespace handling
TEST_F(CommandExecutionTest, WhitespaceHandling) {
    auto result = console->ExecuteCommand("  help  ");
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.output.empty());
    
    result = console->ExecuteCommand("\t\tjs\t\t");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(console->GetMode(), ConsoleMode::JavaScript);
}