#!/bin/bash

mkdir -p ./build

if [ ! -d "./build/CMakeFiles" ] ; then
   ( cd ./build; emcmake cmake .. )
fi

( cd ./build; make ); 

if [ ! $? -eq 0 ]; then
    echo "exit error"; exit;
fi

emrun ./build/main.html