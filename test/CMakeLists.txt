cmake_minimum_required(VERSION 3.6)
project( AMG_CLIENT CXX) 

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/build")


set(EMCC_LINK_FLAGS

    "${CMAKE_CURRENT_SOURCE_DIR}/../lib/libmbedtls.a"
    "${CMAKE_CURRENT_SOURCE_DIR}/../lib/libmbedx509.a"
    "${CMAKE_CURRENT_SOURCE_DIR}/../lib/libmbedcrypto.a"
    
    "-sEXPORTED_RUNTIME_METHODS=['HEAPF32']"
    "-sFORCE_FILESYSTEM=1"
 	"-sASSERTIONS=0"
    "-sASYNCIFY=1"
    "-sUSE_ZLIB=1"
    "-sWASMFS=1"
    "-sWASM=1"
    "-lembind"

)

add_executable( main ./main.cpp )
set_target_properties( main PROPERTIES SUFFIX ".html" )
target_link_options  ( main PRIVATE ${EMCC_LINK_FLAGS})
target_include_directories( main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include" )