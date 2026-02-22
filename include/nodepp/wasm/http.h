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

#include <emscripten/fetch.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { using header_t = map_t< string_t, string_t >; }

namespace nodepp { struct fetch_t {

    string_t  url;
    string_t  body;
    string_t  code;
    uchar     status;
    header_t  headers;
    string_t  filename;
    ulong     timeout= 0;
    string_t  method = "GET";

};}

namespace nodepp { struct http_t : public fetch_t, public file_t { public:

    http_t( emscripten_fetch_t* args, string_t file, header_t headers ) noexcept : fetch_t({}), file_t(file,"r") {
        this->code    = args->statusText; this->status  = args->status;
        this->url     = args->url; /*--*/ this->headers = headers;
        this->filename= file;
    }

   ~http_t() noexcept { if( obj.count()>1 ){ return; } ::remove( filename.get() ); }

    http_t() noexcept : fetch_t({}), file_t() {}

}; using https_t = http_t; }

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace generator { GENERATOR( fetch ) {
protected:

    struct NODE {
        rej_t<except_t> rej; res_t<http_t> res;
        array_t<const char*> hdr;
        file_t file; fetch_t ctx;
        string_t filename;
        bool state =false;
    }; ptr_t<NODE> obj;
    
    /*─······································································─*/

    static void progress( emscripten_fetch_t* args ) {
        auto data = string_t( args->data, args->numBytes );
        auto self = type::cast<fetch>( args->userData );
        /**/ self->obj->file.write( data );
    }

    /*─······································································─*/

    static void fail( emscripten_fetch_t* args ) {

        auto self= type::cast<fetch>(args->userData);
        auto rej = self->obj->rej; header_t headers ;
        auto res = self->obj->res; self->close();

        rej( except_t( "Something Went Wrong" ) );
        emscripten_fetch_close( args ); return;

    }

    /*─······································································─*/

    static void callback( emscripten_fetch_t* args ) {

        auto self= type::cast<fetch>(args->userData);
        auto rej = self->obj->rej; header_t headers ;
        auto res = self->obj->res; self->close();

        if( args->status == 0 ){
            rej( except_t( "Something Went Wrong" ) );
            emscripten_fetch_close( args ); return;
        }   /*>>*/ progress( args ); /*<<*/

        string_t raw ( emscripten_fetch_get_response_headers_length( args ), '\0' );
        emscripten_fetch_get_response_headers( args, raw.get(), raw.size() );

        forEach( x, string::split( raw, '\n' ) ){
            auto y = x.find( ": " ); if( y == nullptr ){ break; }
            headers[ x.slice(0,y[0]) ] = x.slice( y[1], -2 );
        }   
        
        http_t out( args, self->obj->filename, headers );
        emscripten_fetch_close( args ); res( out ); 

    }

public:

    fetch( fetch_t ctx ) : obj( new NODE() ) {
        if( !url::is_valid( ctx.url ) ){ throw except_t("invalid URL"); }
        obj->state = true; obj->ctx = ctx;
    }

    fetch() : obj( new NODE() ) { obj->state = false; }
    
    /*─······································································─*/

    coEmit(){
    coBegin

        do{ emscripten_fetch_attr_t attr; emscripten_fetch_attr_init( &attr );
            memcpy( attr.requestMethod, obj->ctx.method.get(), obj->ctx.method.size() );
    
            if ( obj->ctx.timeout != 0 ){ attr.timeoutMSecs = obj->ctx.timeout; }
            for( auto& x: obj->ctx.headers.data() ){
                 obj->hdr.push( x.first .c_str() );
                 obj->hdr.push( x.second.c_str() );
            }    obj->hdr.push( nullptr );

            obj->filename       = regex::format( "tmp_${0}_${1}", rand(), process::now() );
            obj->file           = file_t( obj->filename, "w" );
    
            attr.attributes     = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
            //                  | EMSCRIPTEN_FETCH_STREAM_DATA   ;

            attr.userData       = type::cast<void>( this );
            attr.requestHeaders = (char**) obj->hdr.data();
            attr.requestDataSize= obj->ctx.body.size();
            attr.requestData    = obj->ctx.body.get();
            attr.onprogress     = progress;
            attr.onsuccess      = callback;
            attr.onerror        = fail    ;
    
            emscripten_fetch( &attr, obj->ctx.url.get() );

        } while(0); coWait( is_closed()==false );

    coFinish }
    
    /*─······································································─*/

    void set_rejected_callback( rej_t<except_t> callback ) const noexcept {
         obj->rej = callback;
    }

    void set_resolved_callback( res_t<http_t> callback ) const noexcept {
         obj->res = callback;
    }
    
    /*─······································································─*/

    bool is_closed() const noexcept { return obj->state==false; }

    void     close() const noexcept { /*--*/ obj->state =false; }

};}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace http {

    inline promise_t<http_t,except_t> fetch( const fetch_t& fetch ) {
    return promise_t<http_t,except_t>([=]( res_t<http_t> res, rej_t<except_t> rej ){

        if( !url::is_valid( fetch.url ) ){ rej(except_t("invalid URL")); return; }

        auto task = type::bind( new generator::fetch( fetch ) );
             task->set_resolved_callback( res );
             task->set_rejected_callback( rej );

        process::add( task );

    }); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace https {

    inline promise_t<https_t,except_t> fetch( const fetch_t& fetch ) {
    return promise_t<https_t,except_t>([=]( res_t<https_t> res, rej_t<except_t> rej ){

        if( !url::is_valid( fetch.url ) ){ rej(except_t("invalid URL")); return; }

        auto task = type::bind( new generator::fetch( fetch ) );
             task->set_resolved_callback( res );
             task->set_rejected_callback( rej );

        process::add( task );

    }); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/