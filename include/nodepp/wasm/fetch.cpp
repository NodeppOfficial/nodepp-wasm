/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#pragma once
#include <emscripten/fetch.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { using header_t = map_t< string_t, string_t >; }

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { struct fetch_t {

    string_t  url;
    string_t  body;
    string_t  code;
    uchar     status;
    header_t  headers;
    ulong     timeout= 0;
    string_t  method = "GET";

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace _fetch_ {

    void EVENT( emscripten_fetch_t* fetch ) {

        if( type::cast<queue_t<any_t>>( fetch->userData )->size() != 3 ){ return; }

        auto dta = *( type::cast<queue_t<any_t>>( fetch->userData ) );
        auto rej = dta[1]->data.as<function_t<void,except_t>>();
        auto res = dta[0]->data.as<function_t<void,fetch_t>>();
                   dta[2]->data.as<function_t<void>>()();

        if( fetch->status == 0 ){
            rej( except_t( "Something Went Wrong" ) );
            emscripten_fetch_close( fetch ); return;
        }

        fetch_t result;
                result.body   = string_t( fetch->data, fetch->numBytes );
                result.code   = fetch->statusText;
                result.status = fetch->status;
                result.url    = fetch->url;

        string_t headers ( emscripten_fetch_get_response_headers_length( fetch ), '\0' );
        emscripten_fetch_get_response_headers( fetch, headers.get(), headers.size() );
        forEach( x, string::split( headers, '\n' ) ){
            auto y = x.find( ": " ); if( y == nullptr ){ break; }
            result.headers[ x.slice(0,y[0]).to_capital_case() ] = x.slice(y[1],-1);
        }

        emscripten_fetch_close( fetch ); res( result );

    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace fetch {

    promise_t<fetch_t,except_t> add ( const fetch_t& fetch ) {
           auto ctx = type::bind( fetch );
    return promise_t<fetch_t,except_t>([=]( function_t<void,fetch_t> res, function_t<void,except_t> rej ){

        if( !url::is_valid( ctx->url ) ){ rej(except_t("invalid URL")); return; }

        struct NODE {
            ptr_t<bool> state = new bool(1);
            array_t<const char*> hdr;
            queue_t<any_t> args;
        }; ptr_t<NODE> obj = new NODE();

        function_t<void> done ([=](){ *obj->state = 0; });

        process::add([=](){
        coStart

            do {

                obj->args.push( res ); obj->args.push( rej ); obj->args.push( done );

                emscripten_fetch_attr_t attr;
                emscripten_fetch_attr_init( &attr );
                memcpy( attr.requestMethod, ctx->method.get(), ctx->method.size() );

                if( ctx->timeout != 0 ){ attr.timeoutMSecs = ctx->timeout; }

                for( auto& x: ctx->headers.data() ){
                    obj->hdr.push( x.first.c_str()  );
                    obj->hdr.push( x.second.c_str() );
                }   obj->hdr.push( nullptr );

                attr.attributes     = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
                attr.userData       = type::cast<void>( &obj->args );
                attr.requestHeaders = (char**) obj->hdr.data();
                attr.requestDataSize= ctx->body.size();
                attr.requestData    = ctx->body.get();
                attr.onsuccess      = _fetch_::EVENT;
                attr.onerror        = _fetch_::EVENT;

                emscripten_fetch( &attr, ctx->url.get() );

            } while(0); while( *obj->state ){ coNext; }

        coStop
        });

    }); }


} using http_t = fetch_t; }

/*────────────────────────────────────────────────────────────────────────────*/
