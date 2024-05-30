/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#define NODEPP_GENERATOR

#if !defined(GENERATOR_TIMER) && defined(NODEPP_TIMER) && defined(NODEPP_GENERATOR)
    #define  GENERATOR_TIMER
namespace nodepp { namespace _timer_ {

    GENERATOR( timer ){ public:

        template< class V, class... T > 
        gnEmit( V func, ulong time, const T&... args ){
        gnStart
            coDelay( time ); if( func(args...)<0 )
            { coEnd; } coGoto(0); 
        gnStop
        }

        template< class V, class... T > 
        gnEmit( V func, ulong* time, const T&... args ){
        gnStart
            coDelay( *time ); if( func(args...)<0 )
            { coEnd; } coGoto(0); 
        gnStop
        }

    };
    
    /*─······································································─*/

    GENERATOR( utimer ){ public:

        template< class V, class... T > 
        gnEmit( V func, ulong time, const T&... args ){
        gnStart
            coUDelay( time ); if( func(args...)<0 )
            { coEnd; } coGoto(0);
        gnStop
        }

        template< class V, class... T > 
        gnEmit( V func, ulong* time, const T&... args ){
        gnStart
            coUDelay( *time ); if( func(args...)<0 )
            { coEnd; } coGoto(0);
        gnStop
        }

    };

}}  
#undef NODEPP_GENERATOR
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#if !defined(GENERATOR_FILE) && defined(NODEPP_FILE) && defined(NODEPP_GENERATOR)
    #define  GENERATOR_FILE
namespace nodepp { namespace _file_ {

    GENERATOR( read ){ 
    private:  
        ulong    d; 
        ulong*   r; 
        
    public: 
        string_t data ;
        int      state; 

    template< class T > gnEmit( T* str, ulong size=CHUNK_SIZE ){
    gnStart state=0; d=0; data.clear(); str->flush();

        if(!str->is_available() ){ coEnd; } r = str->get_range();
        if(!str->get_borrow().empty() ){ data = str->get_borrow(); }

          if ( r[1] != 0  ){ auto pos = str->pos(); d = r[1]-r[0];
          if ( pos < r[0] ){ str->del_borrow(); str->pos( r[0] ); }
        elif ( pos >=r[1] ){ coEnd; } }
        else { d = str->get_buffer_size(); }

        if( data.empty() ) do {
            state=str->_read( str->get_buffer_data(), min(d,size) );
        if( true /* state==-2 */ ){ coNext; } } while ( state==-2 );
        
        if( state > 0 ){
            data = string_t( str->get_buffer_data(), (ulong) state );
        }   state = data.size(); str->del_borrow();
        
    gnStop
    }};
    
    /*─······································································─*/

    GENERATOR( write ){ 
    private:
        string_t b ;

    public:
        ulong    data ; 
        int      state;
        
    template< class T > gnEmit( T* str, const string_t& msg ){
    gnStart state=0; data=0; str->flush();

        if(!str->is_available() || msg.empty() ){ coEnd; }
        if( b.empty() ){ b = msg; }
        
        do { do { state=str->_write( b.data()+data, b.size()-data );
             if ( true /* state==-2 */ )    { coNext;        }
        } while ( state==-2 ); if( state>0 ){ data += state; }
        } while ( state>=0 && data<b.size() ); b.clear();

    gnStop
    }};

    /*─······································································─*/

    GENERATOR( line ){ 
    private:
        _file_::read _read;
        string_t     s;

    public: 
        string_t  data ;  
        ulong     state; 

    template< class T > gnEmit( T* str ){
    gnStart state=1; s.clear(); data.clear(); str->flush();

        while( str->is_available() ){
        while( _read(str) == 1 ){ coNext; }
           if( _read.state<= 0 ){ break; } state = 1; s += _read.data; 
          for( auto &x: s )     { if( x == '\n' ){ break; } state++; }
           if( state<=s.size() ){ break; }
        }      str->set_borrow(s);

        data = str->get_borrow().splice( 0, state );
    
    gnStop
    }};

}}
#undef NODEPP_GENERATOR
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#if !defined(GENERATOR_STREAM) && defined(NODEPP_STREAM) && defined(NODEPP_GENERATOR)
    #define  GENERATOR_STREAM 
namespace nodepp { namespace _stream_ {

