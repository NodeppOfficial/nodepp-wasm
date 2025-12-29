# Nodepp-WASM
> **The DOOM of Async Frameworks: Write Once, Build Everywhere, Process Everything.**

[![Platform](https://img.shields.io/badge/platform-%20WASM%20-blue)](https://github.com/NodeppOfficial/nodepp-wasm)
[![MIT License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Nodepp is a groundbreaking C++ framework that bridges the gap between the language's raw performance and the developer-friendly abstractions of Node.js. By providing a high-level, asynchronous API, Nodepp empowers you to write C++ with a familiar syntax—enabling seamless development across cloud servers, desktop apps, and microcontrollers.

At its core, Nodepp features a 100% asynchronous architecture powered by an internal Event Loop. This allows for massive concurrency and scalable task management with minimal code complexity, effectively bringing the "Write Once, Run Everywhere" philosophy to the world of high-performance C++.

🔗: [Nodepp The MOST Powerful Framework for Asynchronous Programming in C++](https://medium.com/p/c01b84eee67a)

## 💡 Featured Project: Duck Hunt VR (WASM Edition)

To showcase [Nodepp for Web](https://github.com/NodeppOfficial/nodepp-wasm), we ported the classic Duck Hunt to Virtual Reality, running natively in the browser via WebAssembly. This project pushes the limits of web-based VR by combining low-level C++ performance with modern Web APIs.

https://github.com/user-attachments/assets/ab26287e-bd73-4ee8-941b-d97382e203c9

Play it now: [Duck Hunt VR on Itch.io](https://edbcrepo.itch.io/duck-hunt-vr)

## 💡 Featured Project: Cursed-Luna (WASM Remake)
Cursed-Luna is a high-performance remake of the Global Game Jam 2018 classic. Originally built around the theme of Transmission, this version has been completely rewritten in C++ using Nodepp and Raylib to bring tight gameplay mechanics to the browser via WebAssembly.

https://github.com/user-attachments/assets/3647b5b6-fbfd-4281-af0f-f35f3260a319

Play it now: [Cursed-Luna on Itch.io](https://edbcrepo.itch.io/cursed-luna)

## Dependencies & Cmake Integration
```bash
#emscripten
    🪟: pacman -S mingw-w64-ucrt-x86_64-emscripten
    🐧: sudo apt install emscripten
```
```bash
include(FetchContent)

FetchContent_Declare(
	nodepp
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-wasm
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp)

#[...]

target_link_libraries( #[...]
	PUBLIC nodepp #[...]
)
```

## Features

- 📌: **Lightweight:** Minimal dependencies, making it ideal for IoT and embedded systems.
- 📌: **Cross-Platform:** Write once, run on Linux, Windows, Mac, Android, WASM and Arduino/ESP32.
- 📌: **Memory Efficient:** Custom `ptr_t`, `queue_t`, `array_t` and `string_t` primitives provide SSO (Small Stack Optimization) and zero-copy slicing.
- 📌: **Scalability:** Build applications that can handle large workloads and grow with your needs.
- 📌: **Open-source:** Contribute to the project's development and customize it to your specific requirements.

## Batteries Included

- 📌: UTF Support: Comprehensive manipulation for UTF8, UTF16, and UTF32.
- 📌: Built-in JSON & RegExp: Full parsing and text processing engines.
- 📌: I/O Multiplexing: Support for Poll, Epoll, Kqueue, and WSAPoll.
- 📌: Reactive Programming: Built-in Events and Observers system.

## Build & Run

```bash
em++ -o www/index.html main.cpp -lembind -I ./include \
    --shell-file ./shell.html --bind  \
     -s NO_DISABLE_EXCEPTION_CATCHING \
     -s ASYNCIFY=1 -s FETCH=1 -s WASM=1

emrun ./www/index.html
```

## Tests

```bash
em++ -o www/index.html ./test/main.cpp     \
     -I ./include -pthread --bind -lembind \
     -s NO_DISABLE_EXCEPTION_CATCHING  \
     -s PTHREAD_POOL_SIZE=8            \
     -s USE_PTHREADS=1                 \
     -s ASYNCIFY=1                     \
     -s FETCH=1                        \
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
```

### HTTP Client
```cpp
#include <nodepp/nodepp.h>
#include <nodepp/http.h>

using namespace nodepp;

void onMain() {

    fetch_t args;
            args.method = "GET";
            args.url    = "http://localhost:6931/";

    http::fetch( args )

    .fail([=]( except_t err ){ console::log( err ); })

    .then([=]( http_t cli ){
        console::log( stream::await( cli ) );
        console::log( "->", cli.status );
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

### More Examples [here](https://nodeppofficial.github.io/nodepp-doc/guide.html)

## Installing Nodepp-Wasm

### Clone The Repository
```bash
git clone https://github.com/NodeppOfficial/nodepp-wasm ; cd nodepp
```

### Create a main.cpp File
```bash
touch main.cpp ; mkdir ./www
```
```cpp
#include <nodepp/nodepp.h>

using namespace nodepp;

void onMain() {
    console::log("Hello World!");
}
```

### Build Your Code
```bash
em++ -o www/index.html main.cpp -lembind -I./include \
    --shell-file ./shell.html --bind  \
     -s NO_DISABLE_EXCEPTION_CATCHING \
     -s ASYNCIFY=1 -s FETCH=1 -s WASM=1

emrun ./www/index.html
```

## Nodepp Supports Other Platforms Too
- 🔗: [NodePP for Window | Linux | Mac | Bsd ](https://github.com/NodeppOfficial/nodepp)
- 🔗: [NodePP for Arduino](https://github.com/NodeppOfficial/nodepp-arduino)
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
**Nodepp-WASM** is distributed under the MIT License. See the LICENSE file for more details.
