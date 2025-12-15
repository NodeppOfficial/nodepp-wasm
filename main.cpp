#include <nodepp/nodepp.h>
#include <nodepp/bind.h>

using namespace nodepp;

void onMain(){

    process::add( coroutine::add( COROUTINE(){
    coBegin

        while( true ){ EM_EVAL( _STRING_(
           document.querySelector("[counter]").innerHTML = 'Hello World! ${0}';
        ), process::now() ); coDelay(1000); }

    coFinish
    }));

    console::log("hello world!");

}