# Nodepp-EMCC

Emscripten Nodepp port

## Dependencies 
- **emscripten**
    - 🪟: `pacman -S mingw-w64-ucrt-x86_64-emscripten`
    - 🐧: `sudo apt install emscripten`

## Features

- **C++11 | C++14 | C++17 | c++20 | c++23** Compatible
- Includes a **build-in JSON** parser / stringify system
- Includes a **build-in RegExp** engine for processing text strings
- Allows writing **C++** code as if writing in **NodeJS** or **Javascript**
- Adds a high-level abstraction layer to simplify **C++** application development
- Includes support for **HTTP | WS** making it easy to create networked applications
- Includes an **Event Loop** that can handle multiple events and tasks on a single thread
- Supports **Coroutines**, which allows running multiple tasks concurrently on a single thread

## Build & Run

```bash
    em++ -o ./www/index.html main.cpp \
    -I ./include -pthread \
    -s PROXY_POSIX_SOCKETS \
    -s PTHREAD_POOL_SIZE=8 \
    -s ASYNCIFY=1 \
    -s WASM=1 \
    #--embed-file ./assets \
```

## Hello world
```cpp
#include <nodepp/nodepp.h>

using namespace nodepp;

void onMain() {
    console::log("Hello World!");
}
```

## Examples
- Examples : [here](https://github.com/NodeppOficial/Nodepp/tree/main/examples)

## FAQ
- reddit : [/r/Cplusplus/](https://www.reddit.com/r/Cplusplus/comments/19e2kw3/write_asynchronous_code_with_c_nodepp/)
  
## Contribution

If you want to contribute to **Nodepp**, you are welcome to do so! You can contribute in several ways:

- ☕ Buying me a Coffee
- 📢 Reporting bugs and issues
- 📝 Improving the documentation
- 📌 Adding new features or improving existing ones
- 🧪 Writing tests and ensuring compatibility with different platforms
- 🔍 Before submitting a pull request, make sure to read the contribution guidelines.

**NOTE:** We don't use STD here, just Pure C++.

## License

**Nodepp** is distributed under the MIT License. See the LICENSE file for more details.
