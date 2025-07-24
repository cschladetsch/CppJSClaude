# ClaudeConsole Library CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

# Create static library
add_library(ClaudeConsole STATIC
    Source/ClaudeConsole.cpp
)

# Set include directories
target_include_directories(ClaudeConsole 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Set C++20 standard
target_compile_features(ClaudeConsole PUBLIC cxx_std_20)

# Add external dependencies
if(EXISTS ${CMAKE_SOURCE_DIR}/External/rang/include)
    target_include_directories(ClaudeConsole PRIVATE ${CMAKE_SOURCE_DIR}/External/rang/include)
    target_compile_definitions(ClaudeConsole PRIVATE HAS_RANG)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/External/boost)
    target_include_directories(ClaudeConsole PRIVATE ${CMAKE_SOURCE_DIR}/External/boost)
    target_compile_definitions(ClaudeConsole PRIVATE HAS_BOOST)
endif()

# Install library
install(TARGETS ClaudeConsole 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES Include/ClaudeConsole.h 
    DESTINATION include/ClaudeConsole
)