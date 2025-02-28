/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process { namespace env {

    int set( const string_t& name, const string_t& value ){ return setenv( name.c_str(), value.c_str(), 1 ); }

    string_t get( const string_t& name ){ return getenv( name.c_str() ); }

    int del( const string_t& name ){ return unsetenv( name.c_str() ); }

}   /*─······································································─*/

    bool  is_child(){ return !env::get("CHILD").empty(); }

    bool is_parent(){ return env::get("CHILD").empty(); }

    string_t shell(){ return env::get("SHELL"); }

    string_t  home(){ return env::get("HOME"); }

}}

/*────────────────────────────────────────────────────────────────────────────*/
