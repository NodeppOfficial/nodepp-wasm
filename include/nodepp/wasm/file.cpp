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
#include <emscripten.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class file_t {
private:

    void kill() const noexcept { fclose( obj->fd ); }

protected:

    struct NODE {
        ulong        range[2] ={ 0, 0 };
        FILE*        fd       = nullptr;
        bool         keep     = false;
        int          state    = 0;
        int          feof     = 1;
        ptr_t<char>  buffer;
        string_t     borrow;
    };  ptr_t<NODE> obj;

public: file_t() noexcept {}

    event_t<>          onUnpipe;
    event_t<>          onResume;
    event_t<except_t>  onError;
    event_t<>          onDrain;
    event_t<>          onClose;
    event_t<>          onStop;
    event_t<>          onOpen;
    event_t<>          onPipe;
    event_t<string_t>  onData;

    /*─······································································─*/

    virtual ~file_t() noexcept { if( obj.count() > 1 ){ return; } free(); }

    /*─······································································─*/

    file_t( const string_t& path, const string_t& mode, const ulong& _size=CHUNK_SIZE ) : obj( new NODE() ) {
            obj->fd = fopen( path.c_str(), mode.c_str() ); 
        if( obj->fd == nullptr ){
            throw except_t("such file or directory does not exist");
        }   set_buffer_size( _size ); 
    }

    file_t( FILE* fd, const ulong& _size=CHUNK_SIZE ) : obj( new NODE() ) {
        if( fd == nullptr )
          { throw except_t("such file or directory does not exist"); }   
            obj->fd = fd; set_buffer_size( _size ); 
    }

    /*─······································································─*/

    bool     is_closed() const noexcept { return obj->state <  0 ||  is_feof() || obj->fd == nullptr; }
    bool       is_feof() const noexcept { return obj->feof  <= 0 && obj->feof  != -2; }
    bool  is_available() const noexcept { return obj->state >= 0 && !is_closed(); }
    bool is_persistent() const noexcept { return obj->keep; }

    /*─······································································─*/

    void  resume() const noexcept { if(obj->state== 0) { return; } obj->state= 0; onResume.emit(); }
    void    stop() const noexcept { if(obj->state==-3) { return; } obj->state=-3; onStop  .emit(); }
    void   reset() const noexcept { if(obj->state!=-2) { return; } resume(); pos(0); }
    void   flush() const noexcept { obj->buffer.fill(0); }

    /*─······································································─*/

    void close() const noexcept {
        if( obj->state< 0 ){ return; }
        if( obj->keep== 1 ){ stop(); goto DONE; }
            obj->state=-1; DONE:; onDrain.emit();
    }

    /*─······································································─*/

    void set_range( ulong x, ulong y ) const noexcept { obj->range[0] = x; obj->range[1] = y; }
    ulong* get_range() const noexcept { return obj == nullptr ? nullptr : obj->range; }
    int    get_state() const noexcept { return obj == nullptr ?      -1 : obj->state; }
    FILE*     get_fd() const noexcept { return obj == nullptr ? nullptr : obj->fd; }

    /*─······································································─*/

    void   set_borrow( const string_t& brr ) const noexcept { obj->borrow = brr; }
    ulong  get_borrow_size() const noexcept { return obj->borrow.size(); }
    char*  get_borrow_data() const noexcept { return obj->borrow.data(); }
    void        del_borrow() const noexcept { obj->borrow.clear(); }
    string_t&   get_borrow() const noexcept { return obj->borrow; }

    /*─······································································─*/

    ulong   get_buffer_size() const noexcept { return obj->buffer.size(); }
    char*   get_buffer_data() const noexcept { return obj->buffer.data(); }
    ptr_t<char>& get_buffer() const noexcept { return obj->buffer; }

    /*─······································································─*/

    ulong size() const noexcept { auto curr = pos();
        if( fseek( obj->fd, 0 , SEEK_END )>0 ){ return 0; }
        ulong size = ftell(obj->fd); 
        pos( curr ); return size;
    }

    /*─······································································─*/

    virtual ulong set_buffer_size( ulong _size ) const noexcept { 
        obj->buffer = ptr_t<char>( _size ); return _size;
    }

    /*─······································································─*/

    virtual void free() const noexcept {

        if( obj->state == -3 && obj.count() > 1 ){ resume(); return; }
        if( obj->state == -2 ){ return; } obj->state = -2;
       
        onUnpipe.clear(); onResume.clear();
        onError .clear(); onStop  .clear();
        onOpen  .clear(); onPipe  .clear();
        onData  .clear(); /*-------------*/
        
        onDrain.emit(); onClose.emit(); kill();

    }

    /*─······································································─*/

    ulong pos( ulong _pos ) const noexcept {
        fseek( obj->fd, _pos, SEEK_SET ); 
        return pos();
    }

    ulong pos() const noexcept { return ftell( obj->fd ); }

    /*─······································································─*/

    char read_char() const noexcept { return read(1)[0]; }

    string_t read_until( string_t ch ) const noexcept {
        auto gen = generator::file::until();
        while( gen( this, ch ) == 1 )
             { process::next(); }
        return gen.data;
    }

    string_t read_until( char ch ) const noexcept {
        auto gen = generator::file::until();
        while( gen( this, ch ) == 1 )
             { process::next(); }
        return gen.data;
    }

    string_t read_line() const noexcept {
        auto gen = generator::file::line();
        while( gen( this ) == 1 )
             { process::next(); }
        return gen.data;
    }

    /*─······································································─*/

    string_t read( ulong size=CHUNK_SIZE ) const noexcept {
        auto gen = generator::file::read();
        while( gen( this, size ) == 1 )
             { process::next(); }
        return gen.data;
    }

    ulong write( const string_t& msg ) const noexcept {
        auto gen = generator::file::write();
        while( gen( this, msg ) == 1 )
             { process::next(); }
        return gen.data;
    }

    /*─······································································─*/

    virtual int _read ( char* bf, const ulong& sx ) const noexcept { return __read ( bf, sx ); }
    virtual int _write( char* bf, const ulong& sx ) const noexcept { return __write( bf, sx ); }

    /*─······································································─*/

    virtual int __read( char* bf, const ulong& sx ) const noexcept {
        if( is_closed() ){ return -1; } if( sx==0 ){ return 0; }
        obj->feof = fread( bf, sizeof(char), sx, obj->fd );
        if( obj->feof <= 0 || feof( obj->fd ) ){ free(); } 
        return obj->feof;
    }

    virtual int __write( char* bf, const ulong& sx ) const noexcept {
        if( is_closed() ){ return -1; } if( sx==0 ){ return 0; }
        obj->feof = fwrite( bf, sizeof(char), sx, obj->fd );
        if( obj->feof <= 0 || feof( obj->fd ) ){ free(); } 
        return obj->feof;
    }

    /*─······································································─*/

    bool _write_( char* bf, const ulong& sx, ulong& sy ) const noexcept {
        if( sx==0 || is_closed() ){ return 1; } while( sy < sx ) {
            int c = __write( bf+sy, sx-sy );
            if( c <= 0 && c != -2 )          { return 0; }
            if( c >  0 ){ sy += c; continue; } return 1;
        }   return 0;
    }

    bool _read_( char* bf, const ulong& sx, ulong& sy ) const noexcept {
        if( sx==0 || is_closed() ){ return 1; } while( sy < sx ) {
            int c = __read( bf+sy, sx-sy );
            if( c <= 0 && c != -2 )          { return 0; }
            if( c >  0 ){ sy += c; continue; } return 1;
        }   return 0;
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/