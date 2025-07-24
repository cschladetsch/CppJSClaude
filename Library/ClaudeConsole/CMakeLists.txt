# ClaudeConsole Library CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

# Create static library
add_library(ClaudeConsole STATIC
    Source/ClaudeConsole.cpp
    Source/DllLoader.cpp
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

# Add V8 dependencies if available
if(DEFINED HAS_V8 AND HAS_V8)
    target_include_directories(ClaudeConsole PRIVATE ${V8_INCLUDE_DIRS})
    target_link_libraries(ClaudeConsole ${V8_LIBRARIES})
    if(DEFINED V8_PLATFORM_LIB)
        target_link_libraries(ClaudeConsole ${V8_PLATFORM_LIB})
    endif()
    target_compile_options(ClaudeConsole PRIVATE ${V8_CFLAGS_OTHER})
    target_compile_definitions(ClaudeConsole PRIVATE HAS_V8)
else()
    target_compile_definitions(ClaudeConsole PRIVATE NO_V8)
endif()

# Add system libraries for DLL loading
target_link_libraries(ClaudeConsole dl pthread)

# Add external dependencies
if(DEFINED HAS_RANG AND HAS_RANG AND DEFINED RANG_INCLUDE_DIR)
    target_include_directories(ClaudeConsole PRIVATE ${RANG_INCLUDE_DIR})
    target_compile_definitions(ClaudeConsole PRIVATE HAS_RANG)
elseif(EXISTS ${CMAKE_SOURCE_DIR}/External/rang/include)
    target_include_directories(ClaudeConsole PRIVATE ${CMAKE_SOURCE_DIR}/External/rang/include)
    target_compile_definitions(ClaudeConsole PRIVATE HAS_RANG)
endif()

if(DEFINED HAS_JSON AND HAS_JSON AND DEFINED JSON_INCLUDE_DIR)
    target_include_directories(ClaudeConsole PRIVATE ${JSON_INCLUDE_DIR})
    target_compile_definitions(ClaudeConsole PRIVATE HAS_JSON)
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

install(FILES Include/ClaudeConsole.h Include/DllLoader.h Include/V8Compat.h
    DESTINATION include/ClaudeConsole
)