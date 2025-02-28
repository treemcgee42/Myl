cmake_minimum_required(VERSION 3.10)
project(Myl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Trinkets
include_directories(/Users/ogmalladii/dev/Trinkets/Cpp/Include)
link_directories(/Users/ogmalladii/dev/Trinkets/Cpp/Lib)
# Homebrew
include_directories(/opt/homebrew/include/)
link_directories(/opt/homebrew/opt/libgccjit/lib/gcc/current)


add_executable(myl Sources/Main.cpp
                   Sources/LibgccjitMgr.cpp)
target_link_libraries(myl tracing)
target_link_libraries(myl gccjit)
