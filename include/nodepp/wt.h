/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WT
#define NODEPP_WT

/*────────────────────────────────────────────────────────────────────────────*/

#include "stream.h"
#include "encoder.h"
#include "wasm/wt.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace wt {
    inline wt_t  client( const string_t& url ) { return wt_t(url); }
    inline wt_t connect( const string_t& url ) { return wt_t(url); }
}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif