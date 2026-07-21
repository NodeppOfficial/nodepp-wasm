#!/bin/bash

mkdir -p "www"

em++ -o ./www/index.html main.cpp -I./include -L./lib \
    -lmbedtls -lmbedcrypto -lmbedx509 -lembind \
    -sASYNCIFY=1 -sWASM=1 -sASSERTIONS=0 \
    -sFORCE_FILESYSTEM=1 -sUSE_ZLIB=1 \
    -sNO_DISABLE_EXCEPTION_CATCHING
   
emrun ./www