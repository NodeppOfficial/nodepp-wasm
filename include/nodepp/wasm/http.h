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

    string_t  url;
    string_t  body;
    uchar     status;
    header_t  headers;
    string_t  filename;
    ulong     timeout= 60000;
    string_t  method = "GET";

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { struct http_t : public fetch_t, public file_t { public:

    http_t( fetch_t fetch ) noexcept : fetch_t(fetch), file_t(fetch.filename,"r") {}
   ~http_t() noexcept { if( obj.count()>1 ){ return; } ::remove( filename.get() ); }
    http_t() noexcept : fetch_t({}), file_t() {}

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace http {

    inline promise_t<http_t,except_t> fetch( const fetch_t& fetch ) {
    return promise_t<http_t,except_t>([=]( 
           res_t<http_t> res, rej_t<except_t> rej 
    ){

        if( !url::is_valid( fetch.url ) ){ rej(except_t("invalid URL")); return; }

        auto krn  = type::bind( process::NODEPP_EVLOOP() );
        auto addr = process::invoke([=]( any_t raw ){ do { 
        auto value= raw.as<EM_VAL>(); krn->wake();

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

            auto stte = value["status"].as<int>();
            auto addr = value["addr"]  .as<EM_STRING>();
            auto raw  = value["data"]  .as<EM_STRING>();
            string_t data ( raw.c_str(), raw.length() );

            auto    name= string::format( "%lu_%s", process::now(), addr.c_str() );
            fetch_t args; file_t ( name, "w" );
                    args.filename= name; 
                    args.status  = stte;
                    args.headers = hdrs;
                    args.url     = fetch.url;

            http_t cli/**/( args ); 
            cli.set_borrow( data ); res( cli );

        } while(0); return -1; });

        string_t obj = "{", hdr = json::stringify( fetch.headers );
        if( !fetch.body   .empty() ){ obj += regex::format( "body:   \"${0}\",", fetch.body    ); }
        if( !fetch.method .empty() ){ obj += regex::format( "method: \"${0}\",", fetch.method  ); }
        if(  fetch.timeout > 0     ){ obj += regex::format( "timeout:\"${0}\",", fetch.timeout ); }
        if( !fetch.headers.empty() ){ obj += regex::format( "headers:  ${0}  ,", hdr           ); }
        obj += "}";

        EM_EVAL( _STRING_( fetch( "${0}", ${1} )

            .then( res => { const headerObj = {};

                res.headers.forEach( (value, key) => {
                    headerObj[key] = value;
                });

                return res.arrayBuffer().then((data) => ({
                    headers: headerObj , 
                    status : res.status,
                    body   : data
                }));
            })

            .then( res => {
                ${2}._nodepp_invoke_( "${3}", {
                    type   : 1, 
                    addr   : "${3}",
                    data   : res.body, 
                    status : res.status,
                    headers: res.headers
                });
            })

            .catch( err => {
                ${2}._nodepp_invoke_( "${3}", {
                    type: 0, data: err.message 
                });
            });

        ), fetch.url, obj, NODEPP_MODULE_NAME, addr );

    }); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/