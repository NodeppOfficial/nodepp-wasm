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
#include <emscripten/websocket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace _socket_ {

    void start_device(){ static bool sockets=false; 
        if( sockets == false ){

            process::onSIGEXIT([=](){
                #ifdef SIGPIPE
                    process::signal::unignore( SIGPIPE );
                #endif 
            });
            
                #ifdef SIGPIPE
                    process::signal::ignore( SIGPIPE );
                #endif

        }   sockets = true;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp {

struct agent_t {
    bool  reuse_address = 1;
    ulong buffer_size   = CHUNK_SIZE;
    bool  reuse_port    = 1;
    bool  keep_alive    = 0;
    bool  broadcast     = 0;
};

class socket_t : public file_t {
protected:

    using SOCKADDR    = struct sockaddr;
    using SOCKADDR_IN = struct sockaddr_in;

    struct DONE {
        socklen_t addrlen; bool srv=0; socklen_t len;
        SOCKADDR server_addr, client_addr;
    };  ptr_t<DONE> skt = new DONE();
    
    /*─······································································─*/

    virtual bool is_blocked( int& c ) const noexcept { 
        if ( c >= 0 ){ return 0; } auto error = os::error(); 
        if ( error == EISCONN ){ c =0; return 0; } return (
             error == EWOULDBLOCK || error == EINPROGRESS ||
             error == EALREADY    || error == EAGAIN      ||
             error == ECONNRESET
        );
    }

public: socket_t() noexcept { _socket_::start_device(); }

    int SOCK  = SOCK_STREAM;
    int AF    = AF_INET; 
    int PROT  = 0;
    
    /*─······································································─*/

    int set_recv_buff( ulong en ) const noexcept { int c;
        while( is_blocked( c=setsockopt( obj->fd, SOL_SOCKET, SO_RCVBUF, (char*)&en, sizeof(en) ) ) )
             { process::next(); } return c;
    }

    int set_send_buff( ulong en ) const noexcept { int c;
        while( is_blocked( c=setsockopt( obj->fd, SOL_SOCKET, SO_SNDBUF, (char*)&en, sizeof(en) ) ) )
             { process::next(); } return c;
    }

    int set_accept_connection( ulong en ) const noexcept { int c;
        while( is_blocked( c=setsockopt( obj->fd, SOL_SOCKET, SO_ACCEPTCONN, (char*)&en, sizeof(en) ) ) )
             { process::next(); } return c;
    }

    int set_dont_route( ulong en ) const noexcept { int c;
        while( is_blocked( c=setsockopt( obj->fd, SOL_SOCKET, SO_DONTROUTE, (char*)&en, sizeof(en) ) ) )
             { process::next(); } return c;
    }

    int set_keep_alive( uint en ) const noexcept { int c;
        while( is_blocked( c=setsockopt( obj->fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&en, sizeof(en) ) ) )
             { process::next(); } return c;
    }

    int set_broadcast( uint en ) const noexcept { int c;
        while( is_blocked( c=setsockopt( obj->fd, SOL_SOCKET, SO_BROADCAST, (char*)&en, sizeof(en) ) ) )
             { process::next(); } return c;
    }

    int set_reuse_address( uint en ) const noexcept { int c;
        while( is_blocked( c=setsockopt( obj->fd, SOL_SOCKET, SO_REUSEADDR, (char*)&en, sizeof(en) ) ) )
             { process::next(); } return c;
    }

    int set_ipv6_only_mode( uint en ) const noexcept { int c;
        while( is_blocked( c=setsockopt( obj->fd, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&en, sizeof(en) ) ) )
             { process::next(); } return c;
    }

#ifdef SO_REUSEPORT
    int set_reuse_port( uint en ) const noexcept { int c;
        while( is_blocked( c=setsockopt( obj->fd, SOL_SOCKET, SO_REUSEPORT, (char*)&en, sizeof(en) ) ) )
             { process::next(); } return c;
    }
#endif

    int get_error() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, SOL_SOCKET, SO_ERROR, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }

    int get_recv_buff() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, SOL_SOCKET, SO_RCVBUF, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }

    int get_send_buff() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, SOL_SOCKET, SO_SNDBUF, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }

    int get_accept_connection() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, SOL_SOCKET, SO_ACCEPTCONN, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }

    int get_dont_route() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, SOL_SOCKET, SO_DONTROUTE, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }

    int get_reuse_address() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, SOL_SOCKET, SO_REUSEADDR, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }

    int get_ipv6_only_mode() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }

#ifdef SO_REUSEPORT
    int get_reuse_port() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, SOL_SOCKET, SO_REUSEPORT, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }
#endif

    int get_keep_alive() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }

    int get_broadcast() const noexcept { int c, en; socklen_t size = sizeof(en);
        while( is_blocked( c=getsockopt(obj->fd, SOL_SOCKET, SO_BROADCAST, (char*)&en, &size) ) )
             { process::next(); } return c==0 ? en : c;
    }
    
    /*─······································································─*/

    string_t get_sockname() const noexcept { int c; string_t buff { INET_ADDRSTRLEN };
        SOCKADDR cli; if( skt->srv==1 ) cli = skt->client_addr; else cli = skt->server_addr;
        while( is_blocked( c=getsockname( obj->fd, &cli, &skt->len )) ){ process::next(); }
        inet_ntop( AF, &(((SOCKADDR_IN*)&cli)->sin_addr), (char*)buff, buff.size() );
        return c < 0 ? "127.0.0.1" : buff;
    }

    string_t get_peername() const noexcept { int c; string_t buff { INET_ADDRSTRLEN };
        SOCKADDR cli; if( skt->srv==1 ) cli = skt->client_addr; else cli = skt->server_addr;
        while( is_blocked( c=getpeername( obj->fd, &cli, &skt->len )) ){ process::next(); }
        inet_ntop( AF, &(((SOCKADDR_IN*)&cli)->sin_addr), (char*)buff, buff.size() );
        return c < 0 ? "127.0.0.1" : buff;
    }
    
    /*─······································································─*/

    virtual ulong set_buffer_size( ulong _size ) const noexcept { 
        set_send_buff( _size ); set_recv_buff( _size );
        obj->buffer = ptr_t<char>(_size); return _size;
    }
    
    /*─······································································─*/

    bool is_server() const noexcept { return skt->srv; }
    
    /*─······································································─*/

    void set_sockopt( agent_t opt ) const noexcept { 
        set_reuse_address( opt.reuse_address );
        set_buffer_size  ( opt.buffer_size   );
    #ifdef SO_REUSEPORT
        set_reuse_port   ( opt.reuse_port    );
    #endif
        set_keep_alive   ( opt.keep_alive    );
        set_broadcast    ( opt.broadcast     );
    }

    agent_t get_sockopt() const noexcept { 
    agent_t opt;
        opt.reuse_address = get_reuse_address();
        opt.buffer_size   = get_buffer_size();
    #ifdef SO_REUSEPORT
        opt.reuse_port    = get_reuse_port();
    #endif
        opt.keep_alive    = get_keep_alive();
        opt.broadcast     = get_broadcast();
    return opt;
    }
    
    /*─······································································─*/
    
    virtual ~socket_t() noexcept {
        if( obj.count() > 1 || obj->fd < 3 ){ return; } 
        if( obj->state == -2 ){ return; } free();
    }
    
    /*─······································································─*/

    socket_t( int fd, ulong _size=CHUNK_SIZE ){ _socket_::start_device();
        if( fd < 0 )  process::error("Such Socket has an Invalid fd");
        obj->fd = fd; set_nonbloking_mode(); set_buffer_size(_size); 
    }

    /*─······································································─*/

    virtual void free() const noexcept {
        if( obj->state == -3 && obj.count() > 1 ){ resume(); return; }
        if( obj->state == -2 ){ return; } obj->state = -2;
        ::shutdown(obj->fd,SHUT_RDWR); ::close( obj->fd ); 
        close(); onClose.emit();
    }

    /*─······································································─*/

    virtual int socket( const string_t& host, int port ) noexcept { 
        if( host.empty() ){ _EERROR(onError,"dns coudn't found ip"); return -1; }
            skt->addrlen = sizeof( skt->server_addr ); _socket_::start_device();

        if((obj->fd=::socket( AF, SOCK, PROT )) <= 0 )
          { _EERROR(onError,"can't initializate socket fd"); return -1; } 
          
        set_buffer_size( CHUNK_SIZE );
        set_nonbloking_mode();
        set_ipv6_only_mode(0);
        set_reuse_address(1);

    #ifdef SO_REUSEPORT
        set_reuse_port(1);
    #endif

        SOCKADDR_IN server, client;
        memset(&server, 0, sizeof(SOCKADDR_IN));
        memset(&client, 0, sizeof(SOCKADDR_IN));
        server.sin_family = AF; if( port>0 ) server.sin_port = htons(port);

          if( host == "0.0.0.0"         || host == "global"    ){ server.sin_addr.s_addr = INADDR_ANY; }
        elif( host == "1.1.1.1"         || host == "loopback"  ){ server.sin_addr.s_addr = INADDR_LOOPBACK; }
        elif( host == "255.255.255.255" || host == "broadcast" ){ server.sin_addr.s_addr = INADDR_BROADCAST; } 
        elif( host == "127.0.0.1"       || host == "localhost" ){ inet_pton(AF, "127.0.0.1", &server.sin_addr); }
        else                                                    { inet_pton(AF, host.c_str(),&server.sin_addr); }

        skt->server_addr = *((SOCKADDR*) &server); skt->client_addr = *((SOCKADDR*) &client); skt->len = sizeof( server ); return 1;
    }
    
    /*─······································································─*/

    int _connect() const noexcept { int c=0; if( skt->srv == 1 ){ return -1; }
        return is_blocked( c=::connect( obj->fd, &skt->server_addr, skt->addrlen ) ) ? -2 : c;
    }

    int _accept() const noexcept { int c=0; if( skt->srv == 0 ){ return -1; }
        return is_blocked( c=::accept( obj->fd, &skt->server_addr, &skt->addrlen ) ) ? -2 : c;
    }

    int _bind() const noexcept { int c=0; skt->srv = 1;
        return is_blocked( c=::bind( obj->fd, &skt->server_addr, skt->addrlen ) ) ? -2 : c;
    }

    int _listen() const noexcept { 
        int c=0; if( skt->srv == 0 ){ return -1; }
        return is_blocked( c=::listen( obj->fd, MAX_SOCKET ) ) ? -2 : c;
    }

    /*─······································································─*/

    int connect() const noexcept { int c=0;
        while( (c=_connect())==-2 ){ process::next(); } return c;
    }

    int listen() const noexcept { int c=0;
        while( (c=_listen())==-2 ){ process::next(); } return c;
    }

    int accept() const noexcept { int c=0;
        while( (c=_accept())==-2 ){ process::next(); } return c;
    }

    int bind() const noexcept { int c=0; 
        while( (c=_bind())==-2 ){ process::next(); } return c;
    }
    
    /*─······································································─*/

    virtual int _read( char* bf, const ulong& sx ) const noexcept {
        if ( is_closed() ){ return -1; } if( sx==0 ){ return 0; } if( SOCK != SOCK_DGRAM ){
            obj->feof = ::recv( obj->fd, bf, sx, 0 );
            obj->feof = is_blocked(obj->feof) ? -2 : obj->feof;
            return obj->feof;
        } else { SOCKADDR* cli; if( skt->srv==1 ) cli = &skt->client_addr; else cli = &skt->server_addr;
            obj->feof =::recvfrom( obj->fd, bf, sx, 0, cli, &skt->len );
            obj->feof = is_blocked(obj->feof) ? -2 : obj->feof;
            return obj->feof;
        }   return -1;
    }
    
    virtual int _write( char* bf, const ulong& sx ) const noexcept {
        if( is_closed() ){ return -1; } if( sx==0 ){ return 0; } if( SOCK != SOCK_DGRAM ){
            obj->feof =::send( obj->fd, bf, sx, 0 );
            obj->feof = is_blocked(obj->feof) ? -2 : obj->feof;
            return obj->feof;
        } else { SOCKADDR* cli; if( skt->srv==1 ) cli = &skt->client_addr; else cli = &skt->server_addr;
            obj->feof =::sendto( obj->fd, bf, sx, 0, cli, skt->len );
            obj->feof = is_blocked(obj->feof) ? -2 : obj->feof;
            return obj->feof;
        }   return -1;
    } 
    
};}

/*────────────────────────────────────────────────────────────────────────────*/