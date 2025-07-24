# Tests CMakeLists.txt

# Test files
set(TEST_SOURCES
    TestClaudeConsole.cpp
    TestCommandExecution.cpp
    TestMultilineMode.cpp
    TestConfiguration.cpp
    TestPromptManagement.cpp
    TestAliasSystem.cpp
    TestUtilities.cpp
)

# Create test executable
add_executable(cll_tests ${TEST_SOURCES})

# Link with ClaudeConsole library and Google Test
target_link_libraries(cll_tests 
    ClaudeConsole
    gtest_main
    gtest
    pthread
)

# Include directories
target_include_directories(cll_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/Library/ClaudeConsole/Include
    ${CMAKE_SOURCE_DIR}/Include
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(cll_tests)

