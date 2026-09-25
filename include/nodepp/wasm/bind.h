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

namespace nodepp { inline EM_VAL& EM_MODULE(){ static EM_VAL out; return out; }}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { template< class... T >
EM_VAL EM_GET( const T&... args ) {
    return EM_VAL::global( args... );
}}

namespace nodepp { template< class... T >
EM_VAL EM_CALL( const EM_VAL& var, const T&... args ) {
    return var.call<EM_VAL>( args... );
}}

namespace nodepp { inline EM_VAL& EM_WINDOW() { 
thread_local static EM_VAL out = EM_GET( "window" ); 
    return out;
}}

namespace nodepp { inline EM_VAL& EM_DOCUMENT() { 
thread_local static EM_VAL out = EM_GET( "document" ); 
    return out;
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { template< class... T >
EM_VAL EM_EVAL( const string_t& code, const T&... args ) {
if( code.empty() ){ return EM_VAL(); }
    string_t eval = string::join ( "", "(()=>{",code,"})();" );
    /*----*/ eval = regex::format( eval, args... );
    return EM_MODULE().call<EM_VAL>("__bridge__",EM_STRING(eval.get()));
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { inline queue_t<string_t>& EM_QUEUE(){
    thread_local static queue_t<string_t> out; 
    return out; 
}}

namespace nodepp { inline void EM_NEXT(){
if( EM_QUEUE().empty() ){ return; }
    EM_EVAL( string::join( EM_QUEUE(), "\n" ) ); 
    EM_QUEUE().clear();
}}

namespace nodepp { template< class... T >
void EM_PUSH( const string_t& code, const T&... args ){
if( code.empty() ){ return; }
    auto raw = string::join ( "", "do{",code,"}while(0);" );
    auto out = regex::format( raw, args... );
    EM_QUEUE().push( out );
}}

/*────────────────────────────────────────────────────────────────────────────*/

#define ___BIND( NAME ) EMSCRIPTEN_BINDINGS( NAME )
#define EM_BIND( NAME, CALLBACK ) ___BIND(__LINE__) { emscripten::function( NAME, CALLBACK ); }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
