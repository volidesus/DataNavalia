cmake_minimum_required(VERSION 3.10)
project(fileEncrypt)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories("D:/cryptopp")
link_directories("D:/cryptopp/x64/Output/Release")
set(LIBS cryptlib.lib)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
add_executable(fileEncrypt main.cpp)
target_link_libraries(fileEncrypt ${LIBS})
set_target_properties(fileEncrypt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)