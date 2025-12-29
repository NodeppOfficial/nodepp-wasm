cmake_minimum_required(VERSION 3.10)
project(nodepp VERSION 1.3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(NODEPP_WASM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# interface-target
add_library(nodepp INTERFACE )

target_link_libraries(nodepp INTERFACE
    "-sNO_DISABLE_EXCEPTION_CATCHING"
    "-sFORCE_FILESYSTEM=1"
    "-lwebsocket.js"
    "-sASYNCIFY=1"
    "-sFETCH=1"
    "-sWASM=1"
    "-lembind"
)

target_include_directories(nodepp INTERFACE ${NODEPP_WASM_INCLUDE_DIR} )