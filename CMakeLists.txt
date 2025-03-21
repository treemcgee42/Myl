cmake_minimum_required(VERSION 3.10)
project(Myl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Trinkets
include_directories(/Users/ogmalladii/dev/Trinkets/Cpp/Include)
link_directories(/Users/ogmalladii/dev/Trinkets/Cpp/Lib)

# Set Homebrew LLVM path
set(LLVM_DIR "/opt/homebrew/opt/llvm/lib/cmake/llvm")
# Try using CMake's built-in LLVM package
find_package(LLVM REQUIRED CONFIG)

if(LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
else()
    message(FATAL_ERROR "LLVM not found. Ensure you installed LLVM via Homebrew and set LLVM_DIR correctly.")
endif()

# --- Main project ------------------------------------------------------------------

set(MYL_SOURCES Sources/Lexer.cpp
                Sources/Main.cpp
                Sources/Parser.cpp
                Sources/Repl.cpp)

add_executable(myl ${MYL_SOURCES})
target_link_libraries(myl tracing)
target_link_libraries(myl unicode)

add_executable(myl_test ${MYL_SOURCES})
target_compile_definitions(myl_test PRIVATE MYL_TEST)
target_link_libraries(myl_test tracing)
target_link_libraries(myl_test unicode)
