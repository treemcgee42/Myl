cmake_minimum_required(VERSION 3.10)
project(Myl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(/Users/ogmalladii/dev/Trinkets/Cpp/Include)
link_directories(/Users/ogmalladii/dev/Trinkets/Cpp/Lib)

add_executable(myl Sources/Main.cpp)
target_link_libraries(myl tracing)
