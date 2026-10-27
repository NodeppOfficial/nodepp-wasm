/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_CONIO
#define NODEPP_WASM_CONIO

/*────────────────────────────────────────────────────────────────────────────*/

#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace conio {

    template< class V, class... T >
    int scan( const V& argc, const T&... args ){ return scanf( (const char*)argc, args... ); }

    /*─······································································─*/
    
    inline int pwrn( const string_t& args ){
        auto code = string::join( "`", "console.warn(", args, ");" );
        emscripten::val::global( "window" ).call<void>( "eval", std::string( code.c_str() ) ); 
        return args.size();
    }

    inline int perr( const string_t& args ){ 
        auto code = string::join( "`", "console.error(", args, ");" );
        emscripten::val::global( "window" ).call<void>( "eval", std::string( code.c_str() ) ); 
        return args.size();
    }
    
    inline int pout( const string_t& args ){
        auto code = string::join( "`", "console.log(", args, ");" );
        emscripten::val::global( "window" ).call<void>( "eval", std::string( code.c_str() ) ); 
        return args.size();
    }
    
    inline int pnfo( const string_t& args ){
        auto code = string::join( "`", "console.info(", args, ");" );
        emscripten::val::global( "window" ).call<void>( "eval", std::string( code.c_str() ) ); 
        return args.size();
    }

    /*─······································································─*/

    template< class... T >
    int log( const T&... args ){
        auto  data = string::join( " ", args... );
        pout( data ); return data.size();
    }

    template< class... T >
    int err( const T&... args ){
        auto  data = string::join( " ", args... );
        perr( data ); return data.size();
    }

    template< class... T >
    int wrn( const T&... args ){
        auto  data = string::join( " ", args... );
        pwrn( data ); return data.size();
    }

    template< class... T >
    int nfo( const T&... args ){
        auto  data = string::join( " ", args... );
        pnfo( data ); return data.size();
    }

    /*─······································································─*/

    inline int error( string_t msg ){ return err( msg ); }
    inline int  info( string_t msg ){ return nfo( msg ); }
    inline int  done( string_t msg ){ return log( msg ); }
    inline int  warn( string_t msg ){ return wrn( msg ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif