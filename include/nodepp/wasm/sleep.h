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

    inline ulong get_time_interval() { 
        
        char res [32]; auto size = EM_ASM_INT({
             let data = Date.now() + ""; /*-----------------*/
             stringToUTF8( data, $0, $1 ); return data.length;
        }, res, 32 );

        return string::to_ulong( string_t( res, size ) );
    }

    inline ulong  micros(){ return get_time_interval() / 1000000; }

    inline ulong seconds(){ return get_time_interval() / 1000; }

    inline ulong  millis(){ return get_time_interval(); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    inline ulong& get_timeout( bool reset=false ) {
    thread_local static ulong stamp=0; 
        if( reset ) { stamp=(ulong)-1; }
    return stamp; }

    inline void clear_timeout() { get_timeout(true); }

    inline ulong set_timeout( int time=0 ) { 
        if( time < 0 ){ /*--------------*/ return 1; }
        auto stamp=&get_timeout(); ulong out=*stamp;
        if( *stamp>(ulong)time ){ *stamp=(ulong)time; }
    return out==0 ? 1 : out; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    inline void delay( ulong time ){ emscripten_sleep( time ); }

    inline void yield(){ delay( TIMEOUT ); }

    inline ulong now(){ return millis(); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/