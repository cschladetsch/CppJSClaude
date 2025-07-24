#include <gtest/gtest.h>
#include "ClaudeConsole.h"
#include <chrono>

using namespace cll;

class UtilitiesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // No need for console instance for utility tests
    }
    
    void TearDown() override {
        // No cleanup needed
    }
};

// Test execution time formatting for microseconds
TEST_F(UtilitiesTest, FormatExecutionTimeMicroseconds) {
    auto us = std::chrono::microseconds(500);
    std::string formatted = ClaudeConsole::FormatExecutionTime(us);
    EXPECT_EQ(formatted, "500μs");
    
    auto us2 = std::chrono::microseconds(999);
    std::string formatted2 = ClaudeConsole::FormatExecutionTime(us2);
    EXPECT_EQ(formatted2, "999μs");
}

// Test execution time formatting for milliseconds
TEST_F(UtilitiesTest, FormatExecutionTimeMilliseconds) {
    auto us = std::chrono::microseconds(1500);
    std::string formatted = ClaudeConsole::FormatExecutionTime(us);
    EXPECT_EQ(formatted, "1.5ms");
    
    auto us2 = std::chrono::microseconds(10000);
    std::string formatted2 = ClaudeConsole::FormatExecutionTime(us2);
    EXPECT_EQ(formatted2, "10.0ms");
    
    auto us3 = std::chrono::microseconds(999999);
    std::string formatted3 = ClaudeConsole::FormatExecutionTime(us3);
    EXPECT_EQ(formatted3, "1000.0ms");
}

// Test execution time formatting for seconds
TEST_F(UtilitiesTest, FormatExecutionTimeSeconds) {
    auto us = std::chrono::microseconds(1500000);
    std::string formatted = ClaudeConsole::FormatExecutionTime(us);
    EXPECT_EQ(formatted, "1.50s");
    
    auto us2 = std::chrono::microseconds(5000000);
    std::string formatted2 = ClaudeConsole::FormatExecutionTime(us2);
    EXPECT_EQ(formatted2, "5.00s");
    
    auto us3 = std::chrono::microseconds(12345678);
    std::string formatted3 = ClaudeConsole::FormatExecutionTime(us3);
    EXPECT_EQ(formatted3, "12.35s");
}

// Test edge cases for execution time formatting
TEST_F(UtilitiesTest, FormatExecutionTimeEdgeCases) {
    // Zero time
    auto us0 = std::chrono::microseconds(0);
    std::string formatted0 = ClaudeConsole::FormatExecutionTime(us0);
    EXPECT_EQ(formatted0, "0μs");
    
    // Exactly 1ms
    auto us1ms = std::chrono::microseconds(1000);
    std::string formatted1ms = ClaudeConsole::FormatExecutionTime(us1ms);
    EXPECT_EQ(formatted1ms, "1.0ms");
    
    // Exactly 1s
    auto us1s = std::chrono::microseconds(1000000);
    std::string formatted1s = ClaudeConsole::FormatExecutionTime(us1s);
    EXPECT_EQ(formatted1s, "1.00s");
}

// Test command splitting with normal input
TEST_F(UtilitiesTest, SplitCommandNormal) {
    auto words = ClaudeConsole::SplitCommand("hello world test");
    ASSERT_EQ(words.size(), 3);
    EXPECT_EQ(words[0], "hello");
    EXPECT_EQ(words[1], "world");
    EXPECT_EQ(words[2], "test");
    
    auto words2 = ClaudeConsole::SplitCommand("git log --oneline");
    ASSERT_EQ(words2.size(), 3);
    EXPECT_EQ(words2[0], "git");
    EXPECT_EQ(words2[1], "log");
    EXPECT_EQ(words2[2], "--oneline");
}

