/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_WS
#define NODEPP_WASM_WS

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class ws_t {
protected:

    bool is_state( uchar value ) const noexcept {
        if( obj->state & value ){ return true; }
    return false; }

    void set_state( uchar value ) const noexcept {
    if( obj->state & STATE::FS_STATE_KILL ){ return; }
        obj->state = value;
    }

    enum STATE {
         FS_STATE_UNKNOWN = 0b00000000,
         FS_STATE_OPEN    = 0b00000001,
         FS_STATE_CLOSE   = 0b00000010,
         FS_STATE_KILL    = 0b00000100,
         FS_STATE_REUSE   = 0b00001000,
         FS_STATE_DISABLE = 0b00001110
    };

protected:

    struct NODE {
        char state = STATE::FS_STATE_CLOSE; 
        EM_VAL fd; string_t addr;
    };  ptr_t<NODE> obj;

    void kill() const noexcept {
        set_state( STATE::FS_STATE_KILL );
        obj->fd.call<EM_VAL>( "close" );
        process::revoke( obj->addr );
    }

public: ws_t() noexcept : obj( new NODE() ){}

    event_t<ws_t>      onConnect;
    event_t<>          onDrain;
    event_t<except_t>  onError;
    event_t<>          onClose;
    event_t<ws_t>      onOpen;
    event_t<string_t>  onData;

    /*─······································································─*/

   ~ws_t() noexcept { if( obj.count() > 1 ) { return; } free(); }

    /*─······································································─*/

    ws_t( const string_t& url ) noexcept : obj( new NODE() ) {
        
        auto krn  = type::bind( process::NODEPP_EVLOOP() );
        auto self = type::bind( this );

        obj->addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); krn->wake();

            switch( value["type"].as<int>() ){

                case 3: self->onError.emit( value["data"].as<EM_STRING>().c_str() ); break; 
                case 0: do {

                    self->obj->state = STATE::FS_STATE_OPEN;
                    self->obj->fd    = value[ "data" ];
                    self->onOpen   .emit(*self); 
                    self->onConnect.emit(*self);

                } while(0); return 1; break;
                
                case 1: do {

                    auto raw = value["data"].as<EM_STRING>(); 
                    string_t buffer ( raw.c_str(), raw.length() );
                    self->onData.emit( buffer ); 

                } while(0); return 1; break;

            }

        return -1; });

        EM_EVAL( _STRING_(( function(){
            
            const cli = new WebSocket( "${0}" ); const addr= "${2}"; 
                  cli.binaryType= "arraybuffer";

            cli.onclose = (e) => {
                ${1}._nodepp_invoke_( addr, { type: 2 } );
            };

            cli.onopen  = (e) => {
                ${1}._nodepp_invoke_( addr, { type: 0, data: cli } );
            };

            cli.onmessage = (e) => {
                ${1}._nodepp_invoke_( addr, { type: 1, data: e.data } );
            };

            cli.onerror = (e) => {
                ${1}._nodepp_invoke_( addr, { type: 3, data: "could not connect to the server" } );
            };

        })(); ), url, NODEPP_MODULE_NAME, obj->addr );

    }

    /*─······································································─*/

    void free() const noexcept { if( !is_available() ){ return; } 
        onConnect.clear(); kill();
        onError  .clear();
        onData   .clear(); onClose.emit();
    }

    void close() const noexcept { if( !is_available() ){ return; }
         onDrain.emit(); free();
    }

    /*─······································································─*/

    bool is_closed()    const noexcept { return is_state( STATE::FS_STATE_DISABLE ); }
    bool is_available() const noexcept { return !is_closed(); }
    int  get_fd()       const noexcept { return (int) &obj->fd; }

    /*─······································································─*/

    string_t read( ulong /*unused*/ ) const noexcept { return nullptr; }

    int write( string_t msg ) const noexcept {
        if( msg.empty() ){ return 0; } if( is_closed() ){ return -1; }
        obj->fd.call<EM_VAL>( "send", EM_STRING( msg.get(), msg.size() ) );
        return msg.size();
    }

}; using tcp_t = ws_t; }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/