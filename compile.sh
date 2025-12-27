if [ ! -d "./www" ]; then
     mkdir "./www"
fi

em++ -o www/index.html ./test/main.cpp     \
     -I ./include -pthread --bind -lembind \
     -s NO_DISABLE_EXCEPTION_CATCHING      \
     -s PTHREAD_POOL_SIZE=8                \
     -s USE_PTHREADS=1                     \
     -s ASYNCIFY=1                         \
     -s FETCH=1                            \
     -s WASM=1

if [ ! $? -eq 0 ]; then
    echo "exit error"; exit;
fi

emrun ./www/index.html