// Test command splitting with single word
TEST_F(UtilitiesTest, SplitCommandSingleWord) {
    auto words = ClaudeConsole::SplitCommand("help");
    ASSERT_EQ(words.size(), 1);
    EXPECT_EQ(words[0], "help");
    
    auto words2 = ClaudeConsole::SplitCommand("quit");
    ASSERT_EQ(words2.size(), 1);
    EXPECT_EQ(words2[0], "quit");
}

// Test command splitting with empty string
TEST_F(UtilitiesTest, SplitCommandEmpty) {
    auto words = ClaudeConsole::SplitCommand("");
    EXPECT_EQ(words.size(), 0);
}

// Test command splitting with extra whitespace
TEST_F(UtilitiesTest, SplitCommandWhitespace) {
    auto words = ClaudeConsole::SplitCommand("  hello   world  ");
    ASSERT_EQ(words.size(), 2);
    EXPECT_EQ(words[0], "hello");
    EXPECT_EQ(words[1], "world");
    
    auto words2 = ClaudeConsole::SplitCommand("\t\ttest\t\tcommand\t\t");
    ASSERT_EQ(words2.size(), 2);
    EXPECT_EQ(words2[0], "test");
    EXPECT_EQ(words2[1], "command");
}

// Test command splitting with tabs and mixed whitespace
TEST_F(UtilitiesTest, SplitCommandMixedWhitespace) {
    auto words = ClaudeConsole::SplitCommand("cmd1\t\targ1 \t arg2\t  arg3");
    ASSERT_EQ(words.size(), 4);
    EXPECT_EQ(words[0], "cmd1");
    EXPECT_EQ(words[1], "arg1");
    EXPECT_EQ(words[2], "arg2");
    EXPECT_EQ(words[3], "arg3");
}

// Test command splitting with special characters
TEST_F(UtilitiesTest, SplitCommandSpecialCharacters) {
    auto words = ClaudeConsole::SplitCommand("grep -r --include='*.cpp'");
    ASSERT_EQ(words.size(), 3);
    EXPECT_EQ(words[0], "grep");
    EXPECT_EQ(words[1], "-r");
    EXPECT_EQ(words[2], "--include='*.cpp'");
    
    auto words2 = ClaudeConsole::SplitCommand("echo \"hello world\"");
    ASSERT_EQ(words2.size(), 2);
    EXPECT_EQ(words2[0], "echo");
    EXPECT_EQ(words2[1], "\"hello");
    // Note: This simple split doesn't handle quoted strings specially
}

// Test command splitting with very long input
TEST_F(UtilitiesTest, SplitCommandLongInput) {
    std::string longCommand = "very long command with many many many arguments";
    auto words = ClaudeConsole::SplitCommand(longCommand);
    EXPECT_EQ(words.size(), 9);
    EXPECT_EQ(words[0], "very");
    EXPECT_EQ(words[8], "arguments");
}

// Test execution time formatting precision
TEST_F(UtilitiesTest, FormatExecutionTimePrecision) {
    // Test millisecond precision
    auto us = std::chrono::microseconds(1234);
    std::string formatted = ClaudeConsole::FormatExecutionTime(us);
    EXPECT_EQ(formatted, "1.2ms");
    
    // Test second precision
    auto us2 = std::chrono::microseconds(1234567);
    std::string formatted2 = ClaudeConsole::FormatExecutionTime(us2);
    EXPECT_EQ(formatted2, "1.23s");
}

// Test boundary conditions for time formatting
TEST_F(UtilitiesTest, FormatExecutionTimeBoundaries) {
    // Just under 1ms
    auto us999 = std::chrono::microseconds(999);
    std::string formatted999 = ClaudeConsole::FormatExecutionTime(us999);
    EXPECT_EQ(formatted999, "999μs");
    
    // Just under 1s
    auto us999ms = std::chrono::microseconds(999999);
    std::string formatted999ms = ClaudeConsole::FormatExecutionTime(us999ms);
    EXPECT_EQ(formatted999ms, "1000.0ms");
}