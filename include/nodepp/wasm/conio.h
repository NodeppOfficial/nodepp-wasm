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

namespace nodepp { namespace conio {

    inline int perr( const string_t& args ){ return ::fwrite( args.get(), sizeof(char), args.size(), stderr ); }
    
    inline int pout( const string_t& args ){ return ::fwrite( args.get(), sizeof(char), args.size(), stdout ); }

    template< class V, class... T >
    int scan( const V& argc, const T&... args ){ return scanf( (const char*)argc, args... ); }

    /*─······································································─*/

    template< class... T >
    int log( const T&... args ){
        auto  data = string::join( " ", args..., "\n" );
        pout( data ); return data.size();
    }

    template< class... T >
    int err( const T&... args ){
        auto  data = string::join( " ", args..., "\n" );
        perr( data ); return data.size();
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif