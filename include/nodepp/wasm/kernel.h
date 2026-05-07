/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_EVENT_SCHEDULER
#define NODEPP_EVENT_SCHEDULER NODEPP_SCHEDULER_NPOLL
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#if NODEPP_EVENT_SCHEDULER == NODEPP_SCHEDULER_NPOLL

namespace nodepp { class kernel_t {
private:

    enum FLAG { 
         KV_STATE_UNKNOWN = 0b00000000, 
         KV_STATE_WRITE   = 0b00000001,
         KV_STATE_READ    = 0b00000010,
         KV_STATE_EDGE    = 0b10000000,
         KV_STATE_USED    = 0b00000100,
         KV_STATE_AWAIT   = 0b00001100,
         KV_STATE_SLEEP   = 0b01000000,
         KV_STATE_CLOSED  = 0b00001000
    };

    struct kevent_t { public:
        function_t<int> callback;
        ulong timeout; int fd, flag; 
    };

protected:

    void clear_timeout() const noexcept { get_timeout(true); }

    ulong set_timeout( int time=0 ) const noexcept { 
        if( time < 0 ){ /*--------------*/ return 1; }
        auto stamp=&get_timeout(); ulong out=*stamp;
        if( *stamp>(ulong)time ){ *stamp=(ulong)time; }
    return out; }

    ulong& get_timeout( bool reset=false ) const noexcept {
        if( reset ) { obj->timeout=(ulong)-1; }
    return obj->timeout; }

    int get_delay_ms() const noexcept {
        ulong tasks= obj->ev_queue.size() + obj->probe.get();
        if(tasks==0 && obj.count()>1 ){ return 1000; }
    return tasks==0 ? 0 : get_timeout(); }

protected:

    struct NODE {
        int /*-*/ state;
        ulong   timeout;
        probe_t   probe;
        loop_t ev_queue;
    };  ptr_t<NODE> obj;

public:

    kernel_t() noexcept : obj( new NODE() ) {}

public:

    void off( ptr_t<task_t> address ) const noexcept { clear( address ); }

    void clear( ptr_t<task_t> address ) const noexcept {
         if( address.null() ) /*--------------*/ { return; }
         if( address->flag & TASK_STATE::CLOSED ){ return; }
             address->flag = TASK_STATE::CLOSED;
    }

    /*─······································································─*/

    ulong size() const noexcept { return obj->ev_queue.size() + obj->probe.get() + obj.count()-1; }

    void clear() const noexcept { /*--*/ obj->ev_queue.clear(); obj->probe.clear(); }
    
    bool should_close() const noexcept { return empty() || NODEPP_SHTDWN(); }

    bool empty() const noexcept { return size()==0; }

    /*─······································································─*/

    bool is_sleeping() const noexcept { return obj->state & KV_STATE_SLEEP; }

    ulong get_delay() const noexcept { return get_delay_ms(); }

    int   wake() const noexcept { return -1; }

    /*─······································································─*/

    template< class T, class U, class... W >
    ptr_t<task_t> poll_add ( T str, int /*unused*/, U cb, ulong timeout=0, const W&... args ) const noexcept {

        auto time = type::bind( timeout>0 ? timeout + process::now() : timeout );
        auto clb  = type::bind( cb ); 
        
        return obj->ev_queue.add( coroutine::add( COROUTINE(){
        coBegin 

            if( *time > 0 && *time < process::now() ){ coEnd; }
            coSet(0); return (*clb)( args... ) >= 0 ? 1 : -1; 

        coFinish
        }));

    }

    template< class T, class... V >
    ptr_t<task_t> loop_add ( T cb, const V&... args ) const noexcept {
        return obj->ev_queue.add( cb, args... );
    }

    /*─······································································─*/

    inline int next() const {

        while( obj->ev_queue.next() == 1 ){ return 1; } 

        obj->state |=  KV_STATE_SLEEP;
        set_timeout(obj->ev_queue.get_delay());
        process::delay( get_delay_ms() );
        clear_timeout();
        obj->state &=~ KV_STATE_SLEEP;

    return 1; }

    /*─······································································─*/

    template< class T, class... V > 
    int await( T cb, const V&... args ) const { 
    int c=0; probe_t tmp = obj->probe;

        if ((c =cb(args...))>=0 ){
        if ( c==1 ){ auto t = coroutine::getno().delay;
        if ( t >0 ){ set_timeout( t ); }
        else /*-*/ { set_timeout(0UL); }} next(); return 1; } 
    
    return -1; }

};}

#endif

/*────────────────────────────────────────────────────────────────────────────*/

#if NODEPP_EVENT_SCHEDULER == NODEPP_SCHEDULER_LITE

namespace nodepp { class kernel_t {
private:

    enum FLAG { 
         KV_STATE_UNKNOWN = 0b00000000, 
         KV_STATE_WRITE   = 0b00000001,
         KV_STATE_READ    = 0b00000010,
         KV_STATE_EDGE    = 0b10000000,
         KV_STATE_USED    = 0b00000100,
         KV_STATE_AWAIT   = 0b00001100,
         KV_STATE_SLEEP   = 0b01000000,
         KV_STATE_CLOSED  = 0b00001000
    };

    struct kevent_t { public:
        function_t<int> callback;
        ulong timeout; int fd, flag; 
    };

protected:

    struct NODE {
        int /*-*/ state;
        loop_t ev_queue;
    };  ptr_t<NODE> obj;

public:

    kernel_t() noexcept : obj( new NODE() ) {}

public:

    void off( ptr_t<task_t> address ) const noexcept { clear( address ); }

    void clear( ptr_t<task_t> address ) const noexcept {
         if( address.null() ) /*--------------*/ { return; }
         if( address->flag & TASK_STATE::CLOSED ){ return; }
             address->flag = TASK_STATE::CLOSED;
    }

    /*─······································································─*/

    ulong size() const noexcept { return obj->ev_queue.size() + obj.count()-1; }

    void clear() const noexcept { /*--*/ obj->ev_queue.clear(); }
    
    bool should_close() const noexcept { return empty() || NODEPP_SHTDWN(); }

    bool empty() const noexcept { return size()==0; }

    /*─······································································─*/

    bool is_sleeping() const noexcept { return obj->state & KV_STATE_SLEEP; }

    ulong get_delay() const noexcept { return -1; }

    int   wake() const noexcept { return -1; }

    /*─······································································─*/

    template< class T, class U, class... W >
    ptr_t<task_t> poll_add ( T str, int /*unused*/, U cb, ulong timeout=0, const W&... args ) const noexcept {

        auto time = type::bind( timeout>0 ? timeout + process::now() : timeout );
        auto clb  = type::bind( cb ); 
        
        return obj->ev_queue.add( coroutine::add( COROUTINE(){
        coBegin 

            if( *time > 0 && *time < process::now() ){ coEnd; }
            coSet(0); return (*clb)( args... ) >= 0 ? 1 : -1; 

        coFinish
        }));

    }

    template< class T, class... V >
    ptr_t<task_t> loop_add ( T cb, const V&... args ) const noexcept {
        return obj->ev_queue.add( cb, args... );
    }

    /*─······································································─*/

    inline int next() const { return obj->ev_queue.next(); }

    /*─······································································─*/

    template< class T, class... V > 
    int await( T cb, const V&... args ) const { int c=0;
        if ((c =cb(args...))>=0 ){ next(); return 1; }
    return -1; }

};}

#endif

/*────────────────────────────────────────────────────────────────────────────*/