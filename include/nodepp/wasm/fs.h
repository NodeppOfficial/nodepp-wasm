/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_WASM_FS
#define NODEPP_WASM_FS

/*────────────────────────────────────────────────────────────────────────────*/

#include <emscripten.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace fs {

    inline file_t readable( const string_t& path, const ulong& _size=NODEPP_CHUNK_SIZE ){ return file_t( path, "r", _size ); }
    inline file_t writable( const string_t& path, const ulong& _size=NODEPP_CHUNK_SIZE ){ return file_t( path, "w", _size ); }

    /*─······································································─*/

    inline file_t std_output( const ulong& _size=NODEPP_CHUNK_SIZE ){ return file_t( stdout, _size ); }
    inline file_t std_input ( const ulong& _size=NODEPP_CHUNK_SIZE ){ return file_t( stdin , _size ); }
    inline file_t std_error ( const ulong& _size=NODEPP_CHUNK_SIZE ){ return file_t( stderr, _size ); }

    /*─······································································─*/

    inline bool exists_folder( const string_t& path ){
        return EM_EVAL( "Module.FS.isDir('${0}')", path ).as<bool>();
    }

    inline bool exists_file( const string_t& path ){
        FILE* fp = fopen( path.get(), "r" );
        if( fp == nullptr ){ return 0; }
        fclose(fp); /*----*/ return 1;
    }

    /*─······································································─*/

    inline expected_t<time_t,except_t> 
    file_modification_time( const string_t& path ){

        auto out = EM_EVAL( NODEPP_STRINGIFY( try {
            return Module.FS.stat('${0}').mtime.getTime()
        } catch(err) { return undefined; }) );
        
        if( out.isUndefined() ){ return except_t( "not found" ); }
        return out.as<time_t>();

    }

    inline expected_t<time_t,except_t> 
    file_access_time( const string_t& path ){

        auto out = EM_EVAL( NODEPP_STRINGIFY( try {
            return Module.FS.stat('${0}').atime.getTime()
        } catch(err) { return undefined; }) );
        
        if( out.isUndefined() ){ return except_t( "not found" ); }
        return out.as<time_t>();
        
    }

    inline expected_t<time_t,except_t> 
    file_creation_time( const string_t& path ){

        auto out = EM_EVAL( NODEPP_STRINGIFY( try {
            return Module.FS.stat('${0}').ctime.getTime()
        } catch(err) { return undefined; }) );
        
        if( out.isUndefined() ){ return except_t( "not found" ); }
        return out.as<time_t>();
        
    }

    /*─······································································─*/

    inline promise_t<string_t,except_t> read_file( const string_t& path ){
    return promise_t<string_t,except_t> ([=]( 
        res_t<string_t> res,  rej_t<except_t> rej
    ){

        if( !exists_file( path ) ){ rej( "file not found" ); return; }

        auto rd1 = type::bind( generator::file::read() );
        auto fl1 = type::bind( file_t( path, "r" ) );
        auto bff = ptr_t<string_t>( 0UL );

        process::add( coroutine::add( COROUTINE(){
        coBegin

            while( fl1->is_available() ){
                
                coWait( (*rd1)( &fl1 ) == 1 );
                if( rd1->state<=0 ){ break; }

               *bff += rd1->data;

            coNext; } res( *bff );

        coFinish
        }));

    }); }

    /*─······································································─*/

    inline promise_t<ulong,except_t> write_file( const string_t& path, const string_t& message ){
    return promise_t<ulong,except_t> ([=]( 
        res_t<ulong> res, rej_t<except_t> rej
    ){

        auto rd1 = type::bind( generator::file::write() );
        auto fl1 = type::bind( file_t( path, "w" ) );
        auto bff = ptr_t<ulong>( 0UL, 0UL );

        process::add( coroutine::add( COROUTINE(){
        coBegin

            while( fl1->is_available() && *bff < message.size() ){
                
                coWait( (*rd1)( &fl1, message ) == 1 );
                if( rd1->state<=0 ){ break; }

               *bff += rd1->state;

            coNext; } res( *bff );

        coFinish
        }));

    }); }

    /*─······································································─*/

    inline promise_t<ulong,except_t> append_file( const string_t& path, const string_t& message ){
    return promise_t<ulong,except_t> ([=]( 
        res_t<ulong> res,  rej_t<except_t> rej
    ){
        
        if( !exists_file( path ) ){ rej( "file not found" ); return; }

        auto rd1 = type::bind( generator::file::write() );
        auto fl1 = type::bind( file_t( path, "a+" ) );
        auto bff = ptr_t<ulong>( 0UL, 0UL );

        process::add( coroutine::add( COROUTINE(){
        coBegin

            while( fl1->is_available() && *bff < message.size() ){
                
                coWait( (*rd1)( &fl1, message ) == 1 );
                if( rd1->state<=0 ){ break; }

               *bff += rd1->state;

            coNext; } res( *bff );

        coFinish
        }));

    }); }

    /*─······································································─*/

    inline int copy_file( const string_t& src, const string_t& des ){
        if( !exists_file( src ) ){ return -1; } 
        stream::pipe( file_t( src, "r" ), file_t( des, "w" ) ); 
    return 1; }

    /*─······································································─*/

    inline int rename_file( const string_t& oname, const string_t& nname ) {
        if( oname.empty() || nname.empty() ){ return -1; }
        EM_EVAL( "Module.FS.rename('${0}','${1}');", oname, nname );
        return exists_file( nname ) ? 1 : -1;
    }

    /*─······································································─*/

    inline int move_file( const string_t& oname, const string_t& nname ) {
        if( oname.empty() || nname.empty() ){ return -1; }
        return rename_file( oname, nname );
    }

    /*─······································································─*/

    inline int remove_file( const string_t& path ){
        if( path.empty() ){ return -1; }
        return remove( path.c_str() );
    }

    /*─······································································─*/

    inline int create_file( const string_t& path ){
        if( path.empty() ){ return -1; }
        file_t( path,"w+"); return  1;
    }

    /*─······································································─*/

    inline len_t file_size( const string_t& path ){
        if( exists_file( path ) ){
            return file_t( path, "r" ).size();
        }   return 0;
    }

    /*─······································································─*/

    inline int create_folder( const string_t& dirname, uint permission=0777 ){
        if( dirname.empty() || exists_folder(dirname) ){ return -1; }
        EM_EVAL( "FS.mkdir('${0}',${1});", dirname, permission ); 
        return exists_folder( dirname ) ? 1 : -1;
    }

    /*─······································································─*/

    inline int remove_folder( const string_t& dirname ){
        if( dirname.empty() ){ return -1; }
        EM_EVAL( "FS.rmdir('${0}');", dirname ); 
        return exists_folder( dirname ) ? -1: 1;
    }

    /*─······································································─*/

    inline int read_folder_iterator( const string_t& dirname, function_t<void,string_t> cb ){
        if( dirname.empty() ){ return -1; }

        auto addr = process::invoke([=]( any_t item ){
        if( !item.has_value() ){ return -1; } do { 

            auto name = string_t( item.as<EM_STRING>() );

            if( string_t(name) == ".." ){ break; }
            if( string_t(name) == "."  ){ break; }
            cb( name );

        } while(0); return 1; });

        EM_EVAL( NODEPP_STRINGIFY (

            for( var x of module.FS.readdir('${0}') ){
                 Module.__call__('${1}',x);
            }    Module.__call__('${1}',undefined);

        ) );

    return 1; }

    /*─······································································─*/

    inline promise_t<ptr_t<string_t>,except_t> read_folder( const string_t& dirname ){
    return promise_t<ptr_t<string_t>,except_t> ([=](
        res_t<ptr_t<string_t>> res, rej_t<except_t> rej
    ){  
     
        queue_t<string_t> queue;

        auto addr = process::invoke([=]( any_t item ){
        if( !item.has_value() ){ 
        if( queue.empty() ){ rej( except_t( "not exists or empty" ) ); }
        else /*---------*/ { res( queue.data() ); } return -1; } do { 
            
            auto name = string_t( item.as<EM_STRING>() );
            if( string_t(name) == ".." ){ break; }
            if( string_t(name) == "."  ){ break; }
            queue.push( name );

        } while(0); return 1; });

        EM_EVAL( NODEPP_STRINGIFY (

            for( var x of module.FS.readdir('${0}') ){
                 Module.__call__('${1}',x);
            }    Module.__call__('${1}',undefined);

        ) );

    }); }

    /*─······································································─*/

    inline bool is_folder( const string_t& path ){ return exists_folder(path); }
    inline bool   is_file( const string_t& path ){ return exists_file  (path); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/