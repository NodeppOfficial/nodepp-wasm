#include <nodepp/nodepp.h>
#include <nodepp/http.h>

using namespace nodepp;

void onMain() {

    fetch_t args;
            args.method = "GET";
            args.url    = "http://localhost:6931/";

    http::fetch( args )
    
    .then([=]( http_t cli ){
        console::log( cli.read() );
    })

    .fail([=]( except_t err ){
        console::log( err.what() );
    });

}