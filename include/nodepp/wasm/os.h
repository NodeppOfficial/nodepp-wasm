/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_OS
#define NODEPP_WASM_OS

/*────────────────────────────────────────────────────────────────────────────*/

#include <emscripten/emscripten.h>
#include <cerrno>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace os {

    inline void exec( string_t cmd ){ ::emscripten_run_script( cmd.get() ); }

    inline void call( string_t cmd ){ ::emscripten_run_script( cmd.get() ); }

    /*─······································································─*/
    
    inline string_t tmp(){ return "/tmp"; }

    inline string_t cwd(){ return "/"; }

    /*─······································································─*/

    inline uint error(){ return errno; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/