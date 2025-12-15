if [ ! -d "./www" ]; then
     mkdir "./www"
fi

FLAGS=(
    -s MODULARIZE=1 -s EXPORT_NAME="Engine"
    -s FETCH=1 -s WASM=1
    -s ASYNCIFY=1
)

LIB="-pthread -lwebsocket.js --bind"
INCLUDE="-I./include"
FILE="main.cpp"

em++ -o www/index.js $FILE $INCLUDE $LIB "${FLAGS[@]}"

if [ ! $? -eq 0 ]; then
    echo "exit error"; exit;
fi

emrun ./www/index.html