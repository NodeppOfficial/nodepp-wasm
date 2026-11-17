#!/bin/bash
( cd "./test" ; ./compile.sh ) ; exit

mkdir -p "www"

em++ -o ./www/index.html main.cpp -I./include -L./lib \
    -sASYNCIFY=1 -sWASM=1 -sASSERTIONS=0 -sWASMFS=1 -sUSE_ZLIB=1 \
	-sASYNCIFY_STACK_SIZE=65536 -sALLOW_MEMORY_GROWTH \
    -lmbedtls -lmbedcrypto -lmbedx509 -lembind
   
emrun ./www