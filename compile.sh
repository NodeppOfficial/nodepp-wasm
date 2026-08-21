#!/bin/bash

mkdir -p "www"

em++ -o ./www/index.html main.cpp -I./include -L./lib \
    -sASYNCIFY=1 -sWASM=1 -sASSERTIONS=0 -sWASMFS=1 \
    -lmbedtls -lmbedcrypto -lmbedx509 -lembind \
    -sFORCE_FILESYSTEM=1 -sUSE_ZLIB=1 \
    -sNO_DISABLE_EXCEPTION_CATCHING \
	-sASYNCIFY_STACK_SIZE=65536 \
	-sALLOW_MEMORY_GROWTH \
	--preload-file ./www
   
emrun ./www