/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_SLEEP
#define NODEPP_WASM_SLEEP

/*────────────────────────────────────────────────────────────────────────────*/

#include <emscripten/emscripten.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    inline ulong get_time_interval() { return type::cast<ulong>(emscripten_get_now()); }

    inline ulong  micros(){ return get_time_interval() / 1000000; }

    inline ulong seconds(){ return get_time_interval() / 1000; }

    inline ulong  millis(){ return get_time_interval(); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    inline ulong now(){ return millis(); }

    inline void delay( ulong time ){ 
        if( time == 0 ){ return; }
        emscripten_sleep( time ); 
    }

    inline void yield(){ delay(1); }
    
}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/