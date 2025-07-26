if [ ! -d "./www" ]; then
     mkdir "./www"
fi

FLAGS=(
    -s WEBSOCKET_SUBPROTOCOL=1
    -s PTHREAD_POOL_SIZE=8
    -s FETCH=1 -s WASM=1
    -s WEBSOCKET_URL=1
    -s USE_PTHREADS=1
    -s ASYNCIFY=1
)

SHELL="--shell-file ./shell.html --bind"
LIB="-pthread -lwebsocket.js"
INCLUDE="-I./include"
FILE="main.cpp"

em++ -o www/index.html $FILE $INCLUDE $LIB $SHELL "${FLAGS[@]}"
emrun ./www/index.html