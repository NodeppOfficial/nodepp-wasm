/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WORKER
#define NODEPP_WORKER

/*────────────────────────────────────────────────────────────────────────────*/

#include "json.h"
#include "object.h"
#include "promise.h"
#include "wasm/webrtc.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace webrtc {
    inline webrtc_t  client( fetch_webrtc_t* arg ) { return webrtc_t(arg); }
    inline webrtc_t connect( fetch_webrtc_t* arg ) { return webrtc_t(arg); }
}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
