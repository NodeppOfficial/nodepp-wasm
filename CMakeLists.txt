cmake_minimum_required(VERSION 3.10)
project(nodepp VERSION 1.3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(NODEPP_WASM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library( nodepp INTERFACE)

# Tell the compiler where to find the headers (including /psa)
target_include_directories( nodepp INTERFACE ${NODEPP_WASM_INCLUDE_DIR} )

# Link the precompiled static libraries (.a files)
target_link_libraries( nodepp INTERFACE

    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmbedtls.a"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmbedx509.a"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmbedcrypto.a"
    
    "-sEXPORTED_RUNTIME_METHODS=['HEAPF32']"
    "-sNO_DISABLE_EXCEPTION_CATCHING"
    "-sFORCE_FILESYSTEM=1"
    "-lwebsocket.js"
    "-sASYNCIFY=1"
    "-sFETCH=1"
    "-sWASM=1"
    "-lembind"

)