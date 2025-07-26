# Nodepp-WASM

Nodepp is a groundbreaking open-source project that simplifies C++ application development by bridging the gap between the language's raw power and the developer-friendly abstractions of Node.js. By providing a high-level API, Nodepp empowers developers to write C++ code in a familiar, Node.js-inspired style.

One of the standout features of Nodepp is its 100% asynchronous architecture, powered by an internal Event Loop. This design efficiently manages Nodepp’s tasks, enabling you to develop scalable and concurrent applications with minimal code. Experience the power and flexibility of Nodepp as you streamline your development process and create robust applications effortlessly!

## Dependencies
```bash
#emscripten
    🪟: pacman -S mingw-w64-ucrt-x86_64-emscripten
    🐧: sudo apt install emscripten
```

## Features

- 📌: **Node.js-like API:** Write C++ code in a syntax and structure similar to Node.js, making it easier to learn and use.
- 📌: **High-performance:** Leverage the speed and efficiency of C++ for demanding applications.
- 📌: **Scalability:** Build applications that can handle large workloads and grow with your needs.
- 📌: **Open-source:** Contribute to the project's development and customize it to your specific requirements.

## Bateries Included

- 📌: Include a **build-in JSON** parser / stringify system.
- 📌: Include a **build-in RegExp** engine for processing text strings.
- 📌: Include a **build-in System** that make every object **Async Task** safety.
- 📌: Include Support for **Poll** making it easy to handle multiple file descriptors.
- 📌: Include a **Smart Pointer** base **Garbage Collector** to avoid **Memory Leaks**.
- 📌: Include support for **HTTP | WS** making it easy to create networked applications.
- 📌: Include support for **Reactive Programming** based on **Events** and **Observers**.
- 📌: Include an **Event Loop** that can handle multiple events and tasks on a single thread.

## Build & Run

```bash
em++ -o www/index.html main.cpp           \
     -I ./include -pthread -lwebsocket.js \
    --shell-file ./shell.html --bind      \
     -s NO_DISABLE_EXCEPTION_CATCHING     \
     -s WEBSOCKET_SUBPROTOCOL=1 \
     -s PTHREAD_POOL_SIZE=8     \
     -s WEBSOCKET_URL=1         \
     -s USE_PTHREADS=1          \
     -s ASYNCIFY=1              \
     -s FETCH=1                 \
     -s WASM=1

emrun ./www/index.html
```

## Tests

```bash
em++ -o www/index.html ./test/main.cpp    \
     -I ./include -pthread -lwebsocket.js --bind \
     -s NO_DISABLE_EXCEPTION_CATCHING \
     -s WEBSOCKET_SUBPROTOCOL=1       \
     -s PTHREAD_POOL_SIZE=8           \
     -s WEBSOCKET_URL=1               \
     -s USE_PTHREADS=1                \
     -s ASYNCIFY=1                    \
     -s FETCH=1                       \
     -s WASM=1

emrun ./www/index.html
```

## Examples
### Hello world
```cpp
#include <nodepp/nodepp.h>

using namespace nodepp;

void onMain() {
    console::log("Hello World!");
}

// note that we are using onMain() instead of main()
```

### HTTP Client
```cpp
#include <nodepp/nodepp.h>
#include <nodepp/fetch.h>

using namespace nodepp;

void onMain() {

    fetch_t args;
            args.url = "https://localhost:8000/";

    fetch::add( args )

    .then([=]( fetch_t res ){
        console::log( "->", res.status );
        console::log( res.headers["Content-Length"] );
    })

    .fail([=]( except_t rej ){
        console::log( rej );
    });

}
```

### Websocket Client
```cpp
#include <nodepp/nodepp.h>
#include <nodepp/ws.h>

using namespace nodepp;

void onMain() {

    auto srv = ws::connect( "wss://localhost:8000" );

    srv.onConnect([=]( ws_t cli ){

        cli.onData([]( string_t data ){
            console::log( data );
        });

        cli.onClose([](){
            console::log( "done" );
        });

    });

}
```

### More Examples [here](https://github.com/NodeppOfficial/Nodepp/tree/main/examples)

Check out some articles on [Medium](https://medium.com/@EDBCBlog)

## Compatibility
- 🔗: [NodePP for Window | Linux | Mac | Bsd ](https://github.com/NodeppOfficial/nodepp)
- 🔗: [NodePP for Arduino](https://github.com/NodeppOfficial/nodepp-arduino)
- 🔗: [Nodepp for ESP32](https://github.com/NodeppOfficial/nodepp-ESPXX)
- 🔗: [Nodepp for WASM](https://github.com/NodeppOfficial/nodepp-wasm)
  
## Contribution

If you want to contribute to **Nodepp**, you are welcome to do so! You can contribute in several ways:

- ☕ Buying me a Coffee
- 📢 Reporting bugs and issues
- 📝 Improving the documentation
- 📌 Adding new features or improving existing ones
- 🧪 Writing tests and ensuring compatibility with different platforms
- 🔍 Before submitting a pull request, make sure to read the contribution guidelines.

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/edbc_repo)

## License

**Nodepp** is distributed under the MIT License. See the LICENSE file for more details.