/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#pragma once
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>

/*────────────────────────────────────────────────────────────────────────────*/

#define BIND_ADD( NAME, CALLBACK ) emscripten::function( NAME, &CALLBACK );
#define BIND_BEGIN( MODULE ) EMSCRIPTEN_BINDINGS( MODULE ){
#define BIND_RUN(...) emscripten_run_script( #__VA_ARGS__ )
#define BIND_END() }

/*────────────────────────────────────────────────────────────────────────────*/

#define BIND( MODULE, NAME, CALLBACK ) EMSCRIPTEN_BINDINGS( MODULE ) { \
    emscripten::function( NAME, CALLBACK );                            \
}

/*────────────────────────────────────────────────────────────────────────────*/
