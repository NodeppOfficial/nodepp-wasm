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

    inline ulong get_new_timeval() { char res [32];

        auto size = EM_ASM_INT({
            let data = Date.now() + ""; /*-----------------*/
            stringToUTF8( data, $0, $1 ); return data.length;
        }, res, 32 );

        return string::to_ullong( string_t( res, size ) );
    }

    inline ulong  micros(){ return get_new_timeval() / 1000000; }

    inline ulong seconds(){ return get_new_timeval() / 1000; }

    inline ulong  millis(){ return get_new_timeval(); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    inline void delay( ulong time ){ emscripten_sleep( time ); }

    inline void yield(){ emscripten_sleep(TIMEOUT); }

    inline ulong now(){ return millis(); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/