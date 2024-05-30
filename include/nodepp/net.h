/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_NET
#define NODEPP_NET

/*────────────────────────────────────────────────────────────────────────────*/

#include "socket.h"
#include "poll.h"
#include "dns.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp {

/*────────────────────────────────────────────────────────────────────────────*/

class net_t {
protected:

    struct NODE {
        int                       state = 0;
        bool                      chck  = 1;
        agent_t                   agent;
        poll_t                    poll ;
        function_t<void,socket_t> func ;
    };  ptr_t<NODE> obj;
    
    /*─······································································─*/

    void init_poll_loop( ptr_t<const net_t>& self ) const noexcept { process::poll::add([=](){
        if( self->is_closed() ){ return -1; } if( self->obj->poll.emit() != -1 ) { auto x = self->obj->poll.get_last_poll();
            if( x[0] == 0 ){ socket_t cli(x[1]); cli.set_sockopt(self->obj->agent); self->onSocket.emit(cli); self->obj->func(cli); }
            if( x[0] == 1 ){ socket_t cli(x[1]); cli.set_sockopt(self->obj->agent); self->onSocket.emit(cli); self->obj->func(cli); }
        #if _KERNEL == NODEPP_KERNEL_WINDOWS
            if( x[0] ==-1 ){ ::closesocket(x[1]); }
        #else
            if( x[0] ==-1 ){ ::close(x[1]); }
        #endif
        }   return 1;
    }); }
    
public: net_t() noexcept : obj( new NODE() ) {}

    event_t<socket_t> onConnect;
    event_t<socket_t> onSocket;
    event_t<>         onClose;
    event_t<except_t> onError;
    event_t<socket_t> onOpen;
    
    /*─······································································─*/
    
    net_t( decltype(NODE::func) _func, agent_t* opt=nullptr ) noexcept : obj( new NODE() )
         { obj->func=_func; obj->agent=opt==nullptr?agent_t():*opt; }
    
    /*─······································································─*/
    
    void     close() const noexcept { if( obj->state<0 ){ return; } obj->state=-1; onClose.emit(); }
    bool is_closed() const noexcept { return obj == nullptr ? 1 : obj->state < 0; }
    
    /*─······································································─*/

    void poll( bool chck ) const noexcept { obj->chck = chck; }
    
    /*─······································································─*/

    void listen( const string_t& host, int port, decltype(NODE::func)* cb=nullptr ) const noexcept {
        if( obj->state == 1 ){ return; } obj->state = 1; auto self = type::bind( this );
        if( dns::lookup(host).empty() ){ _EERROR(onError,"dns couldn't get ip"); close(); return; }

        socket_t *sk = new socket_t; 
                  sk->PROT = IPPROTO_NET;
                  sk->socket( dns::lookup(host), port ); 
        
        if(   sk->bind()  < 0 ){ _EERROR(onError,"Error while binding NET");   close(); delete sk; return; }
        if( sk->listen()  < 0 ){ _EERROR(onError,"Error while listening NET"); close(); delete sk; return; }
        if( obj->chck == true ){ init_poll_loop( self ); }

        onOpen.emit(*sk); if( cb != nullptr ){ (*cb)(*sk); }
        
        process::task::add([=](){
            static int _accept = 0; 
        coStart

            while( sk != nullptr ){ _accept = sk->_accept();
                if( self->is_closed() || !sk->is_available() )
                  { break; } elif ( _accept != -2 )
                  { break; } coYield(1);
            }
            
            if( _accept == -1 ){ _EERROR(self->onError,"Error while accepting NET"); coGoto(2); }
            elif ( !sk->is_available() || self->is_closed() ){ coGoto(2); }
            elif ( self->obj->chck == true ){ self->obj->poll.push_read(_accept); coGoto(0); }
            else { socket_t cli( _accept ); if( cli.is_available() ){ 
                   process::poll::add([=]( socket_t cli ){
                        cli.set_sockopt( self->obj->agent ); 
                        self->onSocket.emit( cli ); 
                        self->obj->func( cli ); 
                        return -1;
                   }, cli );
            } coGoto(0); }

            coYield(2); self->close(); delete sk;
        
        coStop
        });

    }

    void listen( const string_t& host, int port, decltype(NODE::func) cb ) const noexcept { 
         listen( host, port, &cb ); 
    }

    /*─······································································─*/

    void connect( const string_t& host, int port, decltype(NODE::func)* cb=nullptr ) const noexcept {
        if( obj->state == 1 ){ return; } obj->state = 1; auto self = type::bind( this );
        if( dns::lookup(host).empty() )
          { _EERROR(onError,"dns couldn't get ip"); close(); return; }

        socket_t sk = socket_t(); 
                 sk.PROT = IPPROTO_NET;
                 sk.socket( dns::lookup(host), port );
                 sk.set_sockopt( obj->agent );

        if( sk.connect() < 0 ){ _EERROR(onError,"Error while connecting NET"); close(); return; }
        if( cb != nullptr ){ (*cb)(sk); } sk.onClose.on([=](){ self->close(); });
        onOpen.emit(sk); sk.onOpen.emit(); onSocket.emit(sk); obj->func(sk);
    }

    void connect( const string_t& host, int port, decltype(NODE::func) cb ) const noexcept { 
         connect( host, port, &cb ); 
    }

};

/*────────────────────────────────────────────────────────────────────────────*/

namespace net {

    net_t server( const net_t& server ){ server.onSocket([=]( socket_t cli ){
        ptr_t<_file_::read> _read = new _file_::read;
        cli.onDrain.once([=](){ cli.free(); });

        server.onConnect.once([=]( socket_t cli ){ process::poll::add([=](){
            if(!cli.is_available() )    { cli.close(); return -1; }
            if((*_read)(&cli)==1 )      { return 1; }
            if(  _read->state<=0 )      { return 1; }
            cli.onData.emit(_read->data); return 1;
        }) ; });

        process::task::add([=](){
            server.onConnect.emit(cli); return -1;
        });

    }); server.poll( false ); return server; }

    /*─······································································─*/

    net_t server( agent_t* opt=nullptr ){
        auto server = net_t( [=]( socket_t /*unused*/ ){}, opt );
        net::server( server ); return server; 
    }

    /*─······································································─*/

    net_t client( const net_t& client ){ client.onOpen.once([=]( socket_t cli ){
        ptr_t<_file_::read> _read = new _file_::read;
        cli.onDrain.once([=](){ cli.free(); });

        process::poll::add([=](){
            if(!cli.is_available() )    { cli.close(); return -1; }
            if((*_read)(&cli)==1 )      { return 1; }
            if(  _read->state<=0 )      { return 1; }
            cli.onData.emit(_read->data); return 1;
        }); 

    }); return client; }

    /*─······································································─*/

    net_t client( agent_t* opt=nullptr ){
        auto client = net_t( [=]( socket_t /*unused*/ ){}, opt );
        net::client( client ); return client; 
    }

}

/*────────────────────────────────────────────────────────────────────────────*/

}

#endif