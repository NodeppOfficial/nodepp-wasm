/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WS
#define NODEPP_WS

/*────────────────────────────────────────────────────────────────────────────*/

#include "wasm/ws.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace ws {
    inline ws_t  client( const string_t& url ) { return ws_t(url); }
    inline ws_t connect( const string_t& url ) { return ws_t(url); }
}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif