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

namespace nodepp { template< class... T >
EM_VAL EM_GET( const T&... args ) {
    return EM_VAL::global( args... );
}}

namespace nodepp { EM_VAL& EM_WINDOW() { 
    static EM_VAL out = EM_GET( "window" ); 
    return out;
}}

namespace nodepp { EM_VAL& EM_DOCUMENT() { 
    static EM_VAL out = EM_GET( "document" ); 
    return out;
}}

namespace nodepp { template< class... T >
EM_VAL EM_CALL( const EM_VAL& var, const T&... args ) {
    return var.call<EM_VAL>( args... );
}}

namespace nodepp { template< class... T >
EM_VAL EM_EVAL( const string_t& code, const T&... args ) {
    string_t eval = regex::format( "(()=>{${0}})();",code);
    /*----*/ eval = regex::format( eval, args... );
    return EM_VAL::global().call<EM_VAL>("eval",EM_STRING(eval.get()));
}}

namespace nodepp { 
object_t EM_JSON( const EM_VAL& value ) {
    return json::parse( EM_CALL( EM_GET("JSON"), "stringify", value ).as<EM_STRING>().c_str() );
}}

/*────────────────────────────────────────────────────────────────────────────*/

#define BIND_ADD( NAME, CALLBACK ) emscripten::function( NAME, CALLBACK );
#define BIND_RUN( ...) emscripten_run_script( #__VA_ARGS__ )
#define BIND( MODULE ) EMSCRIPTEN_BINDINGS( MODULE )

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
