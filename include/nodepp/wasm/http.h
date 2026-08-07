/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_FETCH
#define NODEPP_WASM_FETCH

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { using header_t = map_t< string_t, string_t >; }
namespace nodepp { struct fetch_t {

    string_t  url    ;
    string_t  body   ;
    uchar     status ;
    header_t  headers;
    ulong     timeout= 60000;
    string_t  method = "GET";

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class http_t : public stream_t { 
public:

    uint      status = 200;
    string_t  version;
    header_t  headers;

    string_t  body  ;
    string_t  search;
    string_t  method;
    string_t  path  ;
    
    /*─······································································─*/

    http_t( EM_VAL fd, ulong size=NODEPP_CHUNK_SIZE ) : stream_t( fd, EM_VAL::undefined(), size ) {} 

    http_t() : stream_t() {}

    /*─······································································─*/

    promise_t<http_t,except_t> read_body( ulong timeout=60000UL ) const noexcept {

        auto self = type::bind( this ); set_timeout( timeout );

    return promise_t<http_t,except_t> ([=](
        res_t<http_t> res, rej_t<except_t> rej
    ){
        
        auto task = self->onDrain.once([&self,res](){ res( *self ); });

        process::poll( *self, POLL_STATE::READ | POLL_STATE::EDGE, coroutine::add( COROUTINE(){
            int c=0;
        coBegin

            while ( self->is_available() ){
            coWait((c=self->__read( self->get_buffer_data(), self->get_buffer_size() ))==-2 );
                if( c<0 ){ break; } self->body += string_t ( self->get_buffer().slice(0,c) );
            }
            
            self->onDrain.off(task); res( *self ); 

        coFinish
        }), 0UL );

    }); }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace http {

    inline promise_t<http_t,except_t> fetch( const fetch_t& fetch ) {
    return promise_t<http_t,except_t>([=]( 
           res_t<http_t> res, rej_t<except_t> rej 
    ){

        if( !url::is_valid( fetch.url ) ){ rej(except_t("invalid URL")); return; }

        auto addr = process::invoke([=]( any_t raw ){ do { 
        auto value= raw.as<EM_VAL>();

            if( value["type"].as<int>() == 0 ){ 
                rej( value["data"].as<EM_STRING>().c_str() ); 
            break; }

            auto keys = EM_GET("Object").call<EM_VAL>( "keys", value["headers"] );
            int  len  = keys ["length"].as<int>(); header_t hdrs;
            auto tmp  = value["headers"];

            for( int i=0; i<len; i++ ) {
                 EM_STRING key = keys[i] .as<EM_STRING>();
                 EM_STRING val = tmp[key].as<EM_STRING>();
                 hdrs[ key.c_str() ] = val.c_str();
            }

            auto uri  = value["url"]   .as<EM_STRING>();
            auto stte = value["status"].as<int>();

            http_t cli( value["data"] ); 
                   cli.status  = stte;
                   cli.headers = hdrs;
                   cli.search  = url::search( uri );
                   cli.path    = url::path  ( uri );
                   cli.set_timeout( fetch.timeout );

            res( cli );

        } while(0); return -1; });

        string_t obj = "{", hdr = json::stringify( fetch.headers );
        if( !fetch.body   .empty() ){ obj += regex::format( "body:   \"${0}\",", fetch.body    ); }
        if( !fetch.method .empty() ){ obj += regex::format( "method: \"${0}\",", fetch.method  ); }
        if(  fetch.timeout > 0     ){ obj += regex::format( "timeout:\"${0}\",", fetch.timeout ); }
        if( !fetch.headers.empty() ){ obj += regex::format( "headers:  ${0}  ,", hdr           ); }
        obj += "}";

        EM_EVAL( NODEPP_STRINGIFY ( fetch( "${0}", ${1} )

            .then( res => { const headerObj = {};

                res.headers.forEach( (value, key) => {
                    headerObj[key] = value;
                });

                Module.__invoke__("${2}", {
                    type   : 1          , 
                    headers: res.headers,
                    url    : res.url    ,
                    status : res.status ,
                    data   : res.body   , 
                });
                
            })

            .catch( err => {  Module.__invoke__( "${2}", {
                type: 0, data: err.message 
            }); });

        ), fetch.url, obj, addr );

    }); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/