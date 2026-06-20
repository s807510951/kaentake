cmake_minimum_required(VERSION 3.14)
project(Kaentake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa /source-charset:utf-8 /execution-charset:gbk")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Build external dependencies
add_subdirectory(external)

# Build injector and laucher
add_subdirectory(src)