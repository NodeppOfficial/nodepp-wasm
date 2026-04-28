/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_WEBRTC
#define NODEPP_WASM_WEBRTC

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { struct fetch_webrtc_t {

    string_t url             = "stun:stun.l.google.com:19302";
    string_t peer_id         ;
    bool     order           = false;
    bool     ice_restart     = false;
    int      max_retransmits = 0;

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class webrtc_t {
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
        EM_VAL pc, dc; 
        string_t addr;
        int state = 0;
    };  ptr_t<NODE> obj;

    void kill() const noexcept {
        set_state( STATE::FS_STATE_KILL );
        obj->dc.call<EM_VAL>( "close" );
        process::revoke( obj->addr );
    }

public:

    event_t<webrtc_t> onConnect;
    event_t<string_t> onSignal ;
    event_t<>         onDrain  ;
    event_t<except_t> onError  ;
    event_t<>         onClose  ;
    event_t<string_t> onData   ;
    event_t<webrtc_t> onOpen   ;

    /*─······································································─*/

    webrtc_t( fetch_webrtc_t* args=nullptr ) : obj( new NODE() ) {
        auto conf = args==nullptr ? fetch_webrtc_t() : *args;

        if( conf.url.empty() )
          { onError.emit( "invalid stun server" ); return; }

        if( conf.peer_id.empty() )
          { onError.emit( "invalid peer ID" ); return; }

        auto krn  = type::bind( process::NODEPP_EVLOOP() );
        auto self = type::bind( this );
        obj->addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); krn->wake();
        switch( value["type"].as<int>() ){

            case 0: self->onSignal .emit( value["data"].as<EM_STRING>().c_str() ); return 1; break;
            case 4: self->onError  .emit( value["data"].as<EM_STRING>().c_str() ); /*-----*/ break;

            case 2: do {
                auto raw = value["data"].as<EM_STRING>(); 
                string_t buffer ( raw.c_str(), raw.length() );
                self->onData.emit( buffer ); 
            } while(0); return 1; break;

            case 5: self->set_state( STATE::FS_STATE_OPEN );
                    self->obj->dc = value["data"][1]; 
                    self->obj->pc = value["data"][0];
            return 1; /*--------*/ break;

            case 1: self->onConnect.emit( *self );
                    self->onOpen   .emit( *self );
            return 1; /*--------*/ break;

        } return -1; });

        EM_EVAL( _STRING_(( function(){

            const obj = ${2}; const addr = "${0}";
            const pc  = new RTCPeerConnection({ iceServers: [{ urls: obj["url"] }] });
            const dc  = pc.createDataChannel ( obj["peer_id"], { ordered: obj["order"], maxRetransmits: obj["max"] }); 
            
            dc.onmessage      = (e) => ${1}._nodepp_invoke_( addr, { type: 2, data: e.data });
            dc.onopen         = ( ) => ${1}._nodepp_invoke_( addr, { type: 1 });
            dc.onclose        = ( ) => ${1}._nodepp_invoke_( addr, { type: 3 });
            pc.onicecandidate = (e) => { if( e.candidate ){ 
                ${1}._nodepp_invoke_( addr, { type: 0, data: btoa( JSON.stringify( e.candidate ) ) });
            }}; ${1}._nodepp_invoke_( addr, { type: 5, data: [ pc, dc ] } );

        })(); ), obj->addr, NODEPP_MODULE_NAME, json::stringify( object_t({

            { "id"         , conf.peer_id         },
            { "url"        , conf.url             },
            { "order"      , conf.order           },
            { "ice_restart", conf.ice_restart     },
            { "max"        , conf.max_retransmits }

        }) ));

    }

    /*─······································································─*/

   ~webrtc_t() { if( obj.count()>1 ){ return; } free(); }

    /*─······································································─*/

    bool is_closed()    const noexcept { return is_state( STATE::FS_STATE_DISABLE ); }
    bool is_available() const noexcept { return !is_closed(); }
    int  get_fd()       const noexcept { return (int) &obj->pc; }

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

    promise_t<string_t,except_t> create_offer() const noexcept {
    return promise_t<string_t,except_t>([=](
           res_t<string_t> res, rej_t<except_t> rej
    ){

        if( !is_available() ){ rej( "webrtc closed" ); return; } 
        auto self = type::bind( this );

        auto addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); switch( value["type"].as<int>() ){
            case  0: rej( value["data"].as<EM_STRING>().c_str() ); break;
            case  1: res( value["data"].as<EM_STRING>().c_str() ); break;
        } return -1; });

        EM_EVAL( _STRING_( ( async function(){
        try {

            const pc    = emval_handles[${1}];
            const offer = await pc.createOffer( );
            await pc.setLocalDescription( offer );

            ${2}._nodepp_invoke_( "${0}", { type: 1, data: btoa( JSON.stringify( pc.localDescription ) ) });

        } catch( err ) {

            ${2}._nodepp_invoke_( "${0}", { type: 0, data: err.message });

        }} )() ), addr, obj->pc.as_handle(), NODEPP_MODULE_NAME );

    }); }

    /*─······································································─*/

    promise_t<string_t,except_t> accept_offer( string_t peer_sdp ) const noexcept {
    return promise_t<string_t,except_t>([=](
           res_t<string_t> res, rej_t<except_t> rej
    ){

        if( !is_available() ){ rej( "webrtc closed" ); return; } 
        auto self = type::bind( this );

        auto addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); switch( value["type"].as<int>() ){
            case  0: rej( value["data"].as<EM_STRING>().c_str() ); break;
            case  1: res( value["data"].as<EM_STRING>().c_str() ); break;
        } return -1; });

        EM_EVAL( _STRING_( ( async function(){
        try {

            const pc = emval_handles[${1}];

            await pc.setRemoteDescription({ type: "offer", sdp: "${3}" });
            const answer = await pc.createAnswer();
            await pc.setLocalDescription( answer );
            
            ${2}._nodepp_invoke_( "${0}", { type: 1, data: btoa( JSON.stringify( pc.localDescription ) ) });

        } catch( err ) {

            ${2}._nodepp_invoke_( "${0}", { type: 0, data: err.message });

        }} )() ), addr, obj->pc.as_handle(), NODEPP_MODULE_NAME, peer_sdp );

    }); }

    /*─······································································─*/

    promise_t<string_t,except_t> accept_answer( string_t peer_sdp ) const noexcept {
    return promise_t<string_t,except_t>([=](
           res_t<string_t> res, rej_t<except_t> rej
    ){

        if( !is_available() ){ rej( "webrtc closed" ); return; } 
        auto self = type::bind( this );

        auto addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); switch( value["type"].as<int>() ){
            case  0: rej( value["data"].as<EM_STRING>().c_str() ); break;
            case  1: res( value["data"].as<EM_STRING>().c_str() ); break;
        } return -1; });
        
        EM_EVAL( _STRING_(( async function(){
        try {

            const pc= emval_handles[${1}];
            await pc.setRemoteDescription({ type: "answer", sdp: "${3}" });

            ${2}._nodepp_invoke_( "${0}", { type: 1, data: btoa( JSON.stringify( pc.localDescription ) ) });

        } catch( err ) {
            
            ${2}._nodepp_invoke_( "${0}", { type: 0, data: err.message } );

        }})() ), addr, obj->pc.as_handle(), NODEPP_MODULE_NAME, peer_sdp );

    }); }

    /*─······································································─*/

    promise_t<webrtc_t,except_t> append_ice_candidate( object_t candidate ) const noexcept { 
    return promise_t<webrtc_t,except_t> ([=](
           res_t<webrtc_t> res, rej_t<except_t> rej
    ){

        if( !is_available() ){ rej( "webrtc closed" ); return; } 
        auto self = type::bind( this );

        auto addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); switch( value["type"].as<int>() ){
            case  0: rej( value["data"].as<EM_STRING>().c_str() ); break;
            case  1: res( *self ); /*---------------------------*/ break;
        } return -1; });
        
        EM_EVAL( _STRING_(( async function(){
        try {

            const pc  = emval_handles[${1}];
            await pc.addIceCandidate( ${2} );

            ${3}._nodepp_invoke_( "${0}", { type: 1 } );

        } catch( err ) {
            
            ${3}._nodepp_invoke_( "${0}", { type: 0, data: err.message } );

        }})() ), addr, obj->pc.as_handle(), json::stringify( candidate ), NODEPP_MODULE_NAME );

    });}

    /*─······································································─*/

    string_t read( ulong /*unused*/ ) const noexcept { return nullptr; }

    int write( string_t msg ) const noexcept {
        if( msg.empty() ){ return 0; } if( is_closed() ){ return -1; }
        obj->dc.call<EM_VAL>( "send", EM_STRING( msg.get(), msg.size() ) );
        return msg.size();
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/