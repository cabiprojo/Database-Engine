cmake_minimum_required(VERSION 3.15)
project(DatabaseEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/file_manager.cpp
    src/page.cpp
    src/page_manager.cpp
    src/buffer_pool.cpp
    src/kv_store.cpp
    src/bplus_tree.cpp
)

set(HEADERS
    include/file_manager.h
    include/page.h
    include/page_manager.h
    include/buffer_pool.h
    include/bplus_tree.h
)

# Main executable
add_executable(db_engine
    src/main.cpp
    ${SOURCES}
    ${HEADERS}
)

# Compiler options
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Optional: Build tests
option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

