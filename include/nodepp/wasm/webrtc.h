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

namespace nodepp { struct url_webrtc_t {
    string_t url ; string_t user; 
    string_t pass; /*----------*/
};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { struct agent_webrtc_t {

    string_t peer_id         ;
    bool     order           = false;
    bool     ice_restart     = true ;
    int      max_retransmits = 0;
    ptr_t<url_webrtc_t>  url ;

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
        int state = FS_STATE_CLOSE ;
        EM_VAL  pc,dc; uchar_64 pd ; 
        uchar_64 addr; uchar_64 tag;
        agent_webrtc_t /**/ agent;
    };  ptr_t<NODE> obj;

    void kill() const noexcept {
        if( !obj->dc.isUndefined() ) { obj->dc.call<EM_VAL>( "close" ); }
        if( !obj->pc.isUndefined() ) { obj->pc.call<EM_VAL>( "close" ); }
        process::revoke( obj->addr ); set_state( STATE::FS_STATE_KILL ); 
        obj->dc = EM_VAL::undefined();
        obj->pc = EM_VAL::undefined();
    }

public:

    event_t<>         onRenegociation;
    event_t<webrtc_t> onConnect;
    event_t<string_t> onSignal ;
    event_t<>         onDrain  ;
    event_t<except_t> onError  ;
    event_t<>         onClose  ;
    event_t<string_t> onData   ;
    event_t<webrtc_t> onOpen   ;

    /*─······································································─*/

    webrtc_t( agent_webrtc_t* args ) : obj( new NODE() ) {
        obj->agent = args==nullptr ? agent_webrtc_t() : *args;
 
        if( obj->agent.url.empty() )
          { NODEPP_THROW_ERROR( "invalid stun server" ); }

        if( obj->agent.peer_id.empty() )
          { NODEPP_THROW_ERROR( "invalid peer ID" ); }

        auto krn  = type::bind( process::NODEPP_EVLOOP() );
        auto self = type::bind( this );

        obj->addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); krn->wake(); switch( value["type"].as<int>() ){

            case 0: self->onSignal.emit( value["data"].as<EM_STRING>().c_str() ); return  1; break;
            case 4: self->onError .emit( value["data"].as<EM_STRING>().c_str() );
            case 3: self->free(); /*-------------------------------------------*/ return -1; break;

            case 5: self->obj->pc = value["data"]; return 1; break;
            case 7: self->obj->dc = value["data"]; return 1; break;
            case 6: self->onRenegociation.emit (); return 1; break;

            case 2: do {
                auto raw = value["data"].as<EM_STRING>(); 
                string_t buffer ( raw.c_str(), raw.length() );
                self->onData.emit( buffer ); 
            } while(0); return 1; break;

            case 1: self->set_state( STATE::FS_STATE_OPEN );
                    self->onConnect.emit( *self );
                    self->onOpen   .emit( *self );
            return 1; /*--------*/ break;

        } return -1; });

        auto tmp0 = array_t<object_t>(); for( auto x: obj->agent.url ){
        auto tmp  = object_t();
             if( !x.url .empty() ){ tmp["urls"]       = x.url ; }
             if( !x.user.empty() ){ tmp["username"]   = x.user; }
             if( !x.pass.empty() ){ tmp["credential"] = x.pass; }
        tmp0.push( tmp ); }

        auto tmp1 = json::stringify( object_t({ 
        { "iceServers"        , tmp0         },
        { "iceTransportPolicy", "all"        },
        { "bundlePolicy"      , "max-bundle" },
        { "rtcpMuxPolicy"     , "require"    } }) );

        auto tmp2 = json::stringify( object_t({
        { "ordered"       , obj->agent.order           },
        { "maxRetransmits", obj->agent.max_retransmits } }) );

        EM_EVAL( _STRING_(
            const addr= "${0}";
        try{const pc  = new RTCPeerConnection( ${1} );

            pc.ondatachannel = ({ channel }) => {
            let dc = channel ; dc.binaryType= "arraybuffer";
                dc.onerror   = (e)=>{ Module.__invoker__( addr, { type: 4, data: e.data }); };
                dc.onmessage = (e)=>{ Module.__invoker__( addr, { type: 2, data: e.data }); };
                dc.onopen    = ( )=>{ Module.__invoker__( addr, { type: 1 }); };
                dc.onclose   = ( )=>{ Module.__invoker__( addr, { type: 3 }); };
                /*-----------------*/ Module.__invoker__( addr, { type: 7, data: dc });
            };

            pc.addEventListener( "iceconnectionstatechange", (e) => {
            if( pc.iceConnectionState === "failed" ){ 
            if( pc.connectionState    === "failed" ){ 
                /*-------------*/ Module.__invoker__( addr, { type: 3 } ); return; }
                pc.restartIce();  Module.__invoker__( addr, { type: 6 } );
            }});

            pc.onicecandidate = (e) => { if( e.candidate ){ 
                Module.__invoker__( addr, { type: 0, data: btoa( JSON.stringify( e.candidate ) ) });
            }}; Module.__invoker__( addr, { type: 5, data: pc } ); 

        } catch(e) {
            Module.__invoker__( addr, { type: 4, data: "could not connect to the server" });
        }), obj->addr, tmp1 );

    }

    webrtc_t() : obj( new NODE() ) {}

    /*─······································································─*/

   ~webrtc_t() { if( obj.count()>1 ){ return; } free(); }

    /*─······································································─*/

    bool    is_closed() const noexcept { return  is_state ( STATE::FS_STATE_DISABLE ) || obj->state == 0x00; }
    bool is_connected() const noexcept { return !is_closed() && is_state( STATE::FS_STATE_OPEN ); }
    bool is_available() const noexcept { return !is_closed(); }

    /*─······································································─*/

    uchar_64   get_fd() const noexcept { return (uchar_64) obj->dc.as_handle(); }
    uchar_64&  get_pd() const noexcept { return obj->pd ; }
    uchar_64&     tag() const noexcept { return obj->tag; }

    /*─······································································─*/

    void free() const noexcept {

        if( is_state( STATE::FS_STATE_KILL  ) ){ return; } kill();
        if(!is_state( STATE::FS_STATE_CLOSE ) ){ onDrain .emit (); }
       
        onClose.emit (); onRenegociation.clear();
        onError.clear(); onConnect.clear();
        onClose.clear(); onData   .clear();

    }

    void close() const noexcept {
        if( is_state ( STATE::FS_STATE_DISABLE ) ){ return; } onDrain.emit(); 
            set_state( STATE::FS_STATE_CLOSE   );
    free(); }

    /*─······································································─*/

    promise_t<string_t,except_t> create_offer() const noexcept {
           auto self = type::bind( this );
    return promise_t<string_t,except_t>([=](
           res_t<string_t> res, rej_t<except_t> rej
    ){

        if( self->is_state( STATE::FS_STATE_KILL ) ){ rej( "webrtc closed" ); return; } 

        auto addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); switch( value["type"].as<int>() ){
            case  0: rej( value["data"].as<EM_STRING>().c_str() ); break;
            case  1: res( value["data"].as<EM_STRING>().c_str() ); break;
        } return -1; });

        auto tmp1 = json::stringify( object_t({
        { "iceRestart"    , obj->agent.ice_restart     } }) );

        auto tmp2 = json::stringify( object_t({
        { "ordered"       , obj->agent.order           },
        { "maxRetransmits", obj->agent.max_retransmits } }) );

        EM_EVAL( _STRING_(( async ()=>{
            const addr = "${0}";
            const ctx  = "${5}";
        
        try{

            const pc = Module.__handle__   ( ${1} );
            const dc = pc.createDataChannel( "${4}", ${3} );

            dc.binaryType= "arraybuffer";

            dc.onerror   = (e)=>{ Module.__invoker__( ctx, { type: 4, data: e.data }); };
            dc.onmessage = (e)=>{ Module.__invoker__( ctx, { type: 2, data: e.data }); };
            dc.onopen    = ( )=>{ Module.__invoker__( ctx, { type: 1 }); };
            dc.onclose   = ( )=>{ Module.__invoker__( ctx, { type: 3 }); };
            
            const of = await pc.createOffer /*--*/ (${2});
            /*------*/ await pc.setLocalDescription( of );
            
            Module.__invoker__( ctx , { type: 7, data: dc });
            Module.__invoker__( addr, { type: 1, data: btoa( JSON.stringify( pc.localDescription ) ) });

        } catch( err ) {

            Module.__invoker__( addr, { type: 0, data: err.message });

        }})(); ), addr, self->obj->pc.as_handle(), tmp1, tmp2, obj->agent.peer_id, self->obj->addr );

    }); }

    /*─······································································─*/

    promise_t<string_t,except_t> accept_offer( string_t peer_sdp ) const noexcept {
           auto self = type::bind( this );
    return promise_t<string_t,except_t>([=](
           res_t<string_t> res, rej_t<except_t> rej
    ){

        if( self->is_state( STATE::FS_STATE_KILL ) ){ rej( "webrtc closed" ); return; } 

        auto addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); switch( value["type"].as<int>() ){
            case  0: rej( value["data"].as<EM_STRING>().c_str() ); break;
            case  1: res( value["data"].as<EM_STRING>().c_str() ); break;
        } return -1; });

        auto tmp = json::stringify( object_t({
        { "iceRestart", obj->agent.ice_restart } }) );

        EM_EVAL( _STRING_(( async ()=>{

            const addr = "${0}"; 
        
        try{
            
            const pc = Module.__handle__ ( ${1} );
            await pc.setRemoteDescription( JSON.parse(atob("${2}")) );

            const an = await pc.createAnswer(${3});
            await pc.setLocalDescription    ( an );
            
            Module.__invoker__( addr, { type: 1, data: btoa( JSON.stringify( pc.localDescription ) ) });

        } catch( err ) {

            Module.__invoker__( addr, { type: 0, data: err.message });

        }})(); ), addr, self->obj->pc.as_handle(), peer_sdp, tmp );

    }); }

    /*─······································································─*/

    promise_t<string_t,except_t> accept_answer( string_t peer_sdp ) const noexcept {
           auto self = type::bind( this );
    return promise_t<string_t,except_t>([=](
           res_t<string_t> res, rej_t<except_t> rej
    ){

        if( self->is_state( STATE::FS_STATE_KILL ) ){ rej( "webrtc closed" ); return; } 

        auto addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); switch( value["type"].as<int>() ){
            case  0: rej( value["data"].as<EM_STRING>().c_str() ); break;
            case  1: res( value["data"].as<EM_STRING>().c_str() ); break;
        } return -1; });
        
        EM_EVAL( _STRING_(( async ()=>{
        try {

            const pc= Module.__handle__( ${1} );
            await pc.setRemoteDescription( JSON.parse(atob("${2}")) );

            Module.__invoker__( "${0}", { type: 1, data: btoa( JSON.stringify( pc.localDescription ) ) });

        } catch( err ) {
            
            Module.__invoker__( "${0}", { type: 0, data: err.message } );

        }})(); ), addr, self->obj->pc.as_handle(), peer_sdp );

    }); }

    /*─······································································─*/

    int clear_ice_candidate() const noexcept { 

        if( !is_available() ){ return -1; } 

        EM_EVAL( _STRING_(( async ()=>{
        Module.__handle__( ${0} );.restartIce(); })(); ), obj->pc.as_handle() );

    return 1; }

    /*─······································································─*/

    promise_t<webrtc_t,except_t> append_ice_candidate( string_t candidate ) const noexcept { 
           auto self = type::bind( this );
    return promise_t<webrtc_t,except_t> ([=](
           res_t<webrtc_t> res, rej_t<except_t> rej
    ){

        if( self->is_state( STATE::FS_STATE_KILL ) ){ rej( "webrtc closed" ); return; } 

        auto addr = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); switch( value["type"].as<int>() ){
            case  0: rej( value["data"].as<EM_STRING>().c_str() ); break;
            case  1: res( *self ); /*---------------------------*/ break;
        } return -1; });
        
        EM_EVAL( _STRING_(( async ()=>{
        try {

            const pc  = Module.__handle__( ${1} );
            await pc.addIceCandidate( JSON.parse( atob("${2}") ) );

            Module.__invoker__( "${0}", { type: 1 } );

        } catch( err ) {
            
            Module.__invoker__( "${0}", { type: 0, data: err.message } );

        }})(); ), addr, self->obj->pc.as_handle(), candidate );

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