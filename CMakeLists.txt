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

add_executable(myl Sources/Main.cpp
                   Sources/Repl.cpp)
target_link_libraries(myl tracing)
