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