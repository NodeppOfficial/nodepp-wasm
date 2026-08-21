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

    template< class... T >
    inline EM_VAL exec( string_t cmd, T... args ){ return EM_EVAL( cmd, args... ); }

    template< class... T >
    inline EM_VAL call( string_t cmd, T... args ){ return EM_EVAL( cmd, args... ); }

    /*.........................................................................*/

    inline string_t create_url_object( string_t data ) {
        if( data.empty() ){ return nullptr; }
        return string_t( call( NODEPP_STRINGIFY(
            var tm = decodeURIComponent( escape(window.atob( '${0}' )) );
            return URL.createObjectURL ( new Blob([ tm ]) );
        ), encoder::base64::atob ( data ) ).as<EM_STRING>() );
    }

    inline void remove_url_object( string_t data ) {
    if( !data.empty() ){ call( NODEPP_STRINGIFY(
        return URL.revokeObjectURL( "${0}" );
    ), data ); }}

    /*.........................................................................*/

    inline string_t resolve_origin( string_t path ) {
    return string_t( call( "return window.origin" ).as<EM_STRING>() ) + path; }

    /*─······································································─*/
    
    inline string_t tmp(){ return "/tmp"; }

    inline string_t cwd(){ return "/"; }

    inline uint    cpus(){ return 1; }

    /*─······································································─*/

    inline uint error(){ return errno; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/