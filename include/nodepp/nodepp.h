/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_NODEPP
#define NODEPP_NODEPP

/*────────────────────────────────────────────────────────────────────────────*/

#include "import.h"
#include "evloop.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    template< class... T >
    void error( const T&... msg ){ throw except_t( msg... ); }

    /*─······································································─*/

    inline void start(){
        onSIGEXIT.once([=](){ process::exit(0); }); 
        /*-----------------*/ signal::start();
    }

    /*─······································································─*/

    inline void stop(){ 
        while( !process::should_close() ){
            process::next( );
        }   process::exit(0);
    }

    inline void wait(){ process::stop(); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
