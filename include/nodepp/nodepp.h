/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_NODEPP
#define NODEPP_NODEPP

/*────────────────────────────────────────────────────────────────────────────*/

#include "import.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {
    
    inline array_t<string_t>& NODEPP_ARGMNT(){ /*-*/ static array_t<string_t> /*-*/ out; return out; }
    inline kernel_t& /*----*/ NODEPP_EVLOOP(){ thread_local static kernel_t /*---*/ out; return out; }
    inline invoker_t<any_t> & NODEPP_INVOKE(){ thread_local static invoker_t<any_t> out; return out; }
    
    /*─······································································─*/

    template< class... T >
    int call( const T&... args ){ return NODEPP_INVOKE().emit( args... ); }

    template< class... T >
    void revoke( const T&... args ){ NODEPP_INVOKE().off( args... ); }

extern "C" {
    inline int external_call( EM_STRING address, EM_VAL value ){
        return call( string::to_u64( address ), value );
    }
}

    template< class... T >
    uint64_t invoke( const T&... args ){ return NODEPP_INVOKE().add( args... ); }
    
    /*─······································································─*/

    template< class... T >
    void await( const T&... args ){ while(NODEPP_EVLOOP().await( args... )==1){/*unused*/} }

    template< class... T >
    ptr_t<task_t> loop( const T&... args ){ return NODEPP_EVLOOP().loop_add( args... ); }

    template< class... T >
    ptr_t<task_t> poll( const T&... args ){ return NODEPP_EVLOOP().poll_add( args... ); }

    template< class... T >
    ptr_t<task_t> add ( const T&... args ){ return NODEPP_EVLOOP().loop_add( args... ); }
    
    /*─······································································─*/

    inline void clear( ptr_t<task_t> address ){ NODEPP_EVLOOP().off( address ); }
    inline void   off( ptr_t<task_t> address ){ NODEPP_EVLOOP().off( address ); }
    inline int   wake() /*-----------------*/ { return NODEPP_EVLOOP().wake (); }

    /*─······································································─*/

    inline bool should_close(){ return NODEPP_EVLOOP().should_close(); }
    inline bool        empty(){ return NODEPP_EVLOOP().empty(); }
    inline ulong        size(){ return NODEPP_EVLOOP().size (); }
    inline void        clear(){ /*--*/ NODEPP_EVLOOP().clear(); }

    /*─······································································─*/

    inline void exit( int err=0 ){ 
    if( should_close() ){ goto DONE; } do {
        auto *raw = &NODEPP_SHTDWN(); 
             *raw = true; clear(); 
    } while(0); DONE:; ::exit(err); }

    inline int next(){ 
        /*--*/ NODEPP_ALLOC ().next();
        return NODEPP_EVLOOP().next(); 
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#include "env.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    inline array_t<string_t>& arguments() { return NODEPP_ARGMNT(); }
    inline kernel_t& /*----*/ kernel   () { return NODEPP_EVLOOP(); }

    /*─······································································─*/

    inline void start(){

        NODEPP_SIGNAL().onSIGEXIT.once( [=](){ 
        process::exit(0); }); signal::start();

        auto addr= process::invoke([=]( any_t raw ){
        auto val = raw.as<EM_VAL>(); 
        auto &tmp= EM_MODULE(); tmp=val; return -1; });

        MAIN_THREAD_EM_ASM({
            Module.__handle__=( e )=>{ return emval_handles[e]; };
            Module.__bridge__=( e )=>{ return eval(e); };
            Module.__invoke__ ( `${$0}`, Module );
        },  addr );

    }

    /*─······································································─*/

    template< class... T >
    void error( const T&... msg ){ NODEPP_THROW_ERROR( msg... ); }

    /*─······································································─*/

    inline void wait(){ 
        while( !process::should_close() ){ process::next( ); }   
        /*------------------------------*/ process::clear();
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

EM_BIND( "__invoke__", nodepp::process::external_call );

/*────────────────────────────────────────────────────────────────────────────*/

#endif
