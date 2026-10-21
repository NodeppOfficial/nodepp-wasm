/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_WT
#define NODEPP_WASM_WT

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class wt_t : public stream_t {
protected:

    struct DONE {
        EM_VAL  fd ; uchar_64 addr ;
       ~DONE(){ 
            fd =EM_VAL::undefined(); 
            process::revoke( addr );
        }
    };  ptr_t<DONE> wt;

public:

    wt_t( const string_t& url ) noexcept : stream_t(), wt( new DONE() ) {

        auto self = type::bind( this );
        wt->addr  = process::invoke([=]( any_t raw ){
        auto value= raw.as<EM_VAL>(); switch( value["type"].as<int>() ){

            case 1: self->onError.emit (string_t(value["data"].as<EM_STRING>()));
            case 2: self->free(); /*--------------------------------*/ return -1; break; 
            case 0: self->stream_t::stream( value["data"][0], value["data"][1] ); break;
            
        } return -1; });

        EM_PUSH( NODEPP_STRINGIFY ( 
            const addr    = "${1}";    
        try{const cli     = new WebTransport( "${0}" );
            cli.binaryType= "arraybuffer";

            transport.createBidirectionalStream().then((stream)=>{

                const writer = stream.writable.getWriter();
                const reader = stream.readable.getReader();
                
                Module.__call__( addr, { type: 0, data: [ reader, writer ] } );

            })

            .fail((err)=>{
                Module.__call__( addr, { type: 1, data: "could not connect to the server" } ); 
            });

        } catch(e) {
            Module.__call__( addr, { type: 1, data: "webtransport not supported" } ); 
        }), url, wt->addr );

    }
    
    wt_t() noexcept : wt( new DONE() ), stream_t() {}

}; using udp_t = wt_t; }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/