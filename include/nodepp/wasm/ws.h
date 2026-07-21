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
         FS_STATE_REUSE   = 0b01000001,
         FS_STATE_CLOSE   = 0b00000010,
         FS_STATE_READING = 0b00010000,
         FS_STATE_WRITING = 0b00100000,
         FS_STATE_KILL    = 0b00000100,
         FS_STATE_STOP    = 0b00001000,
         FS_STATE_DISABLE = 0b00001110
    };

protected:

    struct NODE {
        char state= STATE::FS_STATE_CLOSE; 
        EM_VAL    fd ; uchar_64 pd ;
        uchar_64 addr; uchar_64 tag;
    };  ptr_t<NODE> obj;

    void kill() const noexcept {
        if( !obj->fd.isUndefined() ) { obj->fd.call<EM_VAL>( "close" ); }
        process::revoke( obj->addr ); set_state( STATE::FS_STATE_KILL ); 
        obj->fd = EM_VAL::undefined();
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
        auto value= raw.as<EM_VAL>(); krn->wake(); switch( value["type"].as<int>() ){

            case 3: self->onError.emit( value["data"].as<EM_STRING>().c_str() ); 
            case 2: self->free(); return -1; break; 

            case 4: self->obj->state = STATE::FS_STATE_OPEN;
                    self->obj->fd    = value[ "data" ];
            return 1; break;

            case 0: self->onOpen   .emit(*self); 
                    self->onConnect.emit(*self);
            return 1; break;
                
            case 1: do {
                auto raw = value["data"].as<EM_STRING>(); 
                string_t buffer ( raw.c_str(), raw.length() );
                self->onData.emit( buffer ); 
            } while(0); return 1; break;

        }   return -1; });

        EM_EVAL( NODEPP_STRINGIFY ( 
            const addr= "${1}"; 
        try{const cli = new WebSocket( "${0}" );
            
            cli.binaryType= "arraybuffer";

            cli.onclose   = (e) => { Module.__invoke__( addr, { type: 2 } ); };
            cli.onopen    = (e) => { Module.__invoke__( addr, { type: 0 } ); };
            cli.onmessage = (e) => { Module.__invoke__( addr, { type: 1, data: e.data } ); };
            cli.onerror   = (e) => { Module.__invoke__( addr, { type: 3, data: "could not connect to the server" } ); };

            /*--------------------*/ Module.__invoke__( addr, { type: 4, data: cli } );

        } catch(e) {
            Module.__invoke__( addr, { type: 3, data: "could not connect to the server" } ); 
        }), url, obj->addr );

    }

    /*─······································································─*/

    void free() const noexcept {

        if( is_state( STATE::FS_STATE_KILL  ) ){ return; } kill();
        if(!is_state( STATE::FS_STATE_CLOSE ) ){ onDrain .emit (); }
       
        onClose.emit ();

        onError.clear(); onConnect.clear();
        onClose.clear(); onData   .clear();

    }

    void close() const noexcept {
        if( is_state ( STATE::FS_STATE_DISABLE ) ){ return; } onDrain.emit(); 
            set_state( STATE::FS_STATE_CLOSE   );
    free(); }

    /*─······································································─*/

    bool    is_closed() const noexcept { return  is_state ( STATE::FS_STATE_DISABLE ) || obj->state == 0x00; }
    bool is_available() const noexcept { return !is_closed(); }
    void set_reusable() const noexcept { obj->state |= STATE::FS_STATE_REUSE; }

    /*─······································································─*/

    uchar_64   get_fd() const noexcept { return (uchar_64) obj->fd.as_handle(); }
    uchar_64&  get_pd() const noexcept { return obj->pd ; }
    uchar_64&     tag() const noexcept { return obj->tag; }

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