    GENERATOR( pipe ){ 
    private:

        _file_::write _write;
        _file_::read  _read;

    public:

        template< class T > gnEmit( const T& inp ){
        gnStart inp.onPipe.emit();
            while( inp.is_available() ){
            while( _read(&inp)==1 ){ coNext; }
               if( _read.state<=0 ){ break;  }
                    inp.onData.emit( _read.data );
            }       inp.close(); 
        gnStop
        }

        template< class T, class V > gnEmit( const T& inp, const V& out ){
        gnStart inp.onPipe.emit(); out.onPipe.emit();
            while( inp.is_available() && out.is_available() ){
            while( _read(&inp) ==1 )           { coNext; }
               if( _read.state <=0 )           { break;  }
            while( _write(&out,_read.data)==1 ){ coNext; }
               if( _write.state<=0 )           { break;  }
                    inp.onData.emit( _read.data );
            }       inp.close(); out.close();
        gnStop
        }

    };
    
    /*─······································································─*/

    GENERATOR( line ){ 
    private:

        _file_::write _write;
        _file_::line  _read;

    public:

        template< class T > gnEmit( const T& inp ){
        gnStart inp.onPipe.emit();
            while( inp.is_available() ){
            while( _read(&inp)==1 ){ coNext; } 
               if( _read.state<=0 ){ break;  }
                   inp.onData.emit( _read.data );
            }      inp.close(); 
        gnStop
        }

        template< class T, class V > gnEmit( const T& inp, const V& out ){
        gnStart inp.onPipe.emit(); out.onPipe.emit();
            while( inp.is_available() && out.is_available() ){
            while( _read(&inp)==1 )            { coNext; } 
               if( _read.state<=0 )            { break;  }
            while( _write(&out,_read.data)==1 ){ coNext; }
               if( _write.state<=0 )           { break;  }
                    inp.onData.emit( _read.data );
            }       inp.close(); out.close();
        gnStop
        }

    };
    
}}
#undef NODEPP_GENERATOR
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#if !defined(GENERATOR_ZLIB) && defined(NODEPP_ZLIB) && defined(NODEPP_GENERATOR)
    #define  GENERATOR_ZLIB 
namespace nodepp { namespace _zlib_ {

    GENERATOR( inflate ){ 
    private:
    
        ptr_t<z_stream> str = new z_stream;
        int x=0; ulong size; string_t dout;
        _file_::write _write;
        _file_::read  _read;

    public:

        template< class T, class V, class U >
        gnEmit( const T& inp, const V& out, U cb ){
        gnStart inp.onPipe.emit(); out.onPipe.emit();

            str->zfree    = Z_NULL;
            str->zalloc   = Z_NULL;
            str->opaque   = Z_NULL;
            str->next_in  = Z_NULL;
            str->avail_in = Z_NULL;

            if( cb( &str ) != Z_OK ){ 
                string_t message = "Failed to initialize zlib for compression.";
                process::error( inp.onError, message );
                process::error( inp.onError, message ); coEnd;
            }

            while( inp.is_available() && out.is_available() ){
            while( _read(&inp)==1 ){ coNext; }
               if( _read.state<=0 ){ break;  }

                str->avail_in = _read.data.size();
                str->avail_out= inp.get_buffer_size();
                str->next_in  = (Bytef*)_read.data.data();
                str->next_out = (Bytef*)inp.get_buffer_data(); 
                            x = ::inflate( &str, Z_FINISH );

                if(( size=inp.get_buffer_size()-str->avail_out )>0){
                    dout = string_t( inp.get_buffer_data(), size );
                    inp.onData.emit(dout); 
                    while( _write(&out,dout)==1 ){ coNext; }
                       if( _write.state<=0 )     { break;  } continue;
                }
                
                if( x==Z_STREAM_END ) { break; } elif( x < 0 ){ 
                    string_t message = string::format("ZLIB: %s",str->msg);
                    process::error( inp.onError, message );
                    process::error( out.onError, message ); break;
                }
            
            }   inflateEnd( &str ); out.close(); inp.close(); 
        
        gnStop
        }

        template< class T, class U >
        gnEmit( const T& inp, U cb ){
        gnStart inp.onPipe.emit();

            str->zfree    = Z_NULL;
            str->zalloc   = Z_NULL;
            str->opaque   = Z_NULL;
            str->next_in  = Z_NULL;
            str->avail_in = Z_NULL;

            if( cb( &str ) != Z_OK ){ 
                string_t message = "Failed to initialize zlib for compression.";
                process::error( inp.onError, message ); coEnd;
            }

            while( inp.is_available() ){
            while( _read(&inp)==1 ){ coNext; }
               if( _read.state<=0 ){ break;  }

                str->avail_in = _read.data.size();
                str->avail_out= inp.get_buffer_size();
                str->next_in  = (Bytef*)_read.data.data();
                str->next_out = (Bytef*)inp.get_buffer_data(); 
                            x = ::inflate( &str, Z_PARTIAL_FLUSH );

                if(( size=inp.get_buffer_size()-str->avail_out )>0){
                    dout = string_t( inp.get_buffer_data(), size );
                    inp.onData.emit(dout); continue;
                }

                if( x==Z_STREAM_END ) { break; } elif( x < 0 ){ 
                    string_t message = string::format("ZLIB: %s",str->msg);
                    process::error( inp.onError, message ); break;
                } 

            }   inflateEnd( &str ); inp.close(); 
            
        gnStop
        }

    };
    
    /*─······································································─*/

    GENERATOR( deflate ){ 
    private:

        ptr_t<z_stream> str = new z_stream;
        int x=0; ulong size; string_t dout;
        _file_::write _write;
        _file_::read  _read;

    public:

        template< class T, class V, class U >
        gnEmit( const T& inp, const V& out, U cb ){
        gnStart inp.onPipe.emit(); out.onPipe.emit();

            str->zfree    = Z_NULL;
            str->zalloc   = Z_NULL;
            str->opaque   = Z_NULL;
            str->next_in  = Z_NULL;
            str->avail_in = Z_NULL;

            if( cb( &str ) != Z_OK ){ 
                string_t message = "Failed to initialize zlib for compression.";
                process::error( inp.onError, message );
                process::error( inp.onError, message ); coEnd;
            }

            while( inp.is_available() && out.is_available() ){
            while( _read(&inp)==1 ){ coNext; }
               if( _read.state<=0 ){ break;  }

                str->avail_in = _read.data.size();
                str->avail_out= inp.get_buffer_size();
                str->next_in  = (Bytef*)_read.data.data();
                str->next_out = (Bytef*)inp.get_buffer_data(); 
                            x = ::deflate( &str, Z_PARTIAL_FLUSH );

                if(( size=inp.get_buffer_size()-str->avail_out )>0){
                    dout = string_t( inp.get_buffer_data(), size );
                    inp.onData.emit(dout); 
                    while( _write(&out,dout)==1 ){ coNext; }
                       if( _write.state<=0 )     { break;  } continue;
                }

                if( x==Z_STREAM_END ) { break; } elif( x < 0 ){ 
                    string_t message = string::format("ZLIB: %s",str->msg);
                    process::error( inp.onError, message );
                    process::error( out.onError, message ); break;
                }
            
            }   deflateEnd( &str ); out.close(); inp.close(); 
            
        gnStop
        }

        template< class T, class U >
        gnEmit( const T& inp, U cb ){
        gnStart inp.onPipe.emit();

            str->zfree    = Z_NULL;
            str->zalloc   = Z_NULL;
            str->opaque   = Z_NULL;
            str->next_in  = Z_NULL;
            str->avail_in = Z_NULL;

            if( cb( &str ) != Z_OK ){ 
                string_t message = "Failed to initialize zlib for compression.";
                process::error( inp.onError, message ); coEnd;
            }

            while( inp.is_available() ){
            while( _read(&inp)==1 ){ coNext; }
               if( _read.state<=0 ){ break;  }

                str->avail_in = _read.data.size();
                str->avail_out= inp.get_buffer_size();
                str->next_in  = (Bytef*)_read.data.data();
                str->next_out = (Bytef*)inp.get_buffer_data(); 
                            x = ::deflate( &str, Z_PARTIAL_FLUSH );

                if(( size=inp.get_buffer_size()-str->avail_out )>0){
                    dout = string_t( inp.get_buffer_data(), size );
                    inp.onData.emit(dout); continue;
                }
                
                if( x==Z_STREAM_END ) { break; } elif( x < 0 ){ 
                    string_t message = string::format("ZLIB: %s",str->msg);
                    process::error( inp.onError, message ); break;
                } 

            }   deflateEnd( &str ); inp.close(); 
            
        gnStop
        }

    };

}}
#undef NODEPP_GENERATOR
#endif