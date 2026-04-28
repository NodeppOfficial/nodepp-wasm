#include <nodepp/nodepp.h>
#include <nodepp/encoder.h>
#include <nodepp/webrtc.h>

using namespace nodepp;

void onMain() {

    auto arg = fetch_webrtc_t();
         arg.peer_id = "my_personal_id";

    auto rtc = webrtc::connect( &arg );

    rtc.onSignal([=]( string_t ice ){
        console::log( ">>", json::parse( encoder::base64::btoa( ice ) )["candidate"].as<string_t>() );
    });

    auto off = rtc.create_offer().await();

    if( !off.has_value() ){ 
        console::log( "-0-", off.error() ); 
    return; }

    console::log( off.value() );

}