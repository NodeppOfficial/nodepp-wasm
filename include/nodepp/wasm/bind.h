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

namespace nodepp { inline queue_t<string_t>& EM_QUEUE(){
thread_local static queue_t<string_t> out; 
    return out; 
}}

namespace nodepp { template< class... T >
EM_VAL EM_GET( const T&... args ) {
    return EM_VAL::global( args... );
}}

namespace nodepp { template< class... T >
EM_VAL EM_CALL( const EM_VAL& var, const T&... args ) {
    return var.call<EM_VAL>( args... );
}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { template< class... T >
EM_VAL EM_EVAL( const string_t& code, const T&... args ) {
if( code.empty() ){ return EM_VAL(); }

    string_t eval = string::join ( "", "(()=>{ try {",code,"} catch(e) { console.error(e); } })();" );

    auto out   = ptr_t<EM_VAL>  (0UL);
    auto addr  = process::invoke([=]( any_t raw ){ if( raw.has_value() ){
         out[0]= raw.as<EM_VAL> ();
    } return -1; });

    MAIN_THREAD_EM_ASM({
        Module.__call__( `${$0}`, eval( Module.__handle__( $1 ) ) );
    } , addr, EM_VAL( EM_STRING( regex::format( eval, args... ).get() ) ).as_handle() );

    return out[0];

}}


/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { inline void EM_NEXT(){
if( EM_QUEUE().empty() ){ return; }
    EM_EVAL ( string::join( EM_QUEUE(), "\n" ) ); 
    EM_QUEUE().clear();
}}

namespace nodepp { template< class... T >
void EM_PUSH( const string_t& code, const T&... args ){
if( code.empty() ){ return; }
    auto raw = string::join ( "", "try {",code,"} catch(e){ console.error(e); }" );
    auto out = regex::format( raw, args... ); EM_QUEUE().push( out );
}}

/*────────────────────────────────────────────────────────────────────────────*/

#define ___BIND( NAME ) EMSCRIPTEN_BINDINGS( NAME )
#define EM_BIND( NAME, CALLBACK ) ___BIND(__LINE__) { emscripten::function( NAME, CALLBACK ); }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/