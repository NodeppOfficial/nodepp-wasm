# Nodepp-Wasm: The Unified Asynchronous Real-Time C++ Runtime
> **The DOOM of Async Frameworks: Write Once, Compile Everywhere, Process Everything.**

[![Platform](https://img.shields.io/badge/platform-%20WASM%20-blue)](https://github.com/NodeppOfficial/nodepp-wasm)
[![MIT License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Nodepp-WASM brings the raw performance of C++ and the "Shared-Nothing" architecture of Nodepp to the browser. By leveraging Emscripten and WebAssembly, Nodepp allows you to build complex, concurrent web applications, VR experiences, and high-performance tools with a familiar, asynchronous C++ syntax.

Stop compromising between web accessibility and native speed. With Nodepp, your browser-based applications benefit from the same Zero-Copy Slicing and O(1) Event Loop used in our server and embedded versions.

🔗: [Nodepp The MOST Powerful Framework for Asynchronous Programming in C++](https://medium.com/p/c01b84eee67a)

## Featured Project: Duck Hunt VR (WASM Edition)

To showcase [Nodepp for Web](https://github.com/NodeppOfficial/nodepp-wasm), we ported the classic Duck Hunt to Virtual Reality, running natively in the browser via WebAssembly. This project pushes the limits of web-based VR by combining low-level C++ performance with modern Web APIs.

https://github.com/user-attachments/assets/ab26287e-bd73-4ee8-941b-d97382e203c9

Play it now: [Duck Hunt VR on Itch.io](https://edbcrepo.itch.io/duck-hunt-vr)

## Featured Project: Cursed-Luna (WASM Remake)
Cursed-Luna is a high-performance remake of the Global Game Jam 2018 classic. Originally built around the theme of Transmission, this version has been completely rewritten in C++ using Nodepp and Raylib to bring tight gameplay mechanics to the browser via WebAssembly.

https://github.com/user-attachments/assets/3647b5b6-fbfd-4281-af0f-f35f3260a319

Play it now: [Cursed-Luna on Itch.io](https://edbcrepo.itch.io/cursed-luna)

## Dependencies & Cmake Integration
```bash
#emscripten
- 🪟: pacman -S mingw-w64-ucrt-x86_64-emscripten
- 🐧: sudo apt install emscripten
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

- **Near-Native Speed:** Execute C++ logic at nearly the speed of light within the browser sandbox.
- **ASYNCIFY Integration:** Seamlessly handle asynchronous C++ logic within the single-threaded environment of the browser.
- **Smart Memory:** Uses the same `ptr_t` architecture to manage memory safely, preventing leaks in long-running web sessions.
- **Unified API:** Use the exact same `http::fetch` and `ws::connect` code on WASM that you use on Linux or Arduino.

## Code Examples

### Asynchronous HTTP Fetch (Browser)
Nodepp abstracts the complex Emscripten Fetch API into a clean, Promise-based structure.

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

### Real-time WebSockets
Nodepp also suports modern protocols seamlessly like websocket for real-time aplication.

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

## One Codebase, Every Screen
Nodepp is the only framework that lets you share logic between the deepest embedded layers and the highest web layers.

- **Hardware:** [NodePP for Arduino](https://github.com/NodeppOfficial/nodepp-arduino)
- **Desktop:** [Nodepp for Desktop](https://github.com/NodeppOfficial/nodepp)
- **Browser:** [Nodepp for WASM](https://github.com/NodeppOfficial/nodepp-wasm)

## Contributing

Nodepp is an open-source project that values Mechanical Sympathy and Technical Excellence.

- **Sponsorship:** Support the project via [Ko-fi](https://ko-fi.com/edbc_repo).
- **Bug Reports:** Open an issue via GitHub.
- **License:** MIT.

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/edbc_repo)

## License
**Nodepp** is distributed under the MIT License. See the LICENSE file for more details.