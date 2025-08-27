/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WAIT
#define NODEPP_WAIT

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { template< class T, class... A > class wait_t {
protected:

    struct DONE {  bool        *out;
        function_t<bool,T,A...> clb;
    };

    struct NODE {
        char skip     = 1;
        queue_t<DONE> que;
    };  ptr_t<NODE>   obj;

public:

    wait_t() noexcept : obj( new NODE() ) {}

    virtual ~wait_t() noexcept { free(); }

    /*─······································································─*/

    void* operator()( T val, function_t<void> func ) const noexcept { return on(val,func); }

    /*─······································································─*/

    void* once( T val, function_t<void,A...> func ) const noexcept {
        ptr_t<bool> out = new bool(1); DONE ctx;
        ctx.out=&out; ctx.clb=([=]( T arg, A... args ){
            if( val != arg ){ return true;   }
            if(*out != 0   ){ func(args...); }
            if( out.null() ){ return false;  } *out = 0; return *out;
        }); obj->que.push(ctx); return &out;
    }

    void* on( T val, function_t<void,A...> func ) const noexcept {
        ptr_t<bool> out = new bool(1); DONE ctx;
        ctx.out=&out; ctx.clb=([=]( T arg, A... args ){
            if( val != arg ){ return true;   }
            if(*out != 0   ){ func(args...); }
            if( out.null() ){ return false;  } return *out;
        }); obj->que.push(ctx); return &out;
    }

    void off( void* address ) const noexcept {
        if( address == nullptr ){ return; }
        memset( address, 0, sizeof(bool) );
    }

    /*─······································································─*/

    bool  empty() const noexcept { return obj->que.empty(); }
    ulong  size() const noexcept { return obj->que.size (); }

    /*─······································································─*/

    void free() const noexcept {
        if( obj->skip == -1 ){ resume(); }
        auto x=obj->que.first(); while( x!=nullptr && !obj->que.empty() ){
        auto y=x->next; if( *x->data.out==0 ){ obj->que.erase(x); } x=y; }
    }

    void clear() const noexcept {
        auto x=obj->que.first(); while( x!=nullptr && !obj->que.empty() ){
        auto y=x->next; *x->data.out=0; x=y;
    }}

    /*─······································································─*/

    void emit( const T& arg, const A&... args ) const noexcept {
        if( obj.null() || is_paused() ){ return; } auto x=obj->que.first( );
        while( x!=nullptr && !obj->que.empty() ) { auto y=x->next; bool z=0;
          if  ( *x->data.out == 0 ) /*-------------*/ { z=1; }
          elif( !x->data.clb(arg,args...) ) /*-----*/ { z=1; }
          if  ( !x->data.clb.null() && z ) { *x->data.out=0; }
    x=y; }}

    /*─······································································─*/

    bool is_paused() const noexcept { return obj->skip<=0; }

    void    resume() const noexcept { obj->skip = 1; }

    void      stop() const noexcept { obj->skip = 0; }

    void      skip() const noexcept { obj->skip =-1; }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
