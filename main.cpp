#include <nodepp/nodepp.h>
#include <nodepp/http.h>

using namespace nodepp;

void onMain(){

    fetch_t args;
            args.body    = "hello world!";
            args.method  = "POST";
            args.url     = "http://localhost:8000/";
            args.headers = header_t({
                { "Host", url::host(args.url) }
            });

    http::fetch( args )

    .then([]( http_t cli ){
        
        cli.read_body()
        
        .then([=]( http_t cli ){
            console::log( ">>", cli.body );
        })

        .fail([=]( except_t err ){
            console::log( "<>", err.what() );
        });

    })

    .fail([]( except_t err ){
        console::error( err );
    });

}