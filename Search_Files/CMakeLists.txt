cmake_minimum_required(VERSION 3.15)
project(MyProject LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
add_executable(fileSearch main.cpp)
set_target_properties(fileSearch PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)