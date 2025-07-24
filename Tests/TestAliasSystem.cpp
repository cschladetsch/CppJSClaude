#include <gtest/gtest.h>
#include "ClaudeConsole.h"

using namespace cll;

class AliasSystemTest : public ::testing::Test {
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

// Test setting and expanding aliases
TEST_F(AliasSystemTest, SetAndExpandAlias) {
    console->SetAlias("ll", "ls -la");
    
    std::string expanded = console->ExpandAlias("ll");
    EXPECT_EQ(expanded, "ls -la");
    
    // Test with arguments
    std::string expandedWithArgs = console->ExpandAlias("ll /tmp");
    EXPECT_EQ(expandedWithArgs, "ls -la /tmp");
}

// Test multiple aliases
TEST_F(AliasSystemTest, MultipleAliases) {
    console->SetAlias("ll", "ls -la");
    console->SetAlias("la", "ls -A");
    console->SetAlias("cls", "clear");
    
    EXPECT_EQ(console->ExpandAlias("ll"), "ls -la");
    EXPECT_EQ(console->ExpandAlias("la"), "ls -A");
    EXPECT_EQ(console->ExpandAlias("cls"), "clear");
}

// Test alias with no expansion
TEST_F(AliasSystemTest, NoAliasExpansion) {
    std::string original = "nonexistent_alias";
    std::string expanded = console->ExpandAlias(original);
    EXPECT_EQ(expanded, original);
}

// Test empty command expansion
TEST_F(AliasSystemTest, EmptyCommandExpansion) {
    std::string expanded = console->ExpandAlias("");
    EXPECT_EQ(expanded, "");
}

// Test alias overwriting
TEST_F(AliasSystemTest, AliasOverwriting) {
    console->SetAlias("test", "echo hello");
    EXPECT_EQ(console->ExpandAlias("test"), "echo hello");
    
    console->SetAlias("test", "echo world");
    EXPECT_EQ(console->ExpandAlias("test"), "echo world");
}

// Test complex alias values
TEST_F(AliasSystemTest, ComplexAliasValues) {
    console->SetAlias("gitlog", "git log --oneline --graph --decorate");
    std::string expanded = console->ExpandAlias("gitlog");
    EXPECT_EQ(expanded, "git log --oneline --graph --decorate");
    
    console->SetAlias("search", "grep -r --include='*.cpp' --include='*.h'");
    expanded = console->ExpandAlias("search pattern .");
    EXPECT_EQ(expanded, "grep -r --include='*.cpp' --include='*.h' pattern .");
}

// Test alias configuration via config command
TEST_F(AliasSystemTest, AliasViaConfigCommand) {
    auto result = console->ExecuteCommand("config alias mytest=\"echo testing\"");
    EXPECT_TRUE(result.success);
    EXPECT_TRUE(result.output.find("Alias set: mytest = 'echo testing'") != std::string::npos);
    
    std::string expanded = console->ExpandAlias("mytest");
    EXPECT_EQ(expanded, "echo testing");
}

// Test alias with quotes in config command
TEST_F(AliasSystemTest, AliasWithQuotes) {
    auto result = console->ExecuteCommand("config alias quoted=\"echo 'hello world'\"");
    EXPECT_TRUE(result.success);
    
    std::string expanded = console->ExpandAlias("quoted");
    EXPECT_EQ(expanded, "echo 'hello world'");
}

// Test invalid config alias syntax
TEST_F(AliasSystemTest, InvalidConfigAliasSyntax) {
    auto result = console->ExecuteCommand("config alias invalid");
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.error.find("Usage: config alias name=value") != std::string::npos);
    
    result = console->ExecuteCommand("config alias noequals value");
    EXPECT_FALSE(result.success);
}

// Test alias expansion with multiple words
TEST_F(AliasSystemTest, AliasExpansionMultipleWords) {
    console->SetAlias("dockerrun", "docker run -it --rm");
    
    std::string expanded = console->ExpandAlias("dockerrun ubuntu:latest bash");
    EXPECT_EQ(expanded, "docker run -it --rm ubuntu:latest bash");
}

// Test that only first word is expanded
TEST_F(AliasSystemTest, OnlyFirstWordExpanded) {
    console->SetAlias("ls", "ls --color=auto");
    
    // Only the first word should be expanded
    std::string expanded = console->ExpandAlias("echo ls test");
    EXPECT_EQ(expanded, "echo ls test"); // 'ls' in middle should not be expanded
}

// Test special characters in aliases
TEST_F(AliasSystemTest, SpecialCharactersInAliases) {
    console->SetAlias("...", "cd ../..");
    console->SetAlias("~", "cd ~");
    
    EXPECT_EQ(console->ExpandAlias("..."), "cd ../..");
    EXPECT_EQ(console->ExpandAlias("~"), "cd ~");
}

// Test case sensitivity
TEST_F(AliasSystemTest, CaseSensitivity) {
    console->SetAlias("test", "echo lowercase");
    console->SetAlias("TEST", "echo uppercase");
    
    EXPECT_EQ(console->ExpandAlias("test"), "echo lowercase");
    EXPECT_EQ(console->ExpandAlias("TEST"), "echo uppercase");
    EXPECT_NE(console->ExpandAlias("test"), console->ExpandAlias("TEST"));
}

// Test whitespace handling
TEST_F(AliasSystemTest, WhitespaceHandling) {
    console->SetAlias("spaced", "echo hello world");
    
    std::string expanded = console->ExpandAlias("  spaced  arg1  arg2  ");
    // Should preserve argument spacing but expand the alias
    EXPECT_TRUE(expanded.find("echo hello world") != std::string::npos);
    EXPECT_TRUE(expanded.find("arg1") != std::string::npos);
    EXPECT_TRUE(expanded.find("arg2") != std::string::npos);
}