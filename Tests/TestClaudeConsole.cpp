#include <gtest/gtest.h>
#include "ClaudeConsole.h"
#include <chrono>
#include <thread>

using namespace cll;

class ClaudeConsoleTest : public ::testing::Test {
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

// Basic initialization and shutdown tests
TEST_F(ClaudeConsoleTest, InitializationTest) {
    EXPECT_TRUE(console != nullptr);
    EXPECT_EQ(console->GetMode(), ConsoleMode::Shell);
    EXPECT_FALSE(console->IsJavaScriptMode());
    EXPECT_FALSE(console->IsInMultiLineMode());
}

// Mode switching tests
TEST_F(ClaudeConsoleTest, ModeSwitchingTest) {
    // Test switching to JavaScript mode
    console->SetMode(ConsoleMode::JavaScript);
    EXPECT_EQ(console->GetMode(), ConsoleMode::JavaScript);
    EXPECT_TRUE(console->IsJavaScriptMode());
    
    // Test switching back to Shell mode
    console->SetMode(ConsoleMode::Shell);
    EXPECT_EQ(console->GetMode(), ConsoleMode::Shell);
    EXPECT_FALSE(console->IsJavaScriptMode());
}

// Built-in command tests
TEST_F(ClaudeConsoleTest, BuiltinCommandsTest) {
    EXPECT_TRUE(console->IsBuiltinCommand("help"));
    EXPECT_TRUE(console->IsBuiltinCommand("quit"));
    EXPECT_TRUE(console->IsBuiltinCommand("exit"));
    EXPECT_TRUE(console->IsBuiltinCommand("clear"));
    EXPECT_TRUE(console->IsBuiltinCommand("js"));
    EXPECT_TRUE(console->IsBuiltinCommand("javascript"));
    EXPECT_TRUE(console->IsBuiltinCommand("shell"));
    EXPECT_TRUE(console->IsBuiltinCommand("sh"));
    EXPECT_TRUE(console->IsBuiltinCommand("ask"));
    EXPECT_TRUE(console->IsBuiltinCommand("config"));
    EXPECT_TRUE(console->IsBuiltinCommand("reload"));
    
    EXPECT_FALSE(console->IsBuiltinCommand("nonexistent"));
    EXPECT_FALSE(console->IsBuiltinCommand(""));
    EXPECT_FALSE(console->IsBuiltinCommand("ls"));
}

// Command execution tests
TEST_F(ClaudeConsoleTest, CommandExecutionTest) {
    // Test empty command
    auto result = console->ExecuteCommand("");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.empty());
    EXPECT_TRUE(result.error.empty());
    
    // Test mode switch commands
    result = console->ExecuteCommand("js");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(console->GetMode(), ConsoleMode::JavaScript);
    EXPECT_EQ(result.output, "Switched to JavaScript mode");
    
    result = console->ExecuteCommand("shell");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(console->GetMode(), ConsoleMode::Shell);
    EXPECT_EQ(result.output, "Switched to Shell mode");
}

// JavaScript execution tests
TEST_F(ClaudeConsoleTest, JavaScriptExecutionTest) {
    auto result = console->ExecuteJavaScript("Math.sqrt(64)");
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.output.empty());
    EXPECT_TRUE(result.output.find("Math.sqrt(64)") != std::string::npos);
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_GT(result.executionTime.count(), 0);
}

// Shell command execution tests
TEST_F(ClaudeConsoleTest, ShellCommandExecutionTest) {
    auto result = console->ExecuteShellCommand("echo test");
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.executionTime.count(), 0);
    
    // Test command that should fail
    result = console->ExecuteShellCommand("nonexistent_command_12345");
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.exitCode, 0);
}

// Prompt management tests
TEST_F(ClaudeConsoleTest, PromptManagementTest) {
    // Test mode switching functionality
    console->SetMode(cll::ConsoleMode::JavaScript);
    EXPECT_TRUE(console->IsJavaScriptMode());
    
    console->SetMode(cll::ConsoleMode::Shell);
    EXPECT_FALSE(console->IsJavaScriptMode());
}

// Utility function tests
TEST_F(ClaudeConsoleTest, UtilityFunctionsTest) {
    // Test execution time formatting
    auto us1 = std::chrono::microseconds(500);
    auto formatted1 = ClaudeConsole::FormatExecutionTime(us1);
    EXPECT_TRUE(formatted1.find("μs") != std::string::npos);
    
    auto us2 = std::chrono::microseconds(2500);
    auto formatted2 = ClaudeConsole::FormatExecutionTime(us2);
    EXPECT_TRUE(formatted2.find("ms") != std::string::npos);
    
    auto us3 = std::chrono::microseconds(2500000);
    auto formatted3 = ClaudeConsole::FormatExecutionTime(us3);
    EXPECT_TRUE(formatted3.find("s") != std::string::npos);
    
    // Test command splitting
    auto words1 = ClaudeConsole::SplitCommand("hello world test");
    EXPECT_EQ(words1.size(), 3);
    EXPECT_EQ(words1[0], "hello");
    EXPECT_EQ(words1[1], "world");
    EXPECT_EQ(words1[2], "test");
    
    auto words2 = ClaudeConsole::SplitCommand("");
    EXPECT_EQ(words2.size(), 0);
    
    auto words3 = ClaudeConsole::SplitCommand("single");
    EXPECT_EQ(words3.size(), 1);
    EXPECT_EQ(words3[0], "single");
}

// Ask mode functionality tests
TEST_F(ClaudeConsoleTest, AskModeBasicTest) {
    // Test IsAskMode method when not in ask mode
    EXPECT_FALSE(console->IsAskMode());
    
    // Test switching to Ask console mode
    console->SetMode(ConsoleMode::Ask);
    EXPECT_TRUE(console->IsAskMode());
    
    // Test switching back to Shell mode
    console->SetMode(ConsoleMode::Shell);
    EXPECT_FALSE(console->IsAskMode());
    
    // Test multiline ask mode (doesn't affect IsAskMode - that only checks console mode)
    console->StartMultiLineMode(MultiLineMode::Ask);
    EXPECT_FALSE(console->IsAskMode()); // Still false because console mode is Shell
    EXPECT_TRUE(console->IsInMultiLineMode());
    EXPECT_EQ(console->GetMultiLineMode(), MultiLineMode::Ask);
    
    // End multiline mode
    console->EndMultiLineMode();
    EXPECT_FALSE(console->IsInMultiLineMode());
}

// Claude query execution tests
TEST_F(ClaudeConsoleTest, ClaudeQueryExecutionTest) {
    // Test capital questions
    auto result = console->ExecuteClaudeQuery("what is capital of canada");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Ottawa");
    EXPECT_EQ(result.exitCode, 0);
    EXPECT_GE(result.executionTime.count(), 0);
    
    result = console->ExecuteClaudeQuery("what is capital of france");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Paris");
    
    result = console->ExecuteClaudeQuery("what is capital of japan");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Tokyo");
    
    result = console->ExecuteClaudeQuery("what is capital of usa");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Washington, D.C.");
    
    result = console->ExecuteClaudeQuery("what is capital of uk");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "London");
}

// Test case insensitive matching
TEST_F(ClaudeConsoleTest, ClaudeQueryCaseInsensitiveTest) {
    auto result = console->ExecuteClaudeQuery("WHAT IS CAPITAL OF CANADA");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Ottawa");
    
    result = console->ExecuteClaudeQuery("What Is Capital Of France");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Paris");
    
    result = console->ExecuteClaudeQuery("what is CAPITAL of JAPAN");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Tokyo");
}

// Test greeting functionality
TEST_F(ClaudeConsoleTest, ClaudeQueryGreetingsTest) {
    auto result = console->ExecuteClaudeQuery("hello");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Hello! How can I help you today?");
    
    result = console->ExecuteClaudeQuery("hi");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Hello! How can I help you today?");
    
    result = console->ExecuteClaudeQuery("HELLO there");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Hello! How can I help you today?");
}

// Test time/date functionality
TEST_F(ClaudeConsoleTest, ClaudeQueryTimeTest) {
    auto result = console->ExecuteClaudeQuery("what time is it");
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.output.empty());
    EXPECT_GT(result.output.length(), 10); // Should contain date/time string
    
    result = console->ExecuteClaudeQuery("current date");
    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.output.empty());
}

// Test help functionality
TEST_F(ClaudeConsoleTest, ClaudeQueryHelpTest) {
    auto result = console->ExecuteClaudeQuery("help");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("World capitals") != std::string::npos);
    EXPECT_TRUE(result.output.find("Current time and date") != std::string::npos);
    EXPECT_TRUE(result.output.find("Basic greetings") != std::string::npos);
}

// Test unknown query handling
TEST_F(ClaudeConsoleTest, ClaudeQueryUnknownTest) {
    auto result = console->ExecuteClaudeQuery("what is the meaning of life");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("don't have built-in knowledge") != std::string::npos);
    EXPECT_TRUE(result.output.find("world capitals") != std::string::npos);
}

// Test ask command via built-in command system
TEST_F(ClaudeConsoleTest, AskCommandTest) {
    auto result = console->ExecuteBuiltinCommand("ask what is capital of canada");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Ottawa");
    
    result = console->ExecuteBuiltinCommand("ask hello");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Hello! How can I help you today?");
    
    // Test ask command without question
    result = console->ExecuteBuiltinCommand("ask");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error, "Usage: ask <question>");
    EXPECT_EQ(result.exitCode, 1);
}

// Test ask functionality via ExecuteCommand with ? prefix
TEST_F(ClaudeConsoleTest, AskPrefixTest) {
    auto result = console->ExecuteCommand("?what is capital of canada");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Ottawa");
    
    result = console->ExecuteCommand("?hello");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Hello! How can I help you today?");
    
    // Test edge case with just ? - should switch to ask mode
    result = console->ExecuteCommand("?");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Switched to Ask mode");
}

// Test ExecuteAsk method delegation
TEST_F(ClaudeConsoleTest, ExecuteAskDelegationTest) {
    auto result = console->ExecuteAsk("what is capital of canada");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Ottawa");
    
    result = console->ExecuteAsk("hello");
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.output, "Hello! How can I help you today?");
}

// Test help command includes ask functionality
TEST_F(ClaudeConsoleTest, HelpCommandIncludesAskTest) {
    auto result = console->ExecuteBuiltinCommand("help");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("ask - Ask Claude AI a question") != std::string::npos);
    EXPECT_TRUE(result.output.find("?<question> - Ask Claude AI a question") != std::string::npos);
}