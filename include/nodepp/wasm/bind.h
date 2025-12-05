/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_BIND
#define NODEPP_WASM_BIND

/*────────────────────────────────────────────────────────────────────────────*/

#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

/*────────────────────────────────────────────────────────────────────────────*/

#define EM_VAL    emscripten::val
#define EM_STRING std::string
#define EM_ARRAY  std::vector

/*────────────────────────────────────────────────────────────────────────────*/

#define BIND_ADD( NAME, CALLBACK ) emscripten::function( NAME, CALLBACK );
#define BIND_RUN( ...) emscripten_run_script( #__VA_ARGS__ )
#define BIND( MODULE ) EMSCRIPTEN_BINDINGS( MODULE )

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/