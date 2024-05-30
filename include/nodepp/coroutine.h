/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_COROUTINE
#define NODEPP_COROUTINE

/*────────────────────────────────────────────────────────────────────────────*/

template< class T > T   min( const T& min, const T& max ){ return min < max ? min : max; }
template< class T > T   max( const T& min, const T& max ){ return max > min ? max : min; }
template< class T > T clamp( const T& val, const T& _min, const T& _max ){ return max( _min, min( _max, val ) ); }

/*────────────────────────────────────────────────────────────────────────────*/

#define _EERROR( EV, ... ) if ( EV.empty() ){ console::error(__VA_ARGS__); } \
                           else EV.emit( except_t(__VA_ARGS__) );
#define _ERROR( ... )      throw except_t (__VA_ARGS__);

/*────────────────────────────────────────────────────────────────────────────*/

#define onMain INIT(); int main( int argc, char** args ) { \
   process::start( argc, args ); INIT(); \
   process::stop(); return 0;            \
}  void INIT

/*────────────────────────────────────────────────────────────────────────────*/

#define coDelay(VALUE)  do { static auto tm = process::millis()+VALUE; while( process::millis() < tm ){ coNext; } tm = process::millis()+VALUE; break; } while (0)
#define coUDelay(VALUE) do { static auto tm = process::micros()+VALUE; while( process::micros() < tm ){ coNext; } tm = process::micros()+VALUE; break; } while (0)
#define coWait(VALUE)   do { while( !VALUE ){ coNext; } } while(0)

/*────────────────────────────────────────────────────────────────────────────*/

#define coReturn(VALUE) do { _state_ = _LINE_; return VALUE; case _LINE_:; } while (0)
#define coNext          do { _state_ = _LINE_; return 1;     case _LINE_:; } while (0)
#define coAgain         do { _state_ = _LINE_; return 0;     case _LINE_:; } while (0)
#define coGoto(VALUE)   do { _state_ = VALUE ; return 1;                   } while (0)
#define coYield(VALUE)  do { _state_ = VALUE ; return 1;     case VALUE:;  } while (0)

/*────────────────────────────────────────────────────────────────────────────*/

#define coStart static int _state_ = 0; { switch(_state_) { case 0:;
#define coEnd         do { _state_ = 0; return -1; } while (0)
#define coStop           } _state_ = 0; return -1; }
#define coSet(VALUE)       _state_ = VALUE
#define coGet              _state_

/*────────────────────────────────────────────────────────────────────────────*/

#define GENERATOR(NAME) struct NAME : public generator_t
#define gnStart { switch(_state_) { case 0:;
#define gnStop  } _state_ = 0; return -1; }
#define gnEmit    int operator()

/*────────────────────────────────────────────────────────────────────────────*/

#define _FUNC_  __PRETTY_FUNCTION__
#define _NAME_  __FUNCTION__
#define _DATE_  __DATE__
#define _FILE_  __FILE__
#define _LINE_  __LINE__
#define _TIME_  __TIME__

/*────────────────────────────────────────────────────────────────────────────*/

#define CHUNK_TB( VALUE ) ( 1024 * 1024 * 1024 * 1024 * VALUE )
#define CHUNK_GB( VALUE ) ( 1024 * 1024 * 1024 * VALUE )
#define CHUNK_MB( VALUE ) ( 1024 * 1024 * VALUE )
#define CHUNK_KB( VALUE ) ( 1024 * VALUE )
#define CHUNK_B ( VALUE ) ( VALUE )

/*────────────────────────────────────────────────────────────────────────────*/

#define MAX_SOCKET SOMAXCONN
#define CHUNK_SIZE 65536
#define UNBFF_SIZE 4096
#define TIMEOUT    1

/*────────────────────────────────────────────────────────────────────────────*/

#define typeof(DATA) (string_t){ typeid( DATA ).name() }

struct generator_t { protected: int _state_ = 0; };

#define ullong  unsigned long long int
#define ulong   unsigned long int
#define llong   long long int
#define ldouble long double

#define ushort  unsigned short
#define uchar   unsigned char
#define uint    unsigned int
#define wchar   wchar_t

/*────────────────────────────────────────────────────────────────────────────*/

#define forEach( ITEM, CB ) for( auto& x : ITEM ){ CB( x ); }
#define forEver() for (;;)
#define elif else if

/*────────────────────────────────────────────────────────────────────────────*/

#define NODEPP_KERNEL_WASM 1

#define _KERNEL NODEPP_KERNEL_WASM

/*────────────────────────────────────────────────────────────────────────────*/

#endif
