#!/bin/bash

mkdir -p "www"

em++ -o ./www/index.html main.cpp -I./include -L./lib \
    -lmbedtls -lmbedcrypto -lmbedx509 \
    -sNO_DISABLE_EXCEPTION_CATCHING \
    -sFORCE_FILESYSTEM=1 \
    -lwebsocket.js \
    -sASYNCIFY=1 \
    -sFETCH=1 \
    -sWASM=1 \
    -lembind

emrun ./www