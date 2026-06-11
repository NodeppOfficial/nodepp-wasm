#include <nodepp/nodepp.h>
#include <nodepp/crypto.h>

using namespace nodepp;

void onMain(){

    console::log( regex::match_all( "222222", "[A-F0-9]{2}", true ).join("-") );
    console::log( NODEPP_REGEX_ENGINE );

}