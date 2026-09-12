#include <nodepp/nodepp.h>
#include <nodepp/crypto.h>
#include <nodepp/encoder.h>

using namespace nodepp;

void onMain(){

    auto a = encoder::base58 ::atob( "hello world!" );
    auto b = crypto ::encoder::BASE58(); b.update( "hello world!" );

    auto c = encoder::base58 ::btoa( b.get() );
    auto d = crypto ::decoder::BASE58(); d.update( a );

    console::log( a );
    console::log( b.get() );
    console::log( c );
    console::log( d.get() );